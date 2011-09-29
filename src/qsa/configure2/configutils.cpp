#include "configutils.h"

#include <qapplication.h>
#include <qprocess.h>
#include <qobject.h>
#include <qfile.h>
#include <qdir.h>
#include <qregexp.h>
#ifdef Q_WS_WIN
#include <windows.h>
#endif
#ifdef Q_OS_UNIX
#include <stdlib.h>
#endif

QString *qtDir = 0;
int processes = 0;
int errors = 0;
int warnings = 0;
bool autoShutdown = TRUE;

static QString *assistant = 0;

static QStringList static_messages;

void message( const QString &str )
{
    static_messages.append( str );
}

QStringList messages()
{
    return static_messages;
}

QString licenseFile() {
    QString home = QDir::homeDirPath();
    QString licenseFile = home + "/.qt-license";
    QFile f( licenseFile );
    if ( f.exists() && f.open( IO_ReadOnly ) )
	return f.readAll();
    message( "Failed to read license file: " + licenseFile );
    return QString::null;
}

bool isUsLicense() {
    QString text = licenseFile();
    QRegExp re( "LicenseKey=([^\n]*)" );
    Q_ASSERT( re.isValid() );
    if ( re.search( text ) >= 0 ) {
	QString key = re.cap( 1 );
	if ( !key.isEmpty() ) {
	    if ( QString( "3579BDFHbdfh" ).find( key[0] ) >= 0 )
		return TRUE;
	}
    }
    return FALSE;
}

class BlockingProcess : public QProcess
{
    Q_OBJECT

public:
    BlockingProcess()
    {
	connect(this, SIGNAL(readyReadStdout()), this, SLOT(readOut()));
	connect(this, SIGNAL(readyReadStderr()), this, SLOT(readErr()));
	connect(this, SIGNAL(processExited()), this, SLOT(exited()));
	outUsed = errUsed = 0;
    }

public slots:
    void readOut()
    {
 	QByteArray rout = readStdout();
	if (outUsed + rout.size() > out.size())
	    out.resize(outUsed + rout.size());
	memcpy(out.data() + outUsed, rout, rout.size());
	outUsed += rout.size();
    }

    void readErr()
    {
 	QByteArray rerr = readStderr();
	if (errUsed + rerr.size() > err.size())
	    err.resize(errUsed + rerr.size());
	memcpy(err.data() + errUsed, rerr, rerr.size());
	errUsed += rerr.size();
    }

    void exited()
    {
	qApp->exit_loop();
    }

public:
    QByteArray out;
    QByteArray err;
    int outUsed;
    int errUsed;
};


static bool execute( const QStringList &args )
{
    BlockingProcess bp;
    bp.setArguments(args);
    if (!bp.start())
	return FALSE;
    qApp->enter_loop();
    return !bp.exitStatus() && bp.normalExit();
}

static void runQMake(const QString &d, const QStringList &configs, const QString &prefix,
                     const QString &target)
{
    QDir dir(d);
    QString runDir = dir.absPath();
    dir.cdUp();
    QString oldDir = dir.absPath();
    QDir::setCurrent(runDir);
    // make the top level Makefile
    QStringList args;
    args.append( "qmake" );
    if ( !configs.isEmpty() )
	args.append( "CONFIG+=" + configs.join( " " ) );
    if ( !prefix.isEmpty() )
	args.append( "QSA_INSTALL_PREFIX=" + prefix );
    if (!target.isNull()) {
        args.append("-o");
        args.append(target);
    }
    if( !execute( args ) )
	warnings++;
    QDir::setCurrent(oldDir);
}


void runQMake( const QStringList &configs, const QString &prefix )
{
    runQMake(".", configs, prefix, "Makefile");
}



void copyQSAHeaders()
{
    // install qmake feature file
    mkDir( *qtDir + "mkspecs/features" );
    copy( "src/qsa/qsa.prf", *qtDir + "mkspecs/features/qsa.prf" );

    // install header files
    QString headers[] = { "qsa/qsaglobal.h",
			  "qsa/qsobjectfactory.h",
			  "qsa/qswrapperfactory.h",
			  "qsa/qseditor.h",
			  "qsa/qsproject.h",
			  "qsa/qsinterpreter.h",
			  "qsa/qsargument.h",
			  "qsa/qsinputdialogfactory.h",
			  "qsa/qsutilfactory.h",
			  "qsa/qsscript.h",
			  "qsa/qsconfig.h",
			  "ide/qsworkbench.h"
    };
    for ( uint j = 0; j < sizeof(headers)/sizeof(QString); j++ ) {
	int pos = headers[j].find( '/' );
	QString target = pos >= 0 ? headers[j].mid( pos+1 ) : headers[j];
	symLink( "src/" + headers[j], *qtDir + "include/" + target );
    }
}


static void removeContentFile( const QString &name )
{
    QStringList lst;
    lst.append( *assistant );
    // Assistant works differently in 3.2
    lst.append( "-removeContentFile" );
    lst.append( QDir::convertSeparators( name ) );
    execute( lst );
}


static void addContentFile( const QString &name )
{
    removeContentFile( name );
    QStringList lst;
    lst.append( *assistant );
    // Assistant works differently in 3.2
    lst.append( "-addContentFile" );
    lst.append( QDir::convertSeparators( name ) );
    if( !execute( lst ) ) {
	warnings++;
    }
}


void installDocs()
{
    if ( !assistant ) {
	assistant = new QString( "assistant" );
#ifdef Q_OS_MACX
	QStringList dirs = QStringList::split( ':', getenv( "PATH" ) );
	QStringList::const_iterator it;
	for ( it = dirs.begin(); it != dirs.end(); ++it ) {
	    QString a = *it + "/assistant.app/Contents/MacOS/assistant";
	    if ( QFile::exists( a ) ) {
		*assistant = a;
		break;
	    }
	}
#endif
    }
    // install documentation
    QString html = "doc/html/";
    addContentFile( html + "qsa.xml" );
    addContentFile( html + "qt-script-for-applications.xml" );
    addContentFile( html + "language.xml" );
    addContentFile( html + "qtscripter.xml" );
    addContentFile( html + "extensions.xml" );
}


void mkDir( const QString &dir )
{
    QDir current;
    current.mkdir( dir );
}


void copy( const QString &source, const QString &dest )
{
    QString s = QDir::convertSeparators( source );
    QString d = QDir::convertSeparators( dest );
#ifdef Q_OS_UNIX
    system( "cp " + QFile::encodeName( s ) + " " + QFile::encodeName( d ) );
    system( "chmod +w " + QFile::encodeName( d ) );
#else
    QT_WA(
    {
	if ( !CopyFileW( (TCHAR*) s.ucs2(), (TCHAR*) d.ucs2(), FALSE ) ) {
	    message( "Failed to copy file: " + s );
	    errors++;
	}
	if ( !SetFileAttributesW( (TCHAR*) d.ucs2(), FILE_ATTRIBUTE_NORMAL ) ) {
	    message( "Failed to set file attributes to normal" );
	    errors++;
	}
    }, {
	if ( !CopyFileA( QFile::encodeName( s ), QFile::encodeName( d ), FALSE ) ) {
	    message( "Failed to copy file: " + s );
	    errors++;
	}
	if ( !SetFileAttributesA( QFile::encodeName( d ), FILE_ATTRIBUTE_NORMAL ) ) {
	    message( "Failed to set file attributes to normal" );
	    errors++;
	}
    } );

#endif
}


void symLink( const QString &source, const QString &dest )
{
#ifdef Q_OS_UNIX
    QString s = QDir::convertSeparators( source );
    QFileInfo info( s );
    s = info.absFilePath();
    QString d = QDir::convertSeparators( dest );
    system( "rm -f " + QFile::encodeName( d ) );
    system( "ln -s " + QFile::encodeName( s ) + " " + QFile::encodeName( d ) );
#else
    copy( source, dest );
#endif
}


uint convertor( const QString &list )
{
    static const unsigned char checksum[] = {
	0x61, 0x74, 0x18, 0x10, 0x06, 0x74, 0x76, 0x0b, 0x02, 0x7b,
	0x78, 0x18, 0x65, 0x72, 0x06, 0x76, 0x6d, 0x1f, 0x01, 0x75,
	0x7e, 0x79, 0x65, 0x01, 0x03, 0x06, 0x6c, 0x6e, 0x18, 0x14,
	0x8f, 0x75, 0x6a, 0x7a, 0x18, 0x7b, 0x76, 0x01, 0x1f, 0x7b,
	0x65, 0x72, 0x06, 0x06, 0x74, 0x76, 0x1f, 0x61, 0x03, 0x6a
    };

    uint length = 0;
    int temp = list.length();
    while ( temp > 0 ) {
        temp--;
	uint alpha = 0x58;
	int currentIndex = 0;
	for ( ;; ) {
	    if ( (uint)list[temp].latin1() == alpha ) {
		length -= (length << 5) + currentIndex;
		break;
	    }
	    alpha ^= (uchar)checksum[currentIndex];
	    if ( (uchar)checksum[currentIndex] == 0x8f )
		return checksum[currentIndex] ^ 0x80;
	    ++currentIndex;
	}
	length = uint(-int(length));
	if ( (uint) (alpha - 0x8a) < 6 )
	    length += checksum[alpha - 0x8a];
    }
    return length;
}


bool checkLicense()
{
    enum LicenseType { None, GPL, Evaluation, Professional, Enterprise, Internal };
    LicenseType qsaLicense = None;
    LicenseType qtLicense = None;
    bool usVersion = FALSE;

    if ( QFile::exists( *qtDir + "LICENSE.TROLL" ) ) {
	qtLicense = Internal;
    } else if( QFile::exists( *qtDir + "LICENSE.EVAL" ) ) {
	qtLicense = Evaluation;
    } else if ( QFile::exists( *qtDir + "LICENSE.GPL" ) ||
		!QFile::exists( *qtDir + "LICENSE" ) &&
		!QFile::exists( QDir::homeDirPath() + "/.qt-license" ) ) {
	qtLicense = GPL;
    } else if ( QFile::exists( *qtDir + "LICENSE" ) ) {
	QString text = licenseFile();
	if( text.isEmpty() ) {
	    message( "Failed to determine Qt license information" );
	    return FALSE;
	}
	bool enterpriseQt = text.find( "\"qt-enterprise\"" ) >= 0;
	qtLicense = enterpriseQt ? Enterprise : Professional;
	usVersion = isUsLicense();
    } else {
	message( "\nCan't find license file of your Qt Edition\n" );
	return FALSE;
    }

    if( qtLicense != Internal ) {
	if ( QFile::exists( "LICENSE.GPL" ) ) {
	    qsaLicense = GPL;
	} else if ( QFile::exists( "LICENSE.EVAL" ) ) {
	    qsaLicense = Evaluation;
	} else {
	    copy( usVersion ? ".LICENSE-US" : ".LICENSE", "LICENSE" );
	    if ( QFile::exists( "LICENSE" ) ) {
		QString text = licenseFile();
		QRegExp re( "LicenseKey=[A-Z0-9]([A-Z0-9]{3})-([A-Z0-9]{4})(-[A-Z0-9]{4})?" );
		Q_ASSERT( re.isValid() );
		if ( re.search( text ) >= 0 ) {
		    if ( (convertor(re.cap(1) + re.cap(2)) & 0x82 ) == 0x82 ) {
			qsaLicense = Enterprise;
		    }
		}
	    }
	}
    }

    if ( qtLicense == Internal ||
	 qtLicense == qsaLicense ||
	 qtLicense == Enterprise && qsaLicense == Evaluation ) {
	return TRUE;
    } else if ( ( qtLicense == Enterprise || qtLicense == Professional ) &&
		qsaLicense == GPL ) {
	warnings++;
	message( "\nThis QSA Free Edition is licensed under the GNU General "
		 "Public License.\nNote that any derivatives of this software "
		 "must also be licensed under\nthe GNU GPL.\n\n"
		 "See http://www.trolltech.com/gpl for more information or\n"
		 "contact sales@trolltech.com for a commercial edition of QSA.\n" );
	return TRUE;
    } else {
	message( "\nThe required license information cannot be found"
		 "\nor the information is conflicting!\n" );
	QString str = QString( "Error code: %1%2\n\n" ).arg( qtLicense ).arg( qsaLicense );
	message( str );
	return FALSE;
    }
    return FALSE;
}


bool writeQSConfig( bool buildIde )
{
    QFile file( "src/qsa/qsconfig.h" );
    if( !file.open( IO_WriteOnly ) ) {
	message( "Failed to open 'src/qsa/qsconfig.h' for writing." );
	return FALSE;
    }
    QTextStream txt( &file );
    txt << "// This file is autogenerated by QSA configure, do not modify it!\n"
	<< "#ifndef QS_CONFIG_H\n"
	<< "#define QS_CONFIG_H\n"
	<< "\n";
    if( !buildIde )
	txt << "#define QSA_NO_IDE\n";
    txt << "\n"
	<< "#endif\n";
    return TRUE;
}

void rmDirRecursive( const QDir &dir )
{
    const QFileInfoList* list = dir.entryInfoList( QDir::All | QDir::System | QDir::Hidden );
    if ( list ) {
	QFileInfoListIterator it( *list );
	QFileInfo* fi;

	while( ( fi = it.current() ) ) {
	    if( ( fi->fileName() != "." ) && ( fi->fileName() != ".." ) ){
		if( fi->isDir() )
		    rmDirRecursive( QDir(fi->absFilePath()) );
		else
		    QFile::remove( fi->absFilePath() );
	    }
	    ++it;
	}
    }
    // Remove this dir as well
    dir.rmdir( dir.absPath() );
}


#include "configutils.moc"

