
#include <String.h>
String test_ssid = "2,4,6,8,10";
char inByte = 0;  // 定义 串口接收的数据
int angle = 0;    // 定义 角度值
String temp = ""; // 定义 临时字符变量，又或者说是缓存用的吧

void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("test String: ");

  String part01 = fenge(test_ssid, ",", 0);
  String part02 = fenge(test_ssid, ",", 1);
  String part03 = fenge(test_ssid, ",", 2);
  String part04 = fenge(test_ssid, ",", 3);
  String part05 = fenge(test_ssid, ",", 4);
  Serial.print(part01);
  Serial.print(part02);
  Serial.print(part03);
  Serial.print(part04);
  Serial.println(part05); // 没有，输出-1
}

void loop()
{
  delay(10);
  while (Serial.available() > 0) // 判断串口是否有数据，若有则进入while循环
  {
    inByte = Serial.read(); // 读取数据，串口一次只能读1个字符
    temp += inByte;         // 把读到的字符存进临时变量里面缓存，
                            // 再继续判断串口还有没有数据，知道把所有数据都读取出来
    delay(10);
  }
  test_ssid = temp;
  if (test_ssid != "")
  {
    String part01 = fenge(test_ssid, ",", 0);
    String part02 = fenge(test_ssid, ",", 1);
    String part03 = fenge(test_ssid, ",", 2);
    String part04 = fenge(test_ssid, ",", 3);
    String part05 = fenge(test_ssid, ",", 4);
    Serial.print(part01);
    Serial.print(part02);
    Serial.print(part03);
    Serial.print(part04);
    Serial.print(part05); // 没有，输出-1
  }
  temp = "";
  delay(100);
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
