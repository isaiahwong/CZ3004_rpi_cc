import os
import cv2
import numpy as np
import importlib.util
import os


class Result:
    def __init__(self, imageid="", distance=0, name="", confidence=0):
        self.imageid = imageid
        self.distance = distance
        self.name = name
        self.confidence = confidence


class TF:
    def __init__(self,
                 modeldir="model",
                 graph="detectV2.tflite",
                 labelmap="labelmap.txt",
                 threshold=0.5,
                 ):
        self.modeldir = modeldir
        self.labelmap = labelmap
        self.graph = graph
        self.threshold = threshold
        self.init()

    def getImageName(self, id):
        return {
            "10": "Bullseye",
            "11": "One",
            "12": "Two",
            "13": "Three",
            "14": "Four",
            "15": "Five",
            "16": "Six",
            "17": "Seven",
            "18": "Eight",
            "19": "Nine",
            "20": "A",
            "21": "B",
            "22": "C",
            "23": "D",
            "24": "E",
            "25": "F",
            "26": "G",
            "27": "H",
            "28": "S",
            "29": "T",
            "30": "U",
            "31": "V",
            "32": "W",
            "33": "X",
            "34": "Y",
            "35": "Z",
            "36": "Up Arrow",
            "37": "Down Arrow",
            "38": "Right Arrow",
            "39": "Left Arrow",
            "40": "Stop",
        }[id]

    def init(self):
        pkg = importlib.util.find_spec('tflite_runtime')
        if pkg:
            from tflite_runtime.interpreter import Interpreter
        else:
            from tensorflow.lite.python.interpreter import Interpreter

        CWD_PATH = os.path.dirname(__file__)

        # Path to .tflite file, which contains the model that is used for object detection
        PATH_TO_MODEL = os.path.join(CWD_PATH, self.modeldir, self.graph)

        # Path to imageid map file
        PATH_TO_LABELS = os.path.join(CWD_PATH, self.modeldir, self.labelmap)

        # Load the imageid map
        with open(PATH_TO_LABELS, 'r') as f:
            self.labels = [line.strip() for line in f.readlines()]

        # Have to do a weird fix for imageid map if using the COCO "starter model" from
        # https://www.tensorflow.org/lite/models/object_detection/overview
        # First imageid is '???', which has to be removed.
        if self.labels[0] == '???':
            del(self.labels[0])

        self.interpreter = Interpreter(model_path=PATH_TO_MODEL)
        self.interpreter.allocate_tensors()

        # Get model details
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()
        self.modelHeight = self.input_details[0]['shape'][1]
        self.modelWidth = self.input_details[0]['shape'][2]

        self.floating_model = (self.input_details[0]['dtype'] == np.float32)

        self.input_mean = 127.5
        self.input_std = 127.5

        self.ymaxavg = [0]*31
        self.yminavg = [0]*31
        self.xminavg = [0]*31
        self.xmaxavg = [0]*31
        self.average = [0]*31
        self.output = [-1, 500, 500, 180]

    def predict(self, frame, imageWidth, imageHeight):
        # Resize to model's specification
        frame = cv2.resize(frame, (self.modelWidth, self.modelHeight))
        frame_RGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        input_data = np.expand_dims(frame_RGB, axis=0)

        if self.floating_model:
            input_data = (np.float32(input_data) -
                          self.input_mean) / self.input_std
        # Perform the actual detection by running the model with the image as input
        self.interpreter.set_tensor(self.input_details[0]['index'], input_data)
        self.interpreter.invoke()
        try:
            # Retrieve detection results
            # Bounding box coordinates of detected objects
            boxes = self.interpreter.get_tensor(
                self.output_details[1]['index'])[0]
            classes = self.interpreter.get_tensor(
                self.output_details[3]['index'])[0]
            # Confidence of detected objectz
            scores = self.interpreter.get_tensor(
                self.output_details[0]['index'])[0]

            frame, results = self.draw(frame=frame, scores=scores,
                                       boxes=boxes, classes=classes)
            # Resize back to camera resolution
            frame = cv2.resize(frame, (imageWidth, imageHeight))
            return frame, results
        except Exception as e:
            print("error: {}".format(e))
            return None, []

    def filterImagePathStrategy(self, scores=[], boxes=[], classes=[]):
        """
        We only accept one image per box for image rec
        """
        bestScore = None

        for i in range(len(scores)):
            if ((scores[i] < self.threshold)): 
                continue

            imageid = int(self.labels[int(classes[i])])
            confidence = int(scores[i]*100)

            if imageid == 10:
                continue
            
            if bestScore == None or  confidence > bestScore[1]:
                bestScore = (i, confidence)

        if bestScore == None:
            return [], [], []

        i = bestScore[0]
        return [scores[i]], [boxes[i]], [classes[i]]

    def filterFastestPathStrategy(self, scores=[], boxes=[], classes=[]):
        newScores, newBoxes, newClasses = [], [], []
        for i in range(len(scores)):
            if ((scores[i] < self.threshold)): 
                continue

            imageid = int(self.labels[int(classes[i])])
            confidence = int(scores[i]*100)
            # We only want bullseye
            if imageid != 10:
                continue

            print(scores[i], self.threshold)
            
            newScores.append(scores[i])
            newBoxes.append(boxes[i])
            newClasses.append(classes[i])
            
            
        return newScores, newBoxes, new

    def draw(self, frame=None, scores=[], boxes=[], classes=[]):
        results = []

        scores, boxes, classes = self.filterFastestPathStrategy(scores=scores, boxes=boxes, classes=classes)
        # Loop over all detections and draw detection box if confidence is above minimum threshold
        for i in range(len(scores)):
            # Get bounding box coordinates and draw box
            # Interpreter can return coordinates that are outside of image dimensions, need to force them to be within image using max() and min()
            ymin = int(max(1, (boxes[i][0] * self.modelHeight)))
            xmin = int(max(1, (boxes[i][1] * self.modelWidth)))
            ymax = int(
                min(self.modelHeight, (boxes[i][2] * self.modelHeight)))
            xmax = int(
                min(self.modelWidth, (boxes[i][3] * self.modelWidth)))

            cv2.rectangle(frame, (xmin, ymin),
                            (xmax, ymax), (10, 255, 0), 2)
            imageid = int(self.labels[int(classes[i])])

            self.yminavg[imageid-10] += ymin
            self.xminavg[imageid-10] += xmin
            self.ymaxavg[imageid-10] += ymax
            self.xmaxavg[imageid-10] += xmax
            self.average[imageid-10] += 1

            # Look up object name from "labels" array using class index
            # Example: 'person: 72%'
            confidence = int(scores[i]*100)
            name = self.getImageName(str(imageid))

            details = '{}, {}%'.format(imageid, confidence)

            # Get font size
            labelSize, baseLine = cv2.getTextSize(
                details, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)

            # Make sure not to draw imageid too close to top of window
            label_ymin = max(ymin, labelSize[1] + 10)
            # Draw white box to put imageid text in
            cv2.rectangle(frame, (xmin, label_ymin-labelSize[1]-10), (
                xmin+labelSize[0], label_ymin+baseLine-10), (255, 255, 255), cv2.FILLED)
            cv2.putText(frame, details, (xmin, label_ymin-7),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 2)  # Draw imageid text

            # Draw white box to put imageid text in
            cv2.rectangle(frame, (xmin, label_ymin-labelSize[1]-40), (
                xmin+labelSize[0], label_ymin+baseLine-40), (255, 255, 255), cv2.FILLED)
            cv2.putText(frame, name, (xmin, label_ymin-40),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 2)  # Draw imageid text=

            middlePoint = self.modelWidth / 2
            currentMidPoint = xmin + ((xmax - xmin) / 2)

            # +ve implies need to move left
            # -ve implies need to move right
            distanceAwayFromMidPoint = middlePoint - currentMidPoint
     
            results.append(Result(
                imageid=str(imageid),
                name=name,
                distance=int(distanceAwayFromMidPoint),
                confidence=confidence
            ))

        return frame, results
