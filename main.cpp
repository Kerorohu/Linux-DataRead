
#include <iostream>
#include <string>
#include <unistd.h> //For usleep()
#include <pthread.h>
#include "SerialPort.hpp"
#include <thread>
using namespace std;

//struct A
//	{
//		SerialPort * p;
//		void (SerialPort::*Recv)();
//	};

int main(int argc, char *argv[])
{	
	pthread_t id;
	argc = 2 ; argv[1] = (char*)"ttyUSB1"; //dummy input

	string strModemDevice = "/dev/";
	//while(NULL == argv[1])
	//{
	//	char cInput;
	//	cout << "欠缺參數(ttyS0 或 ttyUSB0 之類的)" << endl;
	//	cout << "要幫懶惰的你補上\"ttyUSB0\"嗎？" << endl;
	//	cout << "[Y/n] "; cin >> cInput;
//
//		if(toupper(cInput) == 'N')
//		{
//			cout << "結束程式" << endl;
//			return 0;//跳出程式
//		}
//		else if(toupper(cInput) == 'Y')
//		{
//			argc = 2; argv[1] = (char*)"ttyUSB1"; //dummy input
//			break;
//		}
//		system("clear");
//	}

	cout << "argv[1]: " << argv[1] << endl;
	strModemDevice.append(argv[1]); //在string後面加上一個char*字串
	cout << "strModemDevice = " << strModemDevice << endl;//顯示總字串

	//權限取得
	string strPermissionGetCommand = "sudo chmod 666 " + strModemDevice;
	system(strPermissionGetCommand.c_str());

	//建立对象
	SerialPort* serialPort = new SerialPort(strModemDevice, SerialPort::BR500000);
	serialPort->startUp();

	while(serialPort->isOpen())
	//if(serialPort->isOpen())
	{
		//cout << "Pass!" << endl;

		/*//把 Hex String 轉換到 Byte Array
		string strSendMsg = "HelloWorld!";

		//送出 Byte Array 資料
		cout << "strSendMsg = " << strSendMsg << endl;
		cout << "send return = " << serialPort->Send(strSendMsg) << endl;*/
		
		//printf("DIS_X = %4d\n", serialPort->OF_DIS_X);
		printf("Dx = %4d ,Dy = %4d\n",serialPort->OF_DX2FIX,serialPort->OF_DY2FIX);
		usleep(20*1000*5);

		//接收字串
		//serialPort->Recv();
		//printf("data = %02x\n", strRx);
		//cout << "Serial Port Receive  : " << strRx << endl;
	}

	serialPort->Close();
	delete(serialPort);
}
