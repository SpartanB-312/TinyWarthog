// by lingshunlab.com

// 加载 舵机 库
#include <Servo.h>

Servo myservo;  // 创建一个名为 myservo 的伺服电机实例（名称可随你喜欢命名）

char inByte = 0; // 定义 串口接收的数据
int angle = 0;  // 定义 角度值
String temp = ""; // 定义 临时字符变量，又或者说是缓存用的吧

void setup() 
{
  myservo.attach(9);    // 配置myservo实例的引脚为9
  Serial.begin(9600);  //设置波特率，这是串口应用中很重要的数字
}

void loop() 
{
  while (Serial.available() > 0) // 判断串口是否有数据，若有则进入while循环
  {
    inByte = Serial.read();// 读取数据，串口一次只能读1个字符
    temp += inByte;// 把读到的字符存进临时变量里面缓存，
                   // 再继续判断串口还有没有数据，知道把所有数据都读取出来
    delay(10);
  }

  if(temp != "")   // 判断临时变量是否为空，非空则进行舵机角度控制
  {
    angle = temp.toInt();    // 把变量字符串类型转成整型
    Serial.println(angle);  // 输出数据到串口上，以便观察
  }
  temp = "";// 清空临时变量

  myservo.write(angle);  // 控制舵机转动到相应的角度位置。
  delay(100);// 延时100毫秒
}
