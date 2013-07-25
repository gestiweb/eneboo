/***************************************************************************
                          sslbase.h  -  SSL support
                             -------------------
    begin                : Thu Apr 18 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#ifndef SSLBASE_H
#define SSLBASE_H

#if defined( _WIN32 )
// #define TKCSSL_WIN32
// #define _OLD_EAY_ // for using SSLeay old library
#define TKCSSL_WIN32_LD // ### AbanQ
#include <windows.h>
#ifdef TKCSSL_EXPORTS
#define TKCSSL_API __declspec(dllexport)
#else
#define TKCSSL_API __declspec(dllimport)
#endif
#elif defined( __BEOS__ )
#define TKCSSL_BEOS
#include <image.h>
#define HMODULE image_id
#elif defined( __ATHEOS__ )
#define TKCSSL_ATHEOS
#define TKCSSL_API
#define HMODULE int
#elif defined(__DARWIN_X11__)
#define TKCSSL_DARWIN
#define TKCSSL_MAC
#define TKCSSL_API
#define HMODULE void*
#elif defined(__APPLE__) && defined(__GNUC__)
#define TKCSSL_MACX
#define TKCSSL_MAC
#define TKCSSL_API
#define HMODULE void*
#elif defined(__MACOSX__)
#define TKCSSL_MACX
#define TKCSSL_MAC
#define TKCSSL_API
#define HMODULE void*
#elif defined(macintosh)
#define TKCSSL_MAC9
#define TKCSSL_MAC
#define TKCSSL_API
#define HMODULE void*
#else
#define TKCSSL_UNIX
#define TKCSSL_LINUX
#define TKCSSL_FREEBSD
#define TKCSSL_API
#define HMODULE void*
#endif

#ifndef NO_OPENSSL

#ifdef _OLD_EAY_
#include <stdio.h>
#include <ssleay/ssl.h>
#else
#include <openssl/ssl.h>

#ifdef OpenSSL_add_ssl_algorithms
#undef OpenSSL_add_ssl_algorithms
#endif

#ifdef SSLeay_add_ssl_algorithms
#undef SSLeay_add_ssl_algorithms
#endif

#endif // _OLD_EAY_

//#include <openssl/opensslconf.h>
//#define NO_COMP
//#define NO_BIO
//#define NO_X509
//#define NO_RSA
//#define NO_MD5
//#define NO_SSL2
#endif

#include <qobject.h>

namespace tkcSSL
{

  class Library;

  /**It has the main SSL data.
    *@author Eugen Constantinescu
    */

  class SSLBase : QObject
  {
    Q_OBJECT

  public:
    virtual ~SSLBase();

    /** Reference to this singleton class.*/
    static SSLBase *ref();
    /** Return true if OpenSSL was loaded right.*/
    bool isOpenSSL();

  signals:
    void error();

  public slots:
    virtual void update();

  private:
    /** Private because we are using ref().*/
    SSLBase();
    /** Singleton class object.*/
    static SSLBase *_sslBaseRef;

    /** Load the SSL libraries.
      * It has to set the SSL flag.
      */
    bool loadSSL();
    /** Search in Open SSL library for all the symbols we need.*/
    bool checkSymbols();
    /** SSL library handler.*/
    Library *_sslLibrary;
    /** Crypto library handler.*/
    Library *_cryptoLibrary;
    /** EAY library handler.*/
    Library *_eayLibrary;

    /** SSL flag.*/
    bool _ssl;

  public:
    // SSL calls
#ifndef NO_OPENSSL
    int SSL_library_init();
    void SSLeay_add_ssl_algorithms();
    void SSL_set_ssl_method();
    void SSL_set_connect_state(SSL *);
    void SSL_load_error_strings();
    void ERR_clear_error();
    void ERR_free_strings();
    void ERR_remove_state(unsigned long);
    void ERR_print_errors_fp(FILE *fp);
    void SSL_get_error(SSL *, int);
    SSL *SSL_new(SSL_CTX *);
    SSL_CTX *SSL_CTX_new(SSL_METHOD *);

    long tkc_SSL_CTX_set_mode(SSL_CTX *, long);
    long tkc_SSL_set_mode(SSL *, long);
    long tkc_SSL_CTX_get_mode(SSL_CTX *);
    long tkc_SSL_get_mode(SSL *);

    void SSL_free(SSL *);
    void SSL_CTX_free(SSL_CTX *);
    int SSL_connect(SSL *);
    int SSL_shutdown(SSL *);
    int SSL_read(SSL *, void *, int);
    void SSL_set_read_ahead(SSL *, int);
    int SSL_write(SSL *, const void *, int);
    int SSL_pending(SSL *);
    int SSL_peek(SSL *, void *, int);
    int SSL_set_fd(SSL *, int);
    long SSL_ctrl(SSL *, int, long, char *);
    int SSL_do_handshake(SSL *);
    int SSL_use_certificate(SSL *, X509 *);
    long SSL_get_verify_result(SSL *);
    X509 *SSL_get_peer_certificate(SSL *);
    int i2d_X509(X509 *, unsigned char **);
    int i2d_X509_fp(X509 *, FILE *);
    int X509_print(BIO *, X509 *);
    int X509_print_fp(FILE *, X509 *);
    int PEM_write_X509(FILE *, X509 *);
#ifdef TKCSSL_WIN32
    STACK *SSL_get_peer_cert_chain(SSL *);
    STACK *SSL_load_client_CA_file(const char *);
#else
    STACK_OF(X509) *SSL_get_peer_cert_chain(SSL *);
    STACK_OF(X509_NAME) *SSL_load_client_CA_file(const char *);
#endif
    int SSL_CTX_set_cipher_list(SSL_CTX *, const char *);
    void SSL_CTX_set_verify(SSL_CTX *, int, int ( *)(int, X509_STORE_CTX *));
    int SSL_CTX_load_verify_locations(SSL_CTX *, const char *, const char *);
    void SSL_CTX_set_verify_depth(SSL_CTX *, int);
    int SSL_CTX_use_PrivateKey(SSL_CTX *, EVP_PKEY *);
    SSL_CIPHER *SSL_get_current_cipher(SSL *);
    int SSL_CIPHER_get_bits(SSL_CIPHER * , int *);
    char *SSL_CIPHER_get_version(SSL_CIPHER *);
    const char *SSL_CIPHER_get_name(SSL_CIPHER *);
    char *SSL_CIPHER_description(SSL_CIPHER *, char *, int);
    SSL_METHOD *TLSv1_client_method();
    SSL_METHOD *SSLv2_client_method();
    SSL_METHOD *SSLv3_client_method();
    SSL_METHOD *SSLv23_client_method();
    BIO *BIO_new(BIO_METHOD *);
    BIO_METHOD *BIO_s_mem();
    long SSL_BIO_get_mem_data(BIO *, char **);
    int BIO_write(BIO *, const void *, int);
    int BIO_read(BIO *, void *, int);
    long BIO_ctrl(BIO *, int, long, void *);
    void OpenSSL_add_all_algorithms();
    void OpenSSL_add_all_ciphers();
    void OpenSSL_add_all_digests();
    BIO *BIO_new_fp(FILE *, int);
    int BIO_free(BIO *);

    QString verifyErrorString(long err);
#endif // NO_OPENSSL

  };
};

#endif
