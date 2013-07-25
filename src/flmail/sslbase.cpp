/***************************************************************************
                          sslbase.cpp  -  SSL support
                             -------------------
    begin                : Thu Apr 18 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include "sslbase.h"
#include "library.h"
#include "libraryloader.h"

extern "C" {
#ifndef NO_OPENSSL
  // Library initialization
  static int (*_SSL_library_init)() = 0L;
  static void (*_SSL_set_ssl_method)() = 0L;
  static void (*_SSL_set_connect_state)(SSL *) = 0L;
  // Error handling
  static void (*_SSL_load_error_strings)() = 0L;
  static void (*_ERR_clear_error)() = 0L;
  static void (*_ERR_free_strings)() = 0L;
  static void (*_ERR_remove_state)(unsigned long) = 0L;
  static void (*_SSL_get_error)(SSL *, int) = 0L;
  static void (*_ERR_print_errors_fp)(FILE *) = 0L;
  // SSL
  //  - new
  static SSL  *(*_SSL_new)(SSL_CTX *) = 0L;
  static SSL_CTX *(*_SSL_CTX_new)(SSL_METHOD *) = 0L;
  // - settings
  static long(*_SSL_CTX_set_mode)(SSL_CTX *ctx, long mode) = 0L;
  static long(*_SSL_set_mode)(SSL *ssl, long mode) = 0L;
  static long(*_SSL_CTX_get_mode)(SSL_CTX *ctx) = 0L;
  static long(*_SSL_get_mode)(SSL *ssl) = 0L;
  //  - free
  static void (*_SSL_free)(SSL *) = 0L;
  static void (*_SSL_CTX_free)(SSL_CTX *) = 0L;
  //  - actions
  static int (*_SSL_connect)(SSL *) = 0L;
  static int (*_SSL_shutdown)(SSL *) = 0L;
  static int (*_SSL_read)(SSL *, void *, int) = 0L;
  static void (*_SSL_set_read_ahead)(SSL *, int) = 0L;
  static int (*_SSL_write)(SSL *, const void *, int) = 0L;
  static int (*_SSL_pending)(SSL *) = 0L;
  static int (*_SSL_peek)(SSL *, void *, int) = 0L;
  static int (*_SSL_set_fd)(SSL *, int) = 0L;
  static long(*_SSL_ctrl)(SSL *, int, long, char *) = 0L;
  static int (*_SSL_do_handshake)(SSL *) = 0L;
  static int (*_SSL_use_certificate)(SSL *, X509 *) = 0L;
  static long(*_SSL_get_verify_result)(SSL *) = 0L;
  static X509 *(*_SSL_get_peer_certificate)(SSL *) = 0L;
  static int (*_i2d_X509)(X509 *, unsigned char **) = 0L;
  static int (*_i2d_X509_fp)(X509 *, FILE *) = 0L;
  static int (*_X509_print)(BIO *, X509 *) = 0L;
  static int (*_X509_print_fp)(FILE *, X509 *) = 0L;
  static int (*_PEM_write_X509)(FILE *, X509 *) = 0L;

#ifdef _OLD_EAY_
  static STACK *(*_SSL_get_peer_cert_chain)(SSL *) = 0L;
  static STACK *(*_SSL_load_client_CA_file)(const char *) = 0L;
#else
  static STACK_OF(X509) *(*_SSL_get_peer_cert_chain)(SSL *) = 0L;
  static STACK_OF(X509_NAME) *(*_SSL_load_client_CA_file)(const char *) = 0L;
#endif
  // Context
  static int (*_SSL_CTX_set_cipher_list)(SSL_CTX *, const char *) = 0L;
  static void (*_SSL_CTX_set_verify)(SSL_CTX *, int, int ( *)(int, X509_STORE_CTX *)) = 0L;
  static int (*_SSL_CTX_load_verify_locations)(SSL_CTX *, const char *, const char *) = 0L;
  static void (*_SSL_CTX_set_verify_depth)(SSL_CTX *, int) = 0L;
  static int (*_SSL_CTX_use_PrivateKey)(SSL_CTX *, EVP_PKEY *) = 0L;
  static SSL_CIPHER *(*_SSL_get_current_cipher)(SSL *) = 0L;
  static int (*_SSL_CIPHER_get_bits)(SSL_CIPHER * , int *) = 0L;
  static char *(*_SSL_CIPHER_get_version)(SSL_CIPHER *) = 0L;
  static const char *(*_SSL_CIPHER_get_name)(SSL_CIPHER *) = 0L;
  static char *(*_SSL_CIPHER_description)(SSL_CIPHER *, char *, int) = 0L;
  // Client methods
  static SSL_METHOD *(*_TLSv1_client_method)() = 0L;
  static SSL_METHOD *(*_SSLv2_client_method)() = 0L;
  static SSL_METHOD *(*_SSLv3_client_method)() = 0L;
  static SSL_METHOD *(*_SSLv23_client_method)() = 0L;
  // BIO
  static BIO *(*_BIO_new)(BIO_METHOD *) = 0L;
  static BIO_METHOD *(*_BIO_s_mem)() = 0L;
  static int (*_BIO_write)(BIO *, const void *, int) = 0L;
  static int (*_BIO_read)(BIO *, void *, int) = 0L;
  static long(*_BIO_ctrl)(BIO *, int, long, void *) = 0L;

  // EAY library
  static void (*_SSLeay_add_ssl_algorithms)() = 0L;

  // crypto library
  static void (*_OpenSSL_add_all_algorithms)() = 0L;
  static void (*_OpenSSL_add_all_ciphers)() = 0L;
  static void (*_OpenSSL_add_all_digests)() = 0L;
  static BIO *(*_BIO_new_fp)(FILE *, int) = 0L;
  static int (*_BIO_free)(BIO *) = 0L;

#endif
};

using namespace tkcSSL;

SSLBase *SSLBase::_sslBaseRef = 0L;

SSLBase::SSLBase()
  : _sslLibrary(0L),
    _cryptoLibrary(0L),
    _eayLibrary(0L),
    _ssl(false)
{
  _ssl = loadSSL();
  if (!_ssl)
    qDebug("The SSL libraries aren't loaded!");
}

SSLBase::~SSLBase()
{
  if (_sslLibrary) {
    delete _sslLibrary;
    _sslLibrary = 0L;
  }
  if (_cryptoLibrary) {
    delete _cryptoLibrary;
    _cryptoLibrary = 0L;
  }
  if (_eayLibrary) {
    delete _eayLibrary;
    _eayLibrary = 0L;
  }
}

SSLBase *SSLBase::ref()
{
  if (!_sslBaseRef) {
    _sslBaseRef = new SSLBase();
  }

  return _sslBaseRef;
}

bool SSLBase::isOpenSSL()
{
  return _ssl;
}

void SSLBase::update()
{
}

bool SSLBase::loadSSL()
{
#ifndef NO_OPENSSL

  LibraryLoader *ld = LibraryLoader::ref();

  if (!ld)
    return false;

  if (_ssl)
    return true;

#ifdef TKCSSL_WIN32
  _sslLibrary = ld->loadLibrary("ssleay32");
  _eayLibrary = ld->loadLibrary("libeay32");

  if (!_sslLibrary || !_eayLibrary) {
    qDebug("The SSL libraries aren't loaded!");

    if (_sslLibrary) {
      delete _sslLibrary;
      _sslLibrary = 0L;
    }
    if (_eayLibrary) {
      delete _eayLibrary;
      _eayLibrary = 0L;
    }

    return false;
  }
#else // TKCSSL_WIN32
  QString cryptoLibName("crypto");
  QString sslLibName("ssl");
#ifdef TKCSSL_WIN32_LD
  cryptoLibName += AQSTRSSLVERSION;
  sslLibName += AQSTRSSLVERSION;
#endif

  _cryptoLibrary = ld->loadLibrary(cryptoLibName);
  _sslLibrary = ld->loadLibrary(sslLibName);

  if (!_sslLibrary) {
    qDebug("Could not load the ssl library.");
    if (_cryptoLibrary) {
      delete _cryptoLibrary;
      _cryptoLibrary = 0L;
    }

    return false;
  }
  if (!_cryptoLibrary) {
    qDebug("Could not load the crypto library.");
    if (_sslLibrary) {
      delete _sslLibrary;
      _sslLibrary = 0L;
    }

    return false;
  }
#endif // TKCSSL_WIN32

  if (checkSymbols()) {
    // initialize the libraries
    if (_eayLibrary || _sslLibrary) {
#ifdef _OLD_EAY_
      _SSLeay_add_ssl_algorithms();
#else
      if (-1 == _SSL_library_init()) {
        if (_OpenSSL_add_all_algorithms)
          _OpenSSL_add_all_algorithms();
        else {
          qDebug("No _OpenSSL_add_all_algorithms detected.");
        }
      }

      if (_SSL_load_error_strings)
        _SSL_load_error_strings();
      else {
        qDebug("No _SSL_load_error_strings detected.");
      }
#endif
    }

    if (_cryptoLibrary) {
      if (_OpenSSL_add_all_algorithms)
        _OpenSSL_add_all_algorithms();
      if (_OpenSSL_add_all_ciphers)
        _OpenSSL_add_all_ciphers();
      if (_OpenSSL_add_all_digests)
        _OpenSSL_add_all_digests();
    }

    return true;
  }
#endif // NO_OPENSSL

  return false;
}

bool SSLBase::checkSymbols()
{
#ifndef NO_OPENSSL
  // being paranoic
  if (!_sslLibrary)
    return false;
  if (_ssl)
    return true;

#ifndef _OLD_EAY_
  _SSL_library_init = (int ( *)())_sslLibrary->symbolAddress("SSL_library_init");
#endif
  _SSL_set_ssl_method = (void ( *)())_sslLibrary->symbolAddress("SSL_set_ssl_method");
  _SSL_set_connect_state = (void ( *)(SSL *))_sslLibrary->symbolAddress("SSL_set_connect_state");
  _SSL_load_error_strings = (void ( *)())_sslLibrary->symbolAddress("SSL_load_error_strings");
  _ERR_clear_error = (void ( *)())_sslLibrary->symbolAddress("ERR_clear_error");
  _ERR_print_errors_fp = (void ( *)(FILE *))_cryptoLibrary->symbolAddress("ERR_print_errors_fp");
#ifdef TKCSSL_WIN32
  _ERR_free_strings = (void ( *)())_eayLibrary->symbolAddress("ERR_free_strings");
  _ERR_remove_state = (void ( *)(unsigned long))_eayLibrary->symbolAddress("ERR_remove_state");
#else
  _ERR_free_strings = (void ( *)())_sslLibrary->symbolAddress("ERR_free_strings");
  _ERR_remove_state = (void ( *)(unsigned long))_sslLibrary->symbolAddress("ERR_remove_state");
#endif
  _SSL_get_error = (void ( *)(SSL *, int))_sslLibrary->symbolAddress("SSL_get_error");
  _SSL_new = (SSL * ( *)(SSL_CTX *))_sslLibrary->symbolAddress("SSL_new");
  _SSL_CTX_new = (SSL_CTX * ( *)(SSL_METHOD *))_sslLibrary->symbolAddress("SSL_CTX_new");
  _SSL_CTX_set_mode = (long( *)(SSL_CTX * ctx, long mode))_sslLibrary->symbolAddress("SSL_CTX_set_mode");
  _SSL_set_mode = (long( *)(SSL * ssl, long mode))_sslLibrary->symbolAddress("SSL_set_mode");
  _SSL_CTX_get_mode = (long( *)(SSL_CTX * ctx))_sslLibrary->symbolAddress("SSL_CTX_get_mode");
  _SSL_get_mode = (long( *)(SSL * ssl))_sslLibrary->symbolAddress("SSL_get_mode");
  _SSL_free = (void ( *)(SSL *))_sslLibrary->symbolAddress("SSL_free");
  _SSL_CTX_free = (void ( *)(SSL_CTX *))_sslLibrary->symbolAddress("SSL_CTX_free");
  _SSL_connect = (int ( *)(SSL *))_sslLibrary->symbolAddress("SSL_connect");
  _SSL_shutdown = (int ( *)(SSL *))_sslLibrary->symbolAddress("SSL_shutdown");
  _SSL_read = (int ( *)(SSL *, void *, int))_sslLibrary->symbolAddress("SSL_read");
  _SSL_set_read_ahead = (void ( *)(SSL *, int))_sslLibrary->symbolAddress("SSL_set_read_ahead");
  _SSL_write = (int ( *)(SSL *, const void *, int))_sslLibrary->symbolAddress("SSL_write");
  _SSL_pending = (int ( *)(SSL *))_sslLibrary->symbolAddress("SSL_pending");
  _SSL_peek = (int ( *)(SSL *, void *, int))_sslLibrary->symbolAddress("SSL_peek");
  _SSL_set_fd = (int ( *)(SSL *, int))_sslLibrary->symbolAddress("SSL_set_fd");
  _SSL_ctrl = (long( *)(SSL *, int, long, char *))_sslLibrary->symbolAddress("SSL_ctrl");
  _SSL_do_handshake = (int ( *)(SSL *))_sslLibrary->symbolAddress("SSL_do_handshake");
  _SSL_use_certificate = (int ( *)(SSL *, X509 *))_sslLibrary->symbolAddress("SSL_use_certificate");
  _SSL_get_verify_result = (long( *)(SSL *))_sslLibrary->symbolAddress("SSL_get_verify_result");
  _i2d_X509 = (int ( *)(X509 *, unsigned char **))_sslLibrary->symbolAddress("i2d_X509");
  _i2d_X509_fp = (int ( *)(X509 *, FILE *))_sslLibrary->symbolAddress("i2d_X509_fp");
  _X509_print = (int ( *)(BIO *, X509 *))_sslLibrary->symbolAddress("X509_print");
  _X509_print_fp = (int ( *)(FILE *, X509 *))_sslLibrary->symbolAddress("X509_print_fp");
  _PEM_write_X509 = (int ( *)(FILE *, X509 *))_sslLibrary->symbolAddress("PEM_write_X509");
  _SSL_use_certificate = (int ( *)(SSL *, X509 *))_sslLibrary->symbolAddress("SSL_use_certificate");
  _SSL_get_peer_certificate = (X509 * ( *)(SSL *))_sslLibrary->symbolAddress("SSL_get_peer_certificate");
#ifdef _OLD_EAY_
  _SSL_get_peer_cert_chain = (STACK * ( *)(SSL *))_sslLibrary->symbolAddress("SSL_get_peer_cert_chain");
  _SSL_load_client_CA_file = (STACK * ( *)(const char *))_sslLibrary->symbolAddress("SSL_load_client_CA_file");
#else
  _SSL_get_peer_cert_chain = (STACK_OF(X509) * ( *)(SSL *))_sslLibrary->symbolAddress("SSL_get_peer_cert_chain");
  _SSL_load_client_CA_file = (STACK_OF(X509_NAME) * ( *)(const char *))_sslLibrary->symbolAddress("SSL_load_client_CA_file");
#endif
  _SSL_CTX_set_cipher_list = (int ( *)(SSL_CTX *, const char *))_sslLibrary->symbolAddress("SSL_CTX_set_cipher_list");
  _SSL_CTX_set_verify = (void ( *)(SSL_CTX *, int, int ( *)(int, X509_STORE_CTX *)))_sslLibrary->symbolAddress("SSL_CTX_set_verify");
  _SSL_CTX_load_verify_locations = (int ( *)(SSL_CTX *, const char *, const char *))_sslLibrary->symbolAddress("SSL_CTX_load_verify_locations");
  _SSL_CTX_set_verify_depth = (void ( *)(SSL_CTX *, int))_sslLibrary->symbolAddress("SSL_CTX_set_verify_depth");
  _SSL_CTX_use_PrivateKey = (int ( *)(SSL_CTX *, EVP_PKEY *))_sslLibrary->symbolAddress("SSL_CTX_use_PrivateKey");
  _SSL_get_current_cipher = (SSL_CIPHER * ( *)(SSL *))_sslLibrary->symbolAddress("SSL_get_current_cipher");
  _SSL_CIPHER_get_bits = (int ( *)(SSL_CIPHER * , int *))_sslLibrary->symbolAddress("SSL_CIPHER_get_bits");
  _SSL_CIPHER_get_version = (char * ( *)(SSL_CIPHER *))_sslLibrary->symbolAddress("SSL_CIPHER_get_version");
  _SSL_CIPHER_get_name = (const char * ( *)(SSL_CIPHER *))_sslLibrary->symbolAddress("SSL_CIPHER_get_name");
  _SSL_CIPHER_description = (char * ( *)(SSL_CIPHER *, char *, int))_sslLibrary->symbolAddress("SSL_CIPHER_description");
  _TLSv1_client_method = (SSL_METHOD * ( *)())_sslLibrary->symbolAddress("TLSv1_client_method");
  _SSLv2_client_method = (SSL_METHOD * ( *)())_sslLibrary->symbolAddress("SSLv2_client_method");
  _SSLv3_client_method = (SSL_METHOD * ( *)())_sslLibrary->symbolAddress("SSLv3_client_method");
  _SSLv23_client_method = (SSL_METHOD * ( *)())_sslLibrary->symbolAddress("SSLv23_client_method");
  _SSLeay_add_ssl_algorithms = (void ( *)())_sslLibrary->symbolAddress("SSLeay_add_ssl_algorithms");
  if (!_SSLeay_add_ssl_algorithms) {
    _SSLeay_add_ssl_algorithms = (void ( *)())_sslLibrary->symbolAddress("OpenSSL_add_ssl_algorithms");
  }

#ifdef TKCSSL_WIN32
  _BIO_new = (BIO * ( *)(BIO_METHOD *))_eayLibrary->symbolAddress("BIO_new");
  _BIO_s_mem = (BIO_METHOD * ( *)())_eayLibrary->symbolAddress("BIO_s_mem");
  _BIO_write = (int ( *)(BIO *, const void *, int))_eayLibrary->symbolAddress("BIO_write");
  _BIO_read = (int ( *)(BIO *, void *, int))_eayLibrary->symbolAddress("BIO_read");
  _BIO_ctrl = (long( *)(BIO *, int, long, void *))_eayLibrary->symbolAddress("BIO_ctrl");
#else
  _BIO_new = (BIO * ( *)(BIO_METHOD *))_sslLibrary->symbolAddress("BIO_new");
  _BIO_s_mem = (BIO_METHOD * ( *)())_sslLibrary->symbolAddress("BIO_s_mem");
  _BIO_write = (int ( *)(BIO *, const void *, int))_sslLibrary->symbolAddress("BIO_write");
  _BIO_read = (int ( *)(BIO *, void *, int))_sslLibrary->symbolAddress("BIO_read");
  _BIO_ctrl = (long( *)(BIO *, int, long, void *))_sslLibrary->symbolAddress("BIO_ctrl");
#endif

  // crypto library
  if (_cryptoLibrary) {
    _OpenSSL_add_all_algorithms = (void ( *)())_cryptoLibrary->symbolAddress("OpenSSL_add_all_algorithms");
    _OpenSSL_add_all_ciphers = (void ( *)())_cryptoLibrary->symbolAddress("OpenSSL_add_all_ciphers");
    _OpenSSL_add_all_digests = (void ( *)())_cryptoLibrary->symbolAddress("OpenSSL_add_all_digests");
    _BIO_new_fp = (BIO * ( *)(FILE *, int))_cryptoLibrary->symbolAddress("BIO_new_fp");
    _BIO_free = (int ( *)(BIO *))_cryptoLibrary->symbolAddress("BIO_free");
  }

#ifdef _OLD_EAY_
  if (_SSL_new && _SSL_CTX_new && _SSLeay_add_ssl_algorithms)
    return true;
#else
  if (_SSL_new &&
      _SSL_CTX_new &&
      (_SSL_library_init || _SSLeay_add_ssl_algorithms))
    return true;
#endif

#endif // NO_OPENSSL

  return false;
}


//  SSL  symbols
#ifndef NO_OPENSSL
int SSLBase::SSL_library_init()
{
  if (_SSL_library_init)
    return _SSL_library_init();

  return -1;
}

void SSLBase::SSL_set_ssl_method()
{
  if (_SSL_set_ssl_method)
    _SSL_set_ssl_method();
}

void SSLBase::SSL_set_connect_state(SSL *ssl)
{
  if (_SSL_set_connect_state)
    _SSL_set_connect_state(ssl);
}

void SSLBase::SSL_load_error_strings()
{
  if (_SSL_load_error_strings)
    _SSL_load_error_strings();
}

void SSLBase::ERR_clear_error()
{
  if (_ERR_clear_error)
    _ERR_clear_error();
}

void SSLBase::ERR_free_strings()
{
  if (_ERR_free_strings)
    _ERR_free_strings();
}

void SSLBase::ERR_remove_state(unsigned long ulPid)
{
  if (_ERR_remove_state)
    _ERR_remove_state(ulPid);
}

void SSLBase::ERR_print_errors_fp(FILE *fp)
{
  if (_ERR_print_errors_fp)
    _ERR_print_errors_fp(fp);
}

void SSLBase::SSL_get_error(SSL *ssl, int iRetCode)
{
  if (_SSL_get_error)
    _SSL_get_error(ssl, iRetCode);
}

SSL  *SSLBase::SSL_new(SSL_CTX *sslCTX)
{
  if (_SSL_new)
    return _SSL_new(sslCTX);

  return NULL;
}

SSL_CTX *SSLBase::SSL_CTX_new(SSL_METHOD *sslMethod)
{
  if (_SSL_CTX_new)
    return _SSL_CTX_new(sslMethod);

  return NULL;
}

long SSLBase::tkc_SSL_CTX_set_mode(SSL_CTX *ctx, long mode)
{
  if (_SSL_CTX_set_mode)
    return _SSL_CTX_set_mode(ctx, mode);

  return -1;
}

long SSLBase::tkc_SSL_set_mode(SSL *ssl, long mode)
{
  if (_SSL_set_mode)
    return _SSL_set_mode(ssl, mode);

  qDebug("Symbol SSL_set_mode not found");

  return -1;
}

long SSLBase::tkc_SSL_CTX_get_mode(SSL_CTX *ctx)
{
  if (_SSL_CTX_get_mode)
    return _SSL_CTX_get_mode(ctx);

  return -1;
}

long SSLBase::tkc_SSL_get_mode(SSL *ssl)
{
  if (_SSL_get_mode)
    return _SSL_get_mode(ssl);

  return -1;
}

void SSLBase::SSL_free(SSL *ssl)
{
  if (_SSL_free)
    _SSL_free(ssl);
}

void SSLBase::SSL_CTX_free(SSL_CTX *sslCTX)
{
  if (_SSL_CTX_free)
    _SSL_CTX_free(sslCTX);
}

int SSLBase::SSL_connect(SSL *ssl)
{
  if (_SSL_connect)
    return _SSL_connect(ssl);

  return -1;
}

int SSLBase::SSL_shutdown(SSL *ssl)
{
  if (_SSL_shutdown)
    return _SSL_shutdown(ssl);

  return -1;
}

int SSLBase::SSL_read(SSL *ssl, void *buffer, int length)
{
  if (_SSL_read)
    return _SSL_read(ssl, buffer, length);

  return -1;
}

void SSLBase::SSL_set_read_ahead(SSL *ssl, int yes)
{
  if (_SSL_set_read_ahead)
    _SSL_set_read_ahead(ssl, yes);
  else
    qDebug("SSL_set_read_ahead is NOT available.");
}

int SSLBase::SSL_write(SSL *ssl, const void *buffer, int length)
{
  if (_SSL_write)
    return _SSL_write(ssl, buffer, length);

  return -1;
}

int SSLBase::SSL_pending(SSL *ssl)
{
  if (_SSL_pending)
    return _SSL_pending(ssl);

  return -1;
}

int SSLBase::SSL_peek(SSL *ssl, void *buffer, int length)
{
  if (_SSL_peek)
    return _SSL_peek(ssl, buffer, length);

  return -1;
}

int SSLBase::SSL_set_fd(SSL *ssl, int fd)
{
  if (_SSL_set_fd)
    return _SSL_set_fd(ssl, fd);

  return -1;
}

long SSLBase::SSL_ctrl(SSL *ssl, int command, long argsLength, char *argsList)
{
  if (_SSL_ctrl)
    return _SSL_ctrl(ssl, command, argsLength, argsList);

  return -1;
}

int SSLBase::SSL_do_handshake(SSL *ssl)
{
  if (_SSL_do_handshake)
    return _SSL_do_handshake(ssl);

  return -1;
}

int SSLBase::SSL_use_certificate(SSL *ssl, X509 *x)
{
  if (_SSL_use_certificate)
    return _SSL_use_certificate(ssl, x);

  return -1;
}

long SSLBase::SSL_get_verify_result(SSL *ssl)
{
  if (_SSL_get_verify_result)
    return _SSL_get_verify_result(ssl);

  return -1;
}

int SSLBase::i2d_X509(X509 *x, unsigned char **output)
{
  if (_i2d_X509)
    return _i2d_X509(x, output);

  return -1;
}

int SSLBase::i2d_X509_fp(X509 *x, FILE *fp)
{
  if (_i2d_X509_fp)
    return _i2d_X509_fp(x, fp);

  return -1;
}

int SSLBase::X509_print(BIO *b, X509 *x)
{
  if (_X509_print)
    return _X509_print(b, x);

  return -1;
}

int SSLBase::X509_print_fp(FILE *fp, X509 *x)
{
  if (_X509_print_fp)
    return _X509_print_fp(fp, x);

  return -1;
}

int SSLBase::PEM_write_X509(FILE *fp, X509 *x)
{
  if (_PEM_write_X509)
    return _PEM_write_X509(fp, x);

  return -1;
}

X509 *SSLBase::SSL_get_peer_certificate(SSL *ssl)
{
  if (_SSL_get_peer_certificate)
    return _SSL_get_peer_certificate(ssl);

  return 0L;
}

#ifdef TKCSSL_WIN32
STACK *SSLBase::SSL_get_peer_cert_chain(SSL *ssl)
#else
STACK_OF(X509) *SSLBase::SSL_get_peer_cert_chain(SSL *ssl)
#endif
{
  if (_SSL_get_peer_cert_chain)
    return _SSL_get_peer_cert_chain(ssl);

  return NULL;
}

#ifdef TKCSSL_WIN32
STACK *SSLBase::SSL_load_client_CA_file(const char *file)
#else
STACK_OF(X509_NAME) *SSLBase::SSL_load_client_CA_file(const char *file)
#endif
{
  if (_SSL_load_client_CA_file)
    return _SSL_load_client_CA_file(file);

  return NULL;
}

int SSLBase::SSL_CTX_set_cipher_list(SSL_CTX *ctx, const char *str)
{
  if (_SSL_CTX_set_cipher_list)
    return _SSL_CTX_set_cipher_list(ctx, str);

  return -1;
}

void SSLBase::SSL_CTX_set_verify(SSL_CTX *ctx, int mode, int (*callback)(int, X509_STORE_CTX *))
{
  if (_SSL_CTX_set_verify)
    _SSL_CTX_set_verify(ctx, mode, callback);
}

int SSLBase::SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *fileName, const char *folderName)
{
  if (_SSL_CTX_load_verify_locations)
    return _SSL_CTX_load_verify_locations(ctx, fileName, folderName);

  return -1;
}

void SSLBase::SSL_CTX_set_verify_depth(SSL_CTX *ctx, int depth)
{
  if (_SSL_CTX_set_verify_depth)
    _SSL_CTX_set_verify_depth(ctx, depth);
}

int SSLBase::SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey)
{
  if (_SSL_CTX_use_PrivateKey)
    return _SSL_CTX_use_PrivateKey(ctx, pkey);

  return -1;
}

SSL_CIPHER *SSLBase::SSL_get_current_cipher(SSL *ssl)
{
  if (_SSL_get_current_cipher)
    return _SSL_get_current_cipher(ssl);

  return NULL;
}

int SSLBase::SSL_CIPHER_get_bits(SSL_CIPHER *c, int *algBits)
{
  if (_SSL_CIPHER_get_bits)
    return _SSL_CIPHER_get_bits(c, algBits);

  return -1;
}

char *SSLBase::SSL_CIPHER_get_version(SSL_CIPHER *c)
{
  if (_SSL_CIPHER_get_version)
    return _SSL_CIPHER_get_version(c);

  return NULL;
}

const char *SSLBase::SSL_CIPHER_get_name(SSL_CIPHER *c)
{
  if (_SSL_CIPHER_get_name)
    return _SSL_CIPHER_get_name(c);

  return NULL;
}

char *SSLBase::SSL_CIPHER_description(SSL_CIPHER *c, char *buffer, int length)
{
  if (_SSL_CIPHER_description)
    return _SSL_CIPHER_description(c, buffer, length);

  return NULL;
}

SSL_METHOD *SSLBase::TLSv1_client_method()
{
  if (_TLSv1_client_method)
    return _TLSv1_client_method();

  return NULL;
}

SSL_METHOD *SSLBase::SSLv2_client_method()
{
  if (_SSLv2_client_method)
    return _SSLv2_client_method();

  return NULL;
}

SSL_METHOD *SSLBase::SSLv3_client_method()
{
  if (_SSLv3_client_method)
    return _SSLv3_client_method();

  return NULL;
}

SSL_METHOD *SSLBase::SSLv23_client_method()
{
  if (_SSLv23_client_method)
    return _SSLv23_client_method();

  return NULL;
}

BIO *SSLBase::BIO_new(BIO_METHOD *type)
{
  if (_BIO_new)
    return _BIO_new(type);

  return NULL;
}

BIO_METHOD *SSLBase::BIO_s_mem()
{
  if (_BIO_s_mem)
    return _BIO_s_mem();

  return NULL;
}

long SSLBase::SSL_BIO_get_mem_data(BIO *b, char **ptr)
{
  return BIO_ctrl(b, BIO_CTRL_INFO, 0, (char *) ptr);
}

int SSLBase::BIO_write(BIO *b, const void *buffer, int length)
{
  if (_BIO_write)
    return _BIO_write(b, buffer, length);

  return -1;
}

int SSLBase::BIO_read(BIO *b, void *buffer, int length)
{
  if (_BIO_read)
    return _BIO_read(b, buffer, length);

  return -1;
}

long SSLBase::BIO_ctrl(BIO *b, int command, long argsLength, void *argsList)
{
  if (_BIO_ctrl)
    return _BIO_ctrl(b, command, argsLength, argsList);

  return -1;
}

void SSLBase::OpenSSL_add_all_algorithms()
{
  if (_OpenSSL_add_all_algorithms)
    _OpenSSL_add_all_algorithms();
}

void SSLBase::OpenSSL_add_all_ciphers()
{
  if (_OpenSSL_add_all_ciphers)
    _OpenSSL_add_all_ciphers();
}

void SSLBase::OpenSSL_add_all_digests()
{
  if (_OpenSSL_add_all_digests)
    _OpenSSL_add_all_digests();
}

BIO *SSLBase::BIO_new_fp(FILE *stream, int closeFlag)
{
  if (_BIO_new_fp)
    return _BIO_new_fp(stream, closeFlag);

  return NULL;
}

int SSLBase::BIO_free(BIO *b)
{
  if (_BIO_free)
    return _BIO_free(b);

  return -1;
}

QString SSLBase::verifyErrorString(long err)
{
  // Comments from man verify(1)
  switch (err) {
    case X509_V_OK:
      // the operation was successful.
      return tr("An unknown error occurred");
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
      // the passed certificate is self signed and the same
      // certificate cannot be found in the list of trusted
      // certificates.
      return tr("Self signed certificate");
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
      // the certificate chain could be built up using the
      // untrusted certificates but the root could not be found
      // locally.
      return tr("Self signed certificate in certificate chain");
    case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
      // the certificate signature could not be decrypted. This
      // means that the actual signature value could not be
      // determined rather than it not matching the expected
      // value, this is only meaningful for RSA keys.
      return tr("Unable to decrypt certificate's signature");
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
      // the signature of the certificate is invalid.
      return tr("Certificate signature failure");
    case X509_V_ERR_CERT_NOT_YET_VALID:
      // the certificate is not yet valid: the notBefore date is
      // after the current time.
      return tr("The certificate is not yet valid");
    case X509_V_ERR_CERT_HAS_EXPIRED:
      // the certificate has expired: that is the notAfter date
      // is before the current time.
      return tr("The certificate has expired");
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
      // the certificate notBefore field contains an invalid
      // time.
      return tr("Format error in certificate's notBefore field");
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
      // the certificate notAfter field contains an invalid
      // time.
      return tr("Format error in certificate's notAfter field");
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
      // the issuer certificate of a locally looked up
      // certificate could not be found. This normally means
      // the list of trusted certificates is not complete.
      return tr("Unable to get local issuer certificate");
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
      // no signatures could be verified because the chain
      // contains only one certificate and it is not self
      // signed.
      return tr("Unable to verify the first certificate");
    case X509_V_ERR_INVALID_CA:
      // a CA certificate is invalid. Either it is not a CA or
      // its extensions are not consistent with the supplied
      // purpose
      return tr("Invalid CA certificate");
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
      // the issuer certificate could not be found: this occurs
      // if the issuer certificate of an untrusted certificate
      // cannot be found.
      return tr("Unable to get issuer certificate");
    case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
      // the public key in the certificate SubjectPublicKeyInfo
      // could not be read.
      return tr("Unable to decode issuer public key");
    case X509_V_ERR_PATH_LENGTH_EXCEEDED:
      // the basicConstraints pathlength parameter has been
      // exceeded.
      return tr("Path length constraint exceeded");
    case X509_V_ERR_INVALID_PURPOSE:
      // the supplied certificate cannot be used for the
      // specified purpose.
      return tr("Unsupported certificate purpose");
    case X509_V_ERR_CERT_UNTRUSTED:
      // the root CA is not marked as trusted for the specified
      // purpose.
      return tr("Certificate not trusted");
    case X509_V_ERR_CERT_REJECTED:
      // the root CA is marked to reject the specified purpose.
      return tr("Certificate rejected");
    case X509_V_ERR_SUBJECT_ISSUER_MISMATCH:
      // the current candidate issuer certificate was rejected
      // because its subject name did not match the issuer name
      // of the current certificate.
      return tr("Subject issuer mismatch");
    case X509_V_ERR_AKID_SKID_MISMATCH:
      // the current candidate issuer certificate was rejected
      // because its subject key identifier was present and did
      // not match the authority key identifier current
      // certificate.
      return tr("Authority and subject key identifier mismatch");
    case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH:
      // the current candidate issuer certificate was rejected
      // because its issuer name and serial number was present
      // and did not match the authority key identifier of the
      // current certificate.
      return tr("Authority and issuer serial number mismatch");
    case X509_V_ERR_KEYUSAGE_NO_CERTSIGN:
      // the current candidate issuer certificate was rejected
      // because its keyUsage extension does not permit
      // certificate signing.
      return tr("Key usage does not include certificate signing");
    case X509_V_ERR_OUT_OF_MEM:
      // an error occurred trying to allocate memory. This
      // should never happen.
      return tr("Out of memory");
    default:
      return tr("Unknown error");
  }
}

#endif // NO_OPENSSL
