import socket
import cv2
import numpy
import time
import serial

def somethingelse():
    print(117)

def ArduinoInit():
	global ser
	Port = "/dev/ttyACM0"  # 串口
	baudRate = 9600  # 波特率
	ser = serial.Serial(Port, baudRate, timeout=1)

def ArduinoCtrl():
	global ser
	global receive
	presend=str(receive,encoding='utf-8')
	presend1=''.join(filter(lambda i: i in [','] or i.isalnum(),presend))
	presend2=list(presend1.split(","))
	print(type(presend2))
	print(presend2[3])
	send = presend2[3]  # 发送给arduino的数据
	ser.write(send.encode())
	time.sleep(0.05)
	arduinostr = ser.readline().decode()  # 获取arduino发送的数据
	#time.sleep(0.05)
	if(arduinostr != ""):
		print(arduinostr)
		print('--------')
		print(send)



def SockEstablish():
	#建立sock连接
	#address要连接的服务器IP地址和端口号
	global sock
	address = ('192.168.137.1', 8002)
	try:
		#建立socket对象，参数意义见https://blog.csdn.net/rebelqsp/article/details/22109925
		#socket.AF_INET：服务器之间网络通信 
		#socket.SOCK_STREAM：流式socket , for TCP
		sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		#开启连接
		sock.connect(address)
	except socket.error as msg:
		print(msg)
		sys.exit(1)

def SendVideoInit():
	#建立图像读取对象
	global capture
	capture = cv2.VideoCapture(0)
	#读取一帧图像，读取成功:ret=1 frame=读取到的一帧图像；读取失败:ret=0
	global ret
	global frame
	ret, frame = capture.read()
	#压缩参数，后面cv2.imencode将会用到，对于jpeg来说，15代表图像质量，越高代表图像质量越好为 0-100，默认95
	global encode_param
	encode_param=[int(cv2.IMWRITE_JPEG_QUALITY),15]

def SendVideo():
	global ret
	global frame
	global capture
	global receive
	if ret:
		#停止0.1S 防止发送过快服务的处理不过来，如果服务端的处理很多，那么应该加大这个值
		time.sleep(0.05)
		#cv2.imencode将图片格式转换(编码)成流数据，赋值到内存缓存中;主要用于图像数据格式的压缩，方便网络传输
		#'.jpg'表示将图片按照jpg格式编码。
		result, imgencode = cv2.imencode('.jpg', frame, encode_param)
		#建立矩阵
		data = numpy.array(imgencode)
		#将numpy矩阵转换成字符形式，以便在网络中传输
		stringData = data.tostring()

		#先发送要发送的数据的长度
		#ljust() 方法返回一个原字符串左对齐,并使用空格填充至指定长度的新字符串
		sock.send(str.encode(str(len(stringData)).ljust(16)));
		#发送数据
		sock.send(stringData);
		#读取服务器返回值
		receive = sock.recv(1024)
		if len(receive):print(str(receive,encoding='utf-8'))
		ArduinoCtrl()
		#读取下一帧图片
		ret, frame = capture.read()
		if cv2.waitKey(10) == 27:
			sock.close()
	
if __name__ == '__main__':
	ArduinoInit()
	SockEstablish()
	SendVideoInit()
	while 1:
		SendVideo()
		#ArduinoCtrl()
