;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  
  # define installer name
  Name "Eneboo 2.4 (rev7-6) Installer"
  outFile "eneboo-v2.4-rev7-6-win32-installer.exe"
  
  # SetCompressor zlib          # Faster
  SetCompressor /SOLID lzma # Better
  
  
  # set PROGRAMFILES as install directory
  InstallDir $PROGRAMFILES\eneboo-2.4

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Eneboo-2.4" ""                                      

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\Eneboo-2.4" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "eneboo/COPYING.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "Catalan"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL
;--------------------------------
;Installer Sections

Section "Eneboo 2.4" SecDummy

    SetOutPath "$INSTDIR"
  
    # specify file to go in output path
    file /r eneboo\*.*
    
  
    # create a shortcut named "new shortcut" in the start menu programs directory
    # point the new shortcut at the program uninstaller
    CreateDirectory "$SMPROGRAMS\Eneboo 2.4"
    CreateShortCut "$SMPROGRAMS\Eneboo 2.4\Eneboo.lnk" "$INSTDIR\bin\eneboo.exe"
  
    ;Store installation folder
    WriteRegStr HKCU "Software\Eneboo-2.4" "" $INSTDIR
    
    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd  

;--------------------------------
;Installer Functions

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd
  
;--------------------------------
;Descriptions

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC

  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} "Eneboo 2.4 rev7-6 Program"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\Uninstall.exe"

  # second, remove the link from the start menu
  delete "$SMPROGRAMS\Eneboo 2.4\Abanq.lnk" 
  rmdir "$SMPROGRAMS\Eneboo 2.4"

  # now delete installed files
  rmdir /r $INSTDIR\bin
  rmdir /r $INSTDIR\lib
  rmdir /r $INSTDIR\plugins
  rmdir /r $INSTDIR\share
  rmdir /r $INSTDIR\pics
  delete $INSTDIR\COPYING.txt
  rmdir $INSTDIR

  DeleteRegKey /ifempty HKCU "Software\Eneboo-2.4"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd