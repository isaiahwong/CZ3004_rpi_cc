import time
import grpc
import cv2
import numpy as np
from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *


class ImageServer(VisionServiceServicer):
    def __init__(self):
        self.count = 0

    def SendFrame(self, req, ctx):
        frame = np.array(list(req.image))
        frame = np.array(frame, dtype=np.uint8)
        array = np.reshape(frame, (req.height, req.width, req.channels))
        # cv2.imwrite('out/frame{}.jpg'.format(self.count), array)
        self.count += 1
        return Response(count=self.count)


def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    add_VisionServiceServicer_to_server(ImageServer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


if __name__ == "__main__":
    main()
