import numpy as np
import cv2
import sys
import time
import argparse
import os
import progressbar


numofframe = 1000
webcamID = int(input("Input your WebcamID: "))
print("type 1~3 to Select resulution")
print("1. 640 x 480")
print("2. 800 x 600")
print("3. 1280 x 720")
resolution = int(input(""))

if not os.path.exists("webcam_test"):
    os.makedirs("webcam_test")
save_filename = "webcam_test" + "\\" +"Test" + ".avi"

# The duration in seconds of the video captured
cap = cv2.VideoCapture(webcamID ,cv2.CAP_DSHOW)

if resolution==1:
      w = 640
      h = 480

elif resolution==2:
      w = 800
      h = 600

elif resolution==3:
      w = 1280
      h = 720

else:
      w = 640
      h = 480
      
cap.set(3,w)
cap.set(4,h)

fourcc = cv2.VideoWriter_fourcc(*'XVID')
out = cv2.VideoWriter(save_filename, fourcc, 60, (w,h))

timediff = []
bar = progressbar.ProgressBar(maxval=numofframe, \
    widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])
bar.start()

for i in range(0,numofframe):
    adiff = float(time.time())
    timediff += [adiff]
    ret, frame = cap.read()
    if ret==True:
        # frame = cv2.flip(frame,0)
        out.write(frame)
        # cv2.imshow('frame',frame)
    else:
        break

    #Progess = round(i/(numofframe/100),2)
    #print(Progess,"%",end='\r')
    bar.update(i+1)

#print(timediff)
bar.finish()


diff = []
for i in range(1,numofframe):
    eachdiff = timediff[i]-timediff[i-1]
    diff += [eachdiff]

#print(diff)

t =  sum(diff)/len(diff)

#print(t)

FPS = round(1/t,1)
print("Average FPS for webcam ID ",webcamID,", Resolution",w,"x",h,"is :",FPS)

cap.release()
out.release()
cv2.destroyAllWindows()

input("Done")


