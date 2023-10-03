import numpy as np
import cv2

cap=cv2.VideoCapture(1)  #0代表树莓派上自带的摄像头，1代表USB摄像头


#查看采集图像的参数
print(cap.get(3))
print(cap.get(4))
print(cap.get(5))

while(True):
	ret,color_frame=cap.read()
	cv2.imshow('color_frame',color_frame)  #展示每一帧
	if cv2.waitKey(1)&0xff==ord('q'): #按Q键退出，可以改成任意键
		break
cap.release()
cv2.destroyAllWindows()
