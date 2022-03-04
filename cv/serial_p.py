import grpc
import numpy as np
import os
import serial
import time
import serial

from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *


class SerialServer(SerialServiceServicer):
    def __init__(self):
        self.ser = serial.Serial(
            port="/dev/ttyr1",
            baudrate=115200,
            parity=serial.PARITY_EVEN,
        )

    def SendResponse(self, req, context):
        while True:
            x = self.ser.read()
            yield SendResponse(status=x)

    def SendMessage(self, req, ctx):
        print(req.message)
        self.ser.write(bytes(req.message, 'ascii'))
        return Empty()


def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    add_SerialServiceServicer_to_server(SerialServer(), server)
    server.add_insecure_port('[::]:50052')
    print("Starting Serial")
    server.start()
    server.wait_for_termination()
    server.ser.close()


if __name__ == "__main__":
    main()
