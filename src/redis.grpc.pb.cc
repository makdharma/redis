// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: redis.proto

#include "redis.pb.h"
#include "redis.grpc.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/channel_interface.h>
#include <grpc++/impl/codegen/client_unary_call.h>
#include <grpc++/impl/codegen/method_handler_impl.h>
#include <grpc++/impl/codegen/rpc_service_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/sync_stream.h>
namespace redis {

static const char* GRedis_method_names[] = {
  "/redis.GRedis/Set",
};

std::unique_ptr< GRedis::Stub> GRedis::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  std::unique_ptr< GRedis::Stub> stub(new GRedis::Stub(channel));
  return stub;
}

GRedis::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Set_(GRedis_method_names[0], ::grpc::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status GRedis::Stub::Set(::grpc::ClientContext* context, const ::redis::Req& request, ::redis::Reply* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_Set_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::redis::Reply>* GRedis::Stub::AsyncSetRaw(::grpc::ClientContext* context, const ::redis::Req& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::ClientAsyncResponseReader< ::redis::Reply>::Create(channel_.get(), cq, rpcmethod_Set_, context, request);
}

GRedis::Service::Service() {
  AddMethod(new ::grpc::RpcServiceMethod(
      GRedis_method_names[0],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< GRedis::Service, ::redis::Req, ::redis::Reply>(
          std::mem_fn(&GRedis::Service::Set), this)));
}

GRedis::Service::~Service() {
}

::grpc::Status GRedis::Service::Set(::grpc::ServerContext* context, const ::redis::Req* request, ::redis::Reply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace redis

