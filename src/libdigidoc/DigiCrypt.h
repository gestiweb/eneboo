#ifndef DigiCryptH
#define DigiCryptH


#ifdef  __cplusplus
extern "C" {
#endif


#include <windows.h>
#include <wincrypt.h>

#define dDigiCrypt_Okey                0
#define dDigiCrypt_Error_NotFoundCSP   1
#define dDigiCrypt_Error_UserCancel    2
#define dDigiCrypt_Error_NoDefaultKey  3
#define dDIgiCrypt_Error_NotFoundCert  4

PCCERT_CONTEXT  DigiCrypt_FindContext(BOOL fByKeyContainer, DWORD *dwResult);
char *DigiCrypt_FindContext_GetCSPName(void);
char *DigiCrypt_FindContext_GetKeyName(void);
DWORD DigiCrypt_FindContext_GetCSPType(char *psCSPName);
void  LOG(char *psMsgFmt, ...);

#ifdef  __cplusplus
}
#endif
#endif