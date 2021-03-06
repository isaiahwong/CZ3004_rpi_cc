import torch
import time
import cv2


# Model
start = time.time()
model = torch.hub.load('./yolov5', 'custom',
                       path='./model/yolov5s_16b_100ep.pt', source='local')  # local repo
end = time.time()
print("Model load: {}".format(end - start))

# # Images
img = cv2.imread('./out/image---1.jpg')
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)


def infer(img):
    # Inference
    start = time.time()
    results = model(img, size=416)  # includes NMS
    end = time.time()
    print("Model infer: {}".format(end - start))
    results.save("./out")

for _ in range(1):
    infer(img)
# Results
# results.show()  # or .show()

# results = results.xyxy[0]  # img1 predictions (tensor)
