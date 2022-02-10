import torch
import time
import cv2

class YoloV5:
    def __init__(self,
                modeldir="./model/yolov5s_16b_100ep.pt",
                yolov5="./yolov5",
                conf=0.7,
                ):
        self.modeldir = modeldir
        self.yolov5 = yolov5
        self.conf = conf
        self.init() 

    def init(self):
        self.model = torch.hub.load(
            self.yolov5, 
            'custom',
            path=self.modeldir, 
            source='local'
        )
        self.model.conf = self.conf 

    def predict(self, frame, imageWidth):
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = self.model(frame, size=416)
        results.save("./out")
        if len(results.pandas().xyxy[0]):
            return results.pandas().xyxy[0].iloc[0]['name']
        return '-1'
