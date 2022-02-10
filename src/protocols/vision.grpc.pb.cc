// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: vision.proto

#include "vision.pb.h"
#include "vision.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>

static const char* VisionService_method_names[] = {
  "/VisionService/SendFrame",
};

std::unique_ptr< VisionService::Stub> VisionService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< VisionService::Stub> stub(new VisionService::Stub(channel, options));
  return stub;
}

VisionService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_SendFrame_(VisionService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status VisionService::Stub::SendFrame(::grpc::ClientContext* context, const ::VisionRequest& request, ::VisionResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::VisionRequest, ::VisionResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SendFrame_, context, request, response);
}

void VisionService::Stub::async::SendFrame(::grpc::ClientContext* context, const ::VisionRequest* request, ::VisionResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::VisionRequest, ::VisionResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendFrame_, context, request, response, std::move(f));
}

void VisionService::Stub::async::SendFrame(::grpc::ClientContext* context, const ::VisionRequest* request, ::VisionResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendFrame_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::VisionResponse>* VisionService::Stub::PrepareAsyncSendFrameRaw(::grpc::ClientContext* context, const ::VisionRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::VisionResponse, ::VisionRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SendFrame_, context, request);
}

::grpc::ClientAsyncResponseReader< ::VisionResponse>* VisionService::Stub::AsyncSendFrameRaw(::grpc::ClientContext* context, const ::VisionRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSendFrameRaw(context, request, cq);
  result->StartCall();
  return result;
}

VisionService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      VisionService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< VisionService::Service, ::VisionRequest, ::VisionResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](VisionService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::VisionRequest* req,
             ::VisionResponse* resp) {
               return service->SendFrame(ctx, req, resp);
             }, this)));
}

VisionService::Service::~Service() {
}

::grpc::Status VisionService::Service::SendFrame(::grpc::ServerContext* context, const ::VisionRequest* request, ::VisionResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


