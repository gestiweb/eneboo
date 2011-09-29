
#include "qextserialport.h"

QextSerialPort::QextSerialPort( const QString& name )
{
QESP_LOGTRACE("Constructor("+name+")")

	setName( name );
	setType( IO_Sequential );

	Settings.BaudRate = QextSerialPort::BAUD115200;
	Settings.DataBits = QextSerialPort::DATA_8;
	Settings.Parity = QextSerialPort::PAR_NONE;
	Settings.StopBits = QextSerialPort::STOP_1;
	Settings.FlowControl = QextSerialPort::FLOW_HARDWARE;
	Settings.Timeout_Sec = 0;
	Settings.Timeout_Millisec = 500;

#ifdef Q_WS_WIN
	Win_Handle = INVALID_HANDLE_VALUE;
#else
	Posix_File = 0;
#endif
}

QextSerialPort::~QextSerialPort()
{
QESP_LOGTRACE("Destructor")

	if( isOpen() )
		close();
}

bool QextSerialPort::open( int )
{
QESP_LOGTRACE("open")

	if( ! isOpen() && ! Name.isEmpty() )
	{
#ifdef Q_WS_WIN
		Win_Handle = CreateFileA( Name, GENERIC_READ|GENERIC_WRITE,
						0, NULL, OPEN_EXISTING, 0, NULL );
		if( Win_Handle != INVALID_HANDLE_VALUE )
		{
			setMode( IO_Async|IO_Raw|IO_ReadWrite );
			setState( IO_Open );

			GetCommState( Win_Handle, &Win_CommConfig );

			Win_CommConfig.fBinary = TRUE;
			Win_CommConfig.fInX = FALSE;
			Win_CommConfig.fOutX = FALSE;
			Win_CommConfig.fAbortOnError = FALSE;
			Win_CommConfig.fNull = FALSE;

			setBaudRate( Settings.BaudRate );
			setDataBits( Settings.DataBits );
			setParity( Settings.Parity );
			setStopBits( Settings.StopBits );
			setFlowControl( Settings.FlowControl );
			setTimeout( Settings.Timeout_Sec, Settings.Timeout_Millisec );

			SetCommState( Win_Handle, &Win_CommConfig );
		}
#else
		Posix_File = new QFile( Name );
		if( Posix_File->open( IO_Async|IO_Raw|IO_ReadWrite ) )
		{
			setMode( IO_Async|IO_Raw|IO_ReadWrite );
			setState( IO_Open );

			tcgetattr(Posix_File->handle(), &Posix_CommConfig);

			Posix_CommConfig.c_cflag |= (CREAD|CLOCAL);
			Posix_CommConfig.c_lflag &= (~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ISIG));
			Posix_CommConfig.c_iflag &= (~(INPCK|IGNPAR|PARMRK|ISTRIP|ICRNL|IXANY));
			Posix_CommConfig.c_oflag &= (~OPOST);
			Posix_CommConfig.c_cc[VMIN] = 0;
			Posix_CommConfig.c_cc[VINTR] = _POSIX_VDISABLE;
			Posix_CommConfig.c_cc[VQUIT] = _POSIX_VDISABLE;
			Posix_CommConfig.c_cc[VSTART] = _POSIX_VDISABLE;
			Posix_CommConfig.c_cc[VSTOP] = _POSIX_VDISABLE;
			Posix_CommConfig.c_cc[VSUSP] = _POSIX_VDISABLE;

			setBaudRate( Settings.BaudRate );
			setDataBits( Settings.DataBits );
			setParity( Settings.Parity );
			setStopBits( Settings.StopBits );
			setFlowControl( Settings.FlowControl );
			setTimeout( Settings.Timeout_Sec, Settings.Timeout_Millisec );

			tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
		} else {
			delete Posix_File; Posix_File = 0;
		}
#endif
	}
	return isOpen();
}

void QextSerialPort::close()
{
QESP_LOGTRACE("close")

#ifdef Q_WS_WIN
	CloseHandle( Win_Handle );
	Win_Handle = INVALID_HANDLE_VALUE;
#else
	Posix_File->close();
	delete Posix_File; Posix_File = 0;
#endif
	setState( 0 );
}

void QextSerialPort::flush()
{
QESP_LOGTRACE("flush")

#ifdef Q_WS_WIN
	FlushFileBuffers( Win_Handle );
#else
	tcflush(Posix_File->handle(), TCOFLUSH);
#endif
}

QIODevice::Offset QextSerialPort::size() const
{
QESP_LOGTRACE("size")

	return bytesAvailable();
}

#ifdef _QESP_LOGTRACE_
QIODevice::Offset  QextSerialPort::at() const
{
QESP_LOGTRACE("QIODevice::at")

	return QIODevice::at();
} // non-pure virtual
#endif

bool QextSerialPort::at( QIODevice::Offset /*pos*/ )
{
QESP_LOGTRACE("at(Offset)")

	return FALSE;
} // non-pure virtual

#ifdef _QESP_LOGTRACE_
bool QextSerialPort::atEnd() const
{
QESP_LOGTRACE("QIODevice::atEnd")

	return QIODevice::atEnd();
} // non-pure virtual
#endif

Q_LONG QextSerialPort::readBlock( char *data, Q_ULONG maxSize )
{
QESP_LOGTRACE("readBlock")

	Q_ULONG ungetchRead = 0;
	while( ! ungetchBuffer.isEmpty() && ungetchRead < maxSize )
		data[ ungetchRead++ ] = ungetchBuffer.pop();

	Q_LONG deviceRead = 0;
#ifdef Q_WS_WIN
	DWORD Win_ErrorMask = 0;
	COMSTAT Win_ComStat;
	ClearCommError( Win_Handle, &Win_ErrorMask, &Win_ComStat );

	DWORD Win_BytesRead = 0;
	if( ! ReadFile( Win_Handle, (void*)(data + ungetchRead), (DWORD)(maxSize - ungetchRead), &Win_BytesRead, NULL ) )
		deviceRead = -1;
	else
		deviceRead = (int)Win_BytesRead;
#else
	deviceRead = Posix_File->readBlock( data + ungetchRead, maxSize - ungetchRead );
#endif
	if( deviceRead == -1 )
		return -1;

	return ungetchRead + deviceRead;
}

Q_LONG QextSerialPort::writeBlock( const char *data, Q_ULONG size )
{
QESP_LOGTRACE("writeBlock")

	int deviceWritten = 0;
#ifdef Q_WS_WIN
	DWORD Win_BytesWritten;
	if( ! WriteFile( Win_Handle, (void*)data, (DWORD)size, &Win_BytesWritten, NULL ) )
	{
		deviceWritten = -1;
	} else {
		deviceWritten = (int)Win_BytesWritten;
	}
#else
	deviceWritten = Posix_File->writeBlock( data, size );
#endif
	return deviceWritten;
}

#ifdef _QESP_LOGTRACE_
Q_LONG QextSerialPort::readLine( char *data, Q_ULONG maxlen )
{
QESP_LOGTRACE("QIODevice::readLine")

	return QIODevice::readLine( data, maxlen );
} // non-pure virtual

QByteArray QextSerialPort::readAll()
{
QESP_LOGTRACE("QIODevice::readAll")

	return QIODevice::readAll();
} // non-pure virtual
#endif

int QextSerialPort::getch()
{
QESP_LOGTRACE("getch")

	char ch;
	if( readBlock( &ch, 1 ) == 1 )
		return (uchar)ch;
	else
		return -1;
}

int QextSerialPort::putch( int ch )
{
QESP_LOGTRACE("putch")

	char data = (char)ch;
	if ( writeBlock( &data, 1 ) == 1 )
		return ch;
	else
		return -1;
}

int QextSerialPort::ungetch( int ch )
{
QESP_LOGTRACE("ungetch")

	ungetchBuffer.push( (char)ch );
	return ch;
}

Q_ULONG QextSerialPort::bytesAvailable() const
{
QESP_LOGTRACE("bytesAvailable")

	int nbytes;
#ifdef Q_WS_WIN
	DWORD Win_ErrorMask = 0;
	COMSTAT Win_ComStat;
	ClearCommError( Win_Handle, &Win_ErrorMask, &Win_ComStat );

	nbytes = Win_ComStat.cbInQue;
#else
	if( ioctl( Posix_File->handle(), FIONREAD, &nbytes) == -1 )
		nbytes = 0;
#endif
	return ungetchBuffer.count() + (Q_ULONG)nbytes;
}

void QextSerialPort::setName( const QString& name )
{
QESP_LOGTRACE("setPortName("+name+")")

	Name = name;
}

QString QextSerialPort::name()
{
QESP_LOGTRACE("getPortName")

	return Name;
}

void QextSerialPort::setBaudRate( BaudRateType baudRate )
{
QESP_LOGTRACE("setBaudRate")

	if( Settings.BaudRate!=baudRate )
	{
		Settings.BaudRate = baudRate;
	}

	if ( isOpen() )
	{
#ifdef Q_WS_WIN
		switch( baudRate ) {
			/*110 baud*/
			case QextSerialPort::BAUD110:
				Win_CommConfig.BaudRate = CBR_110;
				break;

			/*300 baud*/
			case QextSerialPort::BAUD300:
				Win_CommConfig.BaudRate = CBR_300;
				break;

			/*600 baud*/
			case QextSerialPort::BAUD600:
				Win_CommConfig.BaudRate = CBR_600;
				break;

			/*1200 baud*/
			case QextSerialPort::BAUD1200:
				Win_CommConfig.BaudRate = CBR_1200;
				break;

			/*2400 baud*/
			case QextSerialPort::BAUD2400:
				Win_CommConfig.BaudRate = CBR_2400;
				break;

			/*4800 baud*/
			case QextSerialPort::BAUD4800:
				Win_CommConfig.BaudRate = CBR_4800;
				break;

			/*9600 baud*/
			case QextSerialPort::BAUD9600:
				Win_CommConfig.BaudRate = CBR_9600;
				break;

			/*19200 baud*/
			case QextSerialPort::BAUD19200:
				Win_CommConfig.BaudRate = CBR_19200;
				break;

			/*38400 baud*/
			case QextSerialPort::BAUD38400:
				Win_CommConfig.BaudRate = CBR_38400;
				break;

			/*57600 baud*/
			case QextSerialPort::BAUD57600:
				Win_CommConfig.BaudRate = CBR_57600;
				break;

			/*115200 baud*/
			case QextSerialPort::BAUD115200:
				Win_CommConfig.BaudRate = CBR_115200;
				break;
			default:
				qDebug("Unsupported BaudRate!");
				exit(-1);
		}
		SetCommState(Win_Handle, &Win_CommConfig);
#else
		switch( baudRate ) {
			/*110 baud*/
			case QextSerialPort::BAUD110:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B110;
		#else
				cfsetispeed( &Posix_CommConfig, B110 );
				cfsetospeed( &Posix_CommConfig, B110 );
		#endif
				break;

			/*300 baud*/
			case QextSerialPort::BAUD300:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B300;
		#else
				cfsetispeed( &Posix_CommConfig, B300 );
				cfsetospeed( &Posix_CommConfig, B300 );
		#endif
				break;

			/*600 baud*/
			case QextSerialPort::BAUD600:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B600;
		#else
				cfsetispeed( &Posix_CommConfig, B600 );
				cfsetospeed( &Posix_CommConfig, B600 );
		#endif
				break;

			/*1200 baud*/
			case QextSerialPort::BAUD1200:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B1200;
		#else
				cfsetispeed( &Posix_CommConfig, B1200 );
				cfsetospeed( &Posix_CommConfig, B1200 );
		#endif
				break;

			/*2400 baud*/
			case QextSerialPort::BAUD2400:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B2400;
		#else
				cfsetispeed( &Posix_CommConfig, B2400 );
				cfsetospeed( &Posix_CommConfig, B2400 );
		#endif
				break;

			/*4800 baud*/
			case QextSerialPort::BAUD4800:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B4800;
		#else
				cfsetispeed( &Posix_CommConfig, B4800 );
				cfsetospeed( &Posix_CommConfig, B4800 );
		#endif
				break;

			/*9600 baud*/
			case QextSerialPort::BAUD9600:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B9600;
		#else
				cfsetispeed( &Posix_CommConfig, B9600 );
				cfsetospeed( &Posix_CommConfig, B9600 );
		#endif
				break;

			/*19200 baud*/
			case QextSerialPort::BAUD19200:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B19200;
		#else
				cfsetispeed( &Posix_CommConfig, B19200 );
				cfsetospeed( &Posix_CommConfig, B19200 );
		#endif
				break;

			/*38400 baud*/
			case QextSerialPort::BAUD38400:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B38400;
		#else
				cfsetispeed( &Posix_CommConfig, B38400 );
				cfsetospeed( &Posix_CommConfig, B38400 );
		#endif
				break;

			/*57600 baud*/
			case QextSerialPort::BAUD57600:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B57600;
		#else
				cfsetispeed( &Posix_CommConfig, B57600 );
				cfsetospeed( &Posix_CommConfig, B57600 );
		#endif
				break;

			/*115200 baud*/
			case QextSerialPort::BAUD115200:
		#ifdef CBAUD
				Posix_CommConfig.c_cflag &= (~CBAUD);
				Posix_CommConfig.c_cflag |= B115200;
		#else
				cfsetispeed( &Posix_CommConfig, B115200 );
				cfsetospeed( &Posix_CommConfig, B115200 );
		#endif
				break;
			default:
				qDebug("Unsupported BaudRate!");
				exit(-1);
		}
		tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
#endif
	}
}

void QextSerialPort::setDataBits( DataBitsType dataBits )
{
QESP_LOGTRACE("setDataBits")

	if( Settings.DataBits != dataBits )
	{
		Settings.DataBits = dataBits;
	}

	if( isOpen() )
	{
#ifdef Q_WS_WIN
		switch( dataBits ) {
			/*6 data bits*/
			case QextSerialPort::DATA_6:
				Win_CommConfig.ByteSize = 6;
				break;

			/*7 data bits*/
			case QextSerialPort::DATA_7:
				Win_CommConfig.ByteSize = 7;
				break;

			/*8 data bits*/
			case QextSerialPort::DATA_8:
				Win_CommConfig.ByteSize = 8;
				break;
			default:
				qDebug("Unsupported DataBits!");
				exit(-1);
		}
		SetCommState( Win_Handle, &Win_CommConfig );
#else
		switch( dataBits ) {
			/*6 data bits*/
			case QextSerialPort::DATA_6:
				Posix_CommConfig.c_cflag &= (~CSIZE);
				Posix_CommConfig.c_cflag |= CS6;
				break;

			/*7 data bits*/
			case QextSerialPort::DATA_7:
				Posix_CommConfig.c_cflag &= (~CSIZE);
				Posix_CommConfig.c_cflag |= CS7;
				break;

			/*8 data bits*/
			case QextSerialPort::DATA_8:
				Posix_CommConfig.c_cflag &= (~CSIZE);
				Posix_CommConfig.c_cflag |= CS8;
				break;
			default:
				qDebug("Unsupported DataBits!");
				exit(-1);
		}
		tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
#endif
	}
}

void QextSerialPort::setParity( ParityType parity )
{
QESP_LOGTRACE("setParity")

	if( Settings.Parity != parity )
	{
		Settings.Parity = parity;
	}

	if( isOpen() )
	{
#ifdef Q_WS_WIN
		switch ( parity ) {
			/*no parity*/
			case QextSerialPort::PAR_NONE:
				Win_CommConfig.fParity = FALSE;
				break;

			/*odd parity*/
			case QextSerialPort::PAR_ODD:
				Win_CommConfig.Parity = (unsigned char)parity;
				Win_CommConfig.fParity = TRUE;
				break;

			/*even parity*/
			case QextSerialPort::PAR_EVEN:
				Win_CommConfig.Parity = (unsigned char)parity;
				Win_CommConfig.fParity = TRUE;
				break;
			default:
				qDebug("Unsupported ParityType!");
				exit(-1);
		}
		SetCommState( Win_Handle, &Win_CommConfig );
#else
		switch ( parity ) {
			/*no parity*/
			case QextSerialPort::PAR_NONE:
				Posix_CommConfig.c_cflag &= (~PARENB);
				break;

			/*odd parity*/
			case QextSerialPort::PAR_ODD:
				Posix_CommConfig.c_cflag |= (PARENB|PARODD);
				break;

			/*even parity*/
			case QextSerialPort::PAR_EVEN:
				Posix_CommConfig.c_cflag &= (~PARODD);
				Posix_CommConfig.c_cflag |= PARENB;
				break;
			default:
				qDebug("Unsupported ParityType!");
				exit(-1);
		}
		tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
#endif
	}
}

void QextSerialPort::setStopBits( StopBitsType stopBits )
{
QESP_LOGTRACE("setStopBits")

	if( Settings.StopBits != stopBits )
	{
		Settings.StopBits = stopBits;
	}

	if( isOpen() )
	{
#ifdef Q_WS_WIN
		switch( stopBits ) {
			/*one stop bit*/
			case QextSerialPort::STOP_1:
				Win_CommConfig.StopBits = ONESTOPBIT;
				break;

			/*two stop bits*/
			case QextSerialPort::STOP_2:
				Win_CommConfig.StopBits = TWOSTOPBITS;
				break;
			default:
				qDebug("Unsupported StopBits!");
				exit(-1);
		}
		SetCommState( Win_Handle, &Win_CommConfig );
#else
		switch( stopBits ) {
			/*one stop bit*/
			case QextSerialPort::STOP_1:
				Posix_CommConfig.c_cflag &= (~CSTOPB);
				break;

			/*two stop bits*/
			case QextSerialPort::STOP_2:
				Posix_CommConfig.c_cflag |= CSTOPB;
				break;
			default:
				qDebug("Unsupported StopBits!");
				exit(-1);
		}
		tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
#endif
	}
}

void QextSerialPort::setFlowControl( FlowType flow )
{
QESP_LOGTRACE("setFlowControl")

	if( Settings.FlowControl != flow )
	{
		Settings.FlowControl = flow;
	}

	if( isOpen() )
	{
#ifdef Q_WS_WIN
		switch( flow ) {
			/*no flow control*/
			case QextSerialPort::FLOW_OFF:
				Win_CommConfig.fOutxCtsFlow = FALSE;
				Win_CommConfig.fRtsControl = RTS_CONTROL_DISABLE;
				Win_CommConfig.fInX = FALSE;
				Win_CommConfig.fOutX = FALSE;
				break;

			/*software (XON/XOFF) flow control*/
			case QextSerialPort::FLOW_XONXOFF:
				Win_CommConfig.fOutxCtsFlow = FALSE;
				Win_CommConfig.fRtsControl = RTS_CONTROL_DISABLE;
				Win_CommConfig.fInX = TRUE;
				Win_CommConfig.fOutX = TRUE;
				break;

			case QextSerialPort::FLOW_HARDWARE:
				Win_CommConfig.fOutxCtsFlow = TRUE;
				Win_CommConfig.fRtsControl = RTS_CONTROL_HANDSHAKE;
				Win_CommConfig.fInX = FALSE;
				Win_CommConfig.fOutX = FALSE;
				break;
		}
		SetCommState( Win_Handle, &Win_CommConfig );
#else
		switch( flow ) {
			/*no flow control*/
			case QextSerialPort::FLOW_OFF:
				Posix_CommConfig.c_cflag &= (~CRTSCTS);
				Posix_CommConfig.c_iflag &= (~(IXON|IXOFF|IXANY));
				break;

			/*software (XON/XOFF) flow control*/
			case QextSerialPort::FLOW_XONXOFF:
				Posix_CommConfig.c_cflag &= (~CRTSCTS);
				Posix_CommConfig.c_iflag |= (IXON|IXOFF|IXANY);
				break;

			case QextSerialPort::FLOW_HARDWARE:
				Posix_CommConfig.c_cflag |= CRTSCTS;
				Posix_CommConfig.c_iflag &= (~(IXON|IXOFF|IXANY));
				break;
		}
		tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
#endif
	}
}

void QextSerialPort::setTimeout( unsigned long sec, unsigned long millisec )
{
QESP_LOGTRACE("setTimeout")

	Settings.Timeout_Sec = sec;
	Settings.Timeout_Millisec = millisec;

	if( isOpen() )
	{
#ifdef Q_WS_WIN
		Win_CommTimeouts.ReadIntervalTimeout = sec*1000 + millisec;
		Win_CommTimeouts.ReadTotalTimeoutMultiplier = sec*1000 + millisec;
		Win_CommTimeouts.ReadTotalTimeoutConstant = 0;
		Win_CommTimeouts.WriteTotalTimeoutMultiplier = sec*1000 + millisec;
		Win_CommTimeouts.WriteTotalTimeoutConstant = 0;
		SetCommTimeouts( Win_Handle, &Win_CommTimeouts );
#else
		tcgetattr( Posix_File->handle(), &Posix_CommConfig );
		Posix_CommConfig.c_cc[VTIME] = sec*10 + millisec/100;
		tcsetattr( Posix_File->handle(), TCSAFLUSH, &Posix_CommConfig );
#endif
	}
}

void QextSerialPort::setRts( bool set )
{
QESP_LOGTRACE("setRts")

	if ( isOpen() )
	{
#ifdef Q_WS_WIN
		if ( set ) {
			EscapeCommFunction( Win_Handle, SETRTS );
		}
		else {
			EscapeCommFunction( Win_Handle, CLRRTS );
		}
#else
		int status;
		ioctl( Posix_File->handle(), TIOCMGET, &status );
		if ( set ) {
			status |= TIOCM_RTS;
		}
		else {
			status &= ~TIOCM_RTS;
		}
		ioctl( Posix_File->handle(), TIOCMSET, &status );
#endif
	}
}

void QextSerialPort::setDtr( bool set )
{
QESP_LOGTRACE("setDtr")

	if ( isOpen() )
	{
#ifdef Q_WS_WIN
		if ( set ) {
			EscapeCommFunction( Win_Handle, SETDTR );
		}
		else {
			EscapeCommFunction( Win_Handle, CLRDTR );
		}
#else
		int status;
		ioctl( Posix_File->handle(), TIOCMGET, &status );
		if ( set ) {
			status |= TIOCM_DTR;
		}
		else {
			status &= ~TIOCM_DTR;
		}
		ioctl( Posix_File->handle(), TIOCMSET, &status );
#endif
	}
}

bool QextSerialPort::cts() const
{
QESP_LOGTRACE("cts")

	if( isOpen() )
	{
		unsigned long status = 0;
#ifdef Q_WS_WIN
		GetCommModemStatus( Win_Handle, &status );
		if( status & MS_CTS_ON )
#else
		ioctl( Posix_File->handle(), TIOCMGET, &status );
		if( status & TIOCM_CTS )
#endif
			return TRUE;
	}
	return FALSE;
}

bool QextSerialPort::dsr() const
{
QESP_LOGTRACE("dsr")

	if( isOpen() )
	{
		unsigned long status = 0;
#ifdef Q_WS_WIN
		GetCommModemStatus( Win_Handle, &status );
		if( status & MS_DSR_ON )
#else
		ioctl( Posix_File->handle(), TIOCMGET, &status );
		if( status & TIOCM_DSR )
#endif
			return TRUE;
	}
	return FALSE;
}

bool QextSerialPort::dcd() const
{
QESP_LOGTRACE("dcd")

	if( isOpen() )
	{
		unsigned long status = 0;
#ifdef Q_WS_WIN
		GetCommModemStatus( Win_Handle, &status );
		if( status & MS_RLSD_ON )
#else
		ioctl( Posix_File->handle(), TIOCMGET, &status );
		if( status & TIOCM_CD )
#endif
			return TRUE;
	}
	return FALSE;
}

bool QextSerialPort::ri() const
{
QESP_LOGTRACE("ri")

	if( isOpen() )
	{
		unsigned long status = 0;
#ifdef Q_WS_WIN
		GetCommModemStatus( Win_Handle, &status );
		if( status & MS_RING_ON )
#else
		ioctl( Posix_File->handle(), TIOCMGET, &status );
		if( status & TIOCM_RI )
#endif
			return TRUE;
	}
	return FALSE;
}
