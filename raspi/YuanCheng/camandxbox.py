import socket
import time
import cv2
import numpy
import pygame

def XboxInit():
	# Initialize the joysticks
    pygame.joystick.init()

def XboxJoystick():
    j=1
    global xboxjoystickinput
    axis = [0]*6
    while 1:
        joystick_count = pygame.joystick.get_count()
        for i in range(joystick_count):
            joystick = pygame.joystick.Joystick(i)
            joystick.init()
    
            # Get the name from the OS for the controller/joystick
            name = joystick.get_name()
        
            # Usually axis run in pairs, up/down for one, and left/right for
            # the other.
            axes = joystick.get_numaxes()
        
            for i in range( axes ):
                axis[i] = joystick.get_axis( i )
                #textPrint.print(screen, "Axis {} value: {:>6.3f}".format(i, axis) )
            #textPrint.unindent()
            
            buttons = joystick.get_numbuttons()
            #textPrint.print(screen, "Number of buttons: {}".format(buttons) )
            #textPrint.indent()
 
            for i in range( buttons ):
                button = joystick.get_button( i )
                #textPrint.print(screen, "Button {:>2} value: {}".format(i,button) )
            #textPrint.unindent()
            
            # Hat switch. All or nothing for direction, not like joysticks.
            # Value comes back in an array.
            hats = joystick.get_numhats()
            #textPrint.print(screen, "Number of hats: {}".format(hats) )
            #textPrint.indent()
 
            for i in range( hats ):
                hat = joystick.get_hat( i )
                #textPrint.print(screen, "Hat {} value: {}".format(i, str(hat)) )
            #textPrint.unindent()
        j=j+1
        if j == 3:
             break
    xboxjoystickinput=axis
    print(xboxjoystickinput)
        #textPrint.unindent()

def xboxinputprocess():
    global xboxjoystickinput
    for i in range(0,5):
        xboxjoystickinput[i]=int(xboxjoystickinput[i])
    

def ReceiveVideoInit():
    global s
    global conn
    global addr
	#IP地址'0.0.0.0'为等待客户端连接
    address = ('0.0.0.0', 8002)
	#建立socket对象，参数意义见https://blog.csdn.net/rebelqsp/article/details/22109925
	#socket.AF_INET：服务器之间网络通信 
	#socket.SOCK_STREAM：流式socket , for TCP
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#将套接字绑定到地址, 在AF_INET下,以元组（host,port）的形式表示地址.
    s.bind(address)
	#开始监听TCP传入连接。参数指定在拒绝连接之前，操作系统可以挂起的最大连接数量。该值至少为1，大部分应用程序设为5就可以了。
    s.listen(5)
    conn, addr = s.accept()
    print('connect from:'+str(addr))

def recvall(sock, count):
	buf = b''#buf是一个byte类型
	while count:
		#接受TCP套接字的数据。数据以字符串形式返回，count指定要接收的最大数据量.
		newbuf = sock.recv(count)
		if not newbuf: return None
		buf += newbuf
		count -= len(newbuf)
	return buf

def ReceiveVideo():
    global s
    global conn
    global addr
    global xboxjoystickinput
	#接受TCP连接并返回（conn,address）,其中conn是新的套接字对象，可以用来接收和发送数据。addr是连接客户端的地址。
	#没有连接则等待有连接
    

    start = time.time()#用于计算帧率信息
    length = recvall(conn,16)#获得图片文件的长度,16代表获取长度
    stringData = recvall(conn, int(length))#根据获得的文件长度，获取图片文件
    data = numpy.frombuffer(stringData, numpy.uint8)#将获取到的字符流数据转换成1维数组
    decimg=cv2.imdecode(data,cv2.IMREAD_COLOR)#将数组解码成图像
    cv2.imshow('SERVER',decimg)#显示图像
		
		#进行下一步处理
		#。
		#。
		#。
 
        #将帧率信息回传，主要目的是测试可以双向通信
    end = time.time()
    seconds = end - start
    fps  = 1/seconds;
    #data4send=int(fps)
    data4send=xboxjoystickinput
    #print(xboxjoystickinput)
    conn.send(bytes(str(data4send),encoding='utf-8'))
    k = cv2.waitKey(10)&0xff
    if k == 27:
        s.close()
        cv2.destroyAllWindows()
      
    
 
if __name__ == '__main__':
    XboxInit()
    ReceiveVideoInit()
    while 1:
        XboxJoystick()
        xboxinputprocess()
        ReceiveVideo()
        print('---------')
    
