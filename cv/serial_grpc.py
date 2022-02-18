import grpc
import numpy as np
import os
import serial
import time

from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *


class SerialServer(SerialServiceServicer):
    def __init__(self):
            self.ser = serial.Serial(
                port="/dev/ttyUSB1",   
                baudrate=115200,        
                parity=serial.PARITY_EVEN,  
            )
            for _ in range(5):
                time.sleep(5)
                self.ser.write(bytes('FF010', 'ascii'))

    def SendResponse(self, req, context):
        print("Reading")
        while True:
            x = self.ser.readline()
            m = x.decode('ascii')
            yield SerialResponse(status=m)

    def SendMessage(self, req, ctx):
        print(req.message);
        self.ser.write(bytes(req.message, 'ascii'))
        return Empty()


def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    add_SerialServiceServicer_to_server(SerialServer(), server)
    server.add_insecure_port('[::]:50052')
    print("Starting Serials")
    server.start()
    server.wait_for_termination()
    server.ser.close()


if __name__ == "__main__":
    main()
