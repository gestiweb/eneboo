#ifdef WIN32
	#include <windows.h>
#else
	#ifdef USEPTHREADS
    	#include <pthread.h>
	#endif
	#include <sys/types.h>
    #include <unistd.h>
#endif
#include <string.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocDebug.h>


ErrorMessage g_ddocErrorStrings[ERR_MAX] = { 
/* ERR_OK */	                  {"No errors", NO_ERRORS},
/* ERR_UNSUPPORTED_DIGEST */	  {"Digest type is not supported", LIBRARY},
/* ERR_FILE_READ */               {"Could not open file for reading", USER},
/* ERR_FILE_WRITE */              {"Could not open file for writing", USER},
/* ERR_DIGEST_LEN */              {"Wrong digest length", LIBRARY},
/* ERR_BUF_LEN	*/                {"Insufficient target buffer length", LIBRARY},
/* ERR_SIGNATURE_LEN */           {"Wrong signature length", LIBRARY},
/* ERR_PRIVKEY_READ */            {"Failed to read the private key", TECHNICAL},
/* ERR_PUBKEY_READ */             {"Failed to read the public key", TECHNICAL},
/* ERR_CERT_READ */               {"Failed to read certificate", USER},
/* ERR_SIGNEDINFO_CREATE */       {"Could not create SignedInfo object", LIBRARY},
/* ERR_SIGNEDINFO_DATA	 */       {"Could not create SignedInfo object", LIBRARY},
/* ERR_SIGNEDINFO_FINAL	 */       {"Could not create SignedInfo object", LIBRARY},
/* ERR_UNSUPPORTED_FORMAT */      {"Wrong signed document format", TECHNICAL},
/* ERR_BAD_INDEX */               {"Bad index", LIBRARY},
/* ERR_TIMESTAMP_DECODE */        {"Failed to decode timestamp", TECHNICAL},
/* ERR_DIGIDOC_PARSE */           {"Error in document parsing", USER}, 
/* ERR_UNSUPPORTED_SIGNATURE */   {"Signature type is not supported", LIBRARY},
/* ERR_CERT_STORE_READ	 */       {"Could not read read certificate from certificate store", TECHNICAL},
/* ERR_SIGPROP_DIGEST */          {"Wrong signature properties digest", TECHNICAL},
/* ERR_COMPARE */                 {"Wrong signature", USER},
/* ERR_DOC_DIGEST */              {"Wrong document digest", USER},
/* ERR_MIME_DIGEST */             {"Wrong document mime digest", TECHNICAL},
/* ERR_SIGNATURE */               {"Wrong signature", TECHNICAL},
/* ERR_CERT_INVALID	 */           {"Invalid certificate", TECHNICAL},
/* ERR_OCSP_UNSUCCESSFUL */       {"OCSP request unsuccessful", TECHNICAL},
/* ERR_OCSP_UNKNOWN_TYPE */       {"Unknown OCSP type", TECHNICAL},
/* ERR_OCSP_NO_BASIC_RESP */      {"No OCSP basic responce", TECHNICAL},
/* ERR_OCSP_WRONG_VERSION */      {"Wrong OCSP version", TECHNICAL},
/* ERR_OCSP_WRONG_RESPID */       {"Wrong OCSP response id", TECHNICAL},
/* ERR_OCSP_ONE_RESPONSE */       {"Incorrect OCSP response count", TECHNICAL},
/* ERR_OCSP_RESP_STATUS	 */       {"Incorrect OCSP response status", USER},
/* ERR_OCSP_NO_SINGLE_EXT */      {"No single OCSP responce", TECHNICAL},
/* ERR_OCSP_NO_NONCE */           {"NONCE is missing", LIBRARY},
/* ERR_NOTARY_NO_SIGNATURE */     {"Missing signature for Notary", TECHNICAL},
/* ERR_NOTARY_SIG_MATCH */        {"Notary signature mismatch", USER},
/* ERR_SIGNERS_CERT_NOT_TRUSTED */ {"Signers cert not trusted, missing CA cert!", USER},
/* ERR_WRONG_CERT	*/  		  {"Wrong certificate", USER},
/* ERR_NULL_POINTER	*/ 		      {"Null pointer exception", LIBRARY},
/* ERR_NULL_CERT_POINTER   */     {"Certificate pointer is null", LIBRARY},
/* ERR_NULL_SER_NUM_POINTER  */   {"Certificate Number pointer is null", LIBRARY},
/* ERR_NULL_KEY_POINTER    */     {"Key pointer is null", LIBRARY}, 
/* ERR_EMPTY_STRING */            {"Empty string", LIBRARY},
/* ERR_BAD_DATAFILE_INDEX */      {"Datafile index out of range", LIBRARY},
/* ERR_BAD_DATAFILE_COUNT */      {"Datafile count is out of sync", LIBRARY},
/* ERR_BAD_ATTR_COUNT */          {"Attribute counter is out of sync", LIBRARY},
/* ERR_BAD_ATTR_INDEX  */         {"Attribute index is out of range", LIBRARY},
/* ERR_BAD_SIG_INDEX */           {"Signature index is out of range", LIBRARY},
/* ERR_BAD_SIG_COUNT */           {"Signature count is out of sync", LIBRARY},
/* ERR_BAD_ROLE_INDEX */          {"Role index is out of range", LIBRARY},
/* ERR_BAD_DOCINFO_COUNT */       {"Document info count is out of sync", LIBRARY},
/* ERR_BAD_DOCINFO_INDEX */       {"Document info index is out of range", LIBRARY},
/* ERR_BAD_NOTARY_INDEX */        {"Notary index is out of range", LIBRARY},
/* ERR_BAD_NOTARY_ID */           {"Bad notary ID", LIBRARY},
/* ERR_BAD_NOTARY_COUNT */        {"Notary count is out of sync", LIBRARY},
/* ERR_X509_DIGEST */             {"X509 digest creation failed", LIBRARY},
/* ERR_CERT_LENGTH */             {"Wrong certificate length", LIBRARY},
/* ERR_PKCS_LIB_LOAD */           {"PKCS #11 DLL load failed", TECHNICAL},
/* ERR_PKCS_SLOT_LIST */          {"Getting PKCS #11 slot list failed", TECHNICAL},
/* ERR_PKCS_WRONG_SLOT */         {"No such PKCS #11 slot", TECHNICAL},
/* ERR_PKCS_LOGIN */              {"No EstID card, wrong PIN or PIN blocked", USER},
/* ERR_PKCS_PK */                 {"Locating private key from EstID failed", TECHNICAL},
/* ERR_PKCS_CERT_LOC */           {"Reading certificate from EstID failed", TECHNICAL},
/* ERR_PKCS_CERT_DECODE */        {"Decoding certificate failed", LIBRARY},
/* ERR_PKCS_SIGN_DATA */          {"Signing data with EstID failed", USER},
/* ERR_PKCS_CARD_READ */          {"Reading EstID card failed", USER},
/* ERR_CSP_NO_CARD_DATA */        {"No EstID card, or card can not be read", USER},
/* ERR_CSP_OPEN_STORE */          {"Can not open system store", TECHNICAL},
/* ERR_CSP_CERT_FOUND*/		  {"Certificate not found from store, probably cetificate not registered", USER},
/* ERR_CSP_SIGN */                {"CSP signing failed", USER},
/* ERR_CSP_NO_HASH_START */       {"Can not start CSP hashing", TECHNICAL},
/* ERR_CSP_NO_HASH */             {"CSP hash failed", LIBRARY},
/* ERR_CSP_NO_HASH_RESULT */      {"Can not read CSP's hashing result", LIBRARY},
/* ERR_CSP_OPEN_KEY	*/            {"Can not open key", TECHNICAL},
/* ERR_CSP_READ_KEY */            {"Can not read key", TECHNICAL},
/* ERR_OCSP_SIGN_NOT_SUPPORTED */ {"Requuested OCSP sign method not suported", LIBRARY},
/* ERR_OCSP_SIGN_CSP_NAME */      {"Can not add Signer's name to requst", LIBRARY},
/* ERR_CSP_CERT_DECODE */         {"Decoding certificate failed", TECHNICAL},
/* ERR_OCSP_SIGN_PKCS_NAME */     {"Can not add Signer's name to requst", TECHNICAL},
/* ERR_OCSP_SIGN_OSLL_CERT */     {"Cannot add cert to OCSP request", TECHNICAL},
/* ERR_OCSP_SIGN */               {"Can not sign OCSP request", USER},
/* ERR_CERT_ISSUER */             {"Cert not issued by this CA, or wrong cert signature", USER},
/* ERR_OCSP_PKCS12_CONTAINER */   {"Can not open pkcs12 container", USER},
/* ERR_MODIFY_SIGNED_DOC */		  {"Cannot modify signed doc. Remove signatures first.", USER},
/* ERR_NOTARY_EXISTS */			  {"Cannot remove signature if notary exists", USER},
/* ERR_UNSUPPORTED_CERT_SEARCH */ {"Unsuported CERT_SEARCH method", LIBRARY},
/* ERR_INCORRECT_CERT_SEARCH */   {"Incorrct CERT_SEARCH pattern", LIBRARY},
/* ERR_BAD_OCSP_RESPONSE_DIGEST */{"Incorrect Notary signature digest", USER},
/* ERR_LAST_ESTID_CACHED */       {"Wrong certificate in cache. Please try again.", USER},
/* ERR_BAD_DATAFILE_XML */		  {"XML content cannot contain the first XML line", USER},
/* ERR_UNSUPPORTED_VERSION */	  {"Unsupported SK-XML version. Please upgrade!", TECHNICAL},
/* ERR_UNSUPPORTED_CHARSET */	  {"Unsupported charset", TECHNICAL},
/* ERR_PKCS12_EXPIRED */		  {"PKCS#12 certificate has expired. Please get a new onew from www.sk.ee", USER},
/* ERR_CSP_USER_CANCEL */		  {"User canceled certificate selection", USER},
/* ERR_CSP_NODEFKEY_CONTAINER */  {"Can't find default key container", TECHNICAL},
/* ERR_CONNECTION_FAILURE */      {"Connection error", USER},
/* ERR_WRONG_URL_OR_PROXY */      {"Wrong URL or Proxy", USER},
/* ERR_NULL_PARAM		*/	      {"Mandatory parameter is NULL", LIBRARY},
/* ERR_BAD_ALLOC			*/    {"Memory allocation error", LIBRARY},
/* ERR_CONF_FILE	*/			  {"Error opening configuration file", USER},
/* ERR_CONF_LINE	*/			  {"Error in configuration file", USER},
/* ERR_OCSP_CERT_REVOKED */ { "Certificate has been revoked!", USER},
/* ERR_OCSP_CERT_UNKNOWN */ { "Certificate status unknow! Not supported by this CA?", USER},
/* ERR_OCSP_PKCS12_NO_FILE */ { "PKCS#12 token file not defined!", USER},
/* ERR_OCSP_PKCS12_NO_PASSWD */ { "PKCS#12 token file password not defined!", USER},
/* ERR_BAD_DATAFILE_CONTENT_TYPE */ { "Invalid DataFile content type!", TECHNICAL},
/* ERR_OCSP_WRONG_URL */ { "Wrong OCSP responder URL!", USER},
/* ERR_OCSP_MALFORMED */ { "Malformed OCSP request!", TECHNICAL},
/* ERR_OCSP_INTERNALERR */ { "Internal error in OCSP responder!", USER},
/* ERR_OCSP_TRYLATER */ { "Try later! OCSP responder is busy", USER},
/* ERR_OCSP_SIGREQUIRED */ { "Must sign OCSP requests!", USER},
/* ERR_OCSP_UNAUTHORIZED */ { "Unauthorized OCSP request!", USER},
/* ERR_UNKNOWN_CA */ { "Unknown Certificate issuer!", USER},

/* ERR_DENC_ENC_METHOD */ { "Invalid encryption method!", TECHNICAL},
/* ERR_DENC_ENC_XMLNS */ { "Invalid xml namespace!", TECHNICAL},
/* ERR_DENC_BAD_PROP_IDX */ { "Invalid EncryptionProperty index!", TECHNICAL},
/* ERR_DENC_BAD_KEY_IDX */ { "Invalid EncryptedKey index!", TECHNICAL},
/* ERR_DENC_KEY_STATUS */ { "Transport key not ready!", USER},
/* ERR_DENC_DATA_STATUS */  { "Invalid data status for this operation!", USER},
/* ERR_DENC_DECRYPT */ { "Failed to decrypt the data!", USER},
/* ERR_CHARSET_CONVERT */ { "Error converting charsets!", TECHNICAL },
/* ERR_COMPRESS */ { "Error compressing the data!", TECHNICAL },
/* ERR_DECOMPRESS */ { "Error decompressing the data!", TECHNICAL },
/* ERR_OCSP_CERT_NOTFOUND */ { "OCSP Responders cetificate not found!", USER },
/* ERR_INVALID_CONTENT */ { "Invalid characters in manifest or addres!", USER },
/* ERR_DENC_NO_KEY_FOUND */ { "No transport key for this smartcard!", USER },
/* ERR_OCSP_RESP_NOT_TRUSTED */ { "OCSP responder is not trusted! No certificate for this responder in local certstore!", USER },
/* ERR_PRIV_CERT_NOT_FOUND */ { "Certificate not found!", USER },
/* ERR_NO_OCSP */ { "Signature has no OCSP confirmation!", USER },
/* ERR_OCSP_WRONG_SIGNATURE */ { "OCSP signature is wrong!", USER },
/* ERR_BAD_PARAM */ { "Invalid parameter!", TECHNICAL },
/* ERR_GENERIC_SOAP_ERR */ { "Generic SOAP error", TECHNICAL },

/* ERR_TS_TIMESTAMPINFO_TYPE */ { "Invalid Timestamp type", TECHNICAL },
/* ERR_TS_BAD_INCLUDEINFO_IDX */ { "Invalid Include index", TECHNICAL },
/* ERR_TS_BAD_TIMESTAMPINFO_IDX */ { "Invalid TimestampInfo index", TECHNICAL },
/* ERR_TS_CREATE_TS_REQ */ { "Error creating timestamp request", TECHNICAL },
/* ERR_CREATE_NONCE */ { "Error creating nonce", TECHNICAL },
/* ERR_TXT2OID */ { "Error converting text to OID", TECHNICAL },
/* ERR_HTTP_ERR */ { "Invalid HTTP response code", TECHNICAL },
/* ERR_BAD_CERTID_IDX */ { "Invalid Include index", TECHNICAL },
/* ERR_BAD_CERTVALUE_IDX */ { "Invalid Include index", TECHNICAL },
/* ERR_TS_VERIFY */ { "Timestamp verification error", USER },
/* ERR_TS_REQUEST */ { "Error getting timestamp", USER },
/* ERR_TSA_NOT_TRUSTED */ { "TSA is not trusted! No certificate for this TSA in local certstore!", USER },
/* ERR_ORPHONED_SIGNATURE */ { "Incomplete or orphoned signature!", TECHNICAL },


/* ERR_WPKI_UNKNOWN_USER */ { "Unknown WPKI user!", USER },
/* ERR_WPKI_INVALID_PHONE_NO */ { "Invalid phone number for this WPKI user!", USER },
/* ERR_WPKI_UNTRUSTED_SRVICE */ { "WPKI service is not trusted!", USER },
/* ERR_WPKI_UNTRUSTED_USER */ { "Service demands customer authentication!", USER },

/* ERR_WPKI_UNUSABLE_PHONE */ { "Error signing with customers mobile phone! Unusable phone type?", USER },
/* ERR_WPKI_TIMEOUT	*/ { "Timeout during mobile signing!", USER },
/* ERR_WPKI_CANCELLED */ { "User cancelled mobile signing!", USER },
/* ERR_WPKI_MID_NOT_READY */ { "MID not ready!", USER },
/* ERR_WPKI_PHONE_NOT_REACHABLE */ { "Users phone is not reachable!", USER },
/* ERR_WPKI_SENDING_ERROR */ { "Error sending signing request to users mobile phone!", USER },
/* ERR_WPKI_SIM_ERROR */ { "SIM card error!", USER },
/* ERR_WPKI_SERVICE_ERR	*/ { "Mobile signing service internal error!", USER },

/*  */                {"", NO_ERRORS}
};


//==========< global variables >====================

//int g_ddocLastError = ERR_OK;

#define INITIAL_MAX_THREADS 1
#define NOT_FOUND   -1
                                                                                                                              
#if defined(WIN32)
    #define THREAD_ID   DWORD
#elif defined(USEPTHREADS)
    #define THREAD_ID   pthread_t
#else
    #define THREAD_ID   pid_t
#endif

typedef struct ThreadErrors_st {
	THREAD_ID	tid; 		
	int 	currentErrorIdx;
	int 	readErrorIdx;
	ErrorInfo	ddocLastErrors[ERROR_BUF_LENGTH];
} ThreadErrors;

static int g_threads = 0;

static ThreadErrors	**ddocErrors = NULL;

// Following mutual exclusion objects are here to
// protect access to ddocErrors and g_threads; whenever we access these variables
// we have to use lock/unlock.

#if	defined(USEPTHREADS)
	pthread_mutex_t	m_ddocErrors = PTHREAD_MUTEX_INITIALIZER;
#elif	defined(WIN32)
	//My bad, no static initializer for critical sections; initialized in DigiDocLib.c
	CRITICAL_SECTION cs_ddocErrors;
#endif

// Returns unique thread identifier
static THREAD_ID getTid(void)
{
#if		defined(WIN32)
	return GetCurrentThreadId();
#elif	defined(USEPTHREADS)
	return pthread_self();
#else 
	return getpid();
#endif
}

static void	lock(void)
{
#if defined WIN32
	EnterCriticalSection(&cs_ddocErrors);
#elif defined USEPTHREADS
	pthread_mutex_lock(&m_ddocErrors);
#else
	// Hope it will be optimized away..
#endif
}


static void unlock(void)
{
#if defined WIN32
    LeaveCriticalSection(&cs_ddocErrors);
#elif defined USEPTHREADS
    pthread_mutex_unlock(&m_ddocErrors);
#else
	// Hope it will be optimized away..
#endif
}


// Grows or creates the array of pointers to ThreadErrors
// Will be always called with locked mutex.
static int growErrorTable(void)
{
  int slotsToAllocate;
  int i;
  ThreadErrors	**tmpErrors;
	
  //	printf("growErrorTable init : g_threads=%d, ddocErrors=%p\n", g_threads, ddocErrors);
  if (g_threads == 0) 
    slotsToAllocate = INITIAL_MAX_THREADS;	// We have no table so far at all
  else
    slotsToAllocate = g_threads * 2;		// We'll double the table size everytime
  tmpErrors = (ThreadErrors**)realloc(ddocErrors, slotsToAllocate * sizeof(ThreadErrors *)); // MEMLEAK: ???
  if (tmpErrors == NULL) 	
    return ERR_BAD_ALLOC;					
  ddocErrors = tmpErrors;
  for (i = g_threads; i < slotsToAllocate; i++) // Initialize new entries;
    ddocErrors[i] = NULL;  
  g_threads = slotsToAllocate;	
  // printf("growErrorTable leave : g_threads=%d, ddocErrors=%p\n", g_threads, ddocErrors);	
  return ERR_OK;
}

static int isThreadEqual(THREAD_ID tid1, THREAD_ID tid2)
{
#if defined(WIN32)
  return tid1 == tid2;
#elif defined(USETHREADS)
  return pthread_equal(tid1, tid2);
#else
  return tid1 == tid2;
#endif
}

// Finds slot number in ddocErrors[] whose pointer points to ThreadErrors belonging to threadID
// or returns NOT_FOUND;
static int findSlotByTid(THREAD_ID threadID)
{
  int i;

  lock();	
  if (ddocErrors == NULL) {
    unlock();
    return NOT_FOUND;
  }
  
  for (i = 0; i < g_threads; i++)
    if (ddocErrors[i] && isThreadEqual(ddocErrors[i]->tid,threadID)) {
      unlock();
      return i;
    }
  unlock();
  return NOT_FOUND;	
}


// Finds thread's ThreadError structure address, returns NULL if the thread doesn't have it (yet).
static ThreadErrors *findThreadErrorsByTid(THREAD_ID threadID)
{
  ThreadErrors *tmpThreadErrors;
  int slot = findSlotByTid(threadID);

  if (slot == NOT_FOUND) 
    return NULL;
  lock();
  tmpThreadErrors = ddocErrors[slot];
  unlock();
  return tmpThreadErrors;
}

// Creates ThreadErrors structure for thread Tid, returns pointer to 
// created object; returns NULL if the object wasn't created.
static ThreadErrors *addThreadErrorsByTid(THREAD_ID Tid)
{
  int i, slot = NOT_FOUND;

  ThreadErrors *threadErrors = findThreadErrorsByTid(Tid);
  if (threadErrors != NULL)			// Already present, do nothing
    return threadErrors;
  lock();
  for (i = 0; i < g_threads; i++)  {  
    if (ddocErrors[i] == NULL)		// Won't enter here if g_threads == 0
      slot = i;					// first free slot found
  }
  if (slot == NOT_FOUND) {
    if (growErrorTable() == ERR_OK) {
      for (i = 0; i < g_threads; i++)  { //Try again...
	if (ddocErrors[i] == NULL)  
	  slot = i; 
      }
    } else {
      unlock();
      return NULL;
    }
  }
	
  threadErrors = (ThreadErrors*)malloc(sizeof(ThreadErrors)); // MEMLEAK: ???
  if (threadErrors == NULL) {
    unlock();
    return NULL;
  }

  memset(threadErrors, 0, sizeof(ThreadErrors));
  threadErrors->tid = Tid;
  threadErrors->readErrorIdx = -1;
  threadErrors->currentErrorIdx = -1;

  ddocErrors[slot] = threadErrors;
  unlock();
  return threadErrors;
}
	
// Releases memory allocated for ThreadError structure of thread threadID
EXP_OPTION void	freeThreadErrorsByTid(THREAD_ID threadID)
{
  ThreadErrors *threadErrors;
  int slot = findSlotByTid(threadID);
	
  if (slot == NOT_FOUND)
    return;	
  
  threadErrors = findThreadErrorsByTid(threadID);

  lock();  
  free(threadErrors);
  ddocErrors[slot] = NULL;
  unlock();
}

//================< error handling functions> =================================

//returns textual explanation of the error code
EXP_OPTION char* getErrorString(int code)
{
  if(code < ERR_MAX && code >= 0) 
    return g_ddocErrorStrings[code].errorMessage;	
  else 
    return "No error message defined for this error";
}

EXP_OPTION ErrorClass getErrorClass(int code)
{
  if(code < ERR_MAX && code >= 0) 
    return g_ddocErrorStrings[code].errorClass;
  else 
    return NO_ERRORS;
}

//returns the last
EXP_OPTION ErrorInfo* getErrorInfo()
{
  ErrorInfo *pErrInfo = 0;
  THREAD_ID Tid = getTid();

  ThreadErrors *threadErrors = findThreadErrorsByTid(Tid);
  if (threadErrors == NULL)
    return 0;

  if(threadErrors->readErrorIdx >= 0 && 
     threadErrors->ddocLastErrors[threadErrors->readErrorIdx].code != ERR_OK) {
    pErrInfo = &(threadErrors->ddocLastErrors[threadErrors->readErrorIdx]);
    threadErrors->readErrorIdx--;
    if(threadErrors->readErrorIdx < 0) //roll over
      threadErrors->readErrorIdx = ERROR_BUF_LENGTH - 1;
  }
  else
    pErrInfo = 0;

  return pErrInfo;
}

//returns 1, if all errors are read and 0 otherwise
EXP_OPTION int hasUnreadErrors() 
{
  THREAD_ID Tid = getTid();

  ThreadErrors *threadErrors = findThreadErrorsByTid(Tid);
  if (threadErrors == NULL)
    return 0;
  return  (threadErrors->readErrorIdx >= 0 ?
	   threadErrors->ddocLastErrors[threadErrors->readErrorIdx].code : ERR_OK);
}


EXP_OPTION void clearErrors()
{
  THREAD_ID Tid = getTid();
  ThreadErrors *threadErrors = findThreadErrorsByTid(Tid);
  if (threadErrors == NULL)
    return;
  memset(threadErrors->ddocLastErrors, 0, sizeof(ErrorInfo) * ERROR_BUF_LENGTH);
  threadErrors->readErrorIdx = -1;
  threadErrors->currentErrorIdx = -1;
}


EXP_OPTION void resetError(ErrorInfo *pErrInfo)
{
  pErrInfo->code = ERR_OK;
  pErrInfo->fileName = "";
  pErrInfo->line = 0;
  pErrInfo->assertion = "";
}


EXP_OPTION void addError(int code, char *fileName, int line, char *assertion)
{
  //no errors found yet. Set a trace-back mark to the end of array.
  //printf("Error : %d at %s line %d, assertion %s\n", code, fileName, line, assertion);

  ThreadErrors	*threadErrors;
  THREAD_ID Tid = getTid();		//Find our identity

  threadErrors = findThreadErrorsByTid(Tid);
  //	printf("addError init: tid=%ld, threadErrors=%p\n", Tid, threadErrors);
  if (threadErrors == NULL) {	//This Tid has no entry in ThreadErrors table
    threadErrors = addThreadErrorsByTid(Tid);  // MEMLEAK:  ???
    if (threadErrors == NULL)
      return;				// What else can we do?
  }
	
  // printf("addError step 1 : tid=%ld, threadErrors=%p\n", Tid, threadErrors);
  if(threadErrors->currentErrorIdx < 0)
    resetError(&(threadErrors->ddocLastErrors[ERROR_BUF_LENGTH - 1]));

  threadErrors->currentErrorIdx++;

  //index at the end -> roll it over to the beginning
  if(threadErrors->currentErrorIdx == ERROR_BUF_LENGTH)
    threadErrors->currentErrorIdx = 0;

  //set the information
  ddocDebug(4, "addError", "Index: %d Error : %d at %s line %d, assertion %s", threadErrors->currentErrorIdx, code, fileName, line, assertion);
  threadErrors->ddocLastErrors[threadErrors->currentErrorIdx].code = code;
  threadErrors->ddocLastErrors[threadErrors->currentErrorIdx].fileName = fileName;
  threadErrors->ddocLastErrors[threadErrors->currentErrorIdx].line = line;
  threadErrors->ddocLastErrors[threadErrors->currentErrorIdx].assertion = assertion;

  //index at the end? Set the traceback mark to the beginning
  if(threadErrors->currentErrorIdx == ERROR_BUF_LENGTH - 1)
    resetError(&(threadErrors->ddocLastErrors[0]));
  else //set the traceback mark to the next position
    resetError(&(threadErrors->ddocLastErrors[threadErrors->currentErrorIdx + 1]));

  threadErrors->readErrorIdx = threadErrors->currentErrorIdx;
}

//--------------------------------------------------
// Checks DigiDoc library internal errors
//--------------------------------------------------
EXP_OPTION int checkDigiDocErrors()
{
  char *errorClass[] = {"NO_ERRORS", "TECHNICAL", "USER", "LIBRARY"};
  int err = ERR_OK;
  while(hasUnreadErrors()) {
    ErrorInfo* pErr = getErrorInfo();
    char* pErrStr = getErrorString(pErr->code);
    printf("Error: %d - %s; file: %s line: %d; failed condition: %s, error class : %s\n",
	   pErr->code, pErrStr, pErr->fileName, pErr->line, pErr->assertion, errorClass[getErrorClass(pErr->code)]);
    err = pErr->code;
  }
  clearErrors();
  return err;
}

EXP_OPTION int getLastError()
{
  THREAD_ID Tid = getTid();

  ThreadErrors *threadErrors = findThreadErrorsByTid(Tid);
  if (threadErrors == NULL)
    return 0;
  return  (threadErrors->readErrorIdx >= 0 ?
	   threadErrors->ddocLastErrors[threadErrors->readErrorIdx].code : ERR_OK);
}
