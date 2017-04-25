#include <grpc++/grpc++.h>

#include "redis.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using redis::Req;
using redis::Reply;
using redis::GRedis;

class GRedisClient {
 public:
  GRedisClient(std::shared_ptr<Channel> channel)
      : stub_(GRedis::NewStub(channel)) {}

  std::string Set() {
    Req request;
    request.set_key("Key1");
    request.set_value("Value1");
    Reply reply;
    ClientContext context;
    Status status = stub_->Set(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<GRedis::Stub> stub_;
};

int main(int argc, char** argv) {
  GRedisClient gredis(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string reply = gredis.Set();
  std::cout << "Gredis received: " << reply << std::endl;
  return 0;
}
