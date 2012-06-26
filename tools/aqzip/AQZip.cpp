/***************************************************************************
 AQZip.cpp
 -------------------
 begin                : 12/12/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#include <zlib.h>

#include <qglobal.h>
#include <qdatetime.h>
#include <qplatformdefs.h>
#include <qdir.h>

#include "AQZipReader.h"
#include "AQZipWriter.h"
#include "qscopedpointer.h"

#if defined(Q_OS_WIN)
#  undef S_IFREG
#  define S_IFREG 0100000
#  ifndef S_IFDIR
#    define S_IFDIR 0040000
#  endif
#  ifndef S_ISDIR
#    define S_ISDIR(x) ((x) & S_IFDIR) > 0
#  endif
#  ifndef S_ISREG
#    define S_ISREG(x) ((x) & 0170000) == S_IFREG
#  endif
#  define S_IFLNK 020000
#  define S_ISLNK(x) ((x) & S_IFLNK) > 0
#  ifndef S_IRUSR
#    define S_IRUSR 0400
#  endif
#  ifndef S_IWUSR
#    define S_IWUSR 0200
#  endif
#  ifndef S_IXUSR
#    define S_IXUSR 0100
#  endif
#  define S_IRGRP 0040
#  define S_IWGRP 0020
#  define S_IXGRP 0010
#  define S_IROTH 0004
#  define S_IWOTH 0002
#  define S_IXOTH 0001
#endif

#if 0
#define ZDEBUG qDebug
#else
#define ZDEBUG //if (0) qDebug
#endif


static inline uint readUInt(const uchar *data)
{
  return (data[0]) + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
}

static inline ushort readUShort(const uchar *data)
{
  return (data[0]) + (data[1] << 8);
}

static inline void writeUInt(uchar *data, uint i)
{
  data[0] = i & 0xff;
  data[1] = (i >> 8) & 0xff;
  data[2] = (i >> 16) & 0xff;
  data[3] = (i >> 24) & 0xff;
}

static inline void writeUShort(uchar *data, ushort i)
{
  data[0] = i & 0xff;
  data[1] = (i >> 8) & 0xff;
}

static inline void copyUInt(uchar *dest, const uchar *src)
{
  dest[0] = src[0];
  dest[1] = src[1];
  dest[2] = src[2];
  dest[3] = src[3];
}

static inline void copyUShort(uchar *dest, const uchar *src)
{
  dest[0] = src[0];
  dest[1] = src[1];
}

static void writeMSDosDate(uchar *dest, const QDateTime &dt)
{
  if (dt.isValid()) {
    Q_UINT16 time =
      (dt.time().hour() << 11)    // 5 bit hour
      | (dt.time().minute() << 5)   // 6 bit minute
      | (dt.time().second() >> 1);  // 5 bit double seconds

    dest[0] = time & 0xff;
    dest[1] = time >> 8;

    Q_UINT16 date =
      ((dt.date().year() - 1980) << 9) // 7 bit year 1980-based
      | (dt.date().month() << 5)           // 4 bit month
      | (dt.date().day());                 // 5 bit day

    dest[2] = char(date);
    dest[3] = char(date >> 8);
  } else {
    dest[0] = 0;
    dest[1] = 0;
    dest[2] = 0;
    dest[3] = 0;
  }
}

static Q_UINT32 permissionsToMode(QFileInfo::PermissionSpec perms)
{
  Q_UINT32 mode = 0;
  if (perms & QFileInfo::ReadOwner)
    mode |= S_IRUSR;
  if (perms & QFileInfo::WriteOwner)
    mode |= S_IWUSR;
  if (perms & QFileInfo::ExeOwner)
    mode |= S_IXUSR;
  if (perms & QFileInfo::ReadUser)
    mode |= S_IRUSR;
  if (perms & QFileInfo::WriteUser)
    mode |= S_IWUSR;
  if (perms & QFileInfo::ExeUser)
    mode |= S_IXUSR;
  if (perms & QFileInfo::ReadGroup)
    mode |= S_IRGRP;
  if (perms & QFileInfo::WriteGroup)
    mode |= S_IWGRP;
  if (perms & QFileInfo::ExeGroup)
    mode |= S_IXGRP;
  if (perms & QFileInfo::ReadOther)
    mode |= S_IROTH;
  if (perms & QFileInfo::WriteOther)
    mode |= S_IWOTH;
  if (perms & QFileInfo::ExeOther)
    mode |= S_IXOTH;
  return mode;
}

static int inflate(Bytef *dest, ulong *destLen, const Bytef *source, ulong sourceLen)
{
  z_stream stream;
  int err;

  stream.next_in = (Bytef *)source;
  stream.avail_in = (uInt)sourceLen;
  if ((uLong)stream.avail_in != sourceLen)
    return Z_BUF_ERROR;

  stream.next_out = dest;
  stream.avail_out = (uInt) * destLen;
  if ((uLong)stream.avail_out != *destLen)
    return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;

  err = inflateInit2(&stream, -MAX_WBITS);
  if (err != Z_OK)
    return err;

  err = inflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) {
    inflateEnd(&stream);
    if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
      return Z_DATA_ERROR;
    return err;
  }
  *destLen = stream.total_out;

  err = inflateEnd(&stream);
  return err;
}

static int deflate(Bytef *dest, ulong *destLen, const Bytef *source, ulong sourceLen)
{
  z_stream stream;
  int err;

  stream.next_in = (Bytef *)source;
  stream.avail_in = (uInt)sourceLen;
  stream.next_out = dest;
  stream.avail_out = (uInt) * destLen;
  if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  stream.opaque = (voidpf)0;

  err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
  if (err != Z_OK) return err;

  err = deflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) {
    deflateEnd(&stream);
    return err == Z_OK ? Z_BUF_ERROR : err;
  }
  *destLen = stream.total_out;

  err = deflateEnd(&stream);
  return err;
}

static QFileInfo::PermissionSpec modeToPermissions(Q_UINT32 mode)
{
  uint ret;
  if (mode & S_IRUSR)
    ret |= QFileInfo::ReadOwner;
  if (mode & S_IWUSR)
    ret |= QFileInfo::WriteOwner;
  if (mode & S_IXUSR)
    ret |= QFileInfo::ExeOwner;
  if (mode & S_IRUSR)
    ret |= QFileInfo::ReadUser;
  if (mode & S_IWUSR)
    ret |= QFileInfo::WriteUser;
  if (mode & S_IXUSR)
    ret |= QFileInfo::ExeUser;
  if (mode & S_IRGRP)
    ret |= QFileInfo::ReadGroup;
  if (mode & S_IWGRP)
    ret |= QFileInfo::WriteGroup;
  if (mode & S_IXGRP)
    ret |= QFileInfo::ExeGroup;
  if (mode & S_IROTH)
    ret |= QFileInfo::ReadOther;
  if (mode & S_IWOTH)
    ret |= QFileInfo::WriteOther;
  if (mode & S_IXOTH)
    ret |= QFileInfo::ExeOther;
  return (QFileInfo::PermissionSpec) ret;
}

static QDateTime readMSDosDate(const uchar *src)
{
  uint dosDate = readUInt(src);
  Q_UINT64 uDate;
  uDate = (Q_UINT64)(dosDate >> 16);
  uint tm_mday = (uDate & 0x1f);
  uint tm_mon = ((uDate & 0x1E0) >> 5);
  uint tm_year = (((uDate & 0x0FE00) >> 9) + 1980);
  uint tm_hour = ((dosDate & 0xF800) >> 11);
  uint tm_min = ((dosDate & 0x7E0) >> 5);
  uint tm_sec = ((dosDate & 0x1f) << 1);

  return QDateTime(QDate(tm_year, tm_mon, tm_mday), QTime(tm_hour, tm_min, tm_sec));
}

struct LocalFileHeader {
  uchar signature[4]; //  0x04034b50
  uchar version_needed[2];
  uchar general_purpose_bits[2];
  uchar compression_method[2];
  uchar last_mod_file[4];
  uchar crc_32[4];
  uchar compressed_size[4];
  uchar uncompressed_size[4];
  uchar file_name_length[2];
  uchar extra_field_length[2];
};

struct DataDescriptor {
  uchar crc_32[4];
  uchar compressed_size[4];
  uchar uncompressed_size[4];
};

struct CentralFileHeader {
  uchar signature[4]; // 0x02014b50
  uchar version_made[2];
  uchar version_needed[2];
  uchar general_purpose_bits[2];
  uchar compression_method[2];
  uchar last_mod_file[4];
  uchar crc_32[4];
  uchar compressed_size[4];
  uchar uncompressed_size[4];
  uchar file_name_length[2];
  uchar extra_field_length[2];
  uchar file_comment_length[2];
  uchar disk_start[2];
  uchar internal_file_attributes[2];
  uchar external_file_attributes[4];
  uchar offset_local_header[4];
  LocalFileHeader toLocalHeader() const;
};

struct EndOfDirectory {
  uchar signature[4]; // 0x06054b50
  uchar this_disk[2];
  uchar start_of_directory_disk[2];
  uchar num_dir_entries_this_disk[2];
  uchar num_dir_entries[2];
  uchar directory_size[4];
  uchar dir_start_offset[4];
  uchar comment_length[2];
};

struct FileHeader {
  CentralFileHeader h;
  QByteArray file_name;
  QByteArray extra_field;
  QByteArray file_comment;
};

AQZipReader::FileInfo::FileInfo()
  : isDir(false), isFile(false), isSymLink(false), crc32(0), size(0)
{
}

AQZipReader::FileInfo::~FileInfo()
{
}

AQZipReader::FileInfo::FileInfo(const FileInfo &other)
{
  operator=(other);
}

AQZipReader::FileInfo &AQZipReader::FileInfo::operator=(const FileInfo &other)
{
  filePath = other.filePath;
  isDir = other.isDir;
  isFile = other.isFile;
  isSymLink = other.isSymLink;
  permissions = other.permissions;
  crc32 = other.crc32;
  size = other.size;
  lastModified = other.lastModified;
  return *this;
}

bool AQZipReader::FileInfo::isValid() const
{
  return isDir || isFile || isSymLink;
}

class AQZipPrivate
{
public:
  AQZipPrivate(QIODevice *device, bool ownDev)
    : device(device), ownDevice(ownDev), dirtyFileTree(true), start_of_directory(0) {
  }

  ~AQZipPrivate() {
    if (ownDevice)
      delete device;
  }

  void fillFileInfo(int index, AQZipReader::FileInfo &fileInfo) const;

  QIODevice *device;
  bool ownDevice;
  bool dirtyFileTree;
  QValueList<FileHeader> fileHeaders;
  QByteArray comment;
  uint start_of_directory;
};

static inline Q_UINT32 qFromLittleEndian(const uchar *src)
{
  return 0
         | src[0]
         | src[1] * Q_UINT32(0x00000100)
         | src[2] * Q_UINT32(0x00010000)
         | src[3] * Q_UINT32(0x01000000);
}

void AQZipPrivate::fillFileInfo(int index, AQZipReader::FileInfo &fileInfo) const
{
  FileHeader header = fileHeaders[index];
  fileInfo.filePath = QString::fromLocal8Bit(header.file_name);
  const Q_UINT32 mode = (qFromLittleEndian(&header.h.external_file_attributes[0]) >> 16) & 0xFFFF;
  fileInfo.isDir = S_ISDIR(mode);
  fileInfo.isFile = S_ISREG(mode);
  fileInfo.isSymLink = S_ISLNK(mode);
  fileInfo.permissions = modeToPermissions(mode);
  fileInfo.crc32 = readUInt(header.h.crc_32);
  fileInfo.size = readUInt(header.h.uncompressed_size);
  fileInfo.lastModified = readMSDosDate(header.h.last_mod_file);
}

class AQZipReaderPrivate : public AQZipPrivate
{
public:
  AQZipReaderPrivate(QIODevice *device, bool ownDev)
    : AQZipPrivate(device, ownDev), status(AQZipReader::NoError) {
  }

  void scanFiles();

  AQZipReader::Status status;
};

class AQZipWriterPrivate : public AQZipPrivate
{
public:
  AQZipWriterPrivate(QIODevice *device, bool ownDev)
    : AQZipPrivate(device, ownDev),
      status(AQZipWriter::NoError),
      permissions(QFileInfo::ReadOwner | QFileInfo::WriteOwner |
                  QFileInfo::ReadGroup | QFileInfo::ReadOther),
      compressionPolicy(AQZipWriter::AlwaysCompress) {
  }

  AQZipWriter::Status status;
  uint permissions;
  AQZipWriter::CompressionPolicy compressionPolicy;

  enum EntryType { Directory, File, Symlink };

  void addEntry(EntryType type, const QString &fileName, const QByteArray &contents);
};

LocalFileHeader CentralFileHeader::toLocalHeader() const
{
  LocalFileHeader h;
  writeUInt(h.signature, 0x04034b50);
  copyUShort(h.version_needed, version_needed);
  copyUShort(h.general_purpose_bits, general_purpose_bits);
  copyUShort(h.compression_method, compression_method);
  copyUInt(h.last_mod_file, last_mod_file);
  copyUInt(h.crc_32, crc_32);
  copyUInt(h.compressed_size, compressed_size);
  copyUInt(h.uncompressed_size, uncompressed_size);
  copyUShort(h.file_name_length, file_name_length);
  copyUShort(h.extra_field_length, extra_field_length);
  return h;
}

void AQZipReaderPrivate::scanFiles()
{
  if (!dirtyFileTree)
    return;

  if (!(device->isOpen() || device->open(IO_ReadOnly))) {
    status = AQZipReader::FileOpenError;
    return;
  }

  if ((device->mode() & IO_ReadOnly) == 0) { // only read the index from readable files.
    status = AQZipReader::FileReadError;
    return;
  }

  dirtyFileTree = false;
  uchar tmp[4];
  device->readBlock((char *)tmp, 4);
  if (readUInt(tmp) != 0x04034b50) {
    qWarning("AQZip: not a zip file!");
    return;
  }

  // find EndOfDirectory header
  int i = 0;
  int start_of_directory = -1;
  int num_dir_entries = 0;
  EndOfDirectory eod;
  while (start_of_directory == -1) {
    int pos = device->size() - sizeof(EndOfDirectory) - i;
    if (pos < 0 || i > 65535) {
      qWarning("AQZip: EndOfDirectory not found");
      return;
    }

    device->at(pos);
    device->readBlock((char *)&eod, sizeof(EndOfDirectory));
    if (readUInt(eod.signature) == 0x06054b50)
      break;
    ++i;
  }

  // have the eod
  start_of_directory = readUInt(eod.dir_start_offset);
  num_dir_entries = readUShort(eod.num_dir_entries);
  qWarning("start_of_directory at %d, num_dir_entries=%d", start_of_directory, num_dir_entries);
  int comment_length = readUShort(eod.comment_length);
  if (comment_length != i)
    qWarning("AQZip: failed to parse zip file.");
  uint cL = QMIN(comment_length, i);
  QByteArray baCom(cL);
  device->readBlock(baCom.data(), cL);
  //comment = device->readBlock(QMIN(comment_length, i));
  comment = baCom;


  device->at(start_of_directory);
  for (i = 0; i < num_dir_entries; ++i) {
    FileHeader header;
    int read = device->readBlock((char *) &header.h, sizeof(CentralFileHeader));
    if (read < (int)sizeof(CentralFileHeader)) {
      qWarning("AQZip: Failed to read complete header, index may be incomplete");
      break;
    }
    if (readUInt(header.h.signature) != 0x02014b50) {
      qWarning("AQZip: invalid header signature, index may be incomplete");
      break;
    }

    uint l = readUShort(header.h.file_name_length);
    QByteArray baFileName(l);
    device->readBlock(baFileName.data(), l);
    //header.file_name = device->read(l);
    header.file_name = baFileName;
    if (header.file_name.size() != l) {
      qWarning("AQZip: Failed to read filename from zip index, index may be incomplete");
      break;
    }
    l = readUShort(header.h.extra_field_length);
    QByteArray baExtra(l);
    device->readBlock(baExtra.data(), l);
    //header.extra_field = device->read(l);
    header.extra_field = baExtra;
    if (header.extra_field.size() != l) {
      qWarning("AQZip: Failed to read extra field in zip file, skipping file, index may be incomplete");
      break;
    }
    l = readUShort(header.h.file_comment_length);
    QByteArray baFileCom(l);
    device->readBlock(baFileCom.data(), l);
    //header.file_comment = device->read(l);
    header.file_comment = baFileCom;
    if (header.file_comment.size() != l) {
      qWarning("AQZip: Failed to read read file comment, index may be incomplete");
      break;
    }

    qWarning("found file '%s'", header.file_name.data());
    fileHeaders.append(header);
  }
}

void AQZipWriterPrivate::addEntry(EntryType type, const QString &fileName, const QByteArray &contents/*, QFile::Permissions permissions, AQZip::Method m*/)
{
#if 0
  //#ifndef NDEBUG
  static const char *entryTypes[] = {
    "directory",
    "file     ",
    "symlink  "
  };
  ZDEBUG() << "adding" << entryTypes[type] << ":" << fileName.toUtf8().data() << (type == 2 ? (" -> " + contents).constData() : "");
#endif

  if (!(device->isOpen() || device->open(IO_WriteOnly))) {
    status = AQZipWriter::FileOpenError;
    return;
  }
  device->at(start_of_directory);

  // don't compress small files
  AQZipWriter::CompressionPolicy compression = compressionPolicy;
  if (compressionPolicy == AQZipWriter::AutoCompress) {
    if (contents.size() < 64)
      compression = AQZipWriter::NeverCompress;
    else
      compression = AQZipWriter::AlwaysCompress;
  }

  FileHeader header;
  memset(&header.h, 0, sizeof(CentralFileHeader));
  writeUInt(header.h.signature, 0x02014b50);

  writeUShort(header.h.version_needed, 0x14);
  writeUInt(header.h.uncompressed_size, contents.size());
  writeMSDosDate(header.h.last_mod_file, QDateTime::currentDateTime());
  QByteArray data = contents;
  data.detach();
  if (compression == AQZipWriter::AlwaysCompress) {
    writeUShort(header.h.compression_method, Z_DEFLATED);
    ulong len = contents.size();
    // shamelessly copied form zlib
    len += (len >> 12) + (len >> 14) + 11;
    int res;
    do {
      data.resize(len);
      res = deflate((uchar *)data.data(), &len, (const uchar *)contents.data(), contents.size());

      switch (res) {
        case Z_OK:
          data.resize(len);
          break;
        case Z_MEM_ERROR:
          qWarning("AQZip: Z_MEM_ERROR: Not enough memory to compress file, skipping");
          data.resize(0);
          break;
        case Z_BUF_ERROR:
          len *= 2;
          break;
      }
    } while (res == Z_BUF_ERROR);
  }
  // TODO add a check if data.length() > contents.length().  Then try to store the original and revert the compression method to be uncompressed
  writeUInt(header.h.compressed_size, data.size());
  Q_UINT32 crc_32 = ::crc32(0L, Z_NULL, 0);
  crc_32 = ::crc32(crc_32, (const Bytef *)contents.data(), contents.size());
  writeUInt(header.h.crc_32, crc_32);

  header.file_name = fileName.local8Bit();
  header.file_name.truncate(fileName.length());
  if (header.file_name.size() > 0xffff) {
    qWarning("AQZip: Filename too long, chopping it to 65535 characters");
    header.file_name = QCString(header.file_name).left(0xffff);
  }
  writeUShort(header.h.file_name_length, header.file_name.size());
  //h.extra_field_length[2];

  writeUShort(header.h.version_made, (3 << 8) | 20);
  //uchar internal_file_attributes[2];
  //uchar external_file_attributes[4];
  Q_UINT32 mode = permissionsToMode((QFileInfo::PermissionSpec) permissions);
  switch (type) {
    case File:
      mode |= S_IFREG;
      break;
    case Directory:
      mode |= S_IFDIR;
      break;
    case Symlink:
      mode |= S_IFLNK;
      break;
  }
  writeUInt(header.h.external_file_attributes, mode << 16);
  writeUInt(header.h.offset_local_header, start_of_directory);


  fileHeaders.append(header);

  LocalFileHeader h = header.h.toLocalHeader();
  device->writeBlock((const char *)&h, sizeof(LocalFileHeader));
  device->writeBlock(header.file_name);
  device->writeBlock(data);
  start_of_directory = device->at();
  dirtyFileTree = true;
}

//////////////////////////////  Reader

/*!
    \class AQZipReader::FileInfo
    \internal
    Represents one entry in the zip table of contents.
*/

/*!
    \variable FileInfo::filePath
    The full filepath inside the archive.
*/

/*!
    \variable FileInfo::isDir
    A boolean type indicating if the entry is a directory.
*/

/*!
    \variable FileInfo::isFile
    A boolean type, if it is one this entry is a file.
*/

/*!
    \variable FileInfo::isSymLink
    A boolean type, if it is one this entry is symbolic link.
*/

/*!
    \variable FileInfo::permissions
    A list of flags for the permissions of this entry.
*/

/*!
    \variable FileInfo::crc32
    The calculated checksum as a crc32 type.
*/

/*!
    \variable FileInfo::size
    The total size of the unpacked content.
*/

/*!
    \variable FileInfo::d
    \internal
    private pointer.
*/

/*!
    \class AQZipReader
    \internal
    \since 4.5

    \brief the AQZipReader class provides a way to inspect the contents of a zip
    archive and extract individual files from it.

    AQZipReader can be used to read a zip archive either from a file or from any
    device. An in-memory QBuffer for instance.  The reader can be used to read
    which files are in the archive using fileInfoList() and entryInfoAt() but
    also to extract individual files using fileData() or even to extract all
    files in the archive using extractAll()
*/

/*!
    Create a new zip archive that operates on the \a fileName.  The file will be
    opened with the \a mode.
*/
AQZipReader::AQZipReader(const QString &archive, uint mode)
{
  QScopedPointer<QFile> f(new QFile(archive));
  f->open(mode);
  AQZipReader::Status status;
  if (f->status() == IO_Ok)
    status = NoError;
  else {
    if (f->status() == IO_ReadError)
      status = FileReadError;
    else if (f->status() == IO_OpenError)
      status = FileOpenError;
    //else if (f->status() == IO_PermissionsError)
    //    status = FilePermissionsError;
    else
      status = FileError;
  }

  d = new AQZipReaderPrivate(f.data(), /*ownDevice=*/true);
  f.take();
  d->status = status;
}

/*!
    Create a new zip archive that operates on the archive found in \a device.
    You have to open the device previous to calling the constructor and only a
    device that is readable will be scanned for zip filecontent.
 */
AQZipReader::AQZipReader(QIODevice *device)
  : d(new AQZipReaderPrivate(device, /*ownDevice=*/false))
{
  Q_ASSERT(device);
}

/*!
    Desctructor
*/
AQZipReader::~AQZipReader()
{
  close();
  delete d;
}

/*!
    Returns device used for reading zip archive.
*/
QIODevice *AQZipReader::device() const
{
  return d->device;
}

/*!
    Returns true if the user can read the file; otherwise returns false.
*/
bool AQZipReader::isReadable() const
{
  return d->device->isReadable();
}

/*!
    Returns true if the file exists; otherwise returns false.
*/
bool AQZipReader::exists() const
{
  QFile *f = static_cast<QFile *>(d->device);
  if (f == 0)
    return true;
  return f->exists();
}

/*!
    Returns the list of files the archive contains.
*/
QValueList<AQZipReader::FileInfo> AQZipReader::fileInfoList() const
{
  d->scanFiles();
  QValueList<AQZipReader::FileInfo> files;
  for (uint i = 0; i < d->fileHeaders.size(); ++i) {
    AQZipReader::FileInfo fi;
    d->fillFileInfo(i, fi);
    files.append(fi);
  }
  return files;
}

/*!
    Return the number of items in the zip archive.
*/
int AQZipReader::count() const
{
  d->scanFiles();
  return d->fileHeaders.count();
}

/*!
    Returns a FileInfo of an entry in the zipfile.
    The \a index is the index into the directory listing of the zipfile.
    Returns an invalid FileInfo if \a index is out of boundaries.

    \sa fileInfoList()
*/
AQZipReader::FileInfo AQZipReader::entryInfoAt(int index) const
{
  d->scanFiles();
  AQZipReader::FileInfo fi;
  if (index >= 0 && index < (int)d->fileHeaders.count())
    d->fillFileInfo(index, fi);
  return fi;
}

/*!
    Fetch the file contents from the zip archive and return the uncompressed bytes.
*/
QByteArray AQZipReader::fileData(const QString &fileName) const
{
  d->scanFiles();
  uint i;
  for (i = 0; i < d->fileHeaders.size(); ++i) {
    if (QString::fromLocal8Bit(d->fileHeaders[i].file_name) == fileName)
      break;
  }
  if (i == d->fileHeaders.size())
    return QByteArray();

  FileHeader header = d->fileHeaders[i];

  int compressed_size = readUInt(header.h.compressed_size);
  int uncompressed_size = readUInt(header.h.uncompressed_size);
  int start = readUInt(header.h.offset_local_header);
  //qDebug("uncompressing file %d: local header at %d", i, start);

  d->device->at(start);
  LocalFileHeader lh;
  d->device->readBlock((char *)&lh, sizeof(LocalFileHeader));
  uint skip = readUShort(lh.file_name_length) + readUShort(lh.extra_field_length);
  d->device->at(d->device->at() + skip);

  int compression_method = readUShort(lh.compression_method);
  //qDebug("file=%s: compressed_size=%d, uncompressed_size=%d", fileName.toLocal8Bit().data(), compressed_size, uncompressed_size);

  //qDebug("file at %lld", d->device->pos());
  QByteArray baCom(compressed_size);
  d->device->readBlock(baCom.data(), compressed_size);
  //QByteArray compressed = d->device->read(compressed_size);
  QByteArray compressed = baCom;
  if (compression_method == 0) {
    // no compression
    compressed.truncate(uncompressed_size);
    return compressed;
  } else if (compression_method == 8) {
    // Deflate
    //qDebug("compressed=%d", compressed.size());
    compressed.truncate(compressed_size);
    QByteArray baunzip;
    ulong len = QMAX(uncompressed_size,  1);
    int res;
    do {
      baunzip.resize(len);
      res = inflate((uchar *)baunzip.data(), &len,
                    (uchar *)compressed.data(), compressed_size);

      switch (res) {
        case Z_OK:
          if (len != baunzip.size())
            baunzip.resize(len);
          break;
        case Z_MEM_ERROR:
          qWarning("AQZip: Z_MEM_ERROR: Not enough memory");
          break;
        case Z_BUF_ERROR:
          len *= 2;
          break;
        case Z_DATA_ERROR:
          qWarning("AQZip: Z_DATA_ERROR: Input data is corrupted");
          break;
      }
    } while (res == Z_BUF_ERROR);
    return baunzip;
  }
  qWarning("AQZip: Unknown compression method");
  return QByteArray();
}

/*!
    Extracts the full contents of the zip file into \a destinationDir on
    the local filesystem.
    In case writing or linking a file fails, the extraction will be aborted.
*/
bool AQZipReader::extractAll(const QString &destinationDir) const
{
  QDir baseDir(destinationDir);

  // create directories first
  QValueList<FileInfo> allFiles = fileInfoList();
  //foreach (FileInfo fi, allFiles) {
  for (uint i = 0; i < allFiles.size(); ++i) {
    FileInfo fi = allFiles[i];
    const QString absPath = destinationDir + QDir::separator() + fi.filePath;
    if (fi.isDir) {
      if (!baseDir.mkdir(fi.filePath))
        return false;
      //if (!QFile::setPermissions(absPath, fi.permissions))
      //    return false;
    }
  }

  // set up symlinks
  //foreach (FileInfo fi, allFiles) {
  for (uint i = 0; i < allFiles.size(); ++i) {
    FileInfo fi = allFiles[i];
    const QString absPath = destinationDir + QDir::separator() + fi.filePath;
    if (fi.isSymLink) {
      QString destination = QFile::decodeName(QCString(fileData(fi.filePath)));
      if (destination.isEmpty())
        return false;
      QFileInfo linkFi(absPath);
      if (!QFile::exists(linkFi.absFilePath()))
        QDir::root().mkdir(linkFi.absFilePath());
      //if (!QFile::link(destination, absPath))
      //    return false;
      /* cannot change permission of links
      if (!QFile::setPermissions(absPath, fi.permissions))
          return false;
      */
    }
  }

  //foreach (FileInfo fi, allFiles) {
  for (uint i = 0; i < allFiles.size(); ++i) {
    FileInfo fi = allFiles[i];
    const QString absPath = destinationDir + QDir::separator() + fi.filePath;
    if (fi.isFile) {
      QFile f(absPath);
      if (!f.open(IO_WriteOnly))
        return false;
      f.writeBlock(fileData(fi.filePath));
      //f.setPermissions(fi.permissions);
      f.close();
    }
  }

  return true;
}

/*!
    \enum AQZipReader::Status

    The following status values are possible:

    \value NoError  No error occurred.
    \value FileReadError    An error occurred when reading from the file.
    \value FileOpenError    The file could not be opened.
    \value FilePermissionsError The file could not be accessed.
    \value FileError        Another file error occurred.
*/

/*!
    Returns a status code indicating the first error that was met by AQZipReader,
    or AQZipReader::NoError if no error occurred.
*/
AQZipReader::Status AQZipReader::status() const
{
  return d->status;
}

/*!
    Close the zip file.
*/
void AQZipReader::close()
{
  d->device->close();
}

////////////////////////////// Writer

/*!
    \class AQZipWriter
    \internal
    \since 4.5

    \brief the AQZipWriter class provides a way to create a new zip archive.

    AQZipWriter can be used to create a zip archive containing any number of files
    and directories. The files in the archive will be compressed in a way that is
    compatible with common zip reader applications.
*/


/*!
    Create a new zip archive that operates on the \a archive filename.  The file will
    be opened with the \a mode.
    \sa isValid()
*/
AQZipWriter::AQZipWriter(const QString &fileName, uint mode)
{
  QScopedPointer<QFile> f(new QFile(fileName));
  f->open(mode);
  AQZipWriter::Status status;
  if (f->status() == IO_Ok)
    status = AQZipWriter::NoError;
  else {
    if (f->status() == IO_WriteError)
      status = AQZipWriter::FileWriteError;
    else if (f->status() == IO_OpenError)
      status = AQZipWriter::FileOpenError;
    //else if (f->error() == QFile::PermissionsError)
    //    status = AQZipWriter::FilePermissionsError;
    else
      status = AQZipWriter::FileError;
  }

  d = new AQZipWriterPrivate(f.data(), /*ownDevice=*/true);
  f.take();
  d->status = status;
}

/*!
    Create a new zip archive that operates on the archive found in \a device.
    You have to open the device previous to calling the constructor and
    only a device that is readable will be scanned for zip filecontent.
 */
AQZipWriter::AQZipWriter(QIODevice *device)
  : d(new AQZipWriterPrivate(device, /*ownDevice=*/false))
{
  Q_ASSERT(device);
}

AQZipWriter::~AQZipWriter()
{
  close();
  delete d;
}

/*!
    Returns device used for writing zip archive.
*/
QIODevice *AQZipWriter::device() const
{
  return d->device;
}

/*!
    Returns true if the user can write to the archive; otherwise returns false.
*/
bool AQZipWriter::isWritable() const
{
  return d->device->isWritable();
}

/*!
    Returns true if the file exists; otherwise returns false.
*/
bool AQZipWriter::exists() const
{
  QFile *f = static_cast<QFile *>(d->device);
  if (f == 0)
    return true;
  return f->exists();
}

/*!
    \enum AQZipWriter::Status

    The following status values are possible:

    \value NoError  No error occurred.
    \value FileWriteError    An error occurred when writing to the device.
    \value FileOpenError    The file could not be opened.
    \value FilePermissionsError The file could not be accessed.
    \value FileError        Another file error occurred.
*/

/*!
    Returns a status code indicating the first error that was met by AQZipWriter,
    or AQZipWriter::NoError if no error occurred.
*/
AQZipWriter::Status AQZipWriter::status() const
{
  return d->status;
}

/*!
    \enum AQZipWriter::CompressionPolicy

    \value AlwaysCompress   A file that is added is compressed.
    \value NeverCompress    A file that is added will be stored without changes.
    \value AutoCompress     A file that is added will be compressed only if that will give a smaller file.
*/

/*!
     Sets the policy for compressing newly added files to the new \a policy.

    \note the default policy is AlwaysCompress

    \sa compressionPolicy()
    \sa addFile()
*/
void AQZipWriter::setCompressionPolicy(CompressionPolicy policy)
{
  d->compressionPolicy = policy;
}

/*!
     Returns the currently set compression policy.
    \sa setCompressionPolicy()
    \sa addFile()
*/
AQZipWriter::CompressionPolicy AQZipWriter::compressionPolicy() const
{
  return d->compressionPolicy;
}

/*!
    Sets the permissions that will be used for newly added files.

    \note the default permissions are QFile::ReadOwner | QFile::WriteOwner.

    \sa creationPermissions()
    \sa addFile()
*/
void AQZipWriter::setCreationPermissions(QFileInfo::PermissionSpec permissions)
{
  d->permissions = permissions;
}

/*!
     Returns the currently set creation permissions.

    \sa setCreationPermissions()
    \sa addFile()
*/
QFileInfo::PermissionSpec AQZipWriter::creationPermissions() const
{
  return (QFileInfo::PermissionSpec) d->permissions;
}

/*!
    Add a file to the archive with \a data as the file contents.
    The file will be stored in the archive using the \a fileName which
    includes the full path in the archive.

    The new file will get the file permissions based on the current
    creationPermissions and it will be compressed using the zip compression
    based on the current compression policy.

    \sa setCreationPermissions()
    \sa setCompressionPolicy()
*/
void AQZipWriter::addFile(const QString &fileName, const QByteArray &data)
{
  d->addEntry(AQZipWriterPrivate::File, fileName, data);
}

/*!
    Add a file to the archive with \a device as the source of the contents.
    The contents returned from QIODevice::readAll() will be used as the
    filedata.
    The file will be stored in the archive using the \a fileName which
    includes the full path in the archive.
*/
void AQZipWriter::addFile(const QString &fileName, QIODevice *device)
{
  Q_ASSERT(device);
  uint mode = device->mode();
  bool opened = false;
  if ((mode & IO_ReadOnly) == 0) {
    opened = true;
    if (! device->open(IO_ReadOnly)) {
      d->status = FileOpenError;
      return;
    }
  }
  d->addEntry(AQZipWriterPrivate::File, fileName, device->readAll());
  if (opened)
    device->close();
}

/*!
    Create a new directory in the archive with the specified \a dirName and
    the \a permissions;
*/
void AQZipWriter::addDirectory(const QString &dirName)
{
  QString name = dirName;
  // separator is mandatory
  if (!name.endsWith(QChar(QDir::separator())))
    name.append(QDir::separator());
  d->addEntry(AQZipWriterPrivate::Directory, name, QByteArray());
}

/*!
    Create a new symbolic link in the archive with the specified \a dirName
    and the \a permissions;
    A symbolic link contains the destination (relative) path and name.
*/
void AQZipWriter::addSymLink(const QString &fileName, const QString &destination)
{
  d->addEntry(AQZipWriterPrivate::Symlink, fileName, QFile::encodeName(destination));
}

/*!
   Closes the zip file.
*/
void AQZipWriter::close()
{
  if (!(d->device->mode() & IO_WriteOnly)) {
    d->device->close();
    return;
  }

  //qDebug("AQZip::close writing directory, %d entries", d->fileHeaders.size());
  d->device->at(d->start_of_directory);
  // write new directory
  for (uint i = 0; i < d->fileHeaders.size(); ++i) {
    const FileHeader &header = d->fileHeaders[i];
    d->device->writeBlock((const char *)&header.h, sizeof(CentralFileHeader));
    d->device->writeBlock(header.file_name);
    d->device->writeBlock(header.extra_field);
    d->device->writeBlock(header.file_comment);
  }
  int dir_size = d->device->at() - d->start_of_directory;
  // write end of directory
  EndOfDirectory eod;
  memset(&eod, 0, sizeof(EndOfDirectory));
  writeUInt(eod.signature, 0x06054b50);
  //uchar this_disk[2];
  //uchar start_of_directory_disk[2];
  writeUShort(eod.num_dir_entries_this_disk, d->fileHeaders.size());
  writeUShort(eod.num_dir_entries, d->fileHeaders.size());
  writeUInt(eod.directory_size, dir_size);
  writeUInt(eod.dir_start_offset, d->start_of_directory);
  writeUShort(eod.comment_length, d->comment.size());

  d->device->writeBlock((const char *)&eod, sizeof(EndOfDirectory));
  d->device->writeBlock(d->comment);
  d->device->close();
}

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
