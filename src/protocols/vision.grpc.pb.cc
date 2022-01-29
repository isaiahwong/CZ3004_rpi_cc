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

static const char* Vision_method_names[] = {
  "/Vision/FrameStream",
};

std::unique_ptr< Vision::Stub> Vision::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Vision::Stub> stub(new Vision::Stub(channel, options));
  return stub;
}

Vision::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_FrameStream_(Vision_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::CLIENT_STREAMING, channel)
  {}

::grpc::ClientWriter< ::Request>* Vision::Stub::FrameStreamRaw(::grpc::ClientContext* context, ::Response* response) {
  return ::grpc::internal::ClientWriterFactory< ::Request>::Create(channel_.get(), rpcmethod_FrameStream_, context, response);
}

void Vision::Stub::async::FrameStream(::grpc::ClientContext* context, ::Response* response, ::grpc::ClientWriteReactor< ::Request>* reactor) {
  ::grpc::internal::ClientCallbackWriterFactory< ::Request>::Create(stub_->channel_.get(), stub_->rpcmethod_FrameStream_, context, response, reactor);
}

::grpc::ClientAsyncWriter< ::Request>* Vision::Stub::AsyncFrameStreamRaw(::grpc::ClientContext* context, ::Response* response, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::Request>::Create(channel_.get(), cq, rpcmethod_FrameStream_, context, response, true, tag);
}

::grpc::ClientAsyncWriter< ::Request>* Vision::Stub::PrepareAsyncFrameStreamRaw(::grpc::ClientContext* context, ::Response* response, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::Request>::Create(channel_.get(), cq, rpcmethod_FrameStream_, context, response, false, nullptr);
}

Vision::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Vision_method_names[0],
      ::grpc::internal::RpcMethod::CLIENT_STREAMING,
      new ::grpc::internal::ClientStreamingHandler< Vision::Service, ::Request, ::Response>(
          [](Vision::Service* service,
             ::grpc::ServerContext* ctx,
             ::grpc::ServerReader<::Request>* reader,
             ::Response* resp) {
               return service->FrameStream(ctx, reader, resp);
             }, this)));
}

Vision::Service::~Service() {
}

::grpc::Status Vision::Service::FrameStream(::grpc::ServerContext* context, ::grpc::ServerReader< ::Request>* reader, ::Response* response) {
  (void) context;
  (void) reader;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


