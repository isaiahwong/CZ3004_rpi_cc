import os
import cv2
import numpy as np
import importlib.util


class TF:
    def __init__(self,
                 modeldir="model",
                 graph="detect.tflite",
                 labelmap="labelmap.txt",
                 threshold=0.5,
                 ):
        self.modeldir = modeldir
        self.labelmap = labelmap
        self.graph = graph
        self.threshold = threshold
        self.init()

    def init(self):
        pkg = importlib.util.find_spec('tflite_runtime')
        if pkg:
            from tflite_runtime.interpreter import Interpreter
        else:
            from tensorflow.lite.python.interpreter import Interpreter

        CWD_PATH = os.getcwd()

        # Path to .tflite file, which contains the model that is used for object detection
        PATH_TO_MODEL = os.path.join(CWD_PATH, self.modeldir, self.graph)

        # Path to label map file
        PATH_TO_LABELS = os.path.join(CWD_PATH, self.modeldir, self.labelmap)

        # Load the label map
        with open(PATH_TO_LABELS, 'r') as f:
            self.labels = [line.strip() for line in f.readlines()]

        # Have to do a weird fix for label map if using the COCO "starter model" from
        # https://www.tensorflow.org/lite/models/object_detection/overview
        # First label is '???', which has to be removed.
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
        input_data = np.expand_dims(frame, axis=0)

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

            frame = self.draw(frame=frame, scores=scores,
                              boxes=boxes, classes=classes)
            # Resize back to camera resolution
            frame = cv2.resize(frame, (imageWidth, imageHeight))
            return frame
        except Exception as e:
            print(e)
            return

    def draw(self, frame=None, scores=[], boxes=[[]], classes=[]):
        # Loop over all detections and draw detection box if confidence is above minimum threshold
        for i in range(len(scores)):
            if ((scores[i] > self.threshold) and (scores[i] <= 1.0)):

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
                object_name = int(self.labels[int(classes[i])])

                self.yminavg[object_name-10] += ymin
                self.xminavg[object_name-10] += xmin
                self.ymaxavg[object_name-10] += ymax
                self.xmaxavg[object_name-10] += xmax
                self.average[object_name-10] += 1

                # Look up object name from "labels" array using class index
                # Example: 'person: 72%'
                label = '%s: %d%%' % (object_name, int(scores[i]*100))

                # Get font size
                labelSize, baseLine = cv2.getTextSize(
                    label, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)

                # Make sure not to draw label too close to top of window
                label_ymin = max(ymin, labelSize[1] + 10)
                # Draw white box to put label text in
                cv2.rectangle(frame, (xmin, label_ymin-labelSize[1]-10), (
                    xmin+labelSize[0], label_ymin+baseLine-10), (255, 255, 255), cv2.FILLED)

                cv2.putText(frame, label, (xmin, label_ymin-7),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 2)  # Draw label text

        return frame
