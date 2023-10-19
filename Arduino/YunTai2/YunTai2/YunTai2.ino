#include <String.h>
#include <Servo.h>

Servo myservo1; // 创建一个名为 myservo 的伺服电机实例（名称可随你喜欢命名）
Servo myservo2;
String Servo1Ctrl;
String Servo2Ctrl;
char inByte = 0;  // 定义 串口接收的数据
int YTangle1 = 0;    // 定义 角度值
int YTangle2 = 0;
String temp = ""; // 定义 临时字符变量，又或者说是缓存用的吧

void setup()
{
  myservo1.attach(9);  // 配置myservo实例的引脚为9
  myservo2.attach(10);
  Serial.begin(9600); // 设置波特率，这是串口应用中很重要的数字
}

void loop()
{
  while (Serial.available() > 0) // 判断串口是否有数据，若有则进入while循环
  {
    inByte = Serial.read(); // 读取数据，串口一次只能读1个字符
    temp += inByte;         // 把读到的字符存进临时变量里面缓存，
                            // 再继续判断串口还有没有数据，知道把所有数据都读取出来
    delay(10);
  }

  if (temp != "") // 判断临时变量是否为空，非空则进行舵机角度控制
  {
    Servo1Ctrl = fenge(temp, ",", 2);
    Servo2Ctrl = fenge(temp, ",", 3);
    YTangle1 = Servo1Ctrl.toInt();  // 把变量字符串类型转成整型
    YTangle2 = Servo2Ctrl.toInt();
    Serial.print(YTangle1);
    Serial.print(",");
    Serial.println(YTangle2); // 输出数据到串口上，以便观察
  }
  temp = ""; // 清空临时变量

  myservo1.write(YTangle1); // 控制舵机转动到相应的角度位置。
  myservo2.write(YTangle2);
  delay(10);           // 延时100毫秒
}

String fenge(String str, String fen, int index)
{
  int weizhi;
  String temps[str.length()];
  int i = 0;
  do
  {
    weizhi = str.indexOf(fen);
    if (weizhi != -1)
    {
      temps[i] = str.substring(0, weizhi);
      str = str.substring(weizhi + fen.length(), str.length());
      i++;
    }
    else
    {
      if (str.length() > 0)
        temps[i] = str;
    }
  } while (weizhi >= 0);

  if (index > i)
    return "-1";
  return temps[index];
}
