/* cryptoki.h include file for PKCS #11.  2001 June 25 */



/* This is a sample file containing the top level include directives

 * for building Win32 Cryptoki libraries and applications.

 */



#ifndef ___CRYPTOKI_H_INC___

#define ___CRYPTOKI_H_INC___



#pragma pack(push, cryptoki, 1)



/* Specifies that the function is a DLL entry point. */

#define CK_IMPORT_SPEC __declspec(dllimport)



/* Define CRYPTOKI_EXPORTS during the build of cryptoki libraries. Do

 * not define it in applications.

 */

#ifdef CRYPTOKI_EXPORTS

/* Specified that the function is an exported DLL entry point. */

#define CK_EXPORT_SPEC __declspec(dllexport) 

#else

#define CK_EXPORT_SPEC CK_IMPORT_SPEC 

#endif



/* Ensures the calling convention for Win32 builds */

#define CK_CALL_SPEC __cdecl



#define CK_PTR *



#define CK_DEFINE_FUNCTION(returnType, name) \
  returnType CK_EXPORT_SPEC CK_CALL_SPEC name



#define CK_DECLARE_FUNCTION(returnType, name) \
  returnType CK_EXPORT_SPEC CK_CALL_SPEC name



#define CK_DECLARE_FUNCTION_POINTER(returnType, name) \
  returnType CK_IMPORT_SPEC (CK_CALL_SPEC CK_PTR name)



#define CK_CALLBACK_FUNCTION(returnType, name) \
  returnType (CK_CALL_SPEC CK_PTR name)



#ifndef NULL_PTR

#define NULL_PTR 0

#endif



#include "pkcs11.h"



#pragma pack(pop, cryptoki)



#endif /* ___CRYPTOKI_H_INC___ */

