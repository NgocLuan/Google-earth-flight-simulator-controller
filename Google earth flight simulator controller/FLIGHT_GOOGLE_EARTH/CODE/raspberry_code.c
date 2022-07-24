//khai bao thu vien
#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>	//...-lwiringPi -lm

//dinh nghia MPU6050
#define Sample_rate		25
#define Config			26
#define Gyro_config		27
#define Acc_config		28
#define Interrupt		56
#define PWR_Managment	107
#define ACC_X			59
#define ACC_Y			61
#define ACC_Z			63

//dinh nghia nut nhan
#define BT1		16		//pause
#define BT2		18		//Gear
#define BT3		7		//Shift
#define BT4		13		//speed+
#define BT5		15		//speed-
#define BT6		19		//center
#define BT7		21		//flap+
#define BT8		23		//flap-
#define BT9		29		//phanh phai
#define BT10	31		//phanh trai

//khai bao bien
int fd, mpu;
float Ax, Ay, Az, roll, pitch;
int unsigned long t1, t2;
int speed, flap, ss, gear, center, shift, ph_left, ph_right, ud_control, lr_control; 
//viet ham con
void init_MPU6050(void)
{
	//register 25->28, 56, 107
	//sample rate 200hz
	wiringPiI2CWriteReg8(mpu,Sample_rate,4);
	//khong su dung nguon xung ngoai, tat DLPF
	wiringPiI2CWriteReg8(mpu,Config,3);
	//gyro FS: +- 500o/s
	wiringPiI2CWriteReg8(mpu,Gyro_config,0x08);
	//acc FS: +- 8g
	wiringPiI2CWriteReg8(mpu,Acc_config,0x10);
	//mo interrupt cua data ready
	wiringPiI2CWriteReg8(mpu,Interrupt,1);
	//chon nguon xung Gyro X
	wiringPiI2CWriteReg8(mpu,PWR_Managment,0x01);
}

int16_t read_2register(uint8_t address)
{
	int16_t D, high, low;
	high = wiringPiI2CReadReg8(mpu, address);
	low = wiringPiI2CReadReg8(mpu, address+1);
	D = (high<<8)|low;
	return D;
}

void ngat_BT1(void)
{
	if(digitalRead(BT1)==0)
	{
		delay(20);
		if(digitalRead(BT1)==0)
		{
			if(ss == 0)	ss = 1;
			else 		ss = 0;
			while(digitalRead(BT1)==0);
		}
	}
}

void ngat_BT2(void)
{
	if(digitalRead(BT2)==0)
	{
		delay(20);
		if(digitalRead(BT2)==0)
		{
			if(gear == 0)	gear = 1;
			else 			gear = 0;
			while(digitalRead(BT2)==0);
		}
	}
}

void ngat_BT3(void)
{
	if(digitalRead(BT3)==0)
	{
		delay(20);
		if(digitalRead(BT3)==0)
		{
			shift = 1;
			while(digitalRead(BT3)==0);
		}
	}			
	else	shift = 0;
}

void ngat_BT4(void)
{
	if(digitalRead(BT4)==0)
	{
		delay(20);
		if(digitalRead(BT4)==0)
		{
			speed = 2;
			while(digitalRead(BT4)==0);
		}
	}	
	else 	speed = 0;
}

void ngat_BT5(void)
{
	if(digitalRead(BT5)==0)
	{
		delay(20);
		if(digitalRead(BT5)==0)
		{
			speed = 1;
			while(digitalRead(BT5)==0);
		}
	}	
	else 	speed = 0;
}

void ngat_BT6(void)
{
	if(digitalRead(BT6)==0)
	{
		delay(20);
		if(digitalRead(BT6)==0)
		{
			if(center == 0)	center = 1;
			else 			center = 0;
			while(digitalRead(BT6)==0);
		}
	}
}

void ngat_BT7(void)
{
	if(digitalRead(BT7)==0)
	{
		delay(20);
		if(digitalRead(BT7)==0)
		{
			flap = 2;
			while(digitalRead(BT7)==0);
		}
	}		
	else	flap = 0;
}

void ngat_BT8(void)
{
	if(digitalRead(BT8)==0)
	{
		delay(20);
		if(digitalRead(BT8)==0)
		{
			flap = 1;
			while(digitalRead(BT8)==0);
		}
	}		
	else	flap = 0;
}

void ngat_BT9(void)
{
	if(digitalRead(BT9)==0)
	{
		delay(20);
		if(digitalRead(BT9)==0)
		{
			ph_right = 1;
			while(digitalRead(BT9)==0);
		}
	}		
	else	ph_right = 0;
}

void ngat_BT10(void)
{
	if(digitalRead(BT10)==0)
	{
		delay(20);
		if(digitalRead(BT10)==0)
		{
			ph_left = 1;
			while(digitalRead(BT10)==0);
		}
	}			
	else	ph_left = 0;
}


//viet ham chinh
int main(void)
{
	//khoi tao ban dau
	wiringPiSetupPhys();
	fd = serialOpen("/dev/serial0",115200);
	printf("Serial begin...\n");
	mpu = wiringPiI2CSetup(0x68);
	init_MPU6050();
	
	pinMode(BT1, INPUT);
	pinMode(BT2, INPUT);
	pinMode(BT3, INPUT);
	pinMode(BT4, INPUT);
	pinMode(BT5, INPUT);
	pinMode(BT6, INPUT);
	pinMode(BT7, INPUT);
	pinMode(BT8, INPUT);
	pinMode(BT9, INPUT);
	pinMode(BT10, INPUT);
	//trang thai ban dau
	ss = speed = center = flap = shift = gear = 0;
	ph_left = ph_right = ud_control=lr_control = 0;
	t1 = t2 = 0;
	
	//ngat nut nhan
	wiringPiISR(BT1, INT_EDGE_FALLING, &ngat_BT1);
	wiringPiISR(BT2, INT_EDGE_FALLING, &ngat_BT2);
	wiringPiISR(BT3, INT_EDGE_BOTH, &ngat_BT3);
	wiringPiISR(BT4, INT_EDGE_BOTH, &ngat_BT4);
	wiringPiISR(BT5, INT_EDGE_BOTH, &ngat_BT5);
	wiringPiISR(BT6, INT_EDGE_FALLING, &ngat_BT6);
	wiringPiISR(BT7, INT_EDGE_BOTH, &ngat_BT7);
	wiringPiISR(BT8, INT_EDGE_BOTH, &ngat_BT8);
	wiringPiISR(BT9, INT_EDGE_BOTH, &ngat_BT9);
	wiringPiISR(BT10, INT_EDGE_BOTH, &ngat_BT10);
	//VONG LAP
	while(1)
	{
		
		Ax = (float)read_2register(ACC_X);
		Ay = (float)read_2register(ACC_Y);
		Az = (float)read_2register(ACC_Z);
		
		pitch = atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/M_PI; 
		roll = atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/M_PI;
		
		
		//printf("roll: %.2f, pitch: %.2f\n",roll,pitch);
		if(pitch>=15)			ud_control = 1;
		else if(pitch<=-2)		ud_control = 2;
		else					ud_control = 0;
		if(roll>=8)				lr_control = 1;
		else if(roll<=-15)		lr_control = 2;
		else					lr_control = 0;
		serialPrintf(fd, "%d %d %d %d %d %d %d %d %d %d e", ud_control,lr_control,ss,gear,shift,center,speed,flap,ph_right,ph_left);
		delay(200);
	}
	serialClose(fd);
	return 0;
}