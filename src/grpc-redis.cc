#include <grpc++/grpc++.h>
#include <assert.h>
#include "gflags/gflags.h"

#include "redis.grpc.pb.h"
#include "redis.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using redis::Req;
using redis::Reply;
using redis::GRedis;

extern "C" {

void redisOutOfMemoryHandler(size_t allocation_size);
void initServerConfig(void);
void grpc_main(int argc, char **argv);
void *createClient(int fd);
void processInputBuffer(void *c);
void *querybuf(void *c, uint32_t len);
char *outbuf(void *c, uint32_t *len);
void settag(void *c, void *tag, void *callback);
}

void addReplyGrpcCallback(void *tag, const char *s, size_t len);

class ServerImpl final {
 public:
  ~ServerImpl() {
    server_->Shutdown();
    cq_->Shutdown();
  }

  // There is no shutdown handling in this code.
  void Run() {
    std::string server_address("0.0.0.0:50051");

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Proceed to the server's main loop.
    HandleRpcs();
  }

  void addReplyGrpc(void *tag, const char *s, size_t len) {
    static_cast<CallData*>(tag)->SendReply(s,len);
  }

 private:
  class CallData {
   public:
    CallData(GRedis::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      // Invoke the serving logic right away.
      Proceed();
    }

    void SendReply(const char *s, size_t len) {
      if (status_ == GETREPLY) {
        reply_.set_message("Blehh");
        responder_.Finish(reply_, Status::OK, this);
        status_ = FINISH;
        delete this;
      } else {
        std::cout << "Wrong state! " << status_ << std::endl;
      }
    }

    void Proceed() {
      if (status_ == CREATE) {
        // Make this instance progress to the SENDREQ state.
        status_ = SENDREQ;

        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestSet(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
      } else if (status_ == SENDREQ) {
        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        status_ = GETREPLY;
        new CallData(service_, cq_);

        void *c = createClient(0);
        settag(c, this, (void *)addReplyGrpcCallback);
        const char *cmd = "PING\r\n";
        uint32_t len = strlen(cmd)+1;
        void *dst = querybuf(c,len);
        memcpy(dst, cmd, len);
        processInputBuffer(c);
      } else if (status_ == FINISH) {
        assert(status_ == FINISH);
        // Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
      }
    }

   private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    GRedis::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    Req request_;
    // What we send back to the client.
    Reply reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<Reply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, SENDREQ, GETREPLY, FINISH };
    CallStatus status_;  // The current serving state.
  };

  // This can be run in multiple threads if needed.
  void HandleRpcs() {
    // Spawn a new CallData instance to serve new clients.
    new CallData(&service_, cq_.get());
    void* tag;  // uniquely identifies a request.
    bool ok;
    while (true) {
      // Block waiting to read the next event from the completion queue. The
      // event is uniquely identified by its tag, which in this case is the
      // memory address of a CallData instance.
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or cq_ is shutting down.
      assert(cq_->Next(&tag, &ok));
      assert(ok);
      static_cast<CallData*>(tag)->Proceed();
    }
  }

  std::unique_ptr<ServerCompletionQueue> cq_;
  GRedis::AsyncService service_;
  std::unique_ptr<Server> server_;
};

// Logic and data behind the server's behavior.
class GRedisServiceImpl final : public GRedis::Service {
  Status Set(ServerContext* context, const Req* request,
                  Reply* reply) override {
    void *c;
    if ((c = createClient(0)) == NULL) {
      reply->set_message("Error: Could not create client");
    } else {
      // ADD CODE TO DO SET COMMAND
      const char *cmd = "PING\r\n";
      uint32_t len = strlen(cmd)+1;
      void *dst = querybuf(c,len);
      memcpy(dst, cmd, len);
      processInputBuffer(c);
      char *res = outbuf(c, &len);
      printf("res = %s\n", res);
      reply->set_message(res);
    }
    return Status::OK;
  }
};

ServerImpl g_server;

void addReplyGrpcCallback(void *tag, const char *s, size_t len) {
  g_server.addReplyGrpc(tag, s, len);
}

int main(int argc, char** argv) {
  grpc_main(argc, argv);
  g_server.Run();
  return 0;
}
