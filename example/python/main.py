
import grpc
import numpy as np
import cv2
from model import Model

from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *


class ImageServer(VisionServiceServicer):
    def __init__(self):
        self.count = 0
        self.model = Model()

    def SendFrame(self, req, ctx):
        frame = np.array(list(req.image))
        frame = np.array(frame, dtype=np.uint8)
        # Reshape pixels back to its dimensions
        array = np.reshape(frame, (req.height, req.width, req.channels))

        cv2.imwrite('out/frame{}.jpg'.format(self.count),
                    self.model.predict(array, req.width, req.height, ))
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
