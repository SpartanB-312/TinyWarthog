/*
*********************************************************************************************************
*
*	模块名称 : UNO 两路编码器电机PID控制
*	文件名称 : Two_Encoder_Motor_PID_Control.ino
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/

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
/*statement--------------------------------------------------------------------*/
void Motor_PWM_Set(float L_PWM, float R_PWM);
void Motor_Init(void);
void PID_Init(void);
void PID_Cal(PID *pid);
void Read_Motor(void);
void Read_Motor_V(void);
void PID_Cal_Computer_Out(void);
void Motor_Test(void);
/*Function prototype Begin*****************************************************/


/***********************************************************
*@fuction	:setup
*@brief		:初始化
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void setup()
{
    Motor_Init();                    //电机端口初始化
    PID_Init();
    MsTimer2::set(50, PID_Cal_Computer_Out); // 50ms period
    MsTimer2::start();
    Serial.begin(9600);              //打开串口
}

/***********************************************************
*@fuction	:loop
*@brief		:主循环
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void loop()
{

    Motor_Test();
    Read_Motor_V();//读取脉冲计算速度ok

    Serial.print(L_Motor_PID.input);
    Serial.print(",");
    Serial.print(L_Motor_PID.feedback);
    Serial.print(",");
    Serial.print(L_Motor_PID.output);
    Serial.print(",");
    Serial.print(R_Motor_PID.output);
    Serial.print(",");
    Serial.println(R_Motor_PID.feedback); ;
}

/***********************************************************
*@fuction	:Motor_Test
*@brief		:测试电机
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void Motor_Test(void)
{
    if(time_seconds < 2)
    {
        L_Motor_PID.input = 0;
        R_Motor_PID.input = 0;
    }
    if(time_seconds >= 2 && time_seconds < 4)
    {
        L_Motor_PID.input = 200;
        R_Motor_PID.input = 200;
    }
    if(time_seconds >= 4 && time_seconds < 6)
    {
        L_Motor_PID.input = 400;
        R_Motor_PID.input = 400;
    }
    if(time_seconds >= 6 && time_seconds < 8)
    {
        L_Motor_PID.input = -200;
        R_Motor_PID.input = -200;
    }
    if(time_seconds >= 8 && time_seconds < 10)
    {
        L_Motor_PID.input = -400;
        R_Motor_PID.input = -400;
    }
    if(time_seconds >= 10)
    {
        time_seconds = 0;
    }
}

/***********************************************************
*@fuction	:PID_Cal_Computer_Out
*@brief		:速度环计算并输出PWM
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void PID_Cal_Computer_Out(void)
{
    PID_Cal(&L_Motor_PID);
    PID_Cal(&R_Motor_PID);
    Motor_PWM_Set(L_Motor_PID.output, R_Motor_PID.output);
    timecnt++;
    if(timecnt == 20)
    {
        time_seconds++;
        timecnt = 0;
    }
}

/***********************************************************
*@fuction	:Motor_PWM_Set
*@brief		:输出PWM 控制电机正反转
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void Motor_PWM_Set(float L_PWM, float R_PWM)
{

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
    }
}


/***********************************************************
*@fuction	:Motor_Init
*@brief		:电机控制引脚初始化
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

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

/***********************************************************
*@fuction	:PID_Init
*@brief		:PID参数
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

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

/***********************************************************
*@fuction	:PID_Cal
*@brief		:PID增量式
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-06-14
***********************************************************/

void PID_Cal(PID *pid)
{
    float p, i, d;

    pid->err_2 = pid->err_1;
    pid->err_1 = pid->input - pid->feedback;

    p = pid->k_p * pid->err_1;
    i = pid->k_i * pid->err_x;
    d = pid->k_d * (pid->err_1 - pid->err_2);
    pid->err_x += pid->err_1;
    pid->output = p + i + d;

    if(pid->output > pid->out_max)      pid->output = pid->out_max;
    if(pid->output < pid->out_min)      pid->output = pid->out_min;
    if(pid->err_x > pid->err_x_max)     pid->err_x = pid->err_x_max;
}


/***********************************************************
*@fuction	:Read_Motor_L
*@brief		:数脉冲（带方向）
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void Read_Motor_L(void)
{
    Motor_L_dir = digitalRead(L_ENCODER_B);
    if(Motor_L_dir == 1)
    {
        motor_L++;
    }
    else
    {
        motor_L--;
    }
}
void Read_Motor_R(void)
{
    Motor_R_dir = digitalRead(R_ENCODER_B);
    if(Motor_R_dir == 1)
    {
        motor_R++;
    }
    else
    {
        motor_R--;
    }
}


/***********************************************************
*@fuction	:Read_Motor_V
*@brief		:下降沿捕获中断 并计算速度
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-27
***********************************************************/

void Read_Motor_V(void)
{
    static float L_Speed = 0, R_Speed = 0;
    float speed_k = 0.3;
    L_Speed = L_Motor_PID.feedback;
    R_Speed = R_Motor_PID.feedback;
    unsigned long nowtime = 0;
    motor_L = 0;
    motor_R = 0;
    nowtime = millis() + 50; //读50毫秒
    attachInterrupt(digitalPinToInterrupt(L_ENCODER_A), Read_Motor_L, FALLING);
    attachInterrupt(digitalPinToInterrupt(R_ENCODER_A), Read_Motor_R, FALLING);
    while(millis() < nowtime); //达到50毫秒关闭中断ok
    detachInterrupt(digitalPinToInterrupt(L_ENCODER_A));//左轮脉冲关中断计数ok
    detachInterrupt(digitalPinToInterrupt(R_ENCODER_A));//左轮脉冲关中断计数ok
    V_L = ((motor_L / 330) * 48 * PI) * 20; //单位mm/s ok
    V_R = ((motor_R / 330) * 48 * PI) * 20; //单位mm/s ok
    L_Motor_PID.feedback = (1 - speed_k) * V_L + speed_k * L_Speed;
    R_Motor_PID.feedback = -1 * (1 - speed_k) * V_R + speed_k * R_Speed;
}


/*Function prototype End*******************************************************/
