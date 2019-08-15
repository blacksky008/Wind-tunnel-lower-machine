#include <HX711.h>//hx711头文件
#include <Servo.h>//舵机头文件

//————————————————————————————————————————————
//————————————————————————————————————————————
//全局变量定义
//————————————————————————————————————————————
//————————————————————————————————————————————

// HX711 x轴读取引脚为pin2，pin3
const int LOADCELL_DOUT_PIN_X = 2;
const int LOADCELL_SCK_PIN_X = 3;
// HX711 y轴读取引脚为pin4，pin5
const int LOADCELL_DOUT_PIN_Y = 4;
const int LOADCELL_SCK_PIN_Y = 5;

//hx711定义读取的变量名
HX711 scale_X;
HX711 scale_Y;
//定义的舵机
Servo servo_9;
//舵机转角数据存储变量
volatile int serv = 0;

volatile char calibration;
//自适应校准参数
double k_x = 0;
double huanjing_x;
double k_y = 0;
double huanjing_y;

String h_k_x;
String h_k_y;
String h_env_k_x;
String h_env_k_y;
//————————————————————————————————————————————
//————————————————————————————————————————————
//校准
//————————————————————————————————————————————
//————————————————————————————————————————————
void calibration_x()
{
	//x轴校准，通过差值得出修正系数
	//放置两个量块
	Serial.println("Place two 10g weights, please.");
	delay(1000);
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前2块量块x轴方向重量数据
	}
	long chu_x = 0;//初值
	chu_x = scale_X.read();//取hx711读取的值
	Serial.println("success!");
	//串口清除
	while (Serial.read() >= 0) {//串口清除
	}
	//放置一块量块
	Serial.println("please set one 10g");
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前1块量块x轴方向重量数据
	}
	long xian_x = 0;//现值
	xian_x = scale_X.read();//取hx711读取的值

	k_x = 10.0 / (chu_x - xian_x);//因为采用的计量单位为10g，算法为：计量差值10g/（20g读到的数据-10g读到的数据），得到修正指数k
	Serial.println("success!");
	while (Serial.read() >= 0) {//串口清除
	}

	Serial.println("please set zero 10g");
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前没有量块x轴方向重量数据
	}

	huanjing_x = scale_X.read();//这个时候读到的是去皮重量

	Serial.println("X_axis is success!");
	while (Serial.read() >= 0) {//串口清除
	}
	delay(1000);
}

void calibration_y()
{

	//y轴校准，通过差值得出修正系数
	//放置两个量块
	Serial.println("please set two 10g");
	delay(1000);
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前2块量块y轴方向重量数据
	}
	long chu_y = 0;//初值
	chu_y = scale_Y.read();//取hx711读取的值
	Serial.println("success!");
	while (Serial.read() >= 0) {	//串口清除
	}

	//放置一块量块
	Serial.println("please set one 10g");
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前1块量块y轴方向重量数据
	}
	long xian_y = 0;//取hx711读取的值
	xian_y = scale_Y.read();//现值
	k_y = 10.0 / (chu_y - xian_y);//因为采用的计量单位为10g，算法为：计量差值10g/（20g读到的数据-10g读到的数据），得到修正指数k
	Serial.println("success!");
	while (Serial.read() >= 0) {//串口清除
	}

	Serial.println("please set zero 10g");
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前2块量块y轴方向重量数据
	}

	huanjing_y = scale_Y.read();//这个时候读到的是去皮重量

	Serial.println("Y_axis is success!");
	while (Serial.read() >= 0) {//串口清除
	}
	delay(1000);
}
//————————————————————————————————————————————
//————————————————————————————————————————————
//定义舵机转动函数
//————————————————————————————————————————————
//————————————————————————————————————————————

void servo() {
	while (Serial.available() > 0) {//有数据可读吗
		serv = Serial.parseInt();//将读到的串口数据赋值给item
		if (serv >= 0 && serv <= 45) {
			Serial.print(serv);
			Serial.println("___________________________");
			serv = serv + 45;
			servo_9.write(serv);
			delay(15);
		}
		else if (serv >= -45 && serv < 0) {
			Serial.print(serv);
			Serial.println("___________________________");
			serv = serv + 45;
			servo_9.write(serv);
			delay(15);
		}

		else {
			Serial.print("Please enter the correct range \r\n");
		}
	}
}
void writer()
{
	//作者信息
	Serial.println("Hello,World");
	Serial.println("Written by HQ");
	Serial.println("Completion time:2019.7.16");
	delay(1000);
}




//————————————————————————————————————————————
//————————————————————————————————————————————
//初始化
//————————————————————————————————————————————
//————————————————————————————————————————————

void setup() {
	Serial.begin(9600);//波特率为9600
	scale_X.begin(LOADCELL_DOUT_PIN_X, LOADCELL_SCK_PIN_X);//hx711初始化x轴的压力传感器
	scale_Y.begin(LOADCELL_DOUT_PIN_Y, LOADCELL_SCK_PIN_Y);//hx711初始化y轴的压力传感器

	servo_9.attach(9);//定义舵机引脚
	servo_9.write(45);//定义舵机初始化位置角度为0°
	delay(0);

	writer();
	Serial.println("Need calibration? ");
	while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前1块量块y轴方向重量数据
	}
	calibration = Serial.read();
	if (calibration == 'Q') {
		calibration_x();
		calibration_y();
		Serial.print("K_X=");
		Serial.print(k_x,12);
		Serial.print("K_Y=");
		Serial.print(k_y,12);
		Serial.print("ENV_X");
		Serial.print(huanjing_x);
		Serial.print("ENV_Y");
		Serial.print(huanjing_y);
		Serial.print("\r\n");

	}
	else if (calibration == 'R')
	{
		Serial.println("Forthcoming assignment ");
		//_______________________________________________________________________
		//_______________________________________________________________________
		while (!(Serial.available() > 0)) {//暂停等待数据流入
		}
		while (Serial.available() > 0)
		{
			h_k_x += char(Serial.read());
			delay(2);
		}
		if (h_k_x.length() > 0)
		{
			Serial.print("kx=");
			Serial.println(h_k_x);
			h_k_x = "";
			
		}
		while (Serial.read() >= 0) {//串口清除
		}
		//_______________________________________________________________________
		//_______________________________________________________________________
		while (!(Serial.available() > 0)) {//暂停等待数据流入
		}
		while (Serial.available() > 0)
		{
			h_k_y += char(Serial.read());
			delay(2);
		}
		if (h_k_y.length() > 0)
		{
			Serial.print("ky=");
			Serial.println(h_k_y);
			h_k_y = "";
		}
		while (Serial.read() >= 0) {//串口清除
		}
		//_______________________________________________________________________
		//_______________________________________________________________________
		while (!(Serial.available() > 0)) {//暂停等待数据流入
		}
		while (Serial.available() > 0)
		{
			h_env_k_x += char(Serial.read());
			delay(2);
		}
		if (h_env_k_x.length() > 0)
		{
			Serial.print("env_x=");
			Serial.println(h_env_k_x);
			h_env_k_x = "";
		}
		while (Serial.read() >= 0) {//串口清除
		}
		//_______________________________________________________________________
		//_______________________________________________________________________
		while (!(Serial.available() > 0)) {//暂停等待数据流入，如果有数据流入，计算当前2块量块x轴方向重量数据
		}
		while (Serial.available() > 0)
		{
			h_env_k_y += char(Serial.read());
			delay(2);
		}
		if (h_env_k_y.length() > 0)
		{
			Serial.print("env_y=");
			Serial.println(h_env_k_y);
			h_env_k_y = "";
		}
		while (Serial.read() >= 0) {//串口清除
		}

		k_x = h_k_x.toDouble();
		k_y = h_k_y.toDouble();
		huanjing_x = h_env_k_x.toDouble();
		huanjing_y = h_env_k_x.toDouble();

	}

}


void loop() {
	
	servo();//自己写的舵机函数

	//————————————————————————————————————————————
	//————————————————————————————————————————————
	//输出数据
	//————————————————————————————————————————————
	//————————————————————————————————————————————

	if (scale_X.is_ready()) {//如果有hx711数据

		double reading = scale_X.read();//将读取数据赋值于reading
		double reading1 = (reading - huanjing_x) * k_x;//计算方法为重量=（当前读到的数据-去皮后的重量）*环境修正指数

		Serial.print("X_axis  reading:\t ");
		Serial.print(reading1);
		Serial.println("g\r\n");
	}
	else {
		Serial.println("X_axis not found.\r\n");
	}

	if (scale_Y.is_ready()) {

		double reading2 = scale_Y.read();//将读取数据赋值于reading
		double reading3 = (reading2 - huanjing_y) * k_y;//计算方法为重量=（当前读到的数据-去皮后的重量）*环境修正指数

		Serial.print("Y_axis  reading:\t ");

		Serial.print(reading3);
		Serial.println("g\r\n");
	}
	else {
		Serial.println("Y_axis not found.\r\n");
	}
	delay(1000);

}