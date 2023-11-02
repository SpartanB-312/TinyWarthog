/*include----------------------------------------------------------------------*/
#include <MsTimer2.h>
/*define-----------------------------------------------------------------------*/
#define L_ENCODER_A 2                //左电机编码器A相引脚 下降沿捕获
#define L_ENCODER_B 4                //左编码器B相引脚

#define R_ENCODER_A 3                //右电机编码器A相引脚 下降沿捕获
#define R_ENCODER_B 5                //编码器B相引脚

#define TB6612_L_IN1 7
#define TB6612_L_IN2 8
#define Motor_L_PWM 9

#define TB6612_R_IN1 12
#define TB6612_R_IN2 13
#define Motor_R_PWM 10
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
int timecnt = 0; //50ms +1
int time_seconds = 0; // 1s +1
float V_L = 0; //临时存储速度变量
float V_R = 0; //临时存储速度变量
int Motor_L_dir = 0; //反馈的电机转动方向
int Motor_R_dir = 0; //反馈的电机转动方向
PID L_Motor_PID, R_Motor_PID; //PID结构体

void setup()
{
    Motor_Init();                    //电机端口初始化
    PID_Init();
    MsTimer2::set(50, PID_Cal_Computer_Out); // 50ms period
    MsTimer2::start();
    Serial.begin(9600);              //打开串口
}
/*
loop
*/
void loop()
{

    Motor_Test();
    //Read_Motor_V();//读取脉冲计算速度ok

    Serial.print(L_Motor_PID.input);
    Serial.print(",");
    Serial.print(L_Motor_PID.feedback);
    Serial.print(",");
    Serial.println(R_Motor_PID.output);
    //Serial.println("-----");
    //Serial.println(R_PWM);
    //Serial.println("-----");
    //Serial.println(R_Motor_PID.feedback); 
}
/*
Motor_Test
*/
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
    L_Motor_PID.output=100;
    R_Motor_PID.output=100;
    //
    /*--------------------*/
    Motor_PWM_Set(L_Motor_PID.output, R_Motor_PID.output);
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
void Motor_PWM_Set(float L_PWM, float R_PWM)
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
        
}
/*
Motor_Init
*/
void Motor_Init(void)
{
    pinMode(L_ENCODER_A, INPUT); //左轮编码器A引脚，设置为输入模式
    pinMode(L_ENCODER_B, INPUT); //左轮编码器B引脚，设置为输入模式
    pinMode(R_ENCODER_A, INPUT); //右轮编码器A引脚，设置为输入模式
    pinMode(R_ENCODER_B, INPUT); //右轮编码器B引脚，设置为输入模式

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

}
