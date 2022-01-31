
import grpc
import numpy as np
from numba import jit

import cv2
from model import Model

from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *


@jit(nopython=True)
def fast_reshape(byteStr, width, height, channels):
    """
    Using JIT for faster numpy reshape
    """
    frame = np.frombuffer(byteStr, dtype=np.uint8)
    return np.reshape(frame, (height, width, channels))


class ImageServer(VisionServiceServicer):
    def __init__(self):
        self.count = 0
        self.model = Model()

    def SendFrame(self, req, ctx):
        print("Receive")
        # Reshape pixels back to its dimensions
        frame = fast_reshape(req.image, req.width, req.height, req.channels)
        cv2.imwrite('out/frame{}.jpg'.format(self.count),
                    self.model.predict(frame, req.width, req.height, ))
        self.count += 1
        return Response(count=self.count)


def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    add_VisionServiceServicer_to_server(ImageServer(), server)
    server.add_insecure_port('[::]:50051')
    print("Starting CV")
    server.start()
    server.wait_for_termination()


if __name__ == "__main__":
    main()
