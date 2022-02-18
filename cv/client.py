import grpc
import numpy as np
import os
import serial
import time

from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *

def main():
    with grpc.insecure_channel('localhost:50052') as channel:
        stub = SerialServiceStub(channel)
        
        stub.SendMessage(SerialMessage(message='FF010'))


if __name__ == "__main__":
    main()
