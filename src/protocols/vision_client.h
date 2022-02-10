#pragma once

#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include "vision.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class VisionClient {
   private:
    std::unique_ptr<VisionService::Stub> stub_;

   public:
    VisionClient(std::string addr)
        : stub_(VisionService::NewStub(
              grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()))) {}

    VisionResponse* SendFrame(std::string frame, int width, int height,
                              int channels) {
        VisionRequest VisionRequest;
        VisionResponse* res = new VisionResponse();

        VisionRequest.set_image(frame);
        VisionRequest.set_width(width);
        VisionRequest.set_height(height);
        VisionRequest.set_channels(channels);

        // Context for the client. It could be used to convey extra information
        // to the server and/or tweak certain RPC behaviors.
        ClientContext context;

        Status status = stub_->SendFrame(&context, VisionRequest, res);

        if (!status.ok()) {
            throw fmt::format("{} : {}", status.error_code(),
                              status.error_message());
        }

        return res;
    }
};