#pragma once

#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include "vision.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class SerialClient {
   private:
    std::unique_ptr<SerialService::Stub> stub_;

   public:
    SerialClient(std::string addr)
        : stub_(SerialService::NewStub(
              grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()))) {}

    
}