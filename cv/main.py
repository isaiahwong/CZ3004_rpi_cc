
import grpc
import numpy as np
import os 

import cv2
from tfmodel import TF, Result
from yolov5model import YoloV5

from concurrent import futures

from vision_pb2 import *
from vision_pb2_grpc import *


def fast_reshape(byteStr, width, height, channels):
    """
    Using JIT for faster numpy reshape
    """
    frame = np.frombuffer(byteStr, dtype=np.uint8)
    return np.reshape(frame, (height, width, channels))


class ImageServer(VisionServiceServicer):
    def __init__(self):
        self.count = 0
        # self.model = YoloV5()
        self.model = TF()
        self.pwd = os.path.dirname(__file__)

    def filterImages(self, results):
        result = Result("-1", -1)
        if results == None and len(results) == 0:
            return result
        for r in results:
            # We override label 10 - bullseye
            if (result.label == "10" or result.label == "-1") and r.label != result.label:
                result = r
                continue

            # do not override existing if the current is 10 - bullseye
            if (r.label == "10" and result.label != "10"):
                continue

            if r.confidence > result.confidence:
                result = r

        return result

    def SendFrame(self, req, ctx):
        # Reshape pixels back to its dimensions
        frame = fast_reshape(req.image, req.width, req.height, req.channels)
        # label = self.model.predict(frame, req.width)
        frame, results = self.model.predict(frame, req.width, req.height)
        result = self.filterImages(results)

        cv2.imwrite('{}/out/image-{}.jpg'.format(self.pwd, result.label), frame)
        status = 0 if str(result.label) == '-1' else 1
        print("ImageId: {}  Distance: {}".format(result.label, result.distance))
        return VisionResponse(imageid=str(result.label), status=status, distance=result.distance)


def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    add_VisionServiceServicer_to_server(ImageServer(), server)
    server.add_insecure_port('[::]:50051')
    print("Starting CV")
    server.start()
    server.wait_for_termination()


if __name__ == "__main__":
    main()
