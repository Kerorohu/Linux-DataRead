#include "SerialPort.hpp"
#include <iostream>
#include <string.h>
#include <unistd.h>

//#include <unistd.h> //For usleep()
using namespace std;
uint8_t		OF_STATE,OF_QUALITY;
int8_t		OF_DX,OF_DY;
int16_t		OF_DX2,OF_DY2,OF_DX2FIX,OF_DY2FIX;
uint16_t	OF_ALT,OF_ALT2;
int16_t		OF_GYR_X,OF_GYR_Y,OF_GYR_Z;
int16_t		OF_GYR_X2,OF_GYR_Y2,OF_GYR_Z2;
int16_t		OF_ACC_X,OF_ACC_Y,OF_ACC_Z;
int16_t		OF_ACC_X2,OF_ACC_Y2,OF_ACC_Z2;
float		OF_ATT_ROL,OF_ATT_PIT,OF_ATT_YAW;
float		OF_ATT_S1,OF_ATT_S2,OF_ATT_S3,OF_ATT_S4;

//////////////////////////////////////////////
int num = 0;
static u8 _data_len = 0;
static u8 state = 0;

void AnoOF_DataAnl(uint8_t *data_buf,uint8_t num);

SerialPort::SerialPort(const string portName, int baudRate)
{

	//in.open("dataTest.txt",ios::trunc);
	printf("baudRate code = %d\n", baudRate);
	//this->nFd = OpenDevice(portName);
	this->nFd = open(portName.c_str(), O_RDWR|O_NOCTTY|O_NDELAY );
	if(-1 == this->nFd)
	{
		this->nFd = -1;
	}

	struct termios options = { 0 };   //宣告一個設定 comport 所需要的結構體 並且清空內部

	/* c_cflag 控制模式：
	 * CLOCAL:忽略任何modem status lines
	 * CREAD:啟動字元接收
	 * CS8:傳送或接收時，使用八個位元
	 */
	options.c_cflag = (baudRate | CLOCAL | CREAD | CS8); //依序,設定 baud rate,不改變 comport 擁有者, 接收致能, 8 data bits

  printf("baudRate code1 = %d\n", baudRate);

	cfsetispeed( &options , baudRate );
	cfsetospeed( &options , baudRate );

	options.c_cc[ VTIME ] = 1;	//10 = 1秒,定義等待的時間，單位是百毫秒
	options.c_cc[ VMIN ] = 0;	//定義了要求等待的最小字節數,這個基本上給 0
	tcflush(this->nFd , TCIOFLUSH );	// 刷新和立刻寫進去fd

	if ( (tcsetattr( this->nFd , TCSANOW , &options )) == -1 )
	{ //寫回本機設備,TCSANOW >> 立刻改變數值
		this->nFd = -1;
	}
}

bool SerialPort::isOpen()
{//檢查port是否已經開啟
	return (-1 == this->nFd) ? false : true;
}

void SerialPort::Close()
{
	 close(this->nFd);
}

SerialPort::~SerialPort() {
	// TODO Auto-generated destructor stub
	//應該有些什麼東西要手動free掉？ 目前看起來沒有
}

int SerialPort::OpenDevice(string strDevice)
{
	int	fd = open(strDevice.c_str(), O_RDWR|O_NOCTTY|O_NDELAY );
	if (-1 == fd)
	{
		perror("Can't Open Serial Port");
		return -1;
	}

	return fd;
}

int SerialPort::GetFileDescriptor()
{//暫時開放，其實物件導向以後，這個資訊不應該public。不然等於是破壞port操作封裝性
	return this->nFd;
}

//寫入字串資訊
int SerialPort::Send(string strOutMsg)
{
	int nCount = write(this->nFd, strOutMsg.c_str(), strOutMsg.length());
	return nCount;
}

//寫入二進位資訊，所以必須自己給長度。
int SerialPort::Send(unsigned char* strOutMsg, size_t nbyte)
{
	int nCount = write(this->nFd, strOutMsg, nbyte);
	return nCount;
}

//SerialPort::~SerialPort(){
	//in.close();
//}

unsigned char SerialPort::Recv(void)
{//接收資訊

	const static size_t rxBufferSize = 1;
	unsigned char strRxBuf[rxBufferSize]; //接收要用unsigned
	string strRxFullMsg = "";
	int nullRecvCounter=0;
	int nullFirstRecvCounter=0;
	int valuex = -999;
	int valuey = -999;
	int bt = 0;
	int ret;
	printf("read start\n");
	do{
		memset(strRxBuf, 0, rxBufferSize); //清空緩衝
		int nRead = read(this->nFd, strRxBuf, rxBufferSize); //接收資料
		usleep(1000);
		//cout << "nRead= " << nRead << endl;
		if(0 >= nRead && !strRxFullMsg.empty())
		{//如果這一次沒有收到東西，且從未收過資料
			nullRecvCounter++;
			//printf("nullRecvCounter = %d\n",nullRecvCounter);
			if(nullRecvCounter>10)break;
		}
		else if(0 < nRead)
		{
			AnoOF_GetOneByte(strRxBuf[0]);

			//printf("%02x", strRxBuf[0]);

		}
		else
		{
			nullFirstRecvCounter++;
			//printf("nullFirstRecvCounter = %d\n",nullFirstRecvCounter);
		}

	}while(nullFirstRecvCounter+nullRecvCounter < 1000);//設定time out為1000mSec

	return strRxBuf[0];
}





static uint8_t _datatemp[50];
static u8 _data_cnt = 0;
static u8 anoof_data_ok;

void SerialPort::AnoOF_DataAnl_Task(u8 dT_ms)
{
//	if(anoof_data_ok)
//	{
//		anoof_data_ok = 0;
//		AnoOF_DataAnl(_datatemp,_data_cnt+5);
//	}

	AnoOF_Check(dT_ms);
}


//AnoOF_GetOneByte�ǳ������ݽ�������������ÿ���յ�һ�ֽڹ������ݣ����ñ�����һ�Σ������������Ǵ����յ�������
//�����������α����ã����ս��յ�������һ֡���ݺ󣬻��Զ��������ݽ�������AnoOF_DataAnl
void SerialPort::AnoOF_GetOneByte(uint8_t data)
{


	//printf("start getbyte\n");
	//printf("%02x", data);
	if(state==0&&data==0xAA)
	{
		state=1;
		_datatemp[0]=data;
		//printf("got tittle\n");
	}
	else if(state==1&&data==0x22)	//Դ��ַ
	{
		state=2;
		_datatemp[1]=data;
		//printf("got 2\n");
	}
	else if(state==2)			//Ŀ�ĵ�ַ
	{
		state=3;
		_datatemp[2]=data;
		//printf("got 3\n");
	}
	else if(state==3)			//������
	{
		state = 4;
		_datatemp[3]=data;
	}
	else if(state==4)			//����
	{
		state = 5;
		_datatemp[4]=data;
		_data_len = data;
		_data_cnt = 0;
	}
	else if(state==5&&_data_len>0)
	{
		_data_len--;
		_datatemp[5+_data_cnt++]=data;
		if(_data_len==0)
			state = 6;
	}
	else if(state==6)
	{
		state = 0;
		_datatemp[5+_data_cnt]=data;
		AnoOF_DataAnl(_datatemp,_data_cnt+6);//anoof_data_ok = 1 ;//
		printf("Dx = %02d", OF_DX);
		printf(" Dy = %02d\n", OF_DY);
		printf("Dx2 = %02d", OF_DX2);
		printf(" Dy2 = %02d\n", OF_DY2);
		//in << "of_dx="<<OF_DX<<" of_dy=" <<OF_DY << endl;
	//	in << "of_dx2="<<OF_DX2<<" of_dy2=" <<OF_DY2 << endl;

	}
	else
		state = 0;
}
//AnoOF_DataAnlΪ�������ݽ�������������ͨ���������õ�����ģ�������ĸ�������
//�������ݵ����壬��������������ģ��ʹ���ֲᣬ����ϸ�Ľ���
static u8 of_check_f[2];
static u16 of_check_cnt[2] = { 10000,10000 };
void SerialPort::AnoOF_Check(u8 dT_ms)
{
	for(u8 i=0;i<2;i++)
	{
		if(of_check_f[i] == 0 )
		{
			if(of_check_cnt[i]<10000)
			{
				of_check_cnt[i] += dT_ms;
			}
		}
		else
		{
			of_check_cnt[i] = 0;
		}


		of_check_f[i] = 0;
	}


	/*if(of_check_cnt[0] > 1000 || of_check_cnt[1] > 1000)
	{
		sens_hd_check.of_ok = 0;
	}
	else
	{
		sens_hd_check.of_ok = 1;
	}
	*/



}

void AnoOF_DataAnl(uint8_t *data_buf,uint8_t num)
{
	u8 sum = 0;
	for(u8 i=0;i<(num-1);i++)
		sum += *(data_buf+i);
	if(!(sum==*(data_buf+num-1)))		return;

	if(*(data_buf+3)==0X51)//������Ϣ
	{
		if(*(data_buf+5)==0)//ԭʼ������Ϣ
		{
			OF_STATE 		= *(data_buf+6);
			OF_DX  		= *(data_buf+7);
			OF_DY  		= *(data_buf+8);
			OF_QUALITY  	= *(data_buf+9);
		}
		else if(*(data_buf+5)==1)//�ںϺ�������Ϣ
		{
			OF_STATE 		= *(data_buf+6);
			OF_DX2		= (int16_t)(*(data_buf+7)<<8)|*(data_buf+8) ;
			OF_DY2		= (int16_t)(*(data_buf+9)<<8)|*(data_buf+10) ;
			OF_DX2FIX	= (int16_t)(*(data_buf+11)<<8)|*(data_buf+12) ;
			OF_DY2FIX	= (int16_t)(*(data_buf+13)<<8)|*(data_buf+14) ;
			//OF_DIS_X = (int16_t)(*(data_buf+15)<<8)|*(data_buf+16) ;
			//OF_DIS_Y = (int16_t)(*(data_buf+17)<<8)|*(data_buf+18) ;
			OF_QUALITY  	= *(data_buf+19);

			of_check_f[0] = 1;
		}
	}
	if(*(data_buf+3)==0X52)//�߶���Ϣ
	{
		if(*(data_buf+5)==0)//ԭʼ�߶���Ϣ
		{
			OF_ALT = (uint16_t)(*(data_buf+6)<<8)|*(data_buf+7) ;
			of_check_f[1] = 1;
		}
		else if(*(data_buf+5)==1)//�ںϺ��߶���Ϣ
		{
			OF_ALT2 = (uint16_t)(*(data_buf+6)<<8)|*(data_buf+7) ;
		}
	}
}
