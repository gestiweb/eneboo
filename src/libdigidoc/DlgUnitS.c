//==================================================
// FILE:	DlgUnitS.c
// PROJECT: Digi Doc
// DESCRIPTION: Low-level built dialog with list selection capability
//              Simple dialog
// 09/10/2003   Changed by AA
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
//==================================================

#include <stdio.h>
#include <windows.h>
//#include <libdigidoc/DigiDocDefs.h>
//Control ID's
#define IDD_DLG_UNIT                    1000
#define IDC_GROUPBOX                    1001
#define IDC_LISTVIEW                    1002  

//min and max sizes for dialog window
#define dDLGWIN_MINWIDTH     170
#define dDLGWIN_MAXWIDTH     800
#define dDLGWIN_MINHEIGHT    150
#define dDLGWIN_MAXHEIGHT    600

//maximum items in the listbox
//Developer can change the value
#define dDLGMAXLISTBOXSIZE  1024
//maximum item length
//Developer can change the value
#define dDLGMAXITEMSIZE      512

//temporary messagebox buffer len
#define dDLGTEMPBUFLEN        64
#define dDLGNOSELECTION       -2
#define dDLGERROR             -1

static BOOL CALLBACK WndDlgUnitProc(HWND, UINT, WPARAM, LPARAM);
static void CalculateDialogSize(HWND hDlg);
static void SetDialogItemSize(HWND hWnd, int iWOffs, int iHOffs);
static void SetDialogItemPosition(HWND hWnd, int iWOffs, int iHOffs);
static void SetDialogItemPosition_Old(HWND hWnd, int iWOffs, int iHOffs);
static void CenterDialogOnScreen(HWND hDlg, HWND hMain);
static void Dialog_End(HWND hDlg, BOOL fCancel);
static void Dialog_Init(HWND hDlg);
static void DispError(char *psError);
static LPWORD  lpwAlign (LPWORD lpIn);
static BOOL    Dialog_Create(void);


static HGLOBAL hgbl;                  //Dialog template
static char *psDlgCaption=NULL;       //Dialog Title
static char *psGrpHeader=NULL;        //Groupbox Header
static char *psOkButtonText=NULL;     //Ok button label
static char *psCancelButtonText=NULL; //Cancel button label
static char **psListItems=NULL;       //Listbox items
static int  iDlgWidth  = 800;
static int  iDlgHeight = 170;
//
//Dialog main function
//
//Limitations:
//  Max. items in the listbox       dDLGMAXLISTBOXSIZE
//  Max. item size in the listbox   dDLGMAXITEMSIZE
//  Dialogbox sizes  dDLGWIN_* values
//
//No dynamic memory allocation
//
//Selection is accepted if, list item is selected and
// - user pressed "Ok" button
// - user double-clicks on the listbox item
// - user pressed <Enter> key
//
//If no items in the psList or psList is NULL,
//then Ok button is disabled
//
//psList         - pointer arrays of listbox elements
//                 last item must be NULL
//psCaption      - dialog caption/title
//                 if NULL, then no caption
//psHeader       - listbox header
//                 if NULL, then no header
//psOkButton     - Ok button label
//                 if NULL or zero length string, then "&Ok"
//psCancelButton - Cancel button label
//                 if NULL or zero length string, then "&Cancel"
//iWinWidth      - dialogbox width, if 0 then default size
//
//iWinHeight     - dialogbox height, if 0 then default size
//
//returns -1 (dDLGNOSELECTION) if listbox item not selected (Cancel button)
//       >=0  selected item index in the psList array
//
//
int RunDialogUnitSimple(char *psList[], char *psCaption, char *psHeader, char *psOkButton, char *psCancelButton, int iWinWidth, int iWinHeight)
{
char  sTemp[dDLGTEMPBUFLEN];
DWORD dwRes;
int   iRes = dDLGNOSELECTION;
//Set global data
psDlgCaption = psCaption;
psGrpHeader = psHeader;
psOkButtonText = psOkButton;
psCancelButtonText = psCancelButton;
iDlgWidth  = iWinWidth;
iDlgHeight = iWinHeight;
//if not allowed value, then default size
if (iDlgWidth < dDLGWIN_MINWIDTH || iDlgWidth > dDLGWIN_MAXWIDTH)
  iDlgWidth = 0;
//if not allowed value, then default size
if (iDlgHeight < dDLGWIN_MINHEIGHT || iDlgHeight > dDLGWIN_MAXHEIGHT)
  iDlgHeight = 0;
//if not specified, then empty string
if (psDlgCaption == NULL)
  psDlgCaption = "";
//if not specified, then empty string
if (psGrpHeader == NULL)
  psGrpHeader = "";
//if not specified, then Ok string
if (psOkButtonText == NULL || lstrlen(psOkButtonText) == 0)
  psOkButtonText = "&Ok";
//if not specified, then Ok string
if (psCancelButtonText == NULL || lstrlen(psCancelButtonText) == 0)
  psCancelButtonText = "&Cancel";
//set items
psListItems = psList;
//Starts dialog, return value from function Dialog_End

if (Dialog_Create() == TRUE)
  iRes = DialogBoxIndirect(NULL, (LPDLGTEMPLATE) hgbl, GetActiveWindow(), (DLGPROC) WndDlgUnitProc);
else
  iRes = dDLGERROR;
//iRes = DialogBox(NULL, MAKEINTRESOURCE(IDD_DLG_UNIT), NULL,(DLGPROC)WndDlgUnitProc);
//if GetLastError returns nonzero, then dialog opening error
if (iRes == dDLGERROR)
  {
  dwRes = GetLastError();
  if (dwRes != 0)
    {
    //prints Windows errorcode
	  _snprintf(sTemp, sizeof(sTemp), "Windows Error=%d",dwRes);
    DispError(sTemp);
    }
  }
if (iRes == dDLGNOSELECTION)
  iRes = dDLGERROR;
if (hgbl != NULL)
  GlobalFree(hgbl);
return(iRes);
}


//Dialog callback functions
static BOOL CALLBACK WndDlgUnitProc(HWND  hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
BOOL fRes = FALSE;
DWORD dwNotifyCode;
switch (message)
  {
  case WM_INITDIALOG:
    //dialog init
    Dialog_Init(hDlg);
    fRes = TRUE;
    break;
  case WM_CLOSE:
    //user closes dialog from the system menu
    Dialog_End(hDlg, TRUE);
    fRes = TRUE;
    break;
  case WM_COMMAND:
    {
    dwNotifyCode = HIWORD(wParam);
    //user presses dblclick in the listbox
    if (dwNotifyCode == LBN_DBLCLK)
      {
      Dialog_End(hDlg,FALSE);
      fRes = TRUE;
      break;
      }
    switch (wParam)
      {
      case IDOK:
        //user pressed OK button
        Dialog_End(hDlg,FALSE);
        fRes = TRUE;
        break;
      case IDCANCEL:
        //user pressed Cancel button
        Dialog_End(hDlg, TRUE);
        fRes = TRUE;
        break;
      }
    }
  }
return(fRes);
}

//Dialog end function
//fCancel - TRUE, if user pressed Cancel button
//
static void Dialog_End(HWND hDlg, BOOL fCancel)
{
int iRes = dDLGNOSELECTION;
int iSelection;
//if TRUE, then always returns -1 (dDLGNOSELECTION)
if (fCancel == FALSE)
  {
  //if listbox item selected, then retuns selected item index
  iSelection = SendMessage(GetDlgItem(hDlg,IDC_LISTVIEW), LB_GETCURSEL,0,0);
  if (iSelection != LB_ERR)
    iRes = iSelection;
  }
EndDialog(hDlg, iRes);
}


//Dialog init
//
static void Dialog_Init(HWND hDlg)
{
int  iItems = 0;
BOOL fError = FALSE;
HWND hWndLst = GetDlgItem(hDlg,IDC_LISTVIEW);
//Set dialog caption
SetWindowText(hDlg, (LPTSTR)psDlgCaption);
//Set listbox header
SetDlgItemText(hDlg,IDC_GROUPBOX,(LPTSTR)psGrpHeader);
//Set Ok button label
SetDlgItemText(hDlg,IDOK,(LPTSTR)psOkButtonText);
//Set Cancel button label
SetDlgItemText(hDlg,IDCANCEL,(LPTSTR)psCancelButtonText);
//Calculate dialog size
CalculateDialogSize(hDlg);
//Center dialog
CenterDialogOnScreen(hDlg,NULL);
//Deletes listbox
SendMessage(hWndLst, LB_RESETCONTENT, 0, 0);
//Set hor scrollbar
SendMessage(hWndLst, LB_SETHORIZONTALEXTENT, 1200,0);
//Add items to the listbox
while (psListItems != NULL && *psListItems != NULL && iItems < dDLGMAXLISTBOXSIZE)
  {
  if (lstrlen(*psListItems) < dDLGMAXITEMSIZE)
    {
    SendMessage(hWndLst, LB_ADDSTRING, 0, (LONG) (LPTSTR)*psListItems);
    ++iItems;
    }
  else
    fError = TRUE;
  ++psListItems;
  }
if (fError == TRUE)
  DispError("Listbox item error");
//if at least one item, then select first item
if (iItems > 0)
  {
  EnableWindow(GetDlgItem(hDlg, IDOK),TRUE);
  SendMessage(hWndLst, LB_SETCURSEL, 0, (LONG) 0);
  }
else
  EnableWindow(GetDlgItem(hDlg, IDOK),FALSE);
}

//Calculate dialog size
//
static void CalculateDialogSize(HWND hDlg)
{

RECT  rcDlg;              // Dialog window rect
int   iWOffs, iHOffs;     // Offsets
if (iDlgWidth > 0 && iDlgHeight > 0)
  {
  GetWindowRect(hDlg, &rcDlg);
  iWOffs  = iDlgWidth - (rcDlg.right  - rcDlg.left);
  iHOffs  = iDlgHeight - (rcDlg.bottom - rcDlg.top);
  SetDialogItemSize(hDlg,iWOffs,iHOffs);
  SetDialogItemSize(GetDlgItem(hDlg,IDC_LISTVIEW),iWOffs,iHOffs);
  SetDialogItemSize(GetDlgItem(hDlg,IDC_GROUPBOX),iWOffs,iHOffs);
  SetDialogItemPosition(GetDlgItem(hDlg,IDOK),iWOffs,iHOffs);
  SetDialogItemPosition(GetDlgItem(hDlg,IDCANCEL),iWOffs,iHOffs);
  }
}

//Set dialog item size
//
static void SetDialogItemSize(HWND hWnd, int iWOffs, int iHOffs)
{
RECT  rcItm;              // Dialog window rect
int   iWidth, iHeight;    // Width and height
GetWindowRect(hWnd, &rcItm);
iWidth  = rcItm.right  - rcItm.left;
iHeight = rcItm.bottom - rcItm.top;
SetWindowPos(hWnd,NULL,0,0,iWidth+iWOffs, iHeight+iHOffs,SWP_NOMOVE|SWP_NOZORDER);
}

//Set dialog item pos
//Not in use
static void SetDialogItemPosition_Old(HWND hWnd, int iWOffs, int iHOffs)
{
RECT  rcItm;              // Dialog window rect
int   iLeft, iTop;        // Left and top
int   iWidth, iHeight;    // Width and height
GetWindowRect(hWnd, &rcItm);
iWidth = rcItm.right - rcItm.left;
iHeight= rcItm.bottom - rcItm.top;
iLeft  = rcItm.left+iWOffs;
iTop   = rcItm.top+iHOffs-20;
SetWindowPos(hWnd,NULL,iLeft,iTop,iWidth,iHeight,SWP_NOZORDER);
}

//Set dialog item pos
//
static void SetDialogItemPosition(HWND hWnd, int iWOffs, int iHOffs)
{
RECT  rcItm;              // Dialog item rect
RECT  rcParent;           // Parent item rect
int   iLeft, iTop;        // Left and top
// GetWindowRect retrieves screen coordinates
GetWindowRect(hWnd, &rcItm);
//Changed by A.Amenberg 27.06.2003
//GetAncestor supported from Win98 and WinNT SP4
//GetWindowRect(GetAncestor(hWnd, GA_PARENT), &rcParent);
GetWindowRect(GetParent(hWnd), &rcParent);
// for SetWindowPos we need left and right on PARENT.
iLeft  = (rcItm.left - rcParent.left) + iWOffs;
iTop   = (rcItm.top - rcParent.top) + iHOffs - 20; // subtract titlebar height
// move, don't resize
SetWindowPos(hWnd, NULL, iLeft, iTop, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
}

//Center dialog
//hMain - parent window, if NULL, then center in desktop
//
static void CenterDialogOnScreen(HWND hDlg, HWND hMain)
{
RECT  rcParent;                         // Parent window client rect
RECT  rcDlg;                            // Dialog window rect
int   iLeft, iTop;                      // Top-left coordinates
int   iWidth, iHeight;                  // Width and height
HWND  hwnd;
// Get frame window client rect in screen coordinates
if (hMain == NULL)
  hwnd = GetDesktopWindow();
else
  hwnd = hMain;
GetWindowRect(hwnd, &rcParent);
// Determine the top-left point for the dialog to be centered
GetWindowRect(hDlg, &rcDlg);
iWidth  = rcDlg.right  - rcDlg.left;
iHeight = rcDlg.bottom - rcDlg.top;
iLeft   = rcParent.left + (((rcParent.right  - rcParent.left) - iWidth ) / 2);
iTop    = rcParent.top  + (((rcParent.bottom - rcParent.top ) - iHeight) / 2);
if (iLeft < 0)
  iLeft = 0;
if (iTop  < 0)
  iTop  = 0;
// Place the dialog
MoveWindow(hDlg, iLeft, iTop, iWidth, iHeight, TRUE);
}

//Error messagebox
//
static void DispError(char *psError)
{
MessageBox(NULL,psError, "Dialog Unit Error",MB_OK|MB_SYSTEMMODAL|MB_ICONERROR);
}



//Helper routine.  Take an input pointer, return closest
//pointer that is aligned on a DWORD (4 byte) boundary.
//
static LPWORD lpwAlign (LPWORD lpIn)
{
ULONG ul;
ul = (ULONG) lpIn;
ul +=3;
ul >>=2;
ul <<=2;
return((LPWORD)ul);
}


static BOOL Dialog_Create(void)
{
BOOL fRes = FALSE;
LPDLGTEMPLATE lpdt;
LPDLGITEMTEMPLATE lpdit;
LPWORD lpw;
LPWSTR lpwsz;
int nchar;
hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
if (hgbl == NULL)
  return(fRes);
lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);
// Define a dialog box.
lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | DS_SETFONT;
lpdt->cdit = 4;  // number of controls
lpdt->x  = 0;  lpdt->y  = 0;
lpdt->cx = 205; lpdt->cy = 204;
lpw = (LPWORD) (lpdt + 1);
*lpw++ = 0;       // no menu
*lpw++ = 0;       // predefined dialog box class (by default)
*lpw++ = 0;       // no caption text
*lpw++ = 0x0008;  // font size
lpwsz = (LPWSTR) lpw;
nchar = 1+ MultiByteToWideChar (CP_ACP, 0, "MS Sans Serif", -1, lpwsz, 50);
lpw   += nchar;

// Define an OK button.
lpw = lpwAlign (lpw);
lpdit = (LPDLGITEMTEMPLATE) lpw;
lpdit->x  = 152; lpdit->y  = 182;
lpdit->cx = 50; lpdit->cy = 14;
lpdit->id = IDOK;  // OK button identifier
lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP;
lpw = (LPWORD) (lpdit + 1);
*lpw++ = 0xFFFF;
*lpw++ = 0x0080;    // button class
*lpw++ = 0x0000;    // no caption
*lpw++ = 0;         // no creation data
// Define a Cancel button.
lpw = lpwAlign (lpw);
lpdit = (LPDLGITEMTEMPLATE) lpw;
lpdit->x  = 97; lpdit->y  = 182;
lpdit->cx = 50; lpdit->cy = 14;
lpdit->id = IDCANCEL; //Cancel button identifier
lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP;
lpw = (LPWORD) (lpdit + 1);
*lpw++ = 0xFFFF;
*lpw++ = 0x0080;    // button class atom
*lpw++ = 0x0000;    // no caption
*lpw++ = 0;         // no creation data
// Define a groupbox control.
lpw = lpwAlign (lpw);
lpdit = (LPDLGITEMTEMPLATE) lpw;
lpdit->x  = 0; lpdit->y  = 2;
lpdit->cx = 205; lpdit->cy = 173;
lpdit->id = IDC_GROUPBOX; // groupbox identifier
lpdit->style = BS_GROUPBOX | WS_VISIBLE;
lpw = (LPWORD) (lpdit + 1);
*lpw++ = 0xFFFF;
*lpw++ = 0x0080;   // listbox
*lpw++ = 0x0000;   // no caption
*lpw++ = 0;                      // no creation data
// Define a listbox control.
lpw = lpwAlign (lpw);
lpdit = (LPDLGITEMTEMPLATE) lpw;
lpdit->x  = 4; lpdit->y  = 12;
lpdit->cx = 197; lpdit->cy = 158;
lpdit->id = IDC_LISTVIEW; // listbox identifier
lpdit->style = LBS_USETABSTOPS | LBS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LBS_NOINTEGRALHEIGHT | WS_BORDER;
lpw = (LPWORD) (lpdit + 1);
*lpw++ = 0xFFFF;
*lpw++ = 0x0083;   // listbox
*lpw++ = 0x0000;   // no caption
//if caption exist, delete previous line and remove next 3 lines comments
//lpwsz = (LPWSTR) lpw;
//nchar = 1+MultiByteToWideChar (CP_ACP, 0, "", -1, lpwsz, 50);
//lpw   += nchar;
*lpw++ = 0;                      // no creation data
GlobalUnlock(hgbl);
fRes = TRUE;
return(fRes);
}


