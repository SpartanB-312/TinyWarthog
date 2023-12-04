/*include----------------------------------------------------------------------*/
#include <String.h>
#include <Servo.h>
#include <MsTimer2.h>
/*define-----------------------------------------------------------------------*/

#define TB6612_L_IN1 22
#define TB6612_L_IN2 23
#define Motor_L_PWM 12

#define TB6612_R_IN1 24
#define TB6612_R_IN2 25
#define Motor_R_PWM 13

#define TB6612_LB_IN1 30
#define TB6612_LB_IN2 31
#define Motor_LB_PWM 7

#define TB6612_RB_IN1 32
#define TB6612_RB_IN2 33
#define Motor_RB_PWM 6
/*variate----------------------------------------------------------------------*/
typedef struct
{
    float input;
    float output;
    float feedback;
    float k_p;
    float k_i;
    float k_d;
    float err_1;
    float err_2;
    float err_x;

    float out_max;
    float out_min;
    float err_x_max;
} PID;

volatile float motor_L = 0; //中断变量，子脉冲计数
volatile float motor_R = 0; //中断变量，子脉冲计数
volatile float motor_LB = 0; //中断变量，子脉冲计数
volatile float motor_RB = 0; //中断变量，子脉冲计数
int timecnt = 0; //50ms +1
int time_seconds = 0; // 1s +1
float V_L = 0; //临时存储速度变量
float V_R = 0; //临时存储速度变量
int Motor_L_dir = 0; //反馈的电机转动方向
int Motor_R_dir = 0; //反馈的电机转动方向
PID L_Motor_PID, R_Motor_PID; //PID结构体
PID LB_Motor_PID, RB_Motor_PID; //PID结构体

Servo myservo1; // 创建一个名为 myservo 的伺服电机实例（名称可随你喜欢命名）
Servo myservo2;
String Servo1Ctrl;
String Servo2Ctrl;
String Speed1Ctrl;
char inByte = 0;  // 定义 串口接收的数据
int YTangle1 = 0;    // 定义 角度值
int YTangle2 = 0;
int Speed1 = 0;
String temp = ""; // 定义 临时字符变量，又或者说是缓存用的吧

void setup()
{
  Motor_Init();                    //电机端口初始化
  PID_Init();
  MsTimer2::set(100, PID_Cal_Computer_Out); // 50ms period
  MsTimer2::start();
  myservo1.attach(3);  // 配置myservo实例的引脚为9
  myservo2.attach(4);
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
    Speed1Ctrl = fenge(temp, ",", 1);
    Servo1Ctrl = fenge(temp, ",", 2);
    Servo2Ctrl = fenge(temp, ",", 3);
    
    Speed1=Speed1Ctrl.toInt();
    Speed1=float(Speed1);
    YTangle1 = Servo1Ctrl.toInt();  // 把变量字符串类型转成整型
    YTangle2 = Servo2Ctrl.toInt();
    L_Motor_PID.input = Speed1;
    R_Motor_PID.input = -Speed1;
    LB_Motor_PID.input = Speed1;
    RB_Motor_PID.input = -Speed1;


    myservo1.write(YTangle1); // 控制舵机转动到相应的角度位置。
    myservo2.write(YTangle2);
    PID_Cal_Computer_Out();
    Serial.print(R_Motor_PID.output);
    Serial.print(",");
    Serial.print(RB_Motor_PID.output);
    Serial.print(",");
    Serial.print(YTangle1);
    Serial.print(",");
    Serial.println(YTangle2); // 输出数据到串口上，以便观察
  }
  temp = ""; // 清空临时变量

  
  //delay(100);           // 延时100毫秒
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

void Motor_Test(void)
{
    L_Motor_PID.input = 100;
    R_Motor_PID.input = -100;
}
/*
PID_Cal_Computer_Out
*/
void PID_Cal_Computer_Out(void)
{
    //PID_Cal(&L_Motor_PID);
    //PID_Cal(&R_Motor_PID);
    /*--------------------*/
    //test
    L_Motor_PID.output=L_Motor_PID.input;
    R_Motor_PID.output=R_Motor_PID.input;
    LB_Motor_PID.output=LB_Motor_PID.input;
    RB_Motor_PID.output=RB_Motor_PID.input;
    //
    /*--------------------*/
    Motor_PWM_Set(L_Motor_PID.output, R_Motor_PID.output, LB_Motor_PID.output, RB_Motor_PID.output);
    timecnt++;
    if(timecnt == 20)
    {
        time_seconds++;
        timecnt = 0;
    }
}
/*
Motor_PWM_Set
*/
void Motor_PWM_Set(float L_PWM, float R_PWM, float LB_PWM, float RB_PWM)
{
  /*

    if(L_PWM > 0)
    {
        digitalWrite(TB6612_L_IN1, LOW);
        digitalWrite(TB6612_L_IN2, HIGH);
        analogWrite(Motor_L_PWM, L_PWM);
    }
    else
    {
        digitalWrite(TB6612_L_IN1, HIGH);
        digitalWrite(TB6612_L_IN2, LOW);
        analogWrite(Motor_L_PWM, -1 * L_PWM);
    }

    if(R_PWM > 0)
    {
        digitalWrite(TB6612_R_IN1, HIGH);
        digitalWrite(TB6612_R_IN2, LOW);
        analogWrite(Motor_R_PWM, R_PWM);
    }
    else
    {
        digitalWrite(TB6612_R_IN1, LOW);
        digitalWrite(TB6612_R_IN2, HIGH);
        analogWrite(Motor_R_PWM, -1 * R_PWM);
    }*/
    digitalWrite(TB6612_L_IN1, LOW);
    digitalWrite(TB6612_L_IN2, HIGH);
    analogWrite(Motor_L_PWM, L_PWM);
    digitalWrite(TB6612_R_IN1, HIGH);
    digitalWrite(TB6612_R_IN2, LOW);
    analogWrite(Motor_R_PWM, R_PWM);
    Serial.println(LB_PWM);
    digitalWrite(TB6612_LB_IN1, LOW);
    digitalWrite(TB6612_LB_IN2, HIGH);
    analogWrite(Motor_LB_PWM, LB_PWM);
    digitalWrite(TB6612_RB_IN1, HIGH);
    digitalWrite(TB6612_RB_IN2, LOW);
    analogWrite(Motor_RB_PWM, RB_PWM);
        
}
/*
Motor_Init
*/
void Motor_Init(void)
{
    pinMode(TB6612_L_IN1, OUTPUT); //设置两个驱动引脚为输出模式
    pinMode(TB6612_L_IN2, OUTPUT); //
    pinMode(TB6612_R_IN1, OUTPUT); //设置两个驱动引脚为输出模式
    pinMode(TB6612_R_IN2, OUTPUT); //

    pinMode(Motor_L_PWM, OUTPUT);  //设置使能引脚为输出模式
    pinMode(Motor_R_PWM, OUTPUT);  //设置使能引脚为输出模式

    //驱动芯片控制引脚全部拉低
    digitalWrite(TB6612_L_IN1, LOW);
    digitalWrite(TB6612_L_IN2, LOW);
    digitalWrite(Motor_L_PWM, LOW);
    digitalWrite(TB6612_R_IN1, LOW);
    digitalWrite(TB6612_R_IN2, LOW);
    digitalWrite(Motor_R_PWM, LOW);
    
    pinMode(TB6612_LB_IN1, OUTPUT); //设置两个驱动引脚为输出模式
    pinMode(TB6612_LB_IN2, OUTPUT); //
    pinMode(TB6612_RB_IN1, OUTPUT); //设置两个驱动引脚为输出模式
    pinMode(TB6612_RB_IN2, OUTPUT); //

    pinMode(Motor_LB_PWM, OUTPUT);  //设置使能引脚为输出模式
    pinMode(Motor_RB_PWM, OUTPUT);  //设置使能引脚为输出模式

    //驱动芯片控制引脚全部拉低
    digitalWrite(TB6612_LB_IN1, LOW);
    digitalWrite(TB6612_LB_IN2, LOW);
    digitalWrite(Motor_LB_PWM, LOW);
    digitalWrite(TB6612_RB_IN1, LOW);
    digitalWrite(TB6612_RB_IN2, LOW);
    digitalWrite(Motor_RB_PWM, LOW);
}
/*
PID_Init
*/
void PID_Init(void)
{
    L_Motor_PID.k_p = 0.08;
    L_Motor_PID.k_i = 0.091;
    L_Motor_PID.k_d = 0.01;
    L_Motor_PID.out_max = 250;
    L_Motor_PID.out_min = -250;
    L_Motor_PID.input = 0;
    L_Motor_PID.err_x_max = 1000;

    R_Motor_PID.k_p = 0.08;
    R_Motor_PID.k_i = 0.091;
    R_Motor_PID.k_d = 0.01;
    R_Motor_PID.out_max = 250;
    R_Motor_PID.out_min = -250;
    R_Motor_PID.input = 0;
    R_Motor_PID.err_x_max = 1000;

    LB_Motor_PID.k_p = 0.08;
    LB_Motor_PID.k_i = 0.091;
    LB_Motor_PID.k_d = 0.01;
    LB_Motor_PID.out_max = 250;
    LB_Motor_PID.out_min = -250;
    LB_Motor_PID.input = 0;
    LB_Motor_PID.err_x_max = 1000;

    RB_Motor_PID.k_p = 0.08;
    RB_Motor_PID.k_i = 0.091;
    RB_Motor_PID.k_d = 0.01;
    RB_Motor_PID.out_max = 250;
    RB_Motor_PID.out_min = -250;
    RB_Motor_PID.input = 0;
    RB_Motor_PID.err_x_max = 1000;
}
