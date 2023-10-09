from braccio import Braccio
import cv2
import sys, time

if len(sys.argv) < 3:
    raise ValueError("Missing parameters: <arduino_port> <webcam_port>")

arm = Braccio(sys.argv[1])
vid = cv2.VideoCapture(sys.argv[2])

(ret, frame) = vid.read()
cv2.imshow('frame', frame)
cv2.waitKey(1)
time.sleep(5)

arm.home()
arm.shutdown()
