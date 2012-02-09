/***************************************************************************
                            psprinter.cpp
                         -------------------
begin                : dom ene 22 2006
copyright            : (C) 2002-2006 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#include <qpaintdevicemetrics.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qapplication.h>
#include <stdlib.h>

#include "psprinter.h"
#include "qpsprinter.h"

#if defined(Q_OS_WIN32)
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#endif

#if defined(_WS_X11_)
#include <X11/Xlib.h>
#endif

#if defined(_OS_OS2EMX_)
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include <os2.h>
#endif

#if defined(_OS_QNX_)
#include <process.h>
#endif

// NOT REVISED

class PSPrinterUnixPrivate
{
public:
  bool marginsSpecified;
  uint topMargin;
  uint leftMargin;
  uint bottomMargin;
  uint rightMargin;
};


/*****************************************************************************
  PSPrinter member functions
 *****************************************************************************/

// PSPrinter states

#define PST_IDLE        0
#define PST_ACTIVE      1
#define PST_ERROR       2
#define PST_ABORTED     3


/*!
  Constructs a printer paint device.
*/

PSPrinter::PSPrinter(PrinterMode m)
  : QPaintDevice(QInternal::Printer | QInternal::ExternalDevice)
{
  pdrv = 0;
  pid = 0;
  orient = Portrait;
  page_size = A4;
  page_order = FirstPageFirst;
  color_mode = GrayScale;
  ncopies = 1;
  from_pg = to_pg = min_pg = max_pg = 0;
  state = PST_IDLE;
  output_file = FALSE;
  to_edge     = FALSE;
  paper_source = OnlyOne;
  buffer = 0;
  switch (m) {
    case Compatible:
    case PrinterResolution:
    case ScreenResolution:
    case HighResolution: {
      res = 72;
    }
    break;
  }

  D = new PSPrinterUnixPrivate;
  D->marginsSpecified = FALSE;
}

/*!
  Destructs the printer paint device and cleans up.
*/

PSPrinter::~PSPrinter()
{
  if (pdrv)
    delete pdrv;
  if (buffer) {
    buffer->close();
    delete buffer;
  }
}


/*!
  Advances to a new page on the printer.
  Returns TRUE if successful, otherwise FALSE.
*/

bool PSPrinter::newPage()
{
  if (state == PST_ACTIVE && pdrv)
    return ((QPSPrinter *) pdrv) ->cmd(QPSPrinter::NewPage, 0, 0);
  return FALSE;
}


/*!
  Aborts the print job.
  Returns TRUE if successful, otherwise FALSE.
  \sa aborted()
*/

bool PSPrinter::abort()
{
  if (state == PST_ACTIVE && pdrv) {
    ((QPSPrinter *) pdrv) ->cmd(QPSPrinter::AbortPrinting, 0, 0);
    state = PST_ABORTED;
  }
  return state == PST_ABORTED;
}


bool PSPrinter::aborted() const
{
  return state == PST_ABORTED;
}


void PSPrinter::setPrinterName(const QString &name)
{
  if (state != 0) {
#if defined(QT_CHECK_STATE)
    qWarning("PSPrinter::setPrinterName: Cannot do this during printing");
#endif

    return ;
  }
  printer_name = name;
}


bool PSPrinter::setup(QWidget * /*parent*/)
{
  return true;
  //return QPrintDialog::getPrinterSetup( this );
}

#ifdef _WS_X11_
static void closeAllOpenFds()
{
  // hack time... getting the maximum number of open
  // files, if possible.  if not we assume it's the
  // larger of 256 and the fd we got
  int i;
#if defined(_OS_OS2EMX_)

  LONG req_count = 0;
  ULONG rc, handle_count;
  rc = DosSetRelMaxFH(&req_count, &handle_count);
  /* if (rc != NO_ERROR) ... */
  i = (int) handle_count;
#elif defined(_SC_OPEN_MAX)

  i = (int) sysconf(_SC_OPEN_MAX);
#elif defined(_POSIX_OPEN_MAX)

  i = (int) _POSIX_OPEN_MAX;
#elif defined(OPEN_MAX)

  i = (int) OPEN_MAX;
#else

  i = QMAX(256, fds[ 0 ]);
#endif // ways-to-set i

  while (--i > 0)
    ::close(i);
}
#endif

/*!
  \internal
  Handles painter commands to the printer.
*/

extern int qWinVersion();

bool PSPrinter::cmd( int c, QPainter * paint, QPDevCmdParam * p ) {
  if ( c == PdcBegin ) {
    if ( state == PST_IDLE ) {
      if ( output_file ) {
        int fd = 0;
#if defined(Q_OS_WIN32)

        if ( qWinVersion() & Qt::WV_NT_based )
          fd = _wopen( ( const wchar_t * ) qt_winTchar( output_filename, TRUE ), _O_CREAT | _O_BINARY | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE );
        else
          fd = _open(output_filename.ascii(), _O_CREAT | _O_BINARY | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
#else

        fd = ::open(output_filename.local8Bit(), O_CREAT | O_NOCTTY | O_TRUNC | O_WRONLY, 0666);
#endif

        if (fd >= 0) {
          pdrv = new QPSPrinter(this, fd);
          state = PST_ACTIVE;
        }
      } else {
        QString pr;
        if (printer_name)
          pr = printer_name;
#if defined(Q_OS_WIN32)
        // Not implemented
        // lpr needs -Sserver argument

        if (buffer) {
          buffer->close();
          delete buffer;
        }
        buffer = new QBuffer();
        buffer->open(IO_WriteOnly);
        pdrv = new QPSPrinter(this, buffer);
        state = PST_ACTIVE;
#else

        QApplication::flushX();
        int fds[ 2 ];
        if (pipe(fds) != 0) {
          qWarning("QPSPrinter: could not open pipe to print");
          state = PST_ERROR;
          return FALSE;
        }
#if 0 && defined(_OS_OS2EMX_)
        // this code is still not used, and maybe it's not
        // usable either, any more.  if you want to use it,
        // you may need to fix it first.

        // old comment:

        // this code is usable but not in use.  spawn() is
        // preferable to fork()/exec() for very large
        // programs.  if fork()/exec() is a problem and you
        // use OS/2, remove '0 && ' from the #if.
        int tmp;
        tmp = dup(0);
        dup2(fds[ 0 ], 0);
        ::close(fds[ 0 ]);
        fcntl(tmp, F_SETFD, FD_CLOEXEC);
        fcntl(fds[ 1 ], F_SETFD, FD_CLOEXEC);
        pr.prepend(option_string ? option_string : "-P");   // ###
        if (spawnlp(P_NOWAIT, print_prog.data(), print_prog.data(),
                    pr.data(), output->name(), 0) == -1) {
          ;                   // couldn't exec, ignored
        }
        dup2(tmp, 0);
        ::close(tmp);
        pdrv = new QPSPrinter(this, fds[ 1 ]);
        state = PST_ACTIVE;
#else

        pid = fork();
        if (pid == 0) {         // child process
          // if possible, exit quickly, so the actual lp/lpr
          // becomes a child of init, and ::waitpid() is
          // guaranteed not to wait.
          if (fork() > 0) {
#ifdef _WS_X11_
            closeAllOpenFds();
#endif

            // try to replace this process with "true" - this prevents
            // global destructors from being called (that could possibly
            // do wrong things to the parent process)
            (void) execlp("true", "true", 0);
            (void) execl("/bin/true", "true", 0);
            (void) execl("/usr/bin/true", "true", 0);
            exit(0);
          }
          dup2(fds[ 0 ], 0);
#if defined(_WS_X11_)

          closeAllOpenFds();
#endif // _WS_X11_

          if (print_prog) {
            pr.prepend(option_string ? option_string :
                       QString::fromLatin1("-P"));
            (void) execlp(print_prog.ascii(), print_prog.ascii(),
                          pr.ascii(), 0);
          } else {
            // if no print program has been specified, be smart
            // about the option string too.
            const char *lprarg = 0;
            QString lprhack;
            const char *lparg = 0;
            QString lphack;
            if (pr || option_string) {
              lprhack = pr;
              lprhack.prepend(option_string ? option_string :
                              QString::fromLatin1("-P"));
              lprarg = lprhack.ascii();
              lphack = pr;
              lphack.prepend(option_string ? option_string :
                             QString::fromLatin1("-d"));
              lparg = lphack.ascii();
            }
            (void) execlp("lp", "lp", lparg, 0);
            (void) execlp("lpr", "lpr", lprarg, 0);
            (void) execl("/bin/lp", "lp", lparg, 0);
            (void) execl("/bin/lpr", "lpr", lprarg, 0);
            (void) execl("/usr/bin/lp", "lp", lparg, 0);
            (void) execl("/usr/bin/lpr", "lpr", lprarg, 0);
          }
          // if we couldn't exec anything, close the fd,
          // wait for a second so the parent process (the
          // child of the GUI process) has exited.  then
          // exit.
          ::close(0);
          (void) ::sleep(1);
          ::exit(0);
        } else {                // parent process
          ::close(fds[ 0 ]);
          pdrv = new QPSPrinter(this, fds[ 1 ]);
          state = PST_ACTIVE;
        }
#endif // else part of _OS_OS2EMX_
#endif // else part for #if Q_OS_WIN32

      }
      if (state == PST_ACTIVE && pdrv)
        return ((QPSPrinter *) pdrv) ->cmd(c, paint, p);
    } else {
      // ignore it?  I don't know
    }
  } else {
    bool r = FALSE;
    if (state == PST_ACTIVE && pdrv) {
      r = ((QPSPrinter *) pdrv) ->cmd(c, paint, p);
      if (c == PdcEnd) {
        state = PST_IDLE;
        delete pdrv;
        pdrv = 0;
        if (buffer)
          buffer->close();
#if !defined(Q_OS_WIN32)

        if (pid) {
          (void) ::waitpid(pid, 0, 0);
          pid = 0;
        }
#endif

      }
    }
    return r;
  }
  return TRUE;
}



#define MM(n) int((n * 720 + 127) / 254)
#ifdef IN
#undef IN
#endif
#define IN(n) int(n * 72)

struct PaperSize {
  int width, height;
};

static PaperSize paperSizes[ PSPrinter::NPageSize ] = {
  { MM(210), MM(297) },             // A4
  { MM(176), MM(250) },             // B5
  { IN(8.5), IN(11) },              // Letter
  { IN(8.5), IN(14) },              // Legal
  { IN(7.5), IN(10) },              // Executive
  { MM(841), MM(1189) },            // A0
  { MM(594), MM(841) },             // A1
  { MM(420), MM(594) },             // A2
  { MM(297), MM(420) },             // A3
  { MM(148), MM(210) },             // A5
  { MM(105), MM(148) },             // A6
  { MM(74), MM(105) },               // A7
  { MM(52), MM(74) },               // A8
  { MM(37), MM(52) },               // A9
  { MM(1000), MM(1414) },           // B0
  { MM(707), MM(1000) },            // B1
  { MM(31), MM(44) },               // B10
  { MM(500), MM(707) },             // B2
  { MM(353), MM(500) },             // B3
  { MM(250), MM(353) },             // B4
  { MM(125), MM(176) },             // B6
  { MM(88), MM(125) },              // B7
  { MM(62), MM(88) },               // B8
  { MM(44), MM(62) },               // B9
  { MM(163), MM(229) },             // C5E
  { MM(105), MM(241) },             // Comm10E
  { MM(110), MM(220) },             // DLE
  { MM(210), MM(330) },             // Folio
  { MM(432), MM(279) },             // Ledger
  { MM(279), MM(432) },             // Tabloid
};


/*!
  Internal implementation of the virtual QPaintDevice::metric() function.

  Use the QPaintDeviceMetrics class instead.

  \internal
  Hard coded return values for PostScript under X.
*/

int PSPrinter::metric(int m) const
{
  int val;
  PageSize s = pageSize();
#if defined(QT_CHECK_RANGE)
  Q_ASSERT((uint)s <= (uint)NPageSize);
#endif
  switch (m) {
    case QPaintDeviceMetrics::PdmWidth:
      if (s == PSPrinter::Custom)
        val = orient == Portrait ? customPaperSize_.width() : customPaperSize_.height();
      else
        val = orient == Portrait ? paperSizes[s].width : paperSizes[s].height;
      if (res != 72)
        val = (val * res + 36) / 72;
      if (!fullPage()) {
        if (D->marginsSpecified)
          val -= D->leftMargin + D->rightMargin;
        else
          val -= 2 * margins().width();
      }
      break;
    case QPaintDeviceMetrics::PdmHeight:
      if (s == PSPrinter::Custom)
        val = orient == Portrait ? customPaperSize_.height() : customPaperSize_.width();
      else
        val = orient == Portrait ? paperSizes[s].height : paperSizes[s].width;
      if (res != 72)
        val = (val * res + 36) / 72;
      if (!fullPage()) {
        if (D->marginsSpecified)
          val -= D->topMargin + D->bottomMargin;
        else
          val -= 2 * margins().height();
      }
      break;
    case QPaintDeviceMetrics::PdmDpiX:
      val = res;
      break;
    case QPaintDeviceMetrics::PdmDpiY:
      val = res;
      break;
    case QPaintDeviceMetrics::PdmPhysicalDpiX:
    case QPaintDeviceMetrics::PdmPhysicalDpiY:
      val = 72;
      break;
    case QPaintDeviceMetrics::PdmWidthMM:
      // double rounding error here.  hooray.
      val = metric(QPaintDeviceMetrics::PdmWidth);
      val = (val * 254 + 5 * res) / (10 * res);
      break;
    case QPaintDeviceMetrics::PdmHeightMM:
      val = metric(QPaintDeviceMetrics::PdmHeight);
      val = (val * 254 + 5 * res) / (10 * res);
      break;
    case QPaintDeviceMetrics::PdmNumColors:
      val = 16777216;
      break;
    case QPaintDeviceMetrics::PdmDepth:
      val = 24;
      break;
    default:
      val = 0;
#if defined(QT_CHECK_RANGE)
      qWarning("QPixmap::metric: Invalid metric command");
#endif
  }
  return val;
}


/*!  Returns the width of the left/right and top/bottom margins of the
printer.  This is a best-effort guess, not based on perfect knowledge.

If you have called setFullPage( TRUE ) (this is recommended for
high-quality printing), margins().width() may be treated as the
smallest sane left/right margin you can use, and margins().height() as
the smallest sane top/bottom margins you can use.

If you have called setFullPage( FALSE ) (this is the default),
margins() is automatically subtracted from the pageSize() by PSPrinter.

\sa setFullPage() QPaintDeviceMetrics PageSize
*/

QSize PSPrinter::margins() const
{
  if (D->marginsSpecified)
    return QSize(D->leftMargin, D->topMargin);

  if (orient == Portrait)
    return QSize(res / 2, res / 3);

  return QSize(res / 3, res / 2);
}

#if defined(Q_WS_WIN)
void PSPrinter::setIdle() {}
void PSPrinter::setActive() {}
void PSPrinter::reinit() {}
void PSPrinter::readPdlg(void *) {}
void PSPrinter::readPdlgA(void *) {}
void PSPrinter::writeDevmode(HANDLE) {}
void PSPrinter::writeDevmodeA(HANDLE) {}
short PSPrinter::winPageSize() const {}
#endif

void PSPrinter::setMargins(uint top, uint left, uint bottom, uint right)
{
  D->topMargin = top;
  D->leftMargin = left;
  D->bottomMargin = bottom;
  D->rightMargin = right;
  D->marginsSpecified = TRUE;
}

void PSPrinter::margins(uint *top, uint *left, uint *bottom, uint *right) const
{
  if (!D->marginsSpecified) {
    int x = orient == Portrait ? res / 2 : res / 3;
    int y = orient == Portrait ? res / 3 : res / 2;
    *top = *bottom = y;
    *left = *right = x;
  } else {
    *top = D->topMargin;
    *left = D->leftMargin;
    *bottom = D->bottomMargin;
    *right = D->rightMargin;
  }
}

void PSPrinter::setOutputToFile(bool enable)
{
  if (state != 0) {
#if defined(QT_CHECK_STATE)
    qWarning("PSPrinter::setOutputToFile: Cannot do this during printing");
#endif

    return ;
  }
  output_file = enable;
}


/*!
    \fn QString PSPrinter::outputFileName() const

    Returns the name of the output file. There is no default file
    name.

    \sa setOutputFileName(), setOutputToFile()
*/

/*!
    Sets the name of the output file to \a fileName.

    Setting a null or empty name (0 or "") disables output to a file,
    i.e. calls setOutputToFile(FALSE). Setting a non-empty name
    enables output to a file, i.e. calls setOutputToFile(TRUE).

    This function is currently only supported under X11.

    \sa outputFileName(), setOutputToFile()
*/

void PSPrinter::setOutputFileName(const QString &fileName)
{
  if (state != 0) {
#if defined(QT_CHECK_STATE)
    qWarning("PSPrinter::setOutputFileName: Cannot do this during printing");
#endif

    return ;
  }
  output_filename = fileName;
  output_file = !output_filename.isEmpty();
}

void PSPrinter::setPrintProgram(const QString &printProg)
{
  print_prog = printProg;
}

void PSPrinter::setDocName(const QString &name)
{
  if (state != 0) {
#if defined(QT_CHECK_STATE)
    qWarning("PSPrinter::setDocName: Cannot do this during printing");
#endif

    return ;
  }
  doc_name = name;
}

void PSPrinter::setCreator(const QString &creator)
{
  creator_name = creator;
}


void PSPrinter::setOrientation(Orientation orientation)
{
  orient = orientation;
#if defined(Q_WS_WIN)

  reinit();
#endif
}

void PSPrinter::setPageSize(PageSize newPageSize)
{
  if (newPageSize > NPageSize) {
#if defined(QT_CHECK_STATE)
    qWarning("PSPrinter::SetPageSize: illegal page size %d", newPageSize);
#endif

    return ;
  }
  page_size = newPageSize;
#if defined(Q_WS_WIN)

  reinit();
#endif
}

void PSPrinter::setPageOrder(PageOrder newPageOrder)
{
  page_order = newPageOrder;
#if defined(Q_WS_WIN)

  reinit();
#endif
}


PSPrinter::PageOrder PSPrinter::pageOrder() const
{
  return page_order;
}


void PSPrinter::setColorMode(ColorMode newColorMode)
{
  color_mode = newColorMode;
#if defined(Q_WS_WIN)

  reinit();
#endif
}


PSPrinter::ColorMode PSPrinter::colorMode() const
{
  return color_mode;
}



void PSPrinter::setFromTo(int fromPage, int toPage)
{
  if (state != 0) {
#if defined(QT_CHECK_STATE)
    qWarning("PSPrinter::setFromTo: Cannot do this during printing");
#endif

    return ;
  }
  from_pg = fromPage;
  to_pg = toPage;
}


void PSPrinter::setMinMax(int minPage, int maxPage)
{
  min_pg = minPage;
  max_pg = maxPage;
  if (from_pg == 0 || from_pg < minPage)
    from_pg = minPage;
  if (to_pg == 0 || to_pg > maxPage)
    to_pg = maxPage;
}


void PSPrinter::setCollateCopies(bool on)
{
  if (!collateCopiesEnabled() && on)
    setCollateCopiesEnabled(on);
  usercolcopies = on;
}


void PSPrinter::setNumCopies(int numCopies)
{
  ncopies = numCopies;
#if defined(Q_WS_WIN)

  reinit();
#endif
}


QString PSPrinter::printerSelectionOption() const
{
  return option_string;
}

void PSPrinter::setPrinterSelectionOption(const QString &option)
{
  option_string = option;
}


void PSPrinter::setFullPage(bool fp)
{
  to_edge = fp;
}


bool PSPrinter::fullPage() const
{
  return to_edge;
}


void PSPrinter::setResolution(int /*dpi*/)
{
  //res = dpi;
  //res_set = TRUE;
}


int PSPrinter::resolution() const
{
  return res;
}

void PSPrinter::setPaperSource(PaperSource source)
{
  paper_source = source;
#if defined(Q_WS_WIN)

  reinit();
#endif
}


PSPrinter::PaperSource PSPrinter::paperSource() const
{
  return paper_source;
}

QIODevice *PSPrinter::outDevice()
{
  return buffer;
}
