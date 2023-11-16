import socket
import time
import cv2
import numpy
import pygame
from imutils.perspective import four_point_transform
from imutils import contours
import imutils
import cv2

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
    for i in range(0,6):
        xboxjoystickinput[i]=int(90*(xboxjoystickinput[i]+1))
    

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
      
def shibieInit():
     # 定义Python字典，代表0~9数字的七段数组
    global DIGITS_LOOKUP
    DIGITS_LOOKUP = {
        (1, 1, 1, 0, 1, 1, 1): 0,
        (0, 0, 1, 0, 0, 1, 0): 1,
        (1, 0, 1, 1, 1, 0, 1): 2,
        (1, 0, 1, 1, 0, 1, 1): 3,
        (0, 1, 1, 1, 0, 1, 0): 4,
        (1, 1, 0, 1, 0, 1, 1): 5,
        (1, 1, 0, 1, 1, 1, 1): 6,
        (1, 0, 1, 0, 0, 1, 0): 7,
        (1, 1, 1, 1, 1, 1, 1): 8,
        (1, 1, 1, 1, 0, 1, 1): 9
    }

def shibie(image):
    global DIGITS_LOOKUP
     # 1. LCD边缘可见
    # 预处理步骤：保持宽高比的缩放，转换灰度，高斯模糊以减少高频噪音，Canny边缘检测器计算边缘图
    image = imutils.resize(image, height=500)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    edged = cv2.Canny(blurred, 50, 200, 255)
 
    # 2. 提取LCD本身
    # 在边缘图中寻找轮廓，并按面积大小倒序排列
    cnts = cv2.findContours(edged.copy(), cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
    displayCnt = None
 
    # 遍历轮廓
    for c in cnts:
        # 应用轮廓近似
        peri = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.02 * peri, True)
 
        # 如果边缘有4个顶点（vertices），则找到了LCD并展示
        if len(approx) == 4:
            displayCnt = approx
            break
 
    # 获得四个顶点后，可以通过四点透视变换提取LCD
    # 提取恒温器，应用透视变换获得从上至下鸟瞰LCD图
    warped = four_point_transform(gray, displayCnt.reshape(4, 2))
    output = four_point_transform(image, displayCnt.reshape(4, 2))
 
    # 3. 从LCD提取数字
    # 阈值化透视变换后的图以在较亮的背景（即LCD显示屏的背景）上显示出较暗的区域（即数字）；
    # 应用一系列形态学运算来清理阈值图像
    thresh = cv2.threshold(warped, 0, 255,
                           cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (1, 5))
    thresh = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
 
    # 应用轮廓过滤，寻找实际的数字
    # 在阈值图像上寻找轮廓，并初始化数字轮廓lists
    cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    digitCnts = []
 
    # 遍历数字候选区域
    for c in cnts:
        # 计算轮廓的边界框
        (x, y, w, h) = cv2.boundingRect(c)
 
        # 确定适当的宽度和高度约束需要几轮反复试验。建议循环遍历每个轮廓，分别绘制它们，并检查其尺寸。执行此过程可确保找到数字轮廓属性的共同点。
        # 如果轮廓足够大，则它是一个数字
        if w >= 15 and (h >= 30 and h <= 40):
            digitCnts.append(c)

    # 4. 实际识别每个数字
    # 从左到右排序轮廓，并初始化实际的数字列表
    digitCnts = contours.sort_contours(digitCnts,
                                       method="left-to-right")[0]
    digits = []
 
    # 遍历每一个数字
    for c in digitCnts:
        # 提取数字ROI区域
        (x, y, w, h) = cv2.boundingRect(c)
        roi = thresh[y:y + h, x:x + w]
 
        # 计算每一个七段部分的宽度、高度
        (roiH, roiW) = roi.shape
        (dW, dH) = (int(roiW * 0.25), int(roiH * 0.15))
        dHC = int(roiH * 0.05)
 
        # 定义七段的集合
        # 根据ROI尺寸计算出每个段的近似宽度和高度。
        segments = [
            ((0, 0), (w, dH)),  # top
            ((0, 0), (dW, h // 2)),  # top-left
            ((w - dW, 0), (w, h // 2)),  # top-right
            ((0, (h // 2) - dHC), (w, (h // 2) + dHC)),  # center
            ((0, h // 2), (dW, h)),  # bottom-left
            ((w - dW, h // 2), (w, h)),  # bottom-right
            ((0, h - dH), (w, h))  # bottom
        ]
        on = [0] * len(segments)
 
        # 遍历分段部分
        for (i, ((xA, yA), (xB, yB))) in enumerate(segments):
            # 提取分段ROI，计算segment的面积，并计算每个线段的非零像素总值
            segROI = roi[yA:yB, xA:xB]
            total = cv2.countNonZero(segROI)
            area = (xB - xA) * (yB - yA)
 
            # 如果非0像素的总数大于面积的50%，则认为分段是打开的
            if total / float(area) > 0.5:
                on[i] = 1
 
        # 查找digit并显示在图像上
        digit = DIGITS_LOOKUP[tuple(on)]
        digits.append(digit)
        cv2.rectangle(output, (x, y), (x + w, y + h), (0, 255, 0), 1)
        cv2.putText(output, str(digit), (x - 10, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.65, (0, 255, 0), 2)
 
if __name__ == '__main__':
    XboxInit()
    ReceiveVideoInit()
    while 1:
        XboxJoystick()
        xboxinputprocess()
        ReceiveVideo()
        print('---------')
    
