import time
import grpc
import numpy as np
import os
import glob

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
            # We override imageid 10 - bullseye
            if (result.imageid == "10" or result.imageid == "-1") and r.imageid != result.imageid:
                result = r
                continue

            # do not override existing if the current is 10 - bullseye
            if (r.imageid == "10" and result.imageid != "10"):
                continue

            if r.confidence > result.confidence:
                result = r

        return result

    def stitchImages(self):
        compileimage = cv2.imread('{}/compile.jpeg'.format(self.pwd))
        filenames = list(glob.glob('{}/out/competition/image*.jpg'.format(self.pwd)))
        filenames.sort()

        compileimage = cv2.resize(compileimage, (640, 480))

        img = [cv2.imread(file) for file in filenames]
        for i in range(9):
            try:
                dummy = img[i]
            except:
                img.append(compileimage)

        row1 = np.concatenate((img[0], img[1], img[2]), axis=1)
        row2 = np.concatenate((img[3], img[4], img[5]), axis=1)
        row3 = np.concatenate((img[6], img[7], compileimage), axis=1)
        imgTable = np.concatenate((row1, row2, row3), axis=0)
        try:
            table = cv2.resize(imgTable, (1920, 1080))
            cv2.imwrite('{}/out/competition/compiled.jpg'.format(self.pwd), table)
        except Exception as e:
            print(e)

    def SendFrame(self, req, ctx):
        # Reshape pixels back to its dimensions
        # cv2.imwrite('{}/out/train/image{}.jpg'.format(self.pwd, self.count), frame)
        # self.count += 1
        # return VisionResponse(
        #     imageid=str(1),
        #     status=1,
        #     name='',
        #     distance=1
        # )
        # imageid = self.model.predict(frame, req.width)
        frame = fast_reshape(req.image, req.width, req.height, req.channels)
        frame, results = self.model.predict(frame, req.width, req.height)
        result = self.filterImages(results)
        str_id = str(result.imageid)
        status = 0 if str_id == '-1' or str_id == '10' else 1
        if status == 0:
            cv2.imwrite('{}/out/failed/image-{}.jpg'.format(self.pwd,
                                                            self.count), frame)
        else:
            cv2.imwrite('{}/out/competition/image-{}-{}.jpg'.format(self.pwd,
                                                        result.name, self.count), frame)

        self.stitchImages()

        print("ImageId: {}, Name: {}, Distance: {}".format(
            result.imageid, result.name, result.distance))
        self.count += 1
        return VisionResponse(
            imageid=str(result.imageid),
            status=status,
            name=result.name,
            distance=result.distance
        )


def main():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    add_VisionServiceServicer_to_server(ImageServer(), server)
    server.add_insecure_port('[::]:50051')
    print("Starting CV")
    server.start()
    server.wait_for_termination()


if __name__ == "__main__":
    main()
