import serial

Port = "/dev/ttyUSB1"  # 串口
baudRate = 9600  # 波特率
ser = serial.Serial(Port, baudRate, timeout=1)


send = '45'  # 发送给arduino的数据
ser.write(send.encode())
str = ser.readline().decode()  # 获取arduino发送的数据
if(str != ""):
    print(str)

ser.close()