#ifndef _SERIALPORT_HPP_
#define _SERIALPORT_HPP_

#include     <stdio.h>
#include     <stdlib.h>
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>
#include     <termios.h>
#include     <errno.h>
#include	 <string>

using namespace std;

////////////////////////////////////////////
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

typedef struct optical_flow_data
{
    int16_t		flow_x_integral;
    int16_t		flow_y_integral;
    uint16_t   	integration_timespan;
    uint16_t   	ground_distance;
    uint8_t    	quality;
    uint8_t    	version;
}UpixelsOpticalFlow;
///////////////////////////////////////

typedef int BaudRate;

class SerialPort {
public:
	SerialPort(const string portName, BaudRate baudRate);
	virtual ~SerialPort();
	bool isOpen();
	void Close();

	int Send(string);
	int Send(unsigned char*, size_t);
	unsigned char Recv(void);
	///////////////////////////////////
	UpixelsOpticalFlow updata;
	int filter(int new_value, int value, int a);
	int16_t flow_parse_char(uint8_t ch);
	//////////////////////////////////////////
	static const BaudRate BR115200 = 0010002;
	static const BaudRate  BR19200 = 0000016;
	static const BaudRate   BR9600 = 0000015;

	int GetFileDescriptor();
private:

	int OpenDevice(string);
	int nFd;
};

#endif /* SERIALPORT_H_ */
