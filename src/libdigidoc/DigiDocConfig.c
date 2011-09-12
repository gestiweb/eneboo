//==================================================
// FILE:	DigiDocCfonfig.c
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for configuration management
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
// Lesser General Public License for more details.
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//      17.06.2004      Fixed buffer overflow vulnerability in setPrivateConfigFile()
//      27.03.2004      Fixed ConfigItem_lookup_bool()
//      20.03.2004      Added functions createOrReplacePrivateConfigItem()
//                      writeConfigFile(), writePrivateConfigFile()
//      20.03.2004      changed function notarizeSignature to check for PKCS12 arguments
//      10.02.2004      Integrated
//      29.01.2004      changed function notarizeSignature
//      26.01.2004      Added <Windows.h> include
//      08.01.2004      Veiko Sinivee
//                      Creation
//==================================================

// config data comes from there
#ifndef WIN32
  #if HAVE_CONFIG_H
    #include <config.h>
  #endif
#endif


//AA 04/01/26
#ifdef WIN32
#include <windows.h>
#define snprintf _snprintf
#endif

#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocPKCS11.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocOCSP.h>
#include <libdigidoc/DigiDocConvert.h>
#ifdef WITH_TS
  #include <libdigidoc/ts/DigiDocTS.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifdef WIN32
#else
#endif
#ifdef WIN32
	#define DIGIDOC_CONF_NAME	"digidoc.ini"
	#define HOME_ENV			"USERPROFILE"
	#define DIGIDOC_CONF_FMT		"%s\\%s"
	char g_szGlobalConfigFile[_MAX_PATH];
#else
	#define DIGIDOC_CONF_NAME   "digidoc.conf"
	#define HOME_ENV	"HOME"
	#define DIGIDOC_CONF_FMT	"%s/.%s"
	char g_szGlobalConfigFile[] = SYSCONFDIR "/" DIGIDOC_CONF_NAME;
#endif

#ifndef _MAX_PATH
	#define _MAX_PATH 200
#endif
char g_szPrivateConfigFile[_MAX_PATH];


//==========< private types and functions >====================


// forward deklarations of private helper functions
int ConfigItem_new(ConfigItem** pItem, const char* key, const char* value, int type, int status);
void ConfigItem_free(ConfigItem* pItem);
ConfigItem* ConfigItem_find(const char* key);



//==========< global variables >====================

// currently I see the need only for one common configuration store
// Distinction can be made by item type
ConfigurationStore  g_configStore; 

//==========< win32 specific functions >===================

#ifdef WIN32


//--------------------------------------------------
// Retrieves a Windows registry key value
// key - key name
// buf - value buffer
// len - value buffer length
//--------------------------------------------------
void getRegKey(const char* key, LPBYTE buf, DWORD* len)
{
  LONG rc;
  HKEY hKey;

  memset(buf, 0, *len);	
  rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key,
		    0, KEY_QUERY_VALUE, &hKey );
  if(rc == ERROR_SUCCESS) {
    rc = RegQueryValueEx(hKey, NULL, NULL, NULL, buf, len);
    RegCloseKey( hKey );
    if(rc == ERROR_SUCCESS) {
      buf[*len] = 0;
    } else {
	  buf[0] = 0;
	}
  }
}

//--------------------------------------------------
// Retrieves a Windows registry key value. If the key 
// is not set then uses the default value and sets it
// key - key name
// buf - value buffer
// len - value buffer length
// defValue - default value
//--------------------------------------------------
void getOrSetRegKey(const char* key, LPBYTE buf, DWORD* len, const char* defValue)
{
  LONG rc;
  HKEY hKey;

  memset(buf, 0, *len);	
  rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key,
		    0, KEY_QUERY_VALUE, &hKey );
  if(rc == ERROR_SUCCESS) {
    rc = RegQueryValueEx(hKey, "", NULL, NULL, buf, len);
    RegCloseKey( hKey );
    if(rc == ERROR_SUCCESS) {
      buf[*len] = 0;
    }
  } else {
    rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE, key,
			0, "", 0, KEY_READ | KEY_WRITE, 0, &hKey, 0);
    if(rc == ERROR_SUCCESS) {
      rc = RegSetValueEx(hKey,  "", 0, REG_SZ, 
			 (const BYTE*)defValue, lstrlen(defValue));
      RegCloseKey(hKey);
    }
  }
}

//--------------------------------------------------
// Sets a Windows registry key value
// key - key name
// buf - value buffer
// len - value buffer length
//--------------------------------------------------
void setRegKey(const char* key, LPBYTE buf, DWORD len)
{
  LONG rc;
  HKEY hKey;

  rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE, key,
		      0, "", 0, KEY_READ | KEY_WRITE, 0, &hKey, 0);
  if(rc == ERROR_SUCCESS) {
    rc = RegSetValueEx(hKey,  "", 0, REG_SZ, buf, len);
    RegCloseKey(hKey);
  }
}


//--------------------------------------------------
// Retrieves the number of subkeys Windows registry 
// key value. 
// returns number of subkeys
//--------------------------------------------------
int getNumSubKeys(const char* key)
{
  int  n = 0;
  HKEY hKey;
  DWORD num;

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, key,
		  0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS) {
    if(RegQueryInfoKey(hKey, NULL, NULL, NULL, &num, 
		       NULL, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) 
      n = (int)num;
    RegCloseKey( hKey );		
  } 
  return n;
}

//--------------------------------------------------
// Deletes a subkey from Windows registry 
// key - parent key
// subkey - subkey to be deleted
//--------------------------------------------------
void deleteSubKey(const char* key, const char* subkey)
{
  HKEY hKey;

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, key,
		  0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS) {
    RegDeleteKey(hKey, subkey);
    RegCloseKey(hKey);		
  } 
}

char* g_regDigiDocRoot = "SOFTWARE\\DigiDocLib";

//--------------------------------------------------
// Retrieves the digidoc librarys config items from registry 
// returns error coder or ERR_OK
//--------------------------------------------------
EXP_OPTION int readConfigFromRegistry()
{
  HKEY hKey;
  DWORD rc, l, i;
  int err = 0;
  char keyName[255], fullName[500];
  char keyValue[3000];
  FILETIME ft;

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_regDigiDocRoot,
		  0, KEY_READ, &hKey ) != ERROR_SUCCESS )
    return ERR_CONF_FILE;

  i = 0;
  do {
    l = sizeof(keyName);
    *keyName = 0;
    rc = RegEnumKeyEx(hKey, i, keyName, &l, NULL, NULL, NULL, &ft);
    if ( (rc == ERROR_SUCCESS) && *keyName ) {
      l = sizeof(keyValue);
      *keyValue = 0;
      snprintf(fullName, sizeof(fullName), "%s\\%s", g_regDigiDocRoot, keyName);
      getRegKey(fullName, (BYTE*)keyValue, &l);
      if (*keyValue) {
		ddocDebug(3, "readConfigFromRegistry", "Reg key: %s val: %s", (const char*)keyName, (const char*)keyValue);
	    err = addConfigItem(NULL, (const char*)keyName, (const char*)keyValue, ITEM_TYPE_PRIVATE, ITEM_STATUS_OK);
	  }
      SET_LAST_ERROR_IF_NOT(err == ERR_OK, err);
    }
    i++;
  } while ( (rc == ERROR_SUCCESS) && *keyName );
  RegCloseKey(hKey);		
  return err;
}

#endif // WIN32

//==========< item handling functions >====================

//--------------------------------------------------
// Initializes configuration store
// szConfigFile - name of config file. Use NULL for default
//--------------------------------------------------
EXP_OPTION int initConfigStore(const char* szConfigFile) 
{ 
  int err = ERR_OK, at_least_one_conf = 0; 

  g_configStore.nItems = 0; 
  g_configStore.pItems = 0; 
  if (szConfigFile && checkFileExists(szConfigFile)) { 
    err = readConfigFile(szConfigFile, ITEM_TYPE_GLOBAL); 
    return err; 
  } 

#ifdef WIN32 
  if(!*g_szGlobalConfigFile && getenv("SystemRoot")) {
    strncpy(g_szGlobalConfigFile, getenv("SystemRoot"), sizeof(g_szGlobalConfigFile));
    if(!g_szGlobalConfigFile[strlen(g_szGlobalConfigFile)-1] == '\\')
      strncat(g_szGlobalConfigFile, "\\", sizeof(g_szGlobalConfigFile) - strlen(g_szGlobalConfigFile));
    strncat(g_szGlobalConfigFile, DIGIDOC_CONF_NAME, sizeof(g_szGlobalConfigFile) - strlen(g_szGlobalConfigFile));
  }
#endif 
  ddocDebug(3, "initConfigStore", "Reading global config file: %s", g_szGlobalConfigFile);
  if(checkFileExists(g_szGlobalConfigFile))
    err = readConfigFile(g_szGlobalConfigFile, ITEM_TYPE_GLOBAL);  // MEMLEAK: ???
  if(err == ERR_CONF_FILE) 
    err = ERR_OK; 
  else 
    at_least_one_conf = 1; 
  if(err) 
    return err; 

  setPrivateConfigFile(NULL); // set default private conf file 
  ddocDebug(3, "initConfigStore", "Reading private config file: %s", g_szPrivateConfigFile);
  if(checkFileExists(g_szPrivateConfigFile))
    err = readConfigFile(g_szPrivateConfigFile, ITEM_TYPE_PRIVATE); 
  if(err == ERR_CONF_FILE) 
    err = ERR_OK; 
  else 
    at_least_one_conf = 1; 
  if(err) 
    return err; 
  
  ddocDebug(2, "initConfigStore", "Reading config file: %s", DIGIDOC_CONF_NAME); 
  if(checkFileExists(DIGIDOC_CONF_NAME))
    err = readConfigFile(DIGIDOC_CONF_NAME, ITEM_TYPE_PRIVATE); 
  if(err == ERR_CONF_FILE) 
    err = ERR_OK; 
  else 
    at_least_one_conf = 1; 
  if(err) 
    return err; 
  
#ifdef WIN32 
  err = readConfigFromRegistry(); 
  if(err == ERR_CONF_FILE) 
    err = ERR_OK; 
  else 
    at_least_one_conf = 1; 
  if(err) 
    return err; 
#endif 

  if(!at_least_one_conf) 
    err = ERR_CONF_FILE; 
  return err; 
} 


//--------------------------------------------------
// Cleans memory of configuration store
// pConfStore - configuration collection (use NULL for default)
//--------------------------------------------------
EXP_OPTION void cleanupConfigStore(ConfigurationStore *pConfStore)
{
  int i;

  if(!pConfStore)
    pConfStore = &g_configStore;
  for(i = 0; (i < pConfStore->nItems) && pConfStore->pItems && pConfStore->pItems[i]; i++)
    ConfigItem_free(pConfStore->pItems[i]);
  free(pConfStore->pItems);
  pConfStore->pItems = 0;
  pConfStore->nItems = 0;
}

//--------------------------------------------------
// Creates a new configration item
// pItem - address of new pointer location
// key - items key
// value - items value
// type - item type
// status - item status
// returns ERR_OK on success
//--------------------------------------------------
int ConfigItem_new(ConfigItem** pItem, const char* key, const char* value, int type, int status)
{
  RETURN_IF_NULL_PARAM(key);
  RETURN_IF_NULL_PARAM(value);
  RETURN_IF_NULL_PARAM(pItem);

  if((*pItem = (ConfigItem*)malloc(sizeof(ConfigItem))) != NULL) {
    (*pItem)->szKey = strdup(key);
    (*pItem)->szValue = strdup(value);
    (*pItem)->nType = type;
    (*pItem)->nStatus = status;
    if(!(*pItem)->szKey || !(*pItem)->szValue) {
      ConfigItem_free(*pItem);
      *pItem = NULL;
      SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
    }
  } else 
    SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
  return ERR_OK;
}

//--------------------------------------------------
// Cleanup of config items memory
// pItem - address of config item
//--------------------------------------------------
void ConfigItem_free(ConfigItem* pItem)
{
  if(pItem) {
    if(pItem->szKey)
      free(pItem->szKey);
    if(pItem->szValue)
      free(pItem->szValue);
    free(pItem);
  }
}


//--------------------------------------------------
// Adds a new configration item
// pConfStore - configuration collection (use NULL for default)
// key - items key
// value - items value
// type - item type
// status - item status
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int addConfigItem(ConfigurationStore *pConfStore, const char* key, const char* value, int type, int status)
{
  int err = ERR_OK;
  ConfigItem* pItem;
  ConfigItem** pItems;

  if(!pConfStore)
    pConfStore = &g_configStore;
  if((err = ConfigItem_new(&pItem, key, value, type, status)) == ERR_OK) {
    if((pItems = (ConfigItem**)realloc(pConfStore->pItems, 
				       sizeof(void*) * (pConfStore->nItems + 1))) != NULL) {
      pItems[pConfStore->nItems] = pItem;
      pConfStore->nItems++;
      pConfStore->pItems = pItems;
    } else
      SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
  } else 
    SET_LAST_ERROR_RETURN_CODE(err);
  return ERR_OK;
}

//--------------------------------------------------
// Adds a new private configration item or modifies
// an existing one
// pConfStore - configuration collection (use NULL for default)
// key - items key
// value - items value
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int createOrReplacePrivateConfigItem(ConfigurationStore *pConfStore, const char* key, const char* value)
{
  int err = ERR_OK, i;
  ConfigItem* pItem = NULL;
#ifdef WIN32
  char keyName[500];
#endif

  if(!pConfStore)
    pConfStore = &g_configStore;
  // first try to find a private config item
  for(i = 0; (i < pConfStore->nItems) && pConfStore->pItems && pConfStore->pItems[i]; i++) {
    if(pConfStore->pItems[i]->szKey && !strcmp(pConfStore->pItems[i]->szKey, key) &&
       pConfStore->pItems[i]->nType == ITEM_TYPE_PRIVATE) {
      pItem = pConfStore->pItems[i];
      break;
    } 
  }
  if(pItem) { // if found then modify it
    if(pItem->szValue)
      free(pItem->szValue);
    pItem->szValue = (char*)malloc(strlen(value)+1);
    if(pItem->szValue)
      strncpy(pItem->szValue, value, strlen(value)+1);
    else
      SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
    pItem->nStatus = ITEM_STATUS_MODIFIED;
  } else {  // else create a new private config item
    err = addConfigItem(pConfStore, key, value, ITEM_TYPE_PRIVATE, ITEM_STATUS_MODIFIED);
  }
#ifdef WIN32
  snprintf(keyName, sizeof(keyName), "%s\\%s", g_regDigiDocRoot, key);
  setRegKey(keyName, (BYTE*)value, (value ? (DWORD)strlen(value) : (DWORD)0));
  if(pItem)
    pItem->nStatus = ITEM_STATUS_OK;
#endif
  return err;
}

//--------------------------------------------------
// Deletes configration item
// key - items key
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ConfigItem_delete(const char* key)
{
  int err = ERR_OK, i, j;

  // first try to find a private config item
  for(i = j = 0; (i < g_configStore.nItems) && g_configStore.pItems && g_configStore.pItems[i]; i++) {
    if(g_configStore.pItems[i]->szKey && !strcmp(g_configStore.pItems[i]->szKey, key) &&
       g_configStore.pItems[i]->nType == ITEM_TYPE_PRIVATE) {
      ConfigItem_free(g_configStore.pItems[i]);
      g_configStore.pItems[i] = 0;
      continue;
    } 
    g_configStore.pItems[j] = g_configStore.pItems[i]; // no-op until moving the last items forward
    j++;
  }
  g_configStore.nItems = j;
#ifdef WIN32
  deleteSubKey(g_regDigiDocRoot, key);
#endif
  return err;
}


//--------------------------------------------------
// Finds a new configration item by key
// key - items key
// returns item pointer or NULL if not found
//--------------------------------------------------
ConfigItem* ConfigItem_find(const char* key)
{
  int i;

  for(i = 0; (i < g_configStore.nItems) && g_configStore.pItems && g_configStore.pItems[i]; i++) {
    if(g_configStore.pItems[i]->szKey && !strcmp(g_configStore.pItems[i]->szKey, key))
      return g_configStore.pItems[i];
  }
  return NULL;
}

//--------------------------------------------------
// Finds a all configration items that start with this prefix
// prefix - item keys prefix
// returns error code or ERR_OK
//--------------------------------------------------
int ConfigItem_findByPrefix(ConfigurationStore *pConfStore, const char* prefix)
{
  int i, err = ERR_OK;

  for(i = 0; (i < g_configStore.nItems) && g_configStore.pItems && g_configStore.pItems[i]; i++) {
    if(g_configStore.pItems[i]->szKey && !strncmp(g_configStore.pItems[i]->szKey, prefix, strlen(prefix)))
      err = addConfigItem(pConfStore, g_configStore.pItems[i]->szKey,
			  g_configStore.pItems[i]->szValue, 
			  g_configStore.pItems[i]->nType,
			  g_configStore.pItems[i]->nStatus);
  }
  return err;
}


//--------------------------------------------------
// Finds a new configration items value by key
// key - items key
// returns value of config item or NULL if not found
//--------------------------------------------------
EXP_OPTION const char* ConfigItem_lookup(const char* key)
{
  int i;
  // first try to find a private item
  for(i = 0; (i < g_configStore.nItems) && g_configStore.pItems && g_configStore.pItems[i]; i++) {
    if(g_configStore.pItems[i]->szKey && !strcmp(g_configStore.pItems[i]->szKey, key) &&
       g_configStore.pItems[i]->nType == ITEM_TYPE_PRIVATE)
      return g_configStore.pItems[i]->szValue;
  }  // if not found use any type of item with the given key
  for(i = 0; (i < g_configStore.nItems) && g_configStore.pItems && g_configStore.pItems[i]; i++) {
    if(g_configStore.pItems[i]->szKey && !strcmp(g_configStore.pItems[i]->szKey, key))
      return g_configStore.pItems[i]->szValue;
  }
  return NULL;
}

//--------------------------------------------------
// Finds a new configration items value by key from the store
// key - items key
// pConfStore - store to search in
// returns value of config item or NULL if not found
//--------------------------------------------------
EXP_OPTION const char* ConfigItem_lookup_fromStore(ConfigurationStore *pConfStore, const char* key)
{
  int i;

  for(i = 0; pConfStore && (i < pConfStore->nItems) && 
	pConfStore->pItems && pConfStore->pItems[i]; i++) {
    if(pConfStore->pItems[i]->szKey && !strcmp(pConfStore->pItems[i]->szKey, key))
      return pConfStore->pItems[i]->szValue;
  }
  return NULL;
}

//--------------------------------------------------
// Finds a numeric configration items value by key
// key - items key
// defValue - default value to be returned
// returns value of config item or defValue if not found
//--------------------------------------------------
EXP_OPTION int ConfigItem_lookup_int(const char* key, int defValue)
{
	int rc = defValue;
	const char* p = ConfigItem_lookup(key);
	if (p)
		rc = atoi(p);
	return rc;
}

//--------------------------------------------------
// Finds a bolean configration items value by key
// key - items key
// defValue - default value to be returned
// returns value of config item or defValue if not found
//--------------------------------------------------
EXP_OPTION int ConfigItem_lookup_bool(const char* key, int defValue)
{
	int rc = defValue;
	const char* p = ConfigItem_lookup(key);
	if(p)
#ifdef WIN32
	  rc = (!stricmp(p, "true")) ? 1 : 0;
#else
	  rc = (!strcasecmp(p, "TRUE")) ? 1 : 0;
#endif
	return rc;
}


//--------------------------------------------------
// Reads and parses configuration file
// fileName - configuration file name
// type - type of config file global/private
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int readConfigFile(const char* fileName, int type)
{
  FILE* hFile;
  char buf[5000]; 
  char *p;
  int err = ERR_OK;

  if((hFile = fopen(fileName, "rt")) != NULL) {
    while(fgets(buf, sizeof(buf), hFile) != NULL && !err) {
      // trim line separators and spaces
      while(strlen(buf) && (buf[strlen(buf)-1] == '\n' ||
			    buf[strlen(buf)-1] == '\r' || 
			    buf[strlen(buf)-1] == '\t' ||
			    buf[strlen(buf)-1] == ' '))
	buf[strlen(buf)-1] = 0;
      if(strlen(buf) && buf[0] != '#') {
	if((p = strchr(buf, '=')) != NULL) {
	  *p = 0;
	  p++;
	  err = addConfigItem(NULL, (const char*)buf, (const char*)p, type, ITEM_STATUS_OK);
	  SET_LAST_ERROR_IF_NOT(err == ERR_OK, err);
	  //printf("CONF: %s = %s\n", buf, p);
	}
      }
    }
    fclose(hFile);
    return err;
  } else
    SET_LAST_ERROR_RETURN_CODE(ERR_CONF_FILE); // MEMLEAK: ???
}


//--------------------------------------------------
// Writes a configuration file
// fileName - configuration file name
// type - type of config file global/private
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int writeConfigFile(const char* fileName, int type)
{
  FILE* hFile;
  //char buf[300]; 
  //char *p;
  int err = ERR_OK, i;

  if((hFile = fopen(fileName, "wt")) != NULL) {
    // first try to find a private item
    for(i = 0; (i < g_configStore.nItems) && 
	  g_configStore.pItems && g_configStore.pItems[i]; i++) {
      if(g_configStore.pItems[i]->nType == type) {
	fprintf(hFile, "%s=%s\n", g_configStore.pItems[i]->szKey,
		g_configStore.pItems[i]->szValue);
	g_configStore.pItems[i]->nStatus = ITEM_STATUS_OK;
      }
    }
    fclose(hFile);
    return err;
  } else
    SET_LAST_ERROR_RETURN_CODE(ERR_CONF_FILE);
}

//--------------------------------------------------
// Saves all private config items in correct file
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int writePrivateConfigFile()
{
#ifndef WIN32
  return writeConfigFile(g_szPrivateConfigFile, ITEM_TYPE_PRIVATE);
#else
  return ERR_OK;
#endif
}

//--------------------------------------------------
// Sets a new name for private config file. Can be
// used to override default of env(HOME)/.digidoc.conf
// Use NULL to restore default value
//--------------------------------------------------
EXP_OPTION void setPrivateConfigFile(const char* fileName)
{
  if (fileName) {
    strncpy( g_szPrivateConfigFile, fileName, sizeof(g_szPrivateConfigFile) );
  } else { // use default
    snprintf(g_szPrivateConfigFile, sizeof(g_szPrivateConfigFile)-1,
             DIGIDOC_CONF_FMT, getenv(HOME_ENV), DIGIDOC_CONF_NAME);
  }
}

//--------------------------------------------------
// Finds CA certificates index by it's CN
// idx - address if index
// cn - CN of the CA
// return -1 if not found or 0 for success
//--------------------------------------------------
int findCAindexByCN(int* idx, const char* cn)
{
  int i, n;
  char buf1[100];
  const char* p;

  *idx = 0;
  p = ConfigItem_lookup("CA_CERTS");
  RETURN_IF_NOT(p != NULL, ERR_CONF_LINE);
  n = atoi(p);
  for(i = 1; (i <= n) && !(*idx); i++) {
    snprintf(buf1, sizeof(buf1), "CA_CERT_%d_CN", i);
    p = ConfigItem_lookup(buf1);
    RETURN_IF_NOT(p != NULL, ERR_UNKNOWN_CA);
    if(!strcmp(p, cn)) { // found it
      *idx = i;
      break;
    }
  }
  return ERR_OK;
}

//--------------------------------------------------
// Finds Responder certificates index by it's CN
// idx - address if index
// cn - CN of the responder cert
// hash - responder certs hash in base64 form
// ca - responder CA CN
// return error code or 0 for success
//--------------------------------------------------
int findResponderIndex(int* idx, const char* cn, const char* hash, const char* ca)
{
  int err = ERR_OK, i, n;
  char buf1[100];
  const char* p;

  *idx = 0;
  ddocDebug(3, "findResponderIndex", "Find CA: %s hash: %s ca-cn: %s", (cn ? cn : "NULL"), 
	    (hash ? hash : "NULL"), (ca ? ca : "NULL"));
  p = ConfigItem_lookup("DIGIDOC_OCSP_RESPONDER_CERTS");
  RETURN_IF_NOT(p != NULL, ERR_CONF_LINE);
  n = atoi(p);
  for(i = 1; (i <= n) && !(*idx); i++) {
    if(cn) {
      snprintf(buf1, sizeof(buf1), "DIGIDOC_OCSP_RESPONDER_CERT_%d_CN", i);
      p = ConfigItem_lookup(buf1);
      if(p && !strcmp(p, cn)) { // found it
	*idx = i;
	break;
      }
    }
    else if(hash) {
      snprintf(buf1, sizeof(buf1), "DIGIDOC_OCSP_RESPONDER_CERT_%d_HASH", i);
      p = ConfigItem_lookup(buf1);
      if(p && !strcmp(p, hash)) { // found it
	*idx = i;
	break;
      }
    }
    else if(ca) {
      snprintf(buf1, sizeof(buf1), "DIGIDOC_OCSP_RESPONDER_CERT_%d_CA", i);
      p = ConfigItem_lookup(buf1);
      if(p && !strcmp(p, ca)) { // found it
	*idx = i;
	break;
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Finds CA certificate by CN
// ppCA - address of found CA
// szCN - CA certs common name
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int findCAForCN(X509** ppCA, const char* szCN)
{
  int err = ERR_OK, i;
  char buf[300];
  const char *p1, *p2;
  X509 *x509 = NULL;

  // initialize
  *ppCA = NULL;
  err = findCAindexByCN(&i, szCN);
  ddocDebug(3, "findCAForCN", "Find CA: %s idx: %d rc: %d", szCN, i, err);
  RETURN_IF_NOT(err == ERR_OK, ERR_CERT_READ);
  snprintf(buf, sizeof(buf), "CA_CERT_%d", i);
  p1 = ConfigItem_lookup(buf);
  if(!p1)
    ddocDebug(3, "findCAForCN", "Failed to read cert for CA: %s rc: %d", buf);
  RETURN_IF_NOT(p1 != NULL, ERR_UNKNOWN_CA);
  if(checkFileExists(p1)) {
    err = ReadCertificate(&x509, p1);
    ddocDebug(4, "findCAForCN", "Read cert: %s rc: %d", p1, err);
  } else {
    p2 = ConfigItem_lookup("CA_CERT_PATH");
    RETURN_IF_NOT(p2 != NULL, ERR_CONF_LINE);
#ifdef WIN32
    snprintf(buf, sizeof(buf), "%s\\%s", p2, p1);
#else
    snprintf(buf, sizeof(buf), "%s/%s", p2, p1);
#endif
    err = ReadCertificate(&x509, buf);
    ddocDebug(4, "findCAForCN", "Read cert: %s rc: %d\n", buf, err);
  }
  RETURN_IF_NOT(err == ERR_OK, ERR_CERT_READ);
  *ppCA = x509;
  return err;
}

//--------------------------------------------------
// Finds Responders certificate by CN
// ppResp - address of found cert
// szCN - Responder certs common name
// hash - responder certs hash in base64
// nIdx - index of the certificate for this respnder. Starts at 0
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int findResponderByCNAndHashAndIndex(X509** ppResp, const char* szCN, 
						const char* hash, int nIdx)
{
  int err = ERR_OK, i;
  char buf[300];
  const char *p1, *p2;
	
  RETURN_IF_NULL_PARAM(ppResp);
  // initialize
  *ppResp = NULL;
  err = findResponderIndex(&i, szCN, hash, NULL);
  ddocDebug(3, "findResponderByCNAndHashAndIndex", "CN: %s hash: %s resp-index: %d, cert-index: %d",
	    (szCN ? szCN : "NULL"), (hash ? hash : "NULL"), i, nIdx);
  RETURN_IF_NOT(err == ERR_OK, err);
  // compose search key
  if(!nIdx)
    snprintf(buf, sizeof(buf), "DIGIDOC_OCSP_RESPONDER_CERT_%d", i);
  else
    snprintf(buf, sizeof(buf), "DIGIDOC_OCSP_RESPONDER_CERT_%d_%d", i, nIdx);
  p1 = ConfigItem_lookup(buf);
  ddocDebug(3, "findResponderByCNAndHashAndIndex", "Read cert key: %s file: %s", buf, (p1 ? p1 : "NULL"));
  if(p1) {
    if(checkFileExists(p1)) {
      err = ReadCertificate(ppResp, p1);
    } else {
      p2 = ConfigItem_lookup("CA_CERT_PATH");
      if(p2) {
#ifdef WIN32
	snprintf(buf, sizeof(buf), "%s\\%s", p2, p1);
#else
	snprintf(buf, sizeof(buf), "%s/%s", p2, p1);
#endif
	if(checkFileExists(buf))
	  err = ReadCertificate(ppResp, buf);
	ddocDebug(3, "findResponderByCNAndHashAndIndex", "Read cert: %s rc: %d got: %s", buf, err, (*ppResp ? "OK" : "NULL"));
      }
    }
  } // if p1
  return err;
}


//--------------------------------------------------
// Finds Responders certificate by CN
// ppResp - address of found cert
// szCN - Responder certs common name
// hash - responder certs hash in base64 form
// szCertSerial - specific serial number to search
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int findResponder(X509** ppResp, const char* szCN, 
			     const char* szHash, char* szCertSerial)
{
  int err = ERR_OK, i, j;
  char buf[300], szSerial[100];
  const char *p1, *p2;
  time_t t1, t2;
  X509 *x509;
	
  RETURN_IF_NULL_PARAM(ppResp);
  // initialize
  *ppResp = NULL;
  err = findResponderIndex(&i, szCN, szHash, NULL);
  ddocDebug(3, "findResponder", "CN: %s hash: %s index: %d, search notary: %s", 
	    (szCN ? szCN : "NULL"), (szHash ? szHash : "NULL"), i, 
	    (szCertSerial ? szCertSerial : "LATEST"));
  RETURN_IF_NOT(err == ERR_OK, err);
  j = 0;
  t1 = t2 = 0; 
  do {
    x509 = 0;
    p1 = p2 = 0;
    // compose search key
    if(!j)
      snprintf(buf, sizeof(buf), "DIGIDOC_OCSP_RESPONDER_CERT_%d", i);
    else
      snprintf(buf, sizeof(buf), "DIGIDOC_OCSP_RESPONDER_CERT_%d_%d", i, j);
    p1 = ConfigItem_lookup(buf);
    ddocDebug(3, "findResponder", "Read cert key: %s file: %s", buf, (p1 ? p1 : "NULL"));
    if(p1) {
      if(checkFileExists(p1)) {
	err = ReadCertificate(&x509, p1);
	if(x509)
	  t2 = getCertNotAfterTimeT(x509);
	ddocDebug(3, "findResponder", "Read cert: %s rc: %d not-after: %ld", p1, err, (unsigned long)t2);
      } else {
	p2 = ConfigItem_lookup("CA_CERT_PATH");
	RETURN_IF_NOT(p2 != NULL, ERR_CONF_LINE);
#ifdef WIN32
	snprintf(buf, sizeof(buf), "%s\\%s", p2, p1);
#else
	snprintf(buf, sizeof(buf), "%s/%s", p2, p1);
#endif
	err = ReadCertificate(&x509, buf);
	if(x509)
	  t2 = getCertNotAfterTimeT(x509);
	ddocDebug(3, "findResponder", "Read cert: %s rc: %d not-after: %ld", buf, err, (unsigned long)t2);
      } // else
      if(!err && x509) { // if cert read successfully
	if(szCertSerial) { // check for specific notary cert serial
	  szSerial[0] = 0;
	  ReadCertSerialNumber(szSerial, sizeof(szSerial), x509);
	  if(!strcmp(szSerial, szCertSerial)) {
	    *ppResp = x509;
	    ddocDebug(4, "findResponder", "assigning CA 1");
	    return ERR_OK;
	  }
	}
	if(!t1 || t2 > t1) { // first cert to check
	  t1 = t2;
	  ddocDebug(4, "findResponder", "assigning CA 2");
	  *ppResp = x509;
	} 
      }
    } // if p1
    j++;
  } while(x509 || j < 1); // until any potential cert found and I have tried to find one of the multiple certs
  RETURN_IF_NOT(err == ERR_OK, ERR_CERT_READ);
  ddocDebug(3, "findResponder", "CN: %s hash: %s index: %d, search notary: %s", 
	    (szCN ? szCN : "NULL"), (szHash ? szHash : "NULL"), (*ppResp ? "OK" : "NULL"), err);
  return err;
}

//--------------------------------------------------
// Finds Responder certificates CA certs CN
// caCN - buffer for responders CA CN
// len - length of buffer for CA CN
// szCN - responder certs common name
// hash - responder certs hash in base64 form
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int findResponderCA(char* caCN, int len, const char* szCN, const char* hash)
{
  int err = ERR_OK, i;
  char buf[50];
  const char* p;

  // initialize
  RETURN_IF_NULL_PARAM(caCN);
  caCN[0] = 0;
  err = findResponderIndex(&i, szCN, hash, NULL);
  RETURN_IF_NOT(err == ERR_OK, err);
  snprintf(buf, sizeof(buf), "DIGIDOC_OCSP_RESPONDER_CERT_%d_CA", i);
  p = ConfigItem_lookup(buf);
  RETURN_IF_NOT(p != NULL, ERR_OCSP_RESP_NOT_TRUSTED);
  strncpy(caCN, p, len);
  return ERR_OK;
}

//--------------------------------------------------
// Finds CA certificate of the given certificate
// ppCA - address of found CA
// pCert - certificate whose CA we are looking for
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int findCAForCertificate(X509** ppCA, const X509* pCert)
{
  int err = ERR_OK;
  DigiDocMemBuf mbuf;

  mbuf.pMem = 0;
  mbuf.nLen = 0;
  // read cert issuers CN
  err = ddocCertGetIssuerCN((X509*)pCert, &mbuf);
  RETURN_IF_NOT(err == ERR_OK, ERR_PKCS_CERT_DECODE);
  err = findCAForCN(ppCA, (const char*)mbuf.pMem);
  ddocMemBuf_free(&mbuf);
  RETURN_IF_NOT(err == ERR_OK, err);
  return err;
}

//--------------------------------------------------
// Finds CA chain 
// ppChain - address of cert pointer array
// nMaxChain - cert pointer arrary length
// szCN - CN of the first CA cert (not the child cert!)
// return error code or 0 for success
//--------------------------------------------------
EXP_OPTION int findCAChainForCN(X509** ppChain, int* nMaxChain, const char* szCN)
{
  int err = ERR_OK, i;
  char oldcn[300];
  X509 * caCerts[10];
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;

  RETURN_IF_NULL_PARAM(ppChain);
  RETURN_IF_NULL_PARAM(nMaxChain);
  RETURN_IF_NULL_PARAM(szCN);
  // initialize
  for(i = 0; i < 10; i++)
    caCerts[i] = 0;
  for(i = 0; i < *nMaxChain; i++)
    ppChain[i] = NULL;
  i = 0;
  strncpy(oldcn, szCN, sizeof(oldcn)); 
  do {
    err = findCAForCN(&caCerts[i], oldcn);
    ddocDebug(3, "findCAChainForCN", "Read CA for CN: %s idx: %d rc: %d", oldcn, i, err);
    SET_LAST_ERROR_IF_NOT(err == ERR_OK, err);
    if (caCerts[i] && !err) {
      err = ddocCertGetIssuerCN(caCerts[i], &mbuf1);

      ddocDebug(3, "findCAChainForCN", "Issuer: %s old was: %s, rc: %d", 
		(const char*)mbuf1.pMem, oldcn, err);
      SET_LAST_ERROR_IF_NOT(err == ERR_OK, err);
    }
    if(!err && strcmp((const char*)mbuf1.pMem, oldcn)) { 
      i++;
      if (!err) 
	strncpy(oldcn, (const char*)mbuf1.pMem, sizeof(oldcn) ); 
      ddocMemBuf_free(&mbuf1);
    } else {
      ddocMemBuf_free(&mbuf1);
      break;
    }
  } while (err == ERR_OK);
  *nMaxChain = i;
  ddocDebug(3, "findCAChainForCN", "Found: %d certs", (*nMaxChain) + 1);
  // now reverse the chain such that the root CA is at the top - is it necessary???
  for(i = 0; i <= *nMaxChain; i++)
    ppChain[i] = caCerts[(*nMaxChain)-i];
  return err;
}

//------------------------------------------
// Get a notary confirmation for signature
// pSigDoc - signed document pointer
// pSigInfo - signature to notarize
// returns error code
//------------------------------------------
EXP_OPTION int notarizeSignature(SignedDoc* pSigDoc, SignatureInfo* pSigInfo)
{
  return notarizeSignatureWithIp(pSigDoc, pSigInfo, 0);
}

//------------------------------------------
// Selects correct OCSP URL for this certificate
// issuerDN - certificate issuer DN
// ppOcspUrl - returned OCSP url or NULL if not found
// returns error code or ERR_OK
//------------------------------------------
int ddocSelectOcspUrl(char* issuerDN, char** ppOcspUrl)
{
  int err = ERR_OK, i;
  char buf1[100], *p;

  RETURN_IF_NULL_PARAM(ppOcspUrl);
  // get default OCSP URL
  *ppOcspUrl = (char*)ConfigItem_lookup("DIGIDOC_OCSP_URL");
  // if possible find OCSP URL by signers CA
  ddocDebug(3, "ddocSelectOcspUrl", "signers CA: %s", (issuerDN ? issuerDN : "NULL"));
  if(issuerDN) {
    buf1[0] = 0;
    findCN(issuerDN, buf1);
    ddocDebug(3, "ddocSelectOcspUrl", "signers CA CN: %s", buf1);
    findResponderIndex(&i, NULL, NULL, buf1);
    if(i > 0) {
      snprintf(buf1, sizeof(buf1), "DIGIDOC_OCSP_RESPONDER_CERT_%d_URL", i);
      p = (char*)ConfigItem_lookup(buf1);
      if(p) {
	*ppOcspUrl = p;
	ddocDebug(3, "ddocSelectOcspUrl", "Selected OCSP URL: %s", p);
      }
    }
    buf1[0] = 0;
  }
  if(!(*ppOcspUrl)) {
    SET_LAST_ERROR(ERR_UNKNOWN_CA);	
	err = ERR_UNKNOWN_CA;
  }
  return err;
}

int ddocFindCaChainForCert(X509* pCert, 
				X509** caCerts, int *nCerts, 
				char* szCA, int caLen)
{
  int err = ERR_OK, i;
  DigiDocMemBuf mbuf1;

  RETURN_IF_NULL_PARAM(caCerts);
  RETURN_IF_NULL_PARAM(nCerts);
  RETURN_IF_NULL_PARAM(szCA);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  for(i = 0; i <= (*nCerts); i++)
    caCerts[i] = 0;
  szCA[0] = 0;
  err = ddocCertGetIssuerDN(pCert, &mbuf1);
  ddocDebug(3, "ddocFindCaChainForCert", "Find chain for: %s", (char*)mbuf1.pMem);
  if(!err) {
    err = findCN((char*)mbuf1.pMem, szCA);
    ddocDebug(3, "ddocFindCaChainForCert", "Find chain for CN: %s", szCA);
    if(!err) {
      err = findCAChainForCN(caCerts, nCerts, szCA);
      ddocDebug(3, "ddocFindCaChainForCert", "Chain length: %d, err: %d", *nCerts, err);
    }
  }
  ddocMemBuf_free(&mbuf1);
  return err;
}

int ddocFindOcspCnCaAndCerts(OCSP_RESPONSE* pResp, int *nType,
				X509** caCerts, int *nCerts, 
				char* szCN, int cnLen,
				char* szCA, int caLen)
{
  int err = ERR_OK, i;
  DigiDocMemBuf mbuf1;

  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(nType);
  RETURN_IF_NULL_PARAM(caCerts);
  RETURN_IF_NULL_PARAM(nCerts);
  RETURN_IF_NULL_PARAM(szCN);
  RETURN_IF_NULL_PARAM(szCA);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  for(i = 0; i <= (*nCerts); i++)
    caCerts[i] = 0;
  szCN[0] = szCA[0] = 0;
  // find responder id type and value
  ddocDebug(3, "ddocFindOcspCnCaAndCerts", "OCSP: %s", (pResp ? "OK" : "NO"));
  err = ddocGetOcspRespIdTypeAndValue(pResp, nType, &mbuf1);
  if((*nType) == RESPID_NAME_TYPE) {
    err = findCN((char*)mbuf1.pMem, szCN);
    if (!err) 
      err = findResponderCA(szCA, caLen, szCN, NULL);
  }
  else if((*nType) == RESPID_KEY_TYPE) {
    i = sizeof(szCN);
    encode((const byte*)mbuf1.pMem, mbuf1.nLen, (byte*)szCN, &i);
    if (!err) 
      err = findResponderCA(szCA, caLen, NULL, szCN);
  }
  else {
    SET_LAST_ERROR(ERR_OCSP_WRONG_RESPID);
    err = ERR_OCSP_WRONG_RESPID;
  }
  ddocDebug(3, "ddocFindOcspCnCaAndCerts", "Responder CN: %s CA: %s, RC: %d", szCN, szCA, err);
  // find CA chain
  if(err == ERR_OK)
    err = findCAChainForCN(caCerts, nCerts, szCA);
  ddocDebug(3, "ddocFindOcspCnCaAndCerts", "Chain length: %d, err: %d", *nCerts, err);
  // cleanup
  ddocMemBuf_free(&mbuf1);
  return err;
}

//------------------------------------------
// Get a notary confirmation for signature
// pSigDoc - signed document pointer
// pSigInfo - signature to notarize
// ip - callers ip address if known
// returns error code
//------------------------------------------
EXP_OPTION int notarizeSignatureWithIp(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, unsigned long ip)
{
  NotaryInfo* pNotInfo;
  int err = ERR_OK, nCAs = 10, i, j, err2 = 0, nType;
  X509* caCerts[10];
  X509* pNotCert;
	//AM 24.09.08 szCA size from 30 to 100 for Portuguese ID card
  char szCN[200], szCA[100];
  char *pkcs12file, *pkcs12passwd, *ocspUrl;
  char *proxyHost, *proxyPort;
  OCSP_RESPONSE *pResp = 0;

  for(i = 0; i <= nCAs; i++)
    caCerts[i] = 0;
  if(ConfigItem_lookup_bool("SIGN_OCSP", 1)) {
    pkcs12file = (char*)ConfigItem_lookup("DIGIDOC_PKCS_FILE");
    RETURN_IF_NOT(pkcs12file, ERR_OCSP_PKCS12_NO_FILE);
    pkcs12passwd = (char*)ConfigItem_lookup("DIGIDOC_PKCS_PASSWD");
    //RETURN_IF_NOT(pkcs12passwd, ERR_OCSP_PKCS12_NO_PASSWD);
  } else {
    pkcs12file = pkcs12passwd = NULL;
  }
  if(ConfigItem_lookup_bool("USE_PROXY", 1)) {
    proxyHost = (char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST");
    RETURN_IF_NOT(proxyHost, ERR_WRONG_URL_OR_PROXY);
    proxyPort = (char*)ConfigItem_lookup("DIGIDOC_PROXY_PORT");
    RETURN_IF_NOT(proxyPort, ERR_WRONG_URL_OR_PROXY);
    ddocDebug(4, "notarizeSignature", "proxy: %s port : %s", proxyHost, proxyPort);
  } else {
    proxyHost = proxyPort = NULL;
  }
  err = ddocSelectOcspUrl((char*)ddocSigInfo_GetSignersCert_IssuerName(pSigInfo), &ocspUrl);
  RETURN_IF_NOT(err == ERR_OK, err);
  // get OCSP confirmation
  ddocDebug(3, "notarizeSignatureWithIp", "Getting OCSP confirmation");
  err = getConfirmationWithIp(pSigDoc, pSigInfo, NULL, NULL,
			      pkcs12file, pkcs12passwd, ocspUrl,
			      proxyHost, proxyPort, ip);
  RETURN_IF_NOT(err == ERR_OK, err);	
  pNotInfo = getNotaryWithSigId(pSigDoc, pSigInfo->szId);
  RETURN_IF_NOT(pNotInfo != NULL, ERR_NOTARY_SIG_MATCH);
  // get OCSP response
  pResp = ddocNotInfo_GetOCSPResponse_Value(pNotInfo);
  RETURN_IF_NOT(pResp != NULL, ERR_NOTARY_SIG_MATCH);
  // find CN from responder-id, right CA and CA certs chain
  err = ddocFindOcspCnCaAndCerts(pResp, &nType, (X509**)caCerts, &nCAs, 
					szCN, sizeof(szCN), szCA, sizeof(szCA));
  // find usable responder cert by trying all of them
  // until verification with one succedes
  if(err == ERR_OK) {
    j = 0;
    do {
      pNotCert = 0;
      err2 = ERR_OK;
      if(nType == RESPID_NAME_TYPE)
		findResponderByCNAndHashAndIndex(&pNotCert, szCN, NULL, j);
      else if(nType == RESPID_KEY_TYPE)
		findResponderByCNAndHashAndIndex(&pNotCert, NULL, szCN, j);
      ddocDebug(1, "notarizeSignatureWithIp", "Find notary: %s idx: %d cert: %s", szCN, j, (pNotCert ? "OK" : "NULL"));
      if(pNotCert)
		err2 = finalizeAndVerifyNotary(pSigDoc, pSigInfo, 
	      pNotInfo, (const X509**)&caCerts, (const X509*)pNotCert);
      ddocDebug(1, "notarizeSignatureWithIp", "Verifying notary: %d", err);
      j++;
      if(err2 && pNotCert)
	    X509_free(pNotCert);
    } while(pNotCert && err2 != ERR_OK);
  }
  if(err2)
    err = err2;
  else
    clearErrors();  
  for(i = 0; i <= nCAs; i++)
    if(caCerts[i])
      X509_free(caCerts[i]);
  if(pResp)
	OCSP_RESPONSE_free(pResp);
  // please not that we cannot free pNotCert here because we gave ownership to pNotInf!
  return err;
}

//--------------------------------------------------
// Signs the document and gets configrmation
// pSigDoc - signed document pointer
// ppSigInfo - address of new signature pointer
// pin - smart card PIN
// manifest - manifest / resolution (NULL)
// city - signers city (NULL)
// state - signers state (NULL)
// zip - signers postal code (NULL)
// country - signers country (NULL)
//--------------------------------------------------
EXP_OPTION int signDocument(SignedDoc* pSigDoc, SignatureInfo** ppSigInfo,
		 const char* pin, const char* manifest,
		 const char* city, const char* state,
		 const char* zip, const char* country)
{
  int err = ERR_OK;
  SignatureInfo* pSigInfo = NULL;
  X509* pTsaCert = NULL;
  char buf1[300];

  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(ppSigInfo);
  clearErrors();

  ddocDebug(1, "signDocument", "Creating new digital signature");
  // add new signature with default id
  err = SignatureInfo_new(&pSigInfo, pSigDoc, NULL);
  RETURN_IF_NOT(err == ERR_OK, err);
  *ppSigInfo = pSigInfo;
  // automatically calculate doc-info elements for this signature
  addAllDocInfos(pSigDoc, *ppSigInfo);
  // add signature production place
  if (city || state || zip || country)
    err = setSignatureProductionPlace(*ppSigInfo, city, state, zip, country);
  // add user roles
  if (manifest)
    err = addSignerRole(*ppSigInfo, 0, manifest, -1, 0);
  // now sign the doc
  err = calculateSignatureWithEstID(pSigDoc, *ppSigInfo, 
				    ConfigItem_lookup_int("DIGIDOC_SIGNATURE_SLOT", 0), pin);
  RETURN_IF_NOT(err == ERR_OK, ERR_PKCS_SIGN_DATA);

#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
    if(ConfigItem_lookup("TSA_CERT")) {
#ifdef WIN32
      snprintf(buf1, sizeof(buf1), "%s\\%s", ConfigItem_lookup("CA_CERT_PATH"), ConfigItem_lookup("TSA_CERT"));
#else
      snprintf(buf1, sizeof(buf1), "%s/%s", ConfigItem_lookup("CA_CERT_PATH"), ConfigItem_lookup("TSA_CERT"));
#endif
      err = ReadCertificate(&pTsaCert, buf1);
      if(err) return err;
    }
    err = ddocGetSignatureTimeStamp(*ppSigInfo, &pTsaCert, ConfigItem_lookup("TSA_POLICY"), 
				    ConfigItem_lookup("TSA_URL"), 
				    ConfigItem_lookup("DIGIDOC_PROXY_HOST"), 
				    ConfigItem_lookup("DIGIDOC_PROXY_PORT"));
    if(pTsaCert)
      X509_free(pTsaCert);
    RETURN_IF_NOT(err == ERR_OK, err);
  }
#endif
  err = notarizeSignature(pSigDoc, *ppSigInfo);
  RETURN_IF_NOT(err == ERR_OK, err);
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
    if(ConfigItem_lookup("TSA_CERT")) {
#ifdef WIN32
      snprintf(buf1, sizeof(buf1), "%s\\%s", ConfigItem_lookup("CA_CERT_PATH"), ConfigItem_lookup("TSA_CERT"));
#else
      snprintf(buf1, sizeof(buf1), "%s/%s", ConfigItem_lookup("CA_CERT_PATH"), ConfigItem_lookup("TSA_CERT"));
#endif
      err = ReadCertificate(&pTsaCert, buf1);
      if(err) return err;
    }
    err = ddocGetSigAndRefsTimeStamp(pSigDoc, *ppSigInfo, &pTsaCert, ConfigItem_lookup("TSA_POLICY"), 
				     ConfigItem_lookup("TSA_URL"), 
				     ConfigItem_lookup("DIGIDOC_PROXY_HOST"), 
				     ConfigItem_lookup("DIGIDOC_PROXY_PORT"));
    RETURN_IF_NOT(err == ERR_OK, err);
  }
#endif
  return ERR_OK;
}

//--------------------------------------------------
// Verify this notary
// pSigDoc - signed document pointer
// pNotInfo - notary to verify
// returns error code
//--------------------------------------------------
int verifyNotary(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, NotaryInfo* pNotInfo)
{
  int err = ERR_OK, nCAs = 10, i;
  X509 * caCerts[10];
  X509* pNotCert = 0;
  const DigiDocMemBuf *pMBuf = 0;
  char buf1[300], buf2[300], szNotSerial[100], szTsaSerial[100];
#ifdef WITH_TS
  X509* pTsaCert = NULL;
  TimestampInfo *pTS;
  DigiDocMemBuf  mbuf1;
  time_t t1 = 0, t2 = 0, t0 = 0;
  int nMaxTimeErrSecs = 0;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
#endif

  for (i = 0; i < 10; i++)
    caCerts[i] = 0;
  // get responder certs serial nr
  szNotSerial[0] = 0;
  if(ddocSigInfo_GetOCSPRespondersCert(pSigInfo))
    err = ReadCertSerialNumber(szNotSerial, sizeof(szNotSerial), 
			       ddocSigInfo_GetOCSPRespondersCert(pSigInfo));
  // find responder cert and it's CA cert
  pMBuf = ddocNotInfo_GetResponderId(pNotInfo);
  RETURN_IF_NULL(pMBuf);
  if(pNotInfo->nRespIdType == RESPID_NAME_TYPE) {
    err = findCN((char*)pMBuf->pMem, buf1);
    ddocDebug(3, "verifyNotary", "Responder text: %s", buf1);
    if (!err) {
      err = findResponderCA(buf2, sizeof(buf2), buf1, NULL);
      ddocDebug(3, "verifyNotary", "Responder CA: %s", buf2);
    }
    if(!err) {
      err = findResponder(&pNotCert, buf1, NULL, (strlen(szNotSerial) ? szNotSerial : NULL));
      ddocDebug(3, "verifyNotary", "Responder %s cert: %s, err: %d", buf1, 
		(pNotCert ? "OK" : "NULL"), err);
    }
  }
  else if(pNotInfo->nRespIdType == RESPID_KEY_TYPE) {
    i = sizeof(buf1);
    encode((const byte*)pMBuf->pMem, pMBuf->nLen, (byte*)buf1, &i);
    ddocDebug(3, "verifyNotary", "Responder bin: %s", buf1);
    if (!err) {
      err = findResponderCA(buf2, sizeof(buf2), NULL, buf1);
      ddocDebug(3, "verifyNotary", "Responder CA: %s", buf2);
    }
    if(!err) {
      err = findResponder(&pNotCert, NULL, buf1, (strlen(szNotSerial) ? szNotSerial : NULL));
      ddocDebug(3, "verifyNotary", "Responder %s cert: %s, err: %d", buf1, 
		(pNotCert ? "OK" : "NULL"), err);
    }
  }
  else {
    SET_LAST_ERROR(ERR_OCSP_WRONG_RESPID);
    err = ERR_OCSP_WRONG_RESPID;
  }
  if (!err)
    err = findCAChainForCN((X509**)caCerts, &nCAs, buf2);
  
  // use specific error code for responders cert not found!
  if(err) {
    SET_LAST_ERROR(ERR_OCSP_RESP_NOT_TRUSTED);
    err = ERR_OCSP_RESP_NOT_TRUSTED;
  }
  ddocDebug(3, "verifyNotary", "Chain length: %d, err: %d", nCAs, err);
  if(pNotCert && !ddocSigInfo_GetOCSPRespondersCert(pSigInfo)) {
    err = ddocSigInfo_SetOCSPRespondersCert(pSigInfo, pNotCert);
    ddocDebug(3, "verifyNotary", "assigned notary cert because it was missing");
  }
  if (!err) {
    err = verifyNotaryInfoCERT(pSigDoc, pSigInfo, pNotInfo,
			       (const X509**)caCerts, ConfigItem_lookup("CA_CERT_PATH"),
			       pNotCert);
    //TODO: assign CA to Notary?
    ddocDebug(3, "verifyNotary", "Verifying Notary %s - %s", pNotInfo->szId, ((!err) ? "OK" : "ERROR"));
  }
  if (!err) {
    err = isCertSignedByCERT(ddocSigInfo_GetOCSPRespondersCert(pSigInfo), caCerts[nCAs]);
    ddocDebug(3, "verifyNotary", "\tCertificate trusted - %s", ((!err) ? "OK" : "ERROR"));
  }
  // verify notary digest
  if (!err) {
    err = verifyNotaryDigest(pSigDoc, pNotInfo);
    ddocDebug(3, "verifyNotary", "\tNotary digest - %s", ((!err) ? "OK" : "ERROR"));
  }
  for (i = 0; i < 10; i++) {
    if(caCerts[i]) {
      X509_free(caCerts[i]);
      caCerts[i] = 0;
    }
  }
#ifdef WITH_TS
  nMaxTimeErrSecs = ConfigItem_lookup_int("MAX_TSA_TIME_ERR_SECS", 1);
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
    err = ddocNotInfo_GetThisUpdate_timet(pNotInfo, &t0);
#ifdef WIN32
    snprintf(buf1, sizeof(buf1), "%s\\%s", ConfigItem_lookup("CA_CERT_PATH"), ConfigItem_lookup("TSA_CA_CERT"));
#else
    snprintf(buf1, sizeof(buf1), "%s/%s", ConfigItem_lookup("CA_CERT_PATH"), ConfigItem_lookup("TSA_CA_CERT"));
#endif
    ddocDebug(3, "verifyNotary", "TSA CA cert: %s", buf1);
    err = ReadCertificate(&(caCerts[0]), buf1);
    nCAs = 1;
    if(err) return err;
    // verify SignatureTimeStamp
    pTS = ddocSigInfo_GetTypestampOfType(pSigInfo, TS_TYPE_SIGNATURE);
    if(pTS) {
      err = ddocVerifyTimestampInfo(pTS, 
				      caCerts, (char*)ConfigItem_lookup("CA_CERT_PATH"), &t1, &pTsaCert);
	  // TODO: verfy existence
	  szTsaSerial[0] = 0;
      ReadCertSerialNumber(szTsaSerial, sizeof(szTsaSerial), pTsaCert);

	  if(pTsaCert)
		  X509_free(pTsaCert);
	  pTsaCert = NULL;
      if(err)
	SET_LAST_ERROR(err);
    }
    // verify SigAndRefsTimeStamp
    pTS = ddocSigInfo_GetTypestampOfType(pSigInfo, TS_TYPE_SIG_AND_REFS);
    if(pTS) {
      err = ddocVerifyTimestampInfo(pTS,  
	       caCerts, (char*)ConfigItem_lookup("CA_CERT_PATH"), &t2, &pTsaCert);
	  // TODO: verfy existence

	  if(pTsaCert)
		  X509_free(pTsaCert);
	  pTsaCert = NULL;
      if(err)
	    SET_LAST_ERROR(err);
    }
    // cleanup
    for (i = 0; i < nCAs; i++) {
      if(caCerts[i]) {
	X509_free(caCerts[i]);
	caCerts[i] = 0;
      }
    }
    // check timestamp dates
    if(t1 > t2) { // SignatureTimeStamp must be before SigAndRefsTimeStamp
      ddocDebug(1, "verifyNotary", "SigAndRefsTimeStamp: %ld was before SignatureTimeStamp: %ld", t2, t1);
      err = ERR_TS_VERIFY;
    }
    time_t2str(t0, buf1, sizeof(buf1));
    ddocDebug(3, "verifyNotary", "Responder time: %s", buf1);
    time_t2str(t1, buf2, sizeof(buf2));
    ddocDebug(3, "verifyNotary", "SignatureTimeStamp time: %s", buf2);
    time_t2str(t2, szNotSerial, sizeof(szNotSerial));
    ddocDebug(3, "verifyNotary", "SigAndRefsTimeStamp time: %s", szNotSerial);
    if(t0 < (t1-nMaxTimeErrSecs) || t1 > (t2+nMaxTimeErrSecs)) { // OCSP timestamp must be between SignatureTimeStamp and SigAndRefsTimeStamp
      ddocDebug(1, "verifyNotary", "SignatureTimeStamp: %s OCSP: %s SigAndRefsTimeStamp: %s", buf2, buf1, szNotSerial);
      err = ERR_TS_VERIFY;
    }
    
  }
#endif
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Verify this signature and it's notary
// pSigDoc - signed document pointer
// pSigInfo - signature to verify
// szFileName - input digidoc filename
// returns error code
//--------------------------------------------------
EXP_OPTION int verifySignatureAndNotary(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, const char* szFileName)
{
  int err = ERR_OK;
  X509* pCA = 0;
  NotaryInfo* pNotInfo;
  
  err = findCAForCertificate(&pCA, ddocSigInfo_GetSignersCert(pSigInfo));
  RETURN_IF_NOT(err == ERR_OK, err);
  err = verifySignatureInfoCERT(pSigDoc, pSigInfo, pCA, szFileName, 1, NULL, 0);
  if (err == ERR_OK) {
    pNotInfo = getNotaryWithSigId(pSigDoc, pSigInfo->szId);
    if (pNotInfo)
      err = verifyNotary(pSigDoc, pSigInfo, pNotInfo);
    else {
      ddocDebug(3, "verifySignatureAndNotary", "\tSignature has no OCSP confirmation!\n"); 
      SET_LAST_ERROR(ERR_NO_OCSP);
      err = ERR_NO_OCSP;
    }
  } else 
    SET_LAST_ERROR(err);
  if (pCA)
    X509_free(pCA);
  return err;
}

//--------------------------------------------------
// Extract common name from cert DN or responder id
// src - DN
// dest - buffer for CN
//--------------------------------------------------
int findCN(char* src, char* dest)
{
  char* p1, *p2;

  p1 = strstr(src, "CN=");
  if(p1) {
    p1 += 3;
    p2 = strchr(p1, ',');
    if(!p2)
      p2 = strchr(p1, '/');
    if(!p2)
      p2 = strchr(p1, 0);
    if(p2) {
      strncpy(dest, p1, (int)(p2-p1));
      dest[(int)(p2-p1)] = 0;
      return ERR_OK;
    }
  }
  SET_LAST_ERROR_RETURN_CODE(ERR_CERT_INVALID);
}

//------------------------------------------
// Verify certificate by OCSP
// pCert - certificate to check
// ppResp - address to return OCSP response. Use NULL if
// you don't want OCSP response to be returned
// returns error code
//------------------------------------------
EXP_OPTION int ddocVerifyCertByOCSP(X509* pCert, OCSP_RESPONSE **ppResp)
{
	return ddocVerifyCertByOCSPWithIp(pCert, ppResp, 0);
}

//------------------------------------------
// Verify certificate by OCSP
// pCert - certificate to check
// ppResp - address to return OCSP response. Use NULL if
// you don't want OCSP response to be returned
// returns error code
//------------------------------------------
EXP_OPTION int ddocVerifyCertByOCSPWithIp(X509* pCert, OCSP_RESPONSE **ppResp, unsigned long ip)
{
  int err = ERR_OK, nCAs = 10, i, j, err2, nType;
  X509 * caCerts[10];
  X509 *pNotCert = 0;
  char *pkcs12file, *pkcs12passwd;
  char *proxyHost, *proxyPort;
  char szCN[100], szCA[100], *ocspUrl;
  OCSP_RESPONSE *pResp = NULL;
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  // check if OCSP must be signed
  if(ConfigItem_lookup_bool("SIGN_OCSP", 1)) {
    pkcs12file = (char*)ConfigItem_lookup("DIGIDOC_PKCS_FILE");
    RETURN_IF_NOT(pkcs12file, ERR_OCSP_PKCS12_NO_FILE);
    pkcs12passwd = (char*)ConfigItem_lookup("DIGIDOC_PKCS_PASSWD");
    RETURN_IF_NOT(pkcs12passwd, ERR_OCSP_PKCS12_NO_PASSWD);
  } else {
    pkcs12file = pkcs12passwd = NULL;
  }
  // check proxy usage
  if(ConfigItem_lookup_bool("USE_PROXY", 1)) {
    proxyHost = (char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST");
    RETURN_IF_NOT(proxyHost, ERR_WRONG_URL_OR_PROXY);
    proxyPort = (char*)ConfigItem_lookup("DIGIDOC_PROXY_PORT");
    RETURN_IF_NOT(proxyPort, ERR_WRONG_URL_OR_PROXY);
    ddocDebug(4, "ddocVerifyCertByOCSPWithIp", "proxy: %s port : %s", proxyHost, proxyPort);
  } else {
    proxyHost = proxyPort = NULL;
  }
  // send OCSP request and don't verify result immediately
  err = ddocCertGetIssuerDN(pCert, &mbuf1);
  RETURN_IF_NOT(err == ERR_OK, err);
  err = ddocSelectOcspUrl((char*)mbuf1.pMem, &ocspUrl);
  ddocMemBuf_free(&mbuf1);
  RETURN_IF_NOT(err == ERR_OK, err);
  // find cert oweners CA cert chain
  err = ddocFindCaChainForCert(pCert, (X509**)caCerts, &nCAs, szCA, sizeof(szCA));
  // if possible find OCSP URL by signers CA
  ddocDebug(3, "ddocVerifyCertByOCSPWithIp", "OCSP URL: %s CA: %s chain: %d", 
	(ocspUrl ? ocspUrl : "NULL"), szCA, nCAs);
  err = verifyCertificateByOCSPWithIp(pCert, NULL, NULL, ocspUrl,
				  proxyHost, proxyPort, pkcs12file, pkcs12passwd, &pResp, ip);
  ddocDebug(3, "ddocVerifyCertByOCSPWithIp", "OCSP verification - RC: %d, resp: %s", err, (pResp ? "OK" : "NULL"));
  // release cert CA chain
  for(i = 0; i <= nCAs; i++) {
    if(caCerts[i]) {
      X509_free(caCerts[i]);
      caCerts[i] = 0;
    }
  }
  // verify OCSP response
  // find CN from responder-id, right CA and CA certs chain
  if(!err)
    err = ddocFindOcspCnCaAndCerts(pResp, &nType, (X509**)caCerts, &nCAs, 
					szCN, sizeof(szCN), szCA, sizeof(szCA));
  // find usable responder cert by trying all of them
  // until verification with one succedes
  if(err == ERR_OK) {
    j = 0;
    do {
      pNotCert = 0;
      err2 = ERR_OK;
      if(nType == RESPID_NAME_TYPE)
	findResponderByCNAndHashAndIndex(&pNotCert, szCN, NULL, j);
      else if(nType == RESPID_KEY_TYPE)
	findResponderByCNAndHashAndIndex(&pNotCert, NULL, szCN, j);
      ddocDebug(1, "ddocVerifyCertByOCSPWithIp", "Find notary: %s idx: %d cert: %s", szCN, j, (pNotCert ? "OK" : "NULL"));
      if(pNotCert)
	err2 = verifyOCSPResponse(pResp, (const X509**)&caCerts, 
					NULL, (const X509*)pNotCert);
	ddocDebug(1, "ddocVerifyCertByOCSPWithIp", "Verifying notary: %d", err);
      j++;
      if(pNotCert)
	    X509_free(pNotCert);
    } while(pNotCert && err2 != ERR_OK);
  }
  if(err2)
    err = err2;
  else
    clearErrors();
  for(i = 0; i <= nCAs; i++)
    if(caCerts[i])
      X509_free(caCerts[i]);
  // return OCSP response if requested
  if(pResp) {
    if(ppResp)
	*ppResp = pResp;
    else
	OCSP_RESPONSE_free(pResp);
  }
  return err;
}

//------------------------------------------
// Reads an arbitrary file into memory buffer
// szFileName - file name and path
// pData - memory buffer object
// returns error code
//------------------------------------------
EXP_OPTION int ddocReadFile(const char* szFileName, DigiDocMemBuf* pData)
{
  int err = ERR_OK;
  long l1;
  FILE *hFile;
  char buf1[2050];

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(pData);
  pData->pMem = 0;
  pData->nLen = 0;
  hFile = fopen(szFileName, "rb");
  RETURN_IF_NOT(hFile, ERR_FILE_READ);
  do {
    l1 = fread(buf1, 1, 2048, hFile);
    err = ddocMemAppendData(pData, buf1, (long)l1);
  } while(l1 == 2048 && !err);
  fclose(hFile);
  return err;
}

#ifdef WITH_MSSP

//------------------------------------------
// Gets MSSP session status and returns status code
// If you pass in a digidoc then the last signature
// will be finalized with signature value if
// available or removed in case of session error,
// timeout or users cancelling signature operation
// pMssp - MSSP context
// pSigDoc - signed document object to be modified
// pMBufSig - buffer for returning signature value [optional]
// returns MSSP session status code
//------------------------------------------
EXP_OPTION int ddocConfMsspGetStatus(MSSP* pMssp, SignedDoc* pSigDoc, DigiDocMemBuf* pMBufSig)
{
  int err = ERR_OK;
  SignatureInfo* pSigInfo = NULL;
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  ddocDebug(3, "ddocConfMsspGetStatus", "Get MSSP status for: %ld", pMssp->lMsspTxnId);
  RETURN_IF_NULL_PARAM(pMssp);
  clearErrors();

  // send signature request
  err = ddocMsspStatusReq(pMssp, &mbuf1);
  // disconnect ???
  ddocMsspCleanup(pMssp);

  ddocDebug(3, "ddocConfMsspGetStatus", "Txn: %ld err: %ld sig-len: %d", 
	    pMssp->lMsspTxnId, err, mbuf1.nLen);

  // use signature value if we got it
  if(pSigDoc) {
      pSigInfo = ddocGetLastSignature(pSigDoc);
      if(pSigInfo) {
	if(err) {
	  // error ocurred in status req -> delete signature
	  ddocDebug(3, "ddocConfMsspGetStatus", "Error: %d removing signature: %s", err, pSigInfo->szId);
	  err = SignatureInfo_delete(pSigDoc, pSigInfo->szId);
	} else {
	  // in case of error, timeout or users cancelling
	  // delete the last incomplete signature
	  if(pMssp->nStatusCode == INTERNAL_ERROR ||
	     pMssp->nStatusCode == USER_CANCEL ||
	     pMssp->nStatusCode == EXPIRED_TRANSACTION) {
	    ddocDebug(3, "ddocConfMsspGetStatus", "MSSP status: %d removing signature: %s", pMssp->nStatusCode, pSigInfo->szId);
	    err = SignatureInfo_delete(pSigDoc, pSigInfo->szId);
	  }
	  if(pMssp->nStatusCode == SIGNATURE && mbuf1.pMem && mbuf1.nLen) {
	    ddocDebug(3, "ddocConfMsspGetStatus", "MSSP status: %d finalizing signature: %s sig len: %d", pMssp->nStatusCode, pSigInfo->szId, mbuf1.nLen);
	    err = setSignatureValue(pSigInfo, (byte*)mbuf1.pMem, mbuf1.nLen);
	    // if everything is ok the get OCSP confirmation
	    if(!err)
	      err = notarizeSignature(pSigDoc, pSigInfo);
	  }
	  if(pMssp->nStatusCode == OUTSTANDING_TRANSACTION) {
	    ddocDebug(3, "ddocConfMsspGetStatus", "MSSP status: %d signature: %s must wait", pMssp->nStatusCode, pSigInfo->szId);
	  }
	}
      }
  }
  if(pMBufSig && mbuf1.pMem)
    ddocMemAssignData(pMBufSig, (const char*)mbuf1.pMem, mbuf1.nLen);
  // cleanup
  ddocMemBuf_free(&mbuf1);

  // we treat SOAP statuses not as errors since request was successfull
  if(!err && (pMssp->nStatusCode == INTERNAL_ERROR || pMssp->nStatusCode == USER_CANCEL ||
	      pMssp->nStatusCode == EXPIRED_TRANSACTION || pMssp->nStatusCode == SIGNATURE ||
	      pMssp->nStatusCode == OUTSTANDING_TRANSACTION))
    err = pMssp->nStatusCode;
  ddocDebug(3, "ddocConfMsspGetStatus", "Txn: %ld status: %d err: %d", pMssp->lMsspTxnId, pMssp->nStatusCode, err);
  return err;
}

//--------------------------------------------------
// Signs the document and gets return status back
// pSigDoc - signed document object
// pMssp - MSSP context
// szPhoneNo - users phone number
// manifest - manifest or role
// city - signers address , city
// state - signers address , state or province
// zip - signers address , postal code
// country - signers address , country name
// szDigiDocFile - name of the file user signs
// szCertUrl - full url where to get cert. Use NULL for default url
// pMBufHash - mem-buf to return <SignedInfo>. Optional
// return returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocConfMsspSign(SignedDoc* pSigDoc, MSSP* pMssp, const char* szPhoneNo,
				const char* manifest, const char* city, 
				const char* state, const char* zip, 
				const char* country, const char* szDigiDocFile,
				const char* szCertUrl, DigiDocMemBuf* pMBufHash)
{
  int err = ERR_OK, l1, l2;
  SignatureInfo* pSigInfo = NULL;
  X509* pCert = NULL;
  char buf1[50], buf2[50], *szDesc = NULL;

  ddocDebug(3, "ddocConfMsspSign", "Creating WPKI signature using: %s ap-txn: %ld, madr: %ld", 
	    szPhoneNo, pMssp->lApTxnId, (long int)pMssp);
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(szPhoneNo);
  // get clients certificate
  err = ddocMsspReadCertificate(szPhoneNo, szCertUrl, &pCert, 0);
  // this certificate will nto be released since new signature takes ownership!
  RETURN_IF_NOT(err == ERR_OK, err);
  // prepare signature
  err = ddocPrepareSignature(pSigDoc, &pSigInfo,
	   manifest, city, state, zip, country, pCert, NULL);
  // base64 encode final hash
  l1 = sizeof(buf1);
  err = ddocGetSignedHash(pSigInfo, buf1, &l1, 0, 1);
  RETURN_IF_NOT(err == ERR_OK, err);
  if(pMBufHash && l1 >= 35)
    ddocMemAssignData(pMBufHash, (const char*)buf1 + 15, 20);
  // debug output
  l2 = sizeof(buf2);
  encode((const byte*)buf1, l1, (byte*)buf2, &l2);
  ddocDebug(3, "ddocConfMsspSign", "signing hash %d with asn1 %s b64len: %d ap-txn: %ld, madr: %ld", 
	    l1, buf2, l2, pMssp->lApTxnId, (long int)pMssp);
  // set data to be displayed
  if(manifest && strlen(manifest)) {
    szDesc = (char*)manifest;
  } else if(szDigiDocFile && strlen(szDigiDocFile)) {
    szDesc = (char*)szDigiDocFile;
  } else {
    szDesc = "digidoc document";
  }
  // send signature request
  err = ddocMsspSignatureReq(pMssp, szPhoneNo,
			     (const char*)buf1, l1, szDesc);
  // report on success
  ddocDebug(3, "ddocConfMsspSign", "SignatureReq id: %s MSSPTransId: %ld Ap-txn: %ld rc: %d", 
	    pSigInfo->szId, pMssp->lMsspTxnId, pMssp->lApTxnId, err);
  if(err)
    ddocDebug(3, "ddocConfMsspSign", "fault: %s detail: %s",
	      (err ? ddocGetSoapFaultString(pMssp) : ""),
	      (err ? ddocGetSoapFaultDetail(pMssp) : ""));
  // disconnect ???
  ddocMsspCleanup(pMssp);

  RETURN_IF_NOT(err == ERR_OK, err);
  return ERR_OK;
}

//--------------------------------------------------
// Polls MSSP status and finalizes signature
// pSigDoc - signed document object
// pMssp - MSSP context
// return error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocConfMsspPoll(SignedDoc* pSigDoc, MSSP* pMssp)
{
  int err = ERR_OK, nPollFreq = 0;

  nPollFreq = ConfigItem_lookup_int("DIGIDOC_MSSP_STATUS_POLL_FREQ", 0);
  ddocDebug(3, "ddocConfMsspPoll", "Polling WPKI status poll freq: %d", nPollFreq);

  if(nPollFreq) {
    // now check status
    do {
      //sleep(nPollFreq); ????
      //err = cmdMsspStatus(pSigDoc, pMssp->lMsspTxnId);
    } while(!err && pMssp->nStatusCode == OUTSTANDING_TRANSACTION);
  }

  return err;
}

//--------------------------------------------------
// Calculates ChallengeID from 20 byte hash or challenge code
// pChallenge - challenge pointer (binary data)
// nLen - length of challenge data
// return error code or ERR_OK
//--------------------------------------------------
EXP_OPTION unsigned short ddocConfCalcChallengeID(const char* pChallenge, int nLen)
{
  if(pChallenge && nLen >= 20) 
    return ((0xFC & pChallenge[0]) << 5) | (pChallenge[19] & 0x7F);
  else
    return 0;
}

#endif // WITH_MSSP



