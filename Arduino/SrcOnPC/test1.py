import socket
import cv2
import numpy
import time
import serial

def somethingelse():
    global receive
    receive='[1,3,5,7,9]'

def ArduinoInit():
	global ser
	Port = "COM7"  # 串口
	baudRate = 9600  # 波特率
	ser = serial.Serial(Port, baudRate, timeout=1)

def ArduinoCtrl():
	global ser
	global receive
	presend=str(receive)
	presend1=''.join(filter(lambda i: i in [','] or i.isalnum(),presend))
	presend2=list(presend1.split(","))
	#print(type(presend1))
	#print(presend1)
	send = presend1  # 发送给arduino的数据
	ser.write(send.encode())
	arduinostr = ser.readline().decode()  # 获取arduino发送的数据
	if(arduinostr != ""):
		print(arduinostr)
		print('--------')
		print(send)
		
if __name__ == '__main__':
	ArduinoInit()
	while 1:
		somethingelse()
		ArduinoCtrl()