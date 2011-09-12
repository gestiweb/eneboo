//==========< HISTORY >=============================
//      29.10.2003  Changes by AA
//                  ReadCertFromCard adapted
//                  from Tarmo's example
//      09.10.2003  Changes by AA
//      02.09.2003  Changes from Tarmo Milva
//Changed functions:
//DigiCrypt_GetDataFromCert
//DigiCrypt_GetFirstAllowedCSPName
//DigiCrypt_GetDefaultKeyContainerName
//DigiCrypt_SelectFromAllKeysCerts
//
#include "DigiCrypt.h"
#include "DigiDocDefs.h"
#include <stdio.h>
#include <tchar.h>

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

#define dSTRING_ITEM_LEN  255
#define dNAME_ITEM_LEN   1024
#define dCSPTYPE_NOTDEFINED -1

static char *psData_CSP_Path = "SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider\\";
static char *psData_Extra_CSP_Name = "Advanced Setec SetCSP";
static char *psData_Ignore_CSP_Name ="XXXEstEID";
static char *psData_Est_CSP_Name = "EstEID";

//Global data
static HCRYPTPROV oG_hProvider = 0;
static char  oG_sCSPName[dSTRING_ITEM_LEN+1];
static char  oG_sKeyContainerName[dNAME_ITEM_LEN+1];
static BOOL  oG_fDialogUserCancel;

extern int RunDialogUnit(char *psList[], int iWinWidth, int iWinHeight);
       PCCERT_CONTEXT DigiCrypt_ReadCertFromCard(void);
static BOOL DigiCrypt_AddCertToStore(PCCERT_CONTEXT pCert);

static BOOL OpenProvider(HCRYPTPROV *phProv, char *psProvider, DWORD dwFlags);
static HCERTSTORE DigiCrypt_OpenStore(void);
static char *DigiCrypt_GetFirstAllowedCSPNameNew(void);
static char *DigiCrypt_GetFirstAllowedCSPName(void);
static void  DigiCrypt_ReleaseFirstAllowedCSP(void);
static BOOL  DigiCrypt_GetCSPFromCert(PCCERT_CONTEXT  pCertContext, char *psResult, int iMaxResLen);
static BOOL  DigiCrypt_GetContainerFromCert(PCCERT_CONTEXT  pCertContext, char *psResult, int iMaxResLen);
static BOOL  DigiCrypt_CertIsSig(PCCERT_CONTEXT  pCertContext);

static char *DigiCrypt_GetDefaultKeyContainerName(char *psCSPName);
static char *DigiCrypt_GetDefaultKeyContainerNameSimple(char *psCSPName);
static char *DigiCrypt_GetDefaultCSPName(void);
static BOOL  DigiCrypt_GetDataFromCert(PCCERT_CONTEXT  pCertContext);
static PCCERT_CONTEXT  DigiCrypt_SelectFromAllCerts(void);
static PCCERT_CONTEXT  DigiCrypt_SelectFromAllKeysCerts(HCRYPTPROV hProvider);
static void            DigiCrypt_SelectCertsFromKeyContainer(HCRYPTPROV hProv, char *psContainerName);
static void            DigiCrypt_ChangeContainerName(char *psContainerName);

static void            RunDlg_Clear(void);
static BOOL            RunDlg_AddItem(PCCERT_CONTEXT  pCertContext, BOOL fTimeCheck);
static PCCERT_CONTEXT  RunDlg_RunDlg(void);
static BOOL            RunDlg_FillItem(PCCERT_CONTEXT  pCertContext, char *psRes, int len);

static void IntLogToFile(char *psMsg, int iMsgLen);
static BOOL IsLogEnabled(void);

PCCERT_CONTEXT  DigiCrypt_FindContext(BOOL fByKeyContainer, DWORD *dwResult)
{
  PCCERT_CONTEXT hCert = NULL;
  char *psCSPName;
  char *psDefaultKeyContainerName;
  oG_fDialogUserCancel = FALSE;

  *dwResult = dDigiCrypt_Okey;
  memset(oG_sCSPName, 0, sizeof(oG_sCSPName));
  memset(oG_sKeyContainerName, 0, sizeof(oG_sKeyContainerName));

  if (fByKeyContainer == TRUE)  {  
    hCert = DigiCrypt_ReadCertFromCard();
    //TEST
    //Test_ReadCertDataC(hCert);
    //ENDTEST
    if (hCert == NULL) {
      psCSPName = DigiCrypt_GetFirstAllowedCSPNameNew();
      if (psCSPName == NULL)
	*dwResult = dDigiCrypt_Error_NotFoundCSP;
      else {
	psDefaultKeyContainerName = DigiCrypt_GetDefaultKeyContainerName(oG_sCSPName);
	if (psDefaultKeyContainerName == NULL)
	  *dwResult = dDigiCrypt_Error_NoDefaultKey;
	else
	  hCert = DigiCrypt_SelectFromAllKeysCerts(oG_hProvider);
      }
    }
  } else {
    hCert = DigiCrypt_SelectFromAllCerts();
    if (hCert != NULL)
      DigiCrypt_GetDataFromCert(hCert);
  }
  if (hCert == NULL) {
    if (oG_fDialogUserCancel == TRUE)
      *dwResult = dDigiCrypt_Error_UserCancel;
    else {
      if (*dwResult == dDigiCrypt_Okey) 
	*dwResult = dDIgiCrypt_Error_NotFoundCert;
    }
  }
  return(hCert); 
}

PCCERT_CONTEXT DigiCrypt_ReadCertFromCard(void)
{
HCRYPTPROV hCryptProv;
BYTE       *pbData = NULL;   
HCRYPTKEY  hKey;          
DWORD cbData = 0;
DWORD dwKeyType=0;
DWORD dwErrCode=0;
DWORD cspType=0;
DWORD cspFlag=CRYPT_SILENT;
char *psCspName = NULL;
char *psKeyContainer;
BOOL fRes = FALSE;
PCCERT_CONTEXT  pCertContext = NULL; 
CRYPT_KEY_PROV_INFO KeyProvInfo;
LPWSTR wszContainerName=NULL;
LPWSTR wszProvName=NULL;
DWORD cchContainerName;
DWORD cchCSPName;
HCRYPTPROV hProv;


DigiCrypt_ReleaseFirstAllowedCSP();

psCspName=DigiCrypt_GetFirstAllowedCSPNameNew();

//very dummy thing.. i check from csp creators why i should do so...
if(!lstrcmp(psCspName,"EstEID Card CSP"))
   fRes = CryptAcquireContext(&hProv,"XXX",psCspName,2, CRYPT_SILENT);
// end dummy//

if (psCspName == NULL || strstr(psCspName,psData_Est_CSP_Name) == NULL)
  return(pCertContext);

cspType=DigiCrypt_FindContext_GetCSPType(psCspName);

psKeyContainer=DigiCrypt_GetDefaultKeyContainerName(psCspName);

fRes = CryptAcquireContext(&hCryptProv,psKeyContainer,psCspName,cspType, CRYPT_SILENT);
if (fRes == FALSE)
  return(pCertContext);

fRes=CryptGetUserKey(hCryptProv, AT_SIGNATURE, &hKey);
if (fRes == TRUE)
  {
  fRes=CryptGetKeyParam(hKey, KP_CERTIFICATE, NULL, &cbData, 0);
  if (fRes == TRUE)
    {
    pbData = malloc(cbData);
    if (pbData == NULL)
      fRes = FALSE;
    }
  if (fRes == TRUE)
    fRes=CryptGetKeyParam(hKey, KP_CERTIFICATE, pbData, &cbData, 0);
  if (fRes == TRUE)
    {
    pCertContext = CertCreateCertificateContext(MY_ENCODING_TYPE,pbData,cbData);
    if (pCertContext != NULL)
      { 
	  wszContainerName=NULL;
	  wszProvName=NULL;
	  cchContainerName = (lstrlen(psKeyContainer) + 1)  * sizeof(WCHAR);
	  cchCSPName = (lstrlen(psCspName) + 1) * sizeof(WCHAR);
      wszContainerName = (LPWSTR) malloc(cchContainerName);
      wszProvName = (LPWSTR) malloc(cchCSPName);
      mbstowcs(wszContainerName, psKeyContainer,cchContainerName);
	  mbstowcs(wszProvName, psCspName, cchCSPName);
      ZeroMemory((PVOID)&KeyProvInfo, sizeof(CRYPT_KEY_PROV_INFO));
      KeyProvInfo.pwszContainerName = (LPWSTR) wszContainerName;
      KeyProvInfo.pwszProvName      = (LPWSTR) wszProvName;
      KeyProvInfo.dwProvType        = PROV_RSA_SIG;
      KeyProvInfo.dwFlags           = 0;
      KeyProvInfo.dwKeySpec         = dwKeyType;
      fRes = CertSetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID, 0, (const void *) &KeyProvInfo);
      if (wszContainerName != NULL)
        free(wszContainerName);
      if (wszProvName != NULL)
        free(wszProvName);

      }
    }
  }

//if (pCertContext != NULL)
//  DigiCrypt_AddCertToStore(pCertContext);
if (fRes == FALSE && pCertContext != NULL)
  {
  CertFreeCertificateContext(pCertContext);
  pCertContext = NULL;
  }
if (pbData != NULL)
  free(pbData);
if (hCryptProv != 0)
  CryptReleaseContext(hCryptProv, 0);
return(pCertContext);
}


static BOOL DigiCrypt_AddCertToStore(PCCERT_CONTEXT pCert)
{
BOOL fRes = FALSE;
HCERTSTORE  hSystemStore = NULL;  // The system store handle.
if (pCert != NULL)
  {
  if (hSystemStore = CertOpenStore(
     CERT_STORE_PROV_SYSTEM_A, 
     0,                   // Encoding type not needed with this PROV.
     0,                   // Accept the default HCRYPTPROV. 
     CERT_STORE_NO_CRYPT_RELEASE_FLAG |
     CERT_SYSTEM_STORE_CURRENT_USER,"MY"))
    {
    if (CertAddCertificateContextToStore(hSystemStore, pCert, CERT_STORE_ADD_REPLACE_EXISTING,NULL))
      fRes = TRUE;
    }
  }
if (hSystemStore != NULL)
  CertCloseStore(hSystemStore, CERT_CLOSE_STORE_CHECK_FLAG);
return(fRes);
}


char *DigiCrypt_FindContext_GetKeyName()
{
 if (lstrlen(oG_sKeyContainerName) > 0)
   return(oG_sKeyContainerName);
 else
  return(NULL);
}

char *DigiCrypt_FindContext_GetCSPName()
{
 if (lstrlen(oG_sCSPName) > 0)
   return(oG_sCSPName);
 else
  return(NULL);
}

DWORD DigiCrypt_FindContext_GetCSPType(char *psCSPName)
{
  DWORD dwRes = PROV_RSA_SIG;
  DWORD dwType = REG_DWORD;
  DWORD dwLen  = dSTRING_ITEM_LEN;
  DWORD *pdwVal;
  HKEY hKey;
  LONG lRes;
  char sKeyNameBuf[dSTRING_ITEM_LEN+1];
  char sValue[dSTRING_ITEM_LEN+1];

  strncpy(sKeyNameBuf, psData_CSP_Path, sizeof(sKeyNameBuf));
  strncat(sKeyNameBuf, psCSPName, sizeof(sKeyNameBuf) - strlen(sKeyNameBuf));
  lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE,sKeyNameBuf, 0, KEY_READ, &hKey);
  if (lRes == ERROR_SUCCESS) {
    lRes = RegQueryValueEx(hKey, "Type", 0, &dwType, (LPBYTE)sValue, &dwLen);
    if (lRes == ERROR_SUCCESS) {
      pdwVal = (DWORD *)sValue;
      dwRes = (*pdwVal);
    }
    RegCloseKey(hKey);
  }
  return(dwRes);
}

static BOOL OpenProvider(HCRYPTPROV *phProv, char *psProvider, DWORD dwFlags)
{
  BOOL fRes;
  DWORD dwType = DigiCrypt_FindContext_GetCSPType(psProvider);
  fRes = CryptAcquireContext(phProv,NULL,psProvider, dwType, dwFlags);
  return(fRes);
}

static HCERTSTORE DigiCrypt_OpenStore(void)
{
HCERTSTORE hStore;
hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,0,(HCRYPTPROV)NULL,CERT_SYSTEM_STORE_CURRENT_USER,L"MY");
return(hStore);
}


static BOOL DigiCrypt_GetCSPFromCert(PCCERT_CONTEXT  pCertContext, char *psResult, int iMaxResLen)
{
BOOL  fRes = FALSE;
CRYPT_KEY_PROV_INFO *poKeyInfo;
DWORD pcbData;
DWORD dwLen;
if (pCertContext == NULL || psResult == 0)
  return(fRes);
fRes = CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID,NULL,&pcbData);
if (fRes == TRUE)
  {
  poKeyInfo = malloc(pcbData);
  if (poKeyInfo != NULL)
    {
    fRes = CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID,poKeyInfo,&pcbData);
	  if (fRes == TRUE)
	    {
      if (poKeyInfo->pwszProvName != NULL)
	      {
        dwLen = WideCharToMultiByte(CP_ACP,0,poKeyInfo->pwszProvName,-1,psResult,iMaxResLen,NULL,NULL);
		    }
	    else
		    fRes = FALSE;
	    }
    free(poKeyInfo);
    }
  }
return(fRes);
}

static BOOL DigiCrypt_GetContainerFromCert(PCCERT_CONTEXT  pCertContext, char *psResult, int iMaxResLen)
{
BOOL  fRes = FALSE;
CRYPT_KEY_PROV_INFO *poKeyInfo;
DWORD pcbData;
DWORD dwLen;
if (pCertContext == NULL || psResult == 0)
  return(fRes);
fRes = CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID,NULL,&pcbData);
if (fRes == TRUE)
  {
  poKeyInfo = malloc(pcbData);
  if (poKeyInfo != NULL)
    {
    fRes = CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID,poKeyInfo,&pcbData);
	  if (fRes == TRUE)
	    {
      if (poKeyInfo->pwszContainerName != NULL)
	      {
        
			  dwLen = WideCharToMultiByte(CP_ACP,0,poKeyInfo->pwszContainerName,-1,psResult,iMaxResLen,NULL,NULL);
		    }
	    else
		    fRes = FALSE;
	    }
    free(poKeyInfo);
    }
  }
return(fRes);
}

static BOOL DigiCrypt_CertIsSig(PCCERT_CONTEXT  pCertContext)
{
BOOL  fRes = FALSE;
CRYPT_KEY_PROV_INFO *poKeyInfo;
DWORD pcbData;
if (pCertContext == NULL)
  return(fRes);
fRes = CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID,NULL,&pcbData);
if (fRes == TRUE)
  {
  poKeyInfo = malloc(pcbData);
  if (poKeyInfo != NULL)
    {
    fRes = CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID,poKeyInfo,&pcbData);
	if (fRes == TRUE)
	  {
      if (poKeyInfo->dwProvType == PROV_RSA_SIG)
        fRes = TRUE;
	  }
    free(poKeyInfo);
    }
  }
return(fRes);
}


static BOOL DigiCrypt_GetDataFromCert(PCCERT_CONTEXT  pCertContext)
{
  BOOL  fRes = FALSE;
  char sContainer[dNAME_ITEM_LEN+1];
  //DWORD dwLen;
  if (pCertContext == NULL)
    return(fRes);
  fRes = DigiCrypt_GetCSPFromCert(pCertContext, oG_sCSPName,dSTRING_ITEM_LEN);

  if (fRes == TRUE) {
    //  DigiCrypt_GetDefaultKeyContainerNameSimple(oG_sCSPName);  //this is not usable. Tarmo changed that! Instead we should use the following:
    if (DigiCrypt_GetContainerFromCert(pCertContext, sContainer, dNAME_ITEM_LEN) == TRUE) {
      strncpy(oG_sKeyContainerName, sContainer, sizeof(oG_sKeyContainerName));   
    }
  }
  return(fRes);
}

static char *DigiCrypt_GetFirstAllowedCSPNameNew(void)
{
  char *psRes = NULL;
  HKEY  hKey = NULL;
  LONG  lRet=0;
  DWORD dwIndex = 0;
  BOOL  fRes;
  char sProvName[dSTRING_ITEM_LEN+1];
  char sKeyNameBuf[dSTRING_ITEM_LEN+1];
  HCRYPTPROV hProvide = 0;
  DWORD dwBufLen;
  FILETIME oTime;
  //char buff[200];
  BYTE pbData[dNAME_ITEM_LEN+1];
  DWORD cbData=dNAME_ITEM_LEN+1;
  //
  DWORD dwProvType;

  strncpy(sKeyNameBuf, psData_CSP_Path, sizeof(sKeyNameBuf));
  lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,sKeyNameBuf,0, KEY_READ, &hKey);

  while (lRet == ERROR_SUCCESS)  {
    dwBufLen = dSTRING_ITEM_LEN;
    lRet = RegEnumKeyEx(hKey,dwIndex,sProvName,&dwBufLen,NULL,NULL,0,&oTime);
    if (lRet == ERROR_SUCCESS) {
      if (lstrcmp(sProvName,psData_Ignore_CSP_Name) != 0) {
	dwProvType = DigiCrypt_FindContext_GetCSPType(sProvName);
	LOG("CSP %s",sProvName);
	//printf("%s :",sProvName);
	if ((lstrcmp(sProvName,psData_Extra_CSP_Name) != 0) && (lstrcmp(sProvName,"Belgium Identity Card CSP")!=0)) {
        //fRes = OpenProvider(&hProvide, sProvName, CRYPT_SILENT);
	  fRes = CryptAcquireContext(&hProvide,NULL,sProvName,dwProvType, CRYPT_SILENT);
	  fRes=CryptGetProvParam(hProvide, PP_ENUMCONTAINERS, pbData, &cbData,CRYPT_FIRST);
	  //	  printf("X\n");
	} else {
	  //fRes = OpenProvider(&hProvide, sProvName, CRYPT_VERIFYCONTEXT);
	  //fRes = CryptAcquireContext(&hProvide,"SetCARDKeyContainer",sProvName,dwProvType, CRYPT_SILENT);
	  fRes = CryptAcquireContext(&hProvide,NULL,sProvName,dwProvType, CRYPT_VERIFYCONTEXT);
	  if (fRes == TRUE) {
	    //the extra csp might give wrong answer. We should ask from provider, why.
	    //The following is the work-around -- try to lookup key container from the card.
	    //if the result is negative this is a not the csp what is needed.
     	    fRes=CryptGetProvParam(hProvide, PP_ENUMCONTAINERS, pbData, &cbData,CRYPT_FIRST);
	    if (fRes == TRUE)
	      fRes=CryptAcquireContext(&hProvide,(char*)pbData,sProvName,dwProvType, CRYPT_SILENT);
	  }
	}
	//printf("fRes: %x\n",GetLastError());
	if (fRes == TRUE) { // && dwProvType == 2) 
	  //  printf("OK %d %s\n",cbData, pbData);
	  //set global values
	  LOG("CSP %s accepted",sProvName);
	  //is it hardware token?
	  cbData=dNAME_ITEM_LEN+1;
	  if (CryptGetProvParam(hProvide, PP_IMPTYPE, pbData, &cbData, 0)) {
	    //printf("implementat: %d\n",pbData[0]);
	    if((pbData[0] & 1))  // hardware token
	      {
		strncpy(oG_sCSPName, sProvName, sizeof(oG_sCSPName));
		//CryptReleaseContext(hProvide, 0);
		psRes = oG_sCSPName;
		break;
	      }
	  }
	}
      }
    }
    //hProvide = 0;
    CryptReleaseContext(hProvide, 0);
    dwIndex++;
  }
  if (hKey != NULL)
    RegCloseKey(hKey);
  return(psRes);
}


static char *DigiCrypt_GetFirstAllowedCSPName(void)
{
  char *psRes = NULL;
  HKEY  hKey = NULL;
  LONG  lRet=0;
  DWORD dwIndex = 0;
  BOOL  fRes;
  char sProvName[dSTRING_ITEM_LEN+1];
  char sKeyNameBuf[dSTRING_ITEM_LEN+1];
  HCRYPTPROV hProvide = 0;
  DWORD dwBufLen;
  FILETIME oTime;
  //char buff[200];
  BYTE pbData[dNAME_ITEM_LEN+1];
  DWORD cbData=dNAME_ITEM_LEN+1;
  DWORD dwProvType;

  strncpy(sKeyNameBuf, psData_CSP_Path, sizeof(sKeyNameBuf));
  lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,sKeyNameBuf,0, KEY_READ, &hKey);
  while (lRet == ERROR_SUCCESS) {
    dwBufLen = dSTRING_ITEM_LEN;
    lRet = RegEnumKeyEx(hKey,dwIndex,sProvName,&dwBufLen,NULL,NULL,0,&oTime);
    if (lRet == ERROR_SUCCESS)    {
      if (lstrcmp(sProvName,psData_Ignore_CSP_Name) != 0) {
	dwProvType = DigiCrypt_FindContext_GetCSPType(sProvName);
	LOG("CSP %s",sProvName);
	if (lstrcmp(sProvName,psData_Extra_CSP_Name) != 0)
          fRes = OpenProvider(&hProvide, sProvName, CRYPT_SILENT);
	else {
	  fRes = OpenProvider(&hProvide, sProvName, CRYPT_VERIFYCONTEXT);
	  //fRes = CryptAcquireContext(&hProvide,"SetCARDKeyContainer",sProvName,dwProvType, CRYPT_SILENT);
	  fRes = CryptAcquireContext(&hProvide,NULL,sProvName,dwProvType, CRYPT_VERIFYCONTEXT);
	  if(fRes) {
	    //the extra csp might give wrong answer. We should ask from provider, why.
	    //The following is the work-around -- try to lookup key container from the card.
	    //if the result is negative this is a not the csp what is needed.
	    fRes=CryptGetProvParam(hProvide, PP_ENUMCONTAINERS, pbData, &cbData,CRYPT_FIRST);	   
	  }
	}
	if (fRes == TRUE) { // && dwProvType == 2)
	  //set global values
	  LOG("CSP %s accepted",sProvName);
	  strncpy(oG_sCSPName, sProvName, sizeof(oG_sCSPName));
	  CryptReleaseContext(hProvide, 0);
	  psRes = oG_sCSPName;
	  break;
        }
      }
    }
    //hProvide = 0;
    CryptReleaseContext(hProvide, 0);
    dwIndex++;
  }
  if (hKey != NULL)
    RegCloseKey(hKey);
  return(psRes);
}


static void DigiCrypt_ReleaseFirstAllowedCSP(void)
{
  //clear global values
  if (oG_hProvider != 0)
    CryptReleaseContext(oG_hProvider, 0);
  oG_hProvider = 0;
  oG_sCSPName[0] = 0;
}

static char *DigiCrypt_GetDefaultKeyContainerName(char *psCSPName)
{
  char *psRes = NULL;
  HCRYPTPROV hProvider;
  BOOL  fRes;
  DWORD dwFlags = CRYPT_VERIFYCONTEXT;
  BYTE pbData[dNAME_ITEM_LEN+1];
  DWORD cbData = dNAME_ITEM_LEN;
  DWORD dwError;
  DWORD dwProvType;

  ZeroMemory(pbData,cbData);
  ZeroMemory(oG_sKeyContainerName,1000);
  dwProvType = DigiCrypt_FindContext_GetCSPType(psCSPName);
  //LOG("GetDefaultKeyContainerName CSP=%s",psCSPName);
  if (lstrcmp(psCSPName,psData_Extra_CSP_Name) != 0)
    fRes = CryptAcquireContext(&hProvider,NULL,psCSPName,dwProvType, CRYPT_SILENT);
  else
    fRes = CryptAcquireContext(&hProvider,NULL,psCSPName,dwProvType, CRYPT_VERIFYCONTEXT);
  if (fRes == FALSE && dwFlags == CRYPT_SILENT) {
    //by description must be CRYPT_VERIFYCONTEXT
    fRes = CryptAcquireContext(&hProvider,NULL,psCSPName,dwProvType, CRYPT_VERIFYCONTEXT);
  }
  if (fRes == TRUE) {
    cbData = dNAME_ITEM_LEN;
    fRes = CryptGetProvParam(hProvider, PP_CONTAINER, pbData, &cbData, 0);
    /*if (fRes == FALSE)
      dwError = GetLastError();*/
  }
  //Some cards should not have default key container
  //let try to find key containers on the card.
  if (fRes == FALSE)  {
    fRes=CryptGetProvParam(hProvider, PP_ENUMCONTAINERS, pbData, &cbData,CRYPT_FIRST);
    if (fRes == FALSE)
      dwError = GetLastError();
  }
  if (fRes == TRUE) {
    //oG_hProvider = hProvider;
    strncpy(oG_sKeyContainerName, (char *) pbData, sizeof(oG_sKeyContainerName));
    //psRes = oG_sKeyContainerName;
    DigiCrypt_ChangeContainerName(oG_sKeyContainerName);
  } else {
      
  }
  if (psRes != NULL)
    LOG("GetDefaultKeyContainerName CSP=%s",psCSPName);
  else
    LOG("GetDefaultKeyContainerName Not found");
  if (hProvider != 0)
    CryptReleaseContext(hProvider, 0);
  return(oG_sKeyContainerName);
}

static char *DigiCrypt_GetDefaultKeyContainerNameSimple(char *psCSPName)
{
  char *psRes = NULL;
  HCRYPTPROV hProvider=0;
  BOOL  fRes;
  DWORD dwFlags = 0;
  BYTE pbData[dNAME_ITEM_LEN+1];
  DWORD cbData = dNAME_ITEM_LEN;
  DWORD dwError;

  fRes = OpenProvider(&hProvider, psCSPName, dwFlags);
  //fRes = CryptAcquireContext(&hProvider,NULL,psCSPName,PROV_RSA_SIG, dwFlags);
  if (fRes == TRUE) {
    fRes = CryptGetProvParam(hProvider, PP_CONTAINER, pbData, &cbData, 0);
    if (fRes == FALSE)
      dwError = GetLastError();
  }
  if (fRes == TRUE) {
    strncpy(oG_sKeyContainerName, pbData, sizeof(oG_sKeyContainerName));
    psRes = oG_sKeyContainerName;
    DigiCrypt_ChangeContainerName(oG_sKeyContainerName);
  }
  if (hProvider != 0)
    CryptReleaseContext(hProvider, 0);
  return(psRes);
}


static PCCERT_CONTEXT  DigiCrypt_SelectFromAllCerts(void)
{
PCCERT_CONTEXT  pCertContext = NULL;
HCERTSTORE hStore;
hStore = DigiCrypt_OpenStore();
if (hStore != NULL)
  {
  while (TRUE)
    {
    pCertContext = CertEnumCertificatesInStore(hStore,pCertContext);
    if (pCertContext == NULL)
      break;
    else
      {
      //TEST
      //Test_ReadCertDataC(pCertContext);
      //END TEST
      RunDlg_AddItem(pCertContext,TRUE);
      }
    }
  pCertContext = RunDlg_RunDlg();
  }
return(pCertContext);
}

static void  DigiCrypt_SelectCertsFromKeyContainer(HCRYPTPROV hProv, char *psContainerName)
{
PCCERT_CONTEXT  pCertContext = NULL;
HCERTSTORE hStore;
BOOL fRelease = FALSE;
char sContainer[dNAME_ITEM_LEN+1];

if (memcmp(psContainerName,"AUT",3) == 0)
  {
  LOG("Find1 Ignore AUT cert");
  return;
  }
hStore = DigiCrypt_OpenStore();
if (hStore != NULL)
  {
  while (TRUE)
    {
    pCertContext = CertEnumCertificatesInStore(hStore,pCertContext);
    if (pCertContext == NULL)
      break;
    else
      {
      if (DigiCrypt_GetContainerFromCert(pCertContext, sContainer, dNAME_ITEM_LEN) == TRUE)
        {

        LOG("Find1 Container %s %s",sContainer,psContainerName);
		if (lstrcmp(sContainer+3,psContainerName+3) == 0)
		  {
          LOG("Find1 Container %s accepted",sContainer);
          RunDlg_AddItem(pCertContext,TRUE);
		  }
        }
      }
    }
  }
else
  LOG("Find1 Can't open store");
if (fRelease == TRUE)
  CryptReleaseContext(hProv, 0);
}

static PCCERT_CONTEXT  DigiCrypt_SelectFromAllKeysCerts(HCRYPTPROV hProvider)
{
PCCERT_CONTEXT  pCertContext = NULL;
HCRYPTPROV hProv;
BYTE pbData[dNAME_ITEM_LEN+1];
DWORD cbData = dNAME_ITEM_LEN;
DWORD dwFlag;
BOOL fRes;
BOOL fRelease = FALSE;
CRYPT_KEY_PROV_INFO* poKeyInfo = NULL;
char sContainer[dNAME_ITEM_LEN+1];

hProv = hProvider;


if (hProv == 0)
  {
  fRes = OpenProvider(&hProv, oG_sCSPName, CRYPT_VERIFYCONTEXT);
  //fRes = CryptAcquireContext(&hProv,NULL,oG_sCSPName,PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
  if (fRes == FALSE)
    {
		
    LOG("Find1 - Can't open provider");
    return(pCertContext);
	}
  fRelease = TRUE;
  }
  dwFlag = CRYPT_FIRST;
  fRes = TRUE;
  while (fRes == TRUE)
    {
    cbData = dNAME_ITEM_LEN;
    cbData = 0;
    fRes = CryptGetProvParam(hProv, PP_ENUMCONTAINERS, NULL, &cbData, dwFlag);
    if (fRes == TRUE)
      fRes = CryptGetProvParam(hProv, PP_ENUMCONTAINERS, pbData, &cbData, dwFlag);
    dwFlag = 0;

    if (fRes == FALSE)
      {
      if (GetLastError() == ERROR_NO_MORE_ITEMS)
        {
        LOG("Find1 End");
        fRes = TRUE;
        break;
        }
      }
	  else
	    {
        LOG("Find1 select certs from %s",pbData);
        DigiCrypt_SelectCertsFromKeyContainer(hProv, pbData); 
	    }
    }
  pCertContext = RunDlg_RunDlg();
  //we have selected the cert, but do we know corresponding key?
  //let change values of globals when these are different
  if(DigiCrypt_GetContainerFromCert(pCertContext, sContainer, dNAME_ITEM_LEN) == TRUE) {
    strncpy(oG_sKeyContainerName, sContainer, sizeof(oG_sKeyContainerName));
  }
  if(fRelease == TRUE)
    CryptReleaseContext(hProv, 0);
  return(pCertContext);
}


static BOOL DigiCrypt_IsValidCert(PCCERT_CONTEXT  pCertContext, BOOL fTimeCheck)
{
BOOL  fIsValid = FALSE;
BOOL  fRes = FALSE;
BYTE  bKeyUsageBits = CERT_NON_REPUDIATION_KEY_USAGE;
DWORD dwKeyUsageBytes = 1;
//Old version
//FILETIME oCurrentTime;
if (pCertContext != NULL && pCertContext->pCertInfo != NULL)
  {
  //not needed (info from Tarmo Milva)
  //if (DigiCrypt_CertIsSig(pCertContext) == TRUE)
    fRes = CertGetIntendedKeyUsage(X509_ASN_ENCODING,pCertContext->pCertInfo,&bKeyUsageBits,dwKeyUsageBytes);
  //else
  //  fRes = FALSE;
  if (fRes == TRUE)
    {
    if(bKeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
	  fIsValid = TRUE;
	if(bKeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) // don't display CA certs
	  fIsValid = FALSE;
	}
  if (fIsValid == TRUE && fTimeCheck == TRUE)
    {
    //Old version
    //GetSystemTimeAsFileTime(&oCurrentTime);
	  //if (CompareFileTime(&oCurrentTime, &pCertContext->pCertInfo->NotBefore) < 0 ||
		//    CompareFileTime(&oCurrentTime, &pCertContext->pCertInfo->NotAfter) > 0 )
    //  fIsValid = FALSE;
    //New version
    //NULL, if current datetime
    if (CertVerifyTimeValidity(NULL,pCertContext->pCertInfo) != 0)
      fIsValid = FALSE;
    }
  }
return(fIsValid);
}

static void DigiCrypt_ChangeContainerName(char *psContainerName)
{
if (memcmp(psContainerName,"AUT",3) == 0)
  memmove(psContainerName,"SIG",3);
}

//dialog functions
#define DLGMAXIEMS 512
static char *psListItems[DLGMAXIEMS];
static PCCERT_CONTEXT psListItemsCert[DLGMAXIEMS];
static int iListItems=0;

static void  RunDlg_Clear(void)
{
int iI;
for (iI=0; iI < DLGMAXIEMS;++iI)
  {
  psListItems[iI] = NULL;
  psListItemsCert[iI] = NULL;
  }
iListItems=0;
}

static BOOL  RunDlg_AddItem(PCCERT_CONTEXT  pCertContext, BOOL fTimeCheck)
{
BOOL fRes = TRUE;
BOOL fAddToList = TRUE;
char *psData;
int  iI, len;
if (iListItems == (DLGMAXIEMS-1))
  return(FALSE);
if (iListItems == 0)
  {
  for (iI=0; iI < DLGMAXIEMS;++iI)
    {
    psListItems[iI] = NULL;
    psListItemsCert[iI] = NULL;
    }
  }
if (pCertContext != NULL)
  {
  fAddToList = DigiCrypt_IsValidCert(pCertContext,fTimeCheck);
  if (fAddToList == TRUE)
    {
	  len = dSTRING_ITEM_LEN*4;
    psListItems[iListItems] = (char *)LocalAlloc(LMEM_ZEROINIT,(len));
    psListItemsCert[iListItems] = CertDuplicateCertificateContext(pCertContext);
    //Make data buffer for list
    psData = psListItems[iListItems];
    RunDlg_FillItem(pCertContext, psData, len);
    ++iListItems;
	}
  }
return(fRes);
}


static PCCERT_CONTEXT  RunDlg_RunDlg(void)
{
PCCERT_CONTEXT hCert = NULL;
int iRes = -1;
int iI;
oG_fDialogUserCancel = FALSE;
if (iListItems > 1)
  {
  LOG("StartDialog");
  iRes = RunDialogUnit(psListItems, 450,300);
  if (iRes < 0)
    oG_fDialogUserCancel = TRUE;
  LOG("EndDialog");
  }
else
  {
  if (iListItems == 1)
    iRes = 0;
  }
for (iI=0; iI < iListItems;++iI)
  {
  LocalFree(psListItems[iI]);
  if (iRes != iI)
    CertFreeCertificateContext(psListItemsCert[iI]);
  else
    hCert = psListItemsCert[iI];
  }
iListItems = 0;
return(hCert);
}

static BOOL  RunDlg_FillItem(PCCERT_CONTEXT  pCertContext, char *psRes, int len)
{
  char sTemp[dSTRING_ITEM_LEN+1];
  SYSTEMTIME oT;
  PCERT_INFO pCertInfo;
  BOOL fRes = TRUE;
  *psRes = 0;

  if (pCertContext == NULL || pCertContext->pCertInfo == NULL || psRes == NULL)
    return(FALSE);
  pCertInfo = pCertContext->pCertInfo;
  CertGetNameString(pCertContext,CERT_NAME_SIMPLE_DISPLAY_TYPE,CERT_NAME_ISSUER_FLAG,NULL,sTemp,dSTRING_ITEM_LEN);
  if (lstrlen(sTemp) > 0) {
    strncat(psRes, sTemp, len - strlen(psRes));
    strncat(psRes, "   \t", len - strlen(psRes));
  }
  CertGetNameString(pCertContext,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,NULL,sTemp,dSTRING_ITEM_LEN);
  //CorrectCharacters(sTemp);
  if (lstrlen(sTemp) > 0) {
    strncat(psRes, sTemp, len - strlen(psRes));
    strncat(psRes, "   \t", len - strlen(psRes));
  }
  if (FileTimeToSystemTime(&pCertInfo->NotBefore,&oT) == TRUE) {
    snprintf(sTemp, sizeof(sTemp), "%02d/%02d/%02d   \t",oT.wYear,oT.wMonth,oT.wDay);
    strncat(psRes, sTemp, len - strlen(psRes));
  }
  else
    fRes = FALSE;
  if (FileTimeToSystemTime(&pCertInfo->NotAfter,&oT) == TRUE) {
    snprintf(sTemp, sizeof(sTemp), "02d/%02d/%02d    \t",oT.wYear,oT.wMonth,oT.wDay);
    strncat(psRes,sTemp, len - strlen(psRes));
  }
  else
    fRes = FALSE;
  if (DigiCrypt_GetCSPFromCert(pCertContext, sTemp, dSTRING_ITEM_LEN) == TRUE)
    strncat(psRes,sTemp, len - strlen(psRes));
  return(fRes);
}

static char sLogLine[4096];
static char sLogFile[256];
static int  iLogCheck = -1;

static BOOL IsLogEnabled(void)
{
  BOOL  fRes = FALSE;
  DWORD dwRes;
  char  cVal;
  HANDLE hSearch;
  WIN32_FIND_DATA oF;
  
  if(iLogCheck == -1) {
    iLogCheck = 0;
    dwRes = GetModuleFileName(NULL, sLogFile, 255);
    //MessageBox(NULL,sLogFile,"LogFile=",MB_OK|MB_SYSTEMMODAL|MB_ICONERROR);
    while (dwRes > 1) {
      cVal = sLogFile[dwRes-1];
      if (cVal == '.') {
	strncat(sLogFile, "log", sizeof(sLogFile) - strlen(sLogFile));
	hSearch = FindFirstFile(sLogFile,&oF);
	if (hSearch != INVALID_HANDLE_VALUE) {
	  iLogCheck = 1;
	  FindClose(hSearch);
        }
	break;
      }
      --dwRes;
    }
  }
  fRes = (BOOL)iLogCheck;
  return(fRes);
}

void LOG(char *psMsgFmt, ...)
{
  int iLen;
  char buff[200];
  va_list ArgList;
  va_start(ArgList, psMsgFmt);
  iLen = wvsprintf((LPTSTR)sLogLine, (LPTSTR)psMsgFmt, ArgList);
  snprintf(buff, sizeof(buff), (LPTSTR)psMsgFmt,ArgList);
  //MessageBox(NULL,(LPTSTR)sLogLine,"Teade",0);
  va_end(ArgList);
  if (IsLogEnabled() == TRUE)
    IntLogToFile(sLogLine,iLen);
}

static void IntLogToFile(char *psMsg, int iMsgLen)
{
char sLogFormat1[]="%02d%02d%02d %02d:%02d.%02d ";
char sTimeBuf[64];
HANDLE hFile;
DWORD  dwLen;
SYSTEMTIME oTime;
if (psMsg == NULL || iMsgLen == 0)
  return;
GetLocalTime(&oTime);
wsprintf((LPTSTR)sTimeBuf,(LPTSTR)sLogFormat1, oTime.wYear-2000, oTime.wMonth, oTime.wDay,
                              oTime.wHour, oTime.wMinute, oTime.wSecond);
hFile = CreateFile((LPCTSTR)sLogFile,
                   GENERIC_READ|GENERIC_WRITE,FILE_SHARE_WRITE, NULL,
                   OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

if (hFile != INVALID_HANDLE_VALUE)
  {
  if (psMsg != NULL)
    {
    SetFilePointer(hFile,0,NULL,FILE_END);
    WriteFile(hFile,sTimeBuf, strlen(sTimeBuf), &dwLen, NULL);
    if (iMsgLen < 0)
      iMsgLen = strlen(psMsg);
    if (iMsgLen > 0)
      WriteFile(hFile,psMsg, iMsgLen, &dwLen, NULL);
    WriteFile(hFile,"\r\n", 2, &dwLen, NULL);
    }
  CloseHandle(hFile);
  }
}



