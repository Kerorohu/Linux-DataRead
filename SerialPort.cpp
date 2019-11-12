#include "SerialPort.hpp"
#include <iostream>
#include <string.h>
#include <unistd.h>
//#include <unistd.h> //For usleep()
using namespace std;


//////////////////////////////////////////////
int num = 0;


SerialPort::SerialPort(const string portName, int baudRate)
{
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
		{//如果有收到東西
			nullRecvCounter=0;//計數歸0
			//printf("%02x",nRead);
			//printf("%02x\n", strRxBuf[0]);
			//printf("%02x\n", strRxBuf[1]);
			//strRxFullMsg.append((char*)strRxBuf, nRead);//加入字串才用signed
			//////////////////////////////////////////
			ret = flow_parse_char(strRxBuf[0]);
			if (!ret) {
					short x = 0, y = 0;
					//struct tm tmTmp;
					char sttemp[32];
					//localtime_s(&tmTmp,&t);
					//asctime_s(sttemp, &tmTmp);
					x = updata.flow_x_integral;
					y = updata.flow_y_integral;
					//timespan = updata.integration_timespan;
					if (valuex != -999) {
							x = filter(x, valuex, 50);
					}
					if (valuey != -999) {
							y = filter(y, valuey, 50);
					}
					valuex = x;
					valuey = y;
					printf("x=%4d , y=%4d\n", x, y);
					// printf_s("time is: %s\n", sttemp);
					//f << x << " " << y << "\n" << "time:" << sttemp << "\n";
					//printf("num=%d \n", num);
					//printf("time=%d \n", timespan);//这个是光流间隔时间
			}
			else if (nRead < 0) {
			printf("Error from read: %d: %s\n", nRead, strerror(errno));
			}
///////////////////////////////////////////////////////////////////////

		}
		else
		{
			nullFirstRecvCounter++;
			//printf("nullFirstRecvCounter = %d\n",nullFirstRecvCounter);
		}

	}while(nullFirstRecvCounter+nullRecvCounter < 1000);//設定time out為1000mSec

	return strRxBuf[0];
}

//////////////////////////////////////////////////////////
int16_t SerialPort::flow_parse_char(uint8_t ch)
{
    num++;
    int16_t ret = 1;
    static int16_t s = 0, p = 0;
    static uint8_t Xor_r = 0x00, Xor_c = 0x00;

    switch (s)
    {
        case 0:
            if (ch == 0xFE)
            {
                s = 1;
                //printf("Got FE\n");
            }
            break;
        case 1:
            if (ch == 0x0A)
            {
                s = 2;
                p = 0;
                Xor_c = 0x00;
                //printf("Got 0A\n");
            }
            else
                s = 0;
            break;
        case 2:
            ((char *)&updata)[p++] = ch;
            Xor_c ^= ch;
            if (p == 10) {
                s = 3;
                p = 0;
            }
            break;
        case 3: //crc
            s = 4;
            Xor_r = ch;
            break;
        case 4://end
            if (ch == 0x55) {
                //printf("Got 0x55\n");
                if (Xor_r == Xor_c) {
                    ret = 0;
                }
                else
                    ret = 2;
            }
            s = 0;
            break;
        default:
            break;
    }
    return ret;
}

int SerialPort::filter(int new_value,int value,int a) {
    return ((100 - a)*value + a * new_value)/100;
}
///////////////////////////////////////////////////
