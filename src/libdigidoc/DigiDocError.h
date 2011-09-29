#ifndef __DIGI_DOC_ERROR_H__
#define __DIGI_DOC_ERROR_H__
//==================================================
// FILE:	DigiDocError.h
// PROJECT: Digi Doc
// DESCRIPTION: Digi Doc error codes and functions 
// AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
//==================================================
// Copyright (C) AS Sertifitseerimiskeskus
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.ode
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//==================================================

#ifdef WIN32
	#include <windows.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif
	
//==========< error codes >=======================

#define ERR_OK						0
#define ERR_UNSUPPORTED_DIGEST		1
#define ERR_FILE_READ				2
#define ERR_FILE_WRITE				3
#define ERR_DIGEST_LEN				4
#define ERR_BUF_LEN					5
#define ERR_SIGNATURE_LEN			6
#define ERR_PRIVKEY_READ			7
#define ERR_PUBKEY_READ				8
#define ERR_CERT_READ				9
#define ERR_SIGNEDINFO_CREATE		10
#define ERR_SIGNEDINFO_DATA			11
#define ERR_SIGNEDINFO_FINAL		12
#define ERR_UNSUPPORTED_FORMAT		13
#define ERR_BAD_INDEX				14
#define ERR_TIMESTAMP_DECODE		15
#define ERR_DIGIDOC_PARSE			16
#define ERR_UNSUPPORTED_SIGNATURE	17
#define ERR_CERT_STORE_READ			18
#define ERR_SIGPROP_DIGEST			19
#define ERR_COMPARE					20
#define ERR_DOC_DIGEST				21
#define ERR_MIME_DIGEST				22
#define ERR_SIGNATURE				23
#define ERR_CERT_INVALID			24
#define ERR_OCSP_UNSUCCESSFUL		25
#define ERR_OCSP_UNKNOWN_TYPE		26
#define ERR_OCSP_NO_BASIC_RESP		27
#define ERR_OCSP_WRONG_VERSION		28
#define ERR_OCSP_WRONG_RESPID		29
#define ERR_OCSP_ONE_RESPONSE		30
#define ERR_OCSP_RESP_STATUS		31
#define ERR_OCSP_NO_SINGLE_EXT		32
#define ERR_OCSP_NO_NONCE			33
#define ERR_NOTARY_NO_SIGNATURE		34
#define ERR_NOTARY_SIG_MATCH		35
#define ERR_SIGNERS_CERT_NOT_TRUSTED   36

#define ERR_WRONG_CERT				37
#define ERR_NULL_POINTER            38
#define ERR_NULL_CERT_POINTER       39
#define ERR_NULL_SER_NUM_POINTER    40
#define ERR_NULL_KEY_POINTER        41
#define ERR_EMPTY_STRING            42
#define ERR_BAD_DATAFILE_INDEX      43
#define ERR_BAD_DATAFILE_COUNT      44
#define ERR_BAD_ATTR_COUNT          45
#define ERR_BAD_ATTR_INDEX          46
#define ERR_BAD_SIG_INDEX           47
#define ERR_BAD_SIG_COUNT           48
#define ERR_BAD_ROLE_INDEX          49
#define ERR_BAD_DOCINFO_COUNT       50
#define ERR_BAD_DOCINFO_INDEX       51
#define	ERR_BAD_NOTARY_INDEX        52
#define	ERR_BAD_NOTARY_ID			53
#define ERR_BAD_NOTARY_COUNT		54
#define ERR_X509_DIGEST             55
#define ERR_CERT_LENGTH             56
#define ERR_PKCS_LIB_LOAD           57
#define ERR_PKCS_SLOT_LIST          58
#define ERR_PKCS_WRONG_SLOT         59
#define ERR_PKCS_LOGIN              60
#define ERR_PKCS_PK                 61
#define ERR_PKCS_CERT_LOC           62
#define ERR_PKCS_CERT_DECODE        63
#define ERR_PKCS_SIGN_DATA          64
#define ERR_PKCS_CARD_READ          65
#define ERR_CSP_NO_CARD_DATA        66
#define ERR_CSP_OPEN_STORE			67
#define ERR_CSP_CERT_FOUND			68
#define ERR_CSP_SIGN				69
#define ERR_CSP_NO_HASH_START	70
#define ERR_CSP_NO_HASH			71
#define ERR_CSP_NO_HASH_RESULT		72
#define ERR_CSP_OPEN_KEY		73
#define ERR_CSP_READ_KEY		74
#define ERR_OCSP_SIGN_NOT_SUPPORTED		75
#define ERR_OCSP_SIGN_CSP_NAME			76
#define ERR_CSP_CERT_DECODE				77
#define ERR_OCSP_SIGN_PKCS_NAME			78
#define ERR_OCSP_SIGN_OSLL_CERT			79
#define ERR_OCSP_SIGN					80
#define ERR_CERT_ISSUER					81
#define ERR_OCSP_PKCS12_CONTAINER		82
#define ERR_MODIFY_SIGNED_DOC       83
#define ERR_NOTARY_EXISTS			84
#define ERR_UNSUPPORTED_CERT_SEARCH	85
#define ERR_INCORRECT_CERT_SEARCH	86
#define ERR_BAD_OCSP_RESPONSE_DIGEST 87
#define ERR_LAST_ESTID_CACHED		88
#define ERR_BAD_DATAFILE_XML		89
#define ERR_UNSUPPORTED_VERSION		90
#define ERR_UNSUPPORTED_CHARSET		91
#define ERR_PKCS12_EXPIRED			92
#define ERR_CSP_USER_CANCEL         93
#define ERR_CSP_NODEFKEY_CONTAINER  94
#define ERR_CONNECTION_FAILURE      95
#define ERR_WRONG_URL_OR_PROXY      96
#define ERR_NULL_PARAM				97
#define ERR_BAD_ALLOC				98
#define ERR_CONF_FILE				99
#define ERR_CONF_LINE			   100
#define ERR_OCSP_CERT_REVOKED              101
#define ERR_OCSP_CERT_UNKNOWN              102
#define ERR_OCSP_PKCS12_NO_FILE            103
#define ERR_OCSP_PKCS12_NO_PASSWD          104
#define ERR_BAD_DATAFILE_CONTENT_TYPE      105
#define ERR_OCSP_WRONG_URL					106
#define ERR_OCSP_MALFORMED			107
#define ERR_OCSP_INTERNALERR		108
#define ERR_OCSP_TRYLATER			109
#define ERR_OCSP_SIGREQUIRED		110
#define ERR_OCSP_UNAUTHORIZED		111
#define ERR_UNKNOWN_CA                  112
// DigiDocEnc errors
#define ERR_DENC_ENC_METHOD             113
#define ERR_DENC_ENC_XMLNS              114
#define ERR_DENC_BAD_PROP_IDX           115
#define ERR_DENC_BAD_KEY_IDX           116
#define ERR_DENC_KEY_STATUS            117
#define ERR_DENC_DATA_STATUS            118
#define ERR_DENC_DECRYPT               119
#define ERR_CHARSET_CONVERT            120
#define ERR_COMPRESS                121
#define ERR_DECOMPRESS              122
#define ERR_OCSP_CERT_NOTFOUND      123
#define ERR_INVALID_CONTENT         124
#define ERR_DENC_NO_KEY_FOUND       125
#define ERR_OCSP_RESP_NOT_TRUSTED   126
#define ERR_PRIV_CERT_NOT_FOUND     127
#define ERR_NO_OCSP                 128
#define ERR_OCSP_WRONG_SIGNATURE    129
#define ERR_BAD_PARAM				130
#define ERR_GENERIC_SOAP_ERR		131

#define ERR_TS_TIMESTAMPINFO_TYPE      132
#define ERR_TS_BAD_INCLUDEINFO_IDX    133
#define ERR_TS_BAD_TIMESTAMPINFO_IDX    134
#define ERR_TS_CREATE_TS_REQ            135
#define ERR_CREATE_NONCE                136
#define ERR_TXT2OID                     137
#define ERR_HTTP_ERR                    138

#define ERR_BAD_CERTID_IDX              139
#define ERR_BAD_CERTVALUE_IDX           140
#define ERR_TS_VERIFY                   141
#define ERR_TS_REQUEST                  142
#define ERR_TSA_NOT_TRUSTED		143
#define ERR_ORPHONED_SIGNATURE		144

#define ERR_WPKI_UNKNOWN_USER		145
#define ERR_WPKI_INVALID_PHONE_NO	146
#define ERR_WPKI_UNTRUSTED_SRVICE	147
#define ERR_WPKI_UNTRUSTED_USER		148

#define ERR_WPKI_UNUSABLE_PHONE    149
#define ERR_WPKI_TIMEOUT		   150
#define ERR_WPKI_CANCELLED		   151
#define ERR_WPKI_MID_NOT_READY	   152
#define ERR_WPKI_PHONE_NOT_REACHABLE  153
#define ERR_WPKI_SENDING_ERROR	   154
#define ERR_WPKI_SIM_ERROR		   155
#define ERR_WPKI_SERVICE_ERR	   156

#define ERR_MAX                    158  //number of error codes. Increment, if you add a new error code


#define ERROR_BUF_LENGTH            20


typedef struct ErrorMessage_st {
		char	*errorMessage;
		int 	errorClass;
} ErrorMessage;

typedef int ErrorClass;

// Error classes 

// Wasn't an error at all, no reaction neccessary
#define NO_ERRORS	0	
// Various problems of technical nature
#define TECHNICAL	1
// User-repairable errors
#define USER		2
// Bug in DigiDoc library (?)
#define LIBRARY		3

//==========< error info structure >==============

typedef struct ErrorInfo_st {
        int code;
        char *fileName;
        int line;
        char *assertion;
} ErrorInfo;

#ifdef WIN32
extern 	CRITICAL_SECTION cs_ddocErrors;
#endif

// checks and prints errors
EXP_OPTION long checkErrors();

//returns textual explanation of the error code
EXP_OPTION char* getErrorString(int code);

//returns the classification for the error code 
EXP_OPTION ErrorClass getErrorClass(int code);

//returns the code of the error that occurred in the library
EXP_OPTION ErrorInfo* getErrorInfo(void);

//returns 1, if all errors are read and 0 otherwise
EXP_OPTION int hasUnreadErrors(void);

//resets error information
EXP_OPTION void clearErrors(void);

EXP_OPTION void addError(int code, char *fileName, int line, char *assertion);

EXP_OPTION int checkDigiDocErrors(void);

EXP_OPTION int getLastError();

//==========< macros >====================

#define SET_LAST_ERROR(code)                (addError((code), __FILE__, __LINE__, ""))
#define SET_LAST_ERROR_IF_NOT(expr, code)   { if(!(expr)) addError((code), __FILE__, __LINE__, #expr); }
#define SET_LAST_ERROR_RETURN(code, retVal) { SET_LAST_ERROR(code); return (retVal); }
#define SET_LAST_ERROR_RETURN_IF_NOT(expr, code, retVal) { if(!(expr)) { addError((code), __FILE__, __LINE__, #expr); return (retVal); } }
#define SET_LAST_ERROR_RETURN_VOID_IF_NOT(expr, code) { if(!(expr)) { addError((code), __FILE__, __LINE__, #expr); return; } }
#define SET_LAST_ERROR_RETURN_VOID_IF(expr, code) { if(expr) { addError((code), __FILE__, __LINE__, #expr); return; } }
#define RETURN_IF_NOT(expr, code) SET_LAST_ERROR_RETURN_IF_NOT((expr), (code), (code));
#define RETURN_IF_NULL(p)                   RETURN_IF_NOT((p), ERR_NULL_POINTER);
#define RETURN_VOID_IF_NULL(p)              SET_LAST_ERROR_RETURN_VOID_IF_NOT((p), ERR_NULL_POINTER);
#define RETURN_OBJ_IF_NULL(p, obj)          SET_LAST_ERROR_RETURN_IF_NOT((p), ERR_NULL_POINTER, (obj));
#define SET_LAST_ERROR_RETURN_CODE(code)    { SET_LAST_ERROR(code); return (code); }
#define RETURN_IF_NULL_PARAM(p)				RETURN_IF_NOT((p), ERR_NULL_PARAM);
#define RETURN_IF_BAD_ALLOC(p)				RETURN_IF_NOT((p), ERR_BAD_ALLOC)
#define RETURN_VOID_IF_BAD_ALLOC(p)	    SET_LAST_ERROR_RETURN_VOID_IF_NOT((p), ERR_BAD_ALLOC);

//========================================

#ifdef  __cplusplus
}
#endif

#endif 

