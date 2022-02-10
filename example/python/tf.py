import cv2
import numpy as np
from tfmodel import TF

vid = cv2.VideoCapture(0)
model = TF()

while(True):
    ret, frame = vid.read()


    f = model.predict(frame, 640, 480)
    cv2.imshow('frame', f)
      
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
  
# After the loop release the cap object
vid.release()
# Destroy all the windows
cv2.destroyAllWindows()

