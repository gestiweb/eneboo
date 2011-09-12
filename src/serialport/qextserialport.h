
#ifndef _QEXTSERIALPORT_H_
#define _QEXTSERIALPORT_H_

#include "qiodevice.h"
#include "qfile.h"
#include "qvaluestack.h"

#ifdef Q_WS_WIN
	#include <windows.h>
#else
	#include <cstdlib>
	#include <unistd.h>
	#include <termios.h>
	#include <sys/ioctl.h>

	/*ensure handling of CRTSCTS constant*/
	#ifdef CNEW_RTSCTS
		#ifndef CRTSCTS
			#define CRTSCTS CNEW_RTSCTS
		#endif
	#else
		#ifndef CRTSCTS
			#define CRTSCTS     0
		#endif
	#endif
#endif

#ifdef _QESP_LOGTRACE_
	#define QESP_LOGTRACE(i) qWarning("["+Name+"] QextSerialPort::"+i);
#else
	#define QESP_LOGTRACE(i)
#endif

class QextSerialPort
 : public QIODevice
{
public:
	enum BaudRateType
	{
		BAUD50,      //POSIX ONLY
		BAUD75,      //POSIX ONLY
		BAUD110,
		BAUD134,     //POSIX ONLY
		BAUD150,     //POSIX ONLY
		BAUD200,     //POSIX ONLY
		BAUD300,
		BAUD600,
		BAUD1200,
		BAUD1800,    //POSIX ONLY
		BAUD2400,
		BAUD4800,
		BAUD9600,
		BAUD14400,   //WINDOWS ONLY
		BAUD19200,
		BAUD38400,
		BAUD56000,   //WINDOWS ONLY
		BAUD57600,
		BAUD76800,   //POSIX ONLY
		BAUD115200,
		BAUD128000,  //WINDOWS ONLY
		BAUD256000   //WINDOWS ONLY
	};

	enum DataBitsType
	{
		DATA_5,      //only for compatibility
		DATA_6,
		DATA_7,
		DATA_8
	};

	enum ParityType
	{
		PAR_NONE,
		PAR_ODD,
		PAR_EVEN,
		PAR_MARK,    //WINDOWS ONLY
		PAR_SPACE    //WINDOWS directly, POSIX simulated
	};

	enum StopBitsType
	{
		STOP_1,
		STOP_1_5,    //WINDOWS ONLY
		STOP_2
	};

	enum FlowType
	{
		FLOW_OFF,
		FLOW_HARDWARE,
		FLOW_XONXOFF
	};

	/*structure to contain port settings*/
	struct PortSettings
	{
		BaudRateType BaudRate;
		DataBitsType DataBits;
		ParityType Parity;
		StopBitsType StopBits;
		FlowType FlowControl;
		unsigned long Timeout_Sec;
		unsigned long Timeout_Millisec;
	};

	QextSerialPort( const QString& name = "" );
	virtual ~QextSerialPort();

	virtual bool open( int mode = 0 );
	virtual void close();
	virtual void flush();

	QIODevice::Offset size() const;
#ifdef _QESP_LOGTRACE_
	QIODevice::Offset at() const;  // non-pure virtual
#endif
	bool at( QIODevice::Offset );  // non-pure virtual
#ifdef _QESP_LOGTRACE_
	bool atEnd() const;  // non-pure virtual
#endif

	virtual Q_LONG readBlock( char *data, Q_ULONG maxSize );
	virtual Q_LONG writeBlock( const char *data, Q_ULONG size );

#ifdef _QESP_LOGTRACE_
	Q_LONG readLine( char *data, Q_ULONG maxlen );  // non-pure virtual
	QByteArray readAll();  // non-pure virtual
#endif

	int getch();
	int putch( int );
	int ungetch( int );

	Q_ULONG bytesAvailable() const;

	void setName( const QString& name );
	QString name();

	virtual void setBaudRate( BaudRateType );
	virtual void setDataBits( DataBitsType );
	virtual void setParity( ParityType );
	virtual void setStopBits( StopBitsType );
	void setFlowControl( FlowType );
	void setTimeout( unsigned long=0, unsigned long=0 );

	BaudRateType baudRate() const { return Settings.BaudRate; };
	DataBitsType setDataBits() const { return Settings.DataBits; };
	ParityType setParity() const { return Settings.Parity; };
	StopBitsType setStopBits() const { return Settings.StopBits; };
	FlowType setFlowControl() const { return Settings.FlowControl; };

	void setRts( bool set = TRUE );
	void setDtr( bool set = TRUE );

	bool cts() const;
	bool dsr() const;
	bool dcd() const;
	bool ri() const;

private:
	QextSerialPort( const QextSerialPort& sp );
	QextSerialPort& operator=( const QextSerialPort& sp );

protected:
#ifdef Q_WS_WIN
	HANDLE Win_Handle;
	DCB Win_CommConfig;
	COMMTIMEOUTS Win_CommTimeouts;
#else
	QFile* Posix_File;
	struct termios Posix_CommConfig;
#endif

	QString Name;
	PortSettings Settings;
private:
	QValueStack<char> ungetchBuffer;
};

#endif
