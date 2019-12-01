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
typedef unsigned char uint8_t;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned short uint16_t;

///////////////////////////////////
extern uint8_t 	OF_STATE,OF_QUALITY;

extern int8_t	OF_DX,OF_DY;

extern int16_t	OF_DX2,OF_DY2,OF_DX2FIX,OF_DY2FIX,OF_DIS_X,OF_DIS_Y;

extern uint16_t	OF_ALT,OF_ALT2;

extern int16_t	OF_GYR_X,OF_GYR_Y,OF_GYR_Z;

extern int16_t	OF_GYR_X2,OF_GYR_Y2,OF_GYR_Z2;

extern int16_t	OF_ACC_X,OF_ACC_Y,OF_ACC_Z;

extern int16_t	OF_ACC_X2,OF_ACC_Y2,OF_ACC_Z2;

extern float	OF_ATT_ROL,OF_ATT_PIT,OF_ATT_YAW;

extern float	OF_ATT_S1,OF_ATT_S2,OF_ATT_S3,OF_ATT_S4;

///////////////////////////////////////////

typedef int BaudRate;

class SerialPort {
public:
	SerialPort(const string portName, BaudRate baudRate);
	virtual ~SerialPort();
	bool isOpen();
	void Close();
	//ofstream in;
	FILE *fp;
	int Send(string);
	int Send(unsigned char*, size_t);
	void Recv(void);
	void start();
	void startUp();

	uint8_t		OF_STATE,OF_QUALITY;
	int8_t		OF_DX,OF_DY;
	int16_t		OF_DX2,OF_DY2,OF_DX2FIX,OF_DY2FIX,OF_DIS_X,OF_DIS_Y;
	uint16_t	OF_ALT,OF_ALT2;
	int16_t		OF_GYR_X,OF_GYR_Y,OF_GYR_Z;
	int16_t		OF_GYR_X2,OF_GYR_Y2,OF_GYR_Z2;
	int16_t		OF_ACC_X,OF_ACC_Y,OF_ACC_Z;
	int16_t		OF_ACC_X2,OF_ACC_Y2,OF_ACC_Z2;
	float		OF_ATT_ROL,OF_ATT_PIT,OF_ATT_YAW;
	float		OF_ATT_S1,OF_ATT_S2,OF_ATT_S3,OF_ATT_S4;


	void AnoOF_DataAnl(uint8_t *data_buf,uint8_t num);
	
	void AnoOF_GetOneByte(uint8_t data);
	//////////////////////////////////////////
	static const BaudRate BR115200 = 0010002;
	static const BaudRate  BR19200 = 0000016;
	static const BaudRate   BR9600 = 0000015;
	static const BaudRate BR500000 = 0010005;

	int GetFileDescriptor();
private:

	void AnoOF_DataAnl_Task(u8 dT_ms);
	void AnoOF_Check(u8 dT_ms);
	int OpenDevice(string);
	int nFd;

};

#endif /* SERIALPORT_H_ */
