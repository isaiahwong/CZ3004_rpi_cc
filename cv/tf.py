import cv2
import numpy as np
from tfmodel import TF

vid = cv2.VideoCapture(0)
vid.set(3, 1280)
vid.set(4, 720)
model = TF()

while(True):
    _, frame = vid.read()

    try:
        f, results = model.predict(frame, 640, 480)
        cv2.imshow('frame', f)
    except Exception as e:
        pass

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# After the loop release the cap object
vid.release()
# Destroy all the windows
cv2.destroyAllWindows()
