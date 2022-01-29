#pragma once

#include <grpcpp/grpcpp.h>
#include "vision.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;


class VisionClient {
     private:
        std::unique_ptr<VisionService::Stub> stub_;
    public:
        VisionClient(std::string addr) : 
            stub_(
                VisionService::NewStub(
                    grpc::CreateChannel(
                        addr, 
                        grpc::InsecureChannelCredentials()
                    )
                )
            ) {}

        void SendFrame(std::string frame, int width, int height, int channels) {
            Request request;
            request.set_image(frame);
            request.set_width(width);
            request.set_height(height);
            request.set_channels(channels);

            // Context for the client. It could be used to convey extra information to
            // the server and/or tweak certain RPC behaviors.
            ClientContext context;

            // stub_->SendFrame()
        }
};