//==================================================
// FILE:	DlgUnit.c
// PROJECT: Digi Doc
// DESCRIPTION: Low-level built dialog with list selection capability
// UPDATES:
//
// Created by Aare Amenberg 06/10/2003
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

#include <libdigidoc/DigiDocDefs.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "DlgUnit.h"

HINSTANCE hInstanceCOM = NULL;
extern int RunDialogUnitSimple(char *psList[], char *psCaption, char *psHeader, char *psOkButton, char *psCancelButton, int iWinWidth, int iWinHeight);
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
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

//internal errors
#define dDLGNOERRORS           0
#define dDLGERROR             -1
#define dDLGNOSELECTION       -9
//output error
#define dDLGERROR_OUT         -2  //can't initiate dialog
#define dDLGNOSELECTION_OUT   -1  //no selection, cancel
//if output result >= 0, then item is selected

//User texts max length (headers,title,tooltips,etc.)
#define dDLGUSERTEXTMAXLEN   128

//Dialog functions
static BOOL CALLBACK WndDlgUnitProc(HWND, UINT, WPARAM, LPARAM);
static void CalculateDialogSize(HWND hDlg);
static void SetDialogItemSize(HWND hWnd, int iWOffs, int iHOffs);
static void SetDialogItemPosition(HWND hWnd, int iWOffs, int iHOffs);
static void CenterDialogOnScreen(HWND hDlg, HWND hMain);
static void Dialog_End(HWND hDlg, BOOL fCancel);
static int  Dialog_Init(HWND hDlg);
static void DispError(char *psError);
//ListView functions
static BOOL ListViewSetHeaderColumn(HWND hWndList, int iIndex, int iSize, char *psHeader);
static int  ListViewCalcColumnSize(HWND hWndList, int iIndex, int iSizePercent);
static BOOL ListViewInserRow(HWND hWndList, int iIndex, char *psVal1, char *psVal2, char *psVal3, char *psVal4, char *psVal5);
static BOOL ListViewInsertItem(HWND hWndListView, int iIndex, char *psData);

//Tooltip functions
static void ToolTipInitiate(HWND hWndDlg);
static HWND ToolTipCreate (HWND hParent);
static void ToolTipForItem (HWND hwnd,char *psText,HWND hTip);

//User defined texts
static char sDlgCaption[dDLGUSERTEXTMAXLEN+1]         ="DigiDoc";
static char sGrpHeader[dDLGUSERTEXTMAXLEN+1]          ="Select certificate:";
static char sOkButtonText[dDLGUSERTEXTMAXLEN+1]       ="&Ok";
static char sCancelButtonText[dDLGUSERTEXTMAXLEN+1]   ="&Cancel";
//ListView headers
static char sListColumn1[dDLGUSERTEXTMAXLEN+1]   ="Issuer";
static char sListColumn2[dDLGUSERTEXTMAXLEN+1]   ="Friendly name";
static char sListColumn3[dDLGUSERTEXTMAXLEN+1]   ="Valid from";
static char sListColumn4[dDLGUSERTEXTMAXLEN+1]   ="Valid to";
static char sListColumn5[dDLGUSERTEXTMAXLEN+1]   ="CSP";
//User defined tooltips
static char sDlgToolTip[dDLGUSERTEXTMAXLEN+1]         ="Certificate selection";
static char sListToolTip[dDLGUSERTEXTMAXLEN+1]        ="Selecting certificate press <Enter>\nor click <Ok> button\nor mouse double-click";
static char sOkButtonToolTip[dDLGUSERTEXTMAXLEN+1]    ="Select and close dialog";
static char sCancelButtonToolTip[dDLGUSERTEXTMAXLEN+1]="Cancel selection";

static char **psListItems=NULL;       //Listbox items
static HWND hToolTipWnd=NULL;         //ToolTip window
static int  iDlgWidth  = 800;
static int  iDlgHeight = 170;
//
//Set texts
//
//psCaption      - dialog caption/title
//                 if NULL, then no caption
//psHeader       - listbox header
//                 if NULL, then no header
//psOkButton     - Ok button label
//                 if NULL or zero length string, then "&Ok"
//psCancelButton - Cancel button label
//                 if NULL or zero length string, then "&Cancel"
EXP_OPTION int DialogUnitSetTexts(char *psCaption, char *psHeader, char *psOkButton, char *psCancelButton)
{
if (psCaption != NULL)
  lstrcpyn(sDlgCaption,psCaption,dDLGUSERTEXTMAXLEN);
if (psHeader != NULL)
  lstrcpyn(sGrpHeader,psHeader,dDLGUSERTEXTMAXLEN);
if (psOkButton != NULL)
  lstrcpyn(sOkButtonText,psOkButton,dDLGUSERTEXTMAXLEN);
if (psCancelButton != NULL)
  lstrcpyn(sCancelButtonText,psCancelButton,dDLGUSERTEXTMAXLEN);
//if not specified, then Ok string
if (lstrlen(sOkButtonText) == 0)
  lstrcpy(sOkButtonText,"&Ok");
//if not specified, then Ok string
if (lstrlen(sCancelButtonText) == 0)
  lstrcpy(sCancelButtonText,"&Cancel");
return(0);
}

EXP_OPTION int DialogUnitSetHeaders(char *psCol1, char *psCol2, char *psCol3, char *psCol4, char *psCol5)
{
if (psCol1 != NULL)
  lstrcpyn(sListColumn1,psCol1,dDLGUSERTEXTMAXLEN);
if (psCol2 != NULL)
  lstrcpyn(sListColumn2,psCol2,dDLGUSERTEXTMAXLEN);
if (psCol3 != NULL)
  lstrcpyn(sListColumn3,psCol3,dDLGUSERTEXTMAXLEN);
if (psCol4 != NULL)
  lstrcpyn(sListColumn4,psCol4,dDLGUSERTEXTMAXLEN);
if (psCol5 != NULL)
  lstrcpyn(sListColumn5,psCol5,dDLGUSERTEXTMAXLEN);
return(0);
}

EXP_OPTION int DialogUnitSetToolTips(char *psCommon, char *psListView, char *psOkButton, char *psCancelButton)
{
if (psCommon != NULL)
  lstrcpyn(sDlgToolTip,psCommon,dDLGUSERTEXTMAXLEN);
else
  sDlgToolTip[0] = 0;
if (psListView != NULL)
  lstrcpyn(sListToolTip,psListView,dDLGUSERTEXTMAXLEN);
else
  sListToolTip[0] = 0;
if (psOkButton != NULL)
  lstrcpyn(sOkButtonToolTip,psOkButton,dDLGUSERTEXTMAXLEN);
else
  sOkButtonToolTip[0] = 0;
if (psCancelButton != NULL)
  lstrcpyn(sCancelButtonToolTip,psCancelButton,dDLGUSERTEXTMAXLEN);
else
  sCancelButtonToolTip[0] = 0;
return(0);
}

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
//returns -2 (dDLGERROR_OUT) if dialog error
//returns -1 (dDLGNOSELECTION_OUT) if listbox item not selected (Cancel button)
//       >=0  selected item index in the psList array
//
//
int RunDialogUnit(char *psList[], int iWinWidth, int iWinHeight)
{
char  sTemp[dDLGTEMPBUFLEN];
DWORD dwRes;
int   iRes = dDLGERROR_OUT;
INITCOMMONCONTROLSEX iccex;
//LPTSTR lpName;
//HRSRC  hRes;
//HGLOBAL hGlobal;
iDlgWidth  = iWinWidth;
iDlgHeight = iWinHeight;
//if not allowed value, then default size
if (iDlgWidth < dDLGWIN_MINWIDTH || iDlgWidth > dDLGWIN_MAXWIDTH)
  iDlgWidth = 0;
//if not allowed value, then default size
if (iDlgHeight < dDLGWIN_MINHEIGHT || iDlgHeight > dDLGWIN_MAXHEIGHT)
  iDlgHeight = 0;
//set items
psListItems = psList;
//Init common control
iccex.dwICC = ICC_WIN95_CLASSES;
iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
if (InitCommonControlsEx(&iccex) == TRUE)
//InitCommonControls();
  { 
  //Starts dialog, return value from function Dialog_End
  //hRes = FindResource(NULL,"#220", RT_DIALOG);
  //hRes = FindResource(NULL,MAKEINTRESOURCE(IDD_DLG_UNIT), RT_DIALOG);
  //hGlobal = LoadResource(NULL,hRes);
  //dwRes  = IDD_DLG_UNIT;
  //lpName = MAKEINTRESOURCE(IDD_DLG_UNIT);
	 
	iRes = DialogBox(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDD_DLG_UNIT), NULL, (DLGPROC)WndDlgUnitProc);
	//iRes = DialogBox(hInstanceCOM, MAKEINTRESOURCE(IDD_DLG_UNIT), NULL, (DLGPROC)WndDlgUnitProc);

	if (iRes == dDLGERROR)
		iRes = dDLGERROR_OUT;
	}
 
//if GetLastError returns nonzero, then dialog opening error
if (iRes == dDLGERROR_OUT)
  {
  dwRes = GetLastError();
  if (dwRes != 0)
    {
    //prints Windows errorcode
      snprintf(sTemp, sizeof(sTemp), "DigiDoc Dialog Error=%d",dwRes);
    //DispError(sTemp);
    }
  #ifdef WIN32_CSP
  iRes = RunDialogUnitSimple(psList, sDlgCaption, sGrpHeader, sOkButtonText, sCancelButtonText, iDlgWidth, iDlgHeight); 
  #endif
  }
if (iRes == dDLGNOSELECTION)
  iRes = dDLGNOSELECTION_OUT;
return(iRes);
}


//Dialog callback functions
static BOOL CALLBACK WndDlgUnitProc(HWND  hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
BOOL fRes = FALSE;
DWORD dwNotifyCode;
LPNMHDR pnmh;
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
  case WM_NOTIFY:
    pnmh = (LPNMHDR)lParam;
    if (pnmh != NULL && pnmh->code == NM_DBLCLK)
      {
      Dialog_End(hDlg,FALSE);
      fRes = TRUE;
      }
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
//if TRUE, then always (dDLGNOSELECTION)
if (fCancel == FALSE)
  {
  //if listbox item selected, then retuns selected item index
  iRes = ListView_GetSelectionMark(GetDlgItem(hDlg,IDC_LISTVIEW));
  }
if (hToolTipWnd != NULL)
  {
  DestroyWindow(hToolTipWnd);
  hToolTipWnd = NULL;
  }
EndDialog(hDlg, iRes);
}


//Dialog init
//
static int Dialog_Init(HWND hDlg)
{
int   iRes = dDLGNOERRORS;
UINT  iState;
UINT  iMask;
DWORD dwStyle;
int   iItems = 0;
HWND hWndListView;
//Set dialog caption
SetWindowText(hDlg, (LPTSTR)sDlgCaption);
//Set listbox header
SetDlgItemText(hDlg,IDC_GROUPBOX,(LPTSTR)sGrpHeader);
//Set Ok button label
SetDlgItemText(hDlg,IDOK,(LPTSTR)sOkButtonText);
//Set Cancel button label
SetDlgItemText(hDlg,IDCANCEL,(LPTSTR)sCancelButtonText);
//Calculate dialog size
CalculateDialogSize(hDlg);
//Center dialog
CenterDialogOnScreen(hDlg,NULL);
//Set topmost
SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
//Set ListView params
hWndListView = GetDlgItem(hDlg,IDC_LISTVIEW);
dwStyle = (LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
SendMessage(hWndListView,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);
//
ToolTipInitiate(hDlg);
ToolTipForItem (GetDlgItem(hDlg,IDOK), sOkButtonToolTip, hToolTipWnd);
ToolTipForItem (GetDlgItem(hDlg,IDCANCEL), sCancelButtonToolTip, hToolTipWnd);
ToolTipForItem (hWndListView, sListToolTip, hToolTipWnd);
ToolTipForItem (hDlg, sDlgToolTip, hToolTipWnd);
ToolTipForItem (ListView_GetHeader(hWndListView), sDlgToolTip, hToolTipWnd);
//
//Set ListView headers
ListViewSetHeaderColumn(hWndListView, 0, 25, sListColumn1);
ListViewSetHeaderColumn(hWndListView, 1, 25, sListColumn2);
ListViewSetHeaderColumn(hWndListView, 2, 15, sListColumn3);
ListViewSetHeaderColumn(hWndListView, 3, 15, sListColumn4);
ListViewSetHeaderColumn(hWndListView, 4, 20, sListColumn5);
//Set ListView rows
while (psListItems != NULL && *psListItems != NULL && iItems < dDLGMAXLISTBOXSIZE)
  {
  if (lstrlen(*psListItems) < dDLGMAXITEMSIZE)
    {
    ListViewInsertItem(hWndListView,iItems,*psListItems);
    ++iItems;
    }
  else
    iRes = dDLGERROR;
  ++psListItems;
  }
if (iRes != dDLGNOERRORS)
  DispError("DigiDoc Dialog data error");
if (iItems > 0)
  {
  EnableWindow(GetDlgItem(hDlg, IDOK),TRUE);
  iState = LVIS_FOCUSED | LVIS_SELECTED;
  iMask = LVIS_FOCUSED | LVIS_SELECTED;
  ListView_SetItemCount(hWndListView,iItems);
  ListView_SetItemState(hWndListView, 0, iState, iMask);
  //iState = ListView_GetItemCount(hWndListView);
  //iMask = ListView_GetNextItem(hWndListView,-1,LVNI_FOCUSED);
  //SetFocus(hWndList);
  }
else
  EnableWindow(GetDlgItem(hDlg, IDOK),FALSE);
return(iRes);
}

//Parse \t separated texts and add to the ListView
static BOOL ListViewInsertItem(HWND hWndListView, int iIndex, char *psData)
{
BOOL fRes = TRUE;
int  iI=0;
char sVal[5][dDLGMAXITEMSIZE+1];
char *psBeg;
char *psEnd;
sVal[0][0] = 0;
sVal[1][0] = 0;
sVal[2][0] = 0;
sVal[3][0] = 0;
sVal[4][0] = 0;
psBeg = psData;
while (psBeg != NULL && *psBeg != 0)
  {
  psEnd = strchr(psBeg,'\t');
  if (psEnd != NULL)
    {
    lstrcpyn(sVal[iI],psBeg,(psEnd-psBeg)+1);
    psBeg=psEnd+1;
    }
  else
    {
    lstrcpyn(sVal[iI],psBeg,dDLGMAXITEMSIZE);
    break;
    }
  ++iI;
  }
//
ListViewInserRow(hWndListView, iIndex, sVal[0], sVal[1], sVal[2], sVal[3], sVal[4]);
return(fRes);
}

//Calculate dialog size
//
static void CalculateDialogSize(HWND hDlg)
{
RECT  rcDlg;              // Dialog window rect
int   iWOffs;
int   iHOffs;
if (iDlgWidth > 0 && iDlgHeight > 0)
  {
  GetWindowRect(hDlg,&rcDlg);
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
//
static void SetDialogItemPosition(HWND hWnd, int iWOffs, int iHOffs)
{
RECT  rcCln;
RECT  rcItm;              // Dialog item rect
RECT  rcParent;           // Parent item rect
int   iLeft;
int   iTop;
int   iTitleW;
// GetWindowRect retrieves screen coordinates
GetWindowRect(hWnd, &rcItm);
//GetClientRect(hWnd, &rcItm);
//Changed by A.Amenberg 27.06.2003
//GetAncestor supported from Win98 and WinNT SP4
//GetWindowRect(GetAncestor(hWnd, GA_PARENT), &rcParent);
GetWindowRect(GetParent(hWnd), &rcParent);
GetClientRect(GetParent(hWnd), &rcCln);
iTitleW = rcParent.bottom - rcCln.bottom;
// for SetWindowPos we need left and right on PARENT.
iLeft  = (rcItm.left - rcParent.left) + iWOffs;
iTop   = (rcItm.top - rcParent.top) + iHOffs - iTitleW;
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


static BOOL ListViewSetHeaderColumn(HWND hWndList, int iIndex, int iSize, char *psHeader)
{
BOOL fRes = TRUE;
LV_COLUMN lvC;
lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
lvC.fmt = LVCFMT_LEFT; //LVCFMT_CENTER;
lvC.cx = ListViewCalcColumnSize(hWndList,iIndex,iSize);
lvC.pszText = psHeader;
lvC.iSubItem = iIndex;
if (ListView_InsertColumn (hWndList, iIndex, &lvC) == -1)
  fRes = FALSE;
return(fRes);
}

static int ListViewCalcColumnSize(HWND hWndList, int iIndex, int iSizePercent)
{
RECT rcRec;
int iListSize;
int iUsedSize=0;
int iRes;
int iI;
double dPercUnit;
GetClientRect(hWndList, &rcRec);
iListSize = rcRec.right;
for(iI=0; iI < iIndex;++iI)
  iUsedSize+=ListView_GetColumnWidth(hWndList,iI);
if (iIndex < 4)
  {
  dPercUnit = (double)iListSize/(double)100;
  iRes = (int)((double)iSizePercent*dPercUnit);
  }
else
  iRes = iListSize-iUsedSize;
return(iRes);
}

static BOOL ListViewInserRow(HWND hWndList, int iIndex, char *psVal1, char *psVal2, char *psVal3, char *psVal4, char *psVal5)
{
BOOL fRes = TRUE;
LV_ITEM lvI;
lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
lvI.state = 0;
lvI.stateMask = 0;
lvI.iItem = iIndex;
lvI.iSubItem = 0;
lvI.pszText = "";
lvI.cchTextMax = 200;
if (ListView_InsertItem (hWndList, &lvI) == -1)
  fRes = FALSE;
ListView_SetItemText (hWndList, iIndex, 0, psVal1);
ListView_SetItemText (hWndList, iIndex, 1, psVal2);
ListView_SetItemText (hWndList, iIndex, 2, psVal3);
ListView_SetItemText (hWndList, iIndex, 3, psVal4);
ListView_SetItemText (hWndList, iIndex, 4, psVal5);
return(fRes);
}

static void ToolTipInitiate(HWND hWndDlg)
{
if (hToolTipWnd == NULL)
  {
  hToolTipWnd = ToolTipCreate(hWndDlg);
  SendMessage(hToolTipWnd,TTM_SETMAXTIPWIDTH,0,300);
  }
}

static HWND ToolTipCreate (HWND hParent)
{
HWND hWnd = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hParent,
        NULL,
        NULL, //oPrm.hAppInstance,
        NULL
        );

    SetWindowPos(hWnd,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
SendMessage(hWnd,TTM_SETMAXTIPWIDTH,0,300);
return(hWnd);
}

static void ToolTipForItem (HWND hwnd, char *psText, HWND hTip)
{
HWND hWnd = hTip;
    // struct specifying info about tool in ToolTip control
    TOOLINFO ti;
    unsigned int uid = 0;       // for ti initialization
    LPTSTR lptstr = (LPTSTR)psText;
    RECT rect;                  // for client area coordinates

    /* GET COORDINATES OF THE MAIN CLIENT AREA */
    GetClientRect (hwnd, &rect);

    /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    //ti.uFlags = 0;
    ti.hwnd = hwnd;
    ti.hinst = NULL; //oPrm.hAppInstance;
    ti.uId = uid;
    ti.lpszText = lptstr;
        // ToolTip control will cover the whole window
    ti.rect.left = rect.left;
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
if (hWnd == NULL)
  hWnd = hToolTipWnd;
 /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
SendMessage(hWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}


EXP_OPTION int RunDialogTestLow(void)
{
int iRes = 0;
/*
int iI;
int iT = 0;
char *psTemp;
char sTemp[5000];
char sVal[128];
*/
char sTest[] = {'\x8E','\x9e','\x8A','\x9A','\x00'};
char *aCertList[] =
{
NULL,
"11111111111111111111111111111111111111111\t222\t333\t4444\t5555",
"11111111111111111111111111111111111111111\t222\t333\t4444\t5555",
"11111111111111111111111111111111111111111\t222\t333\t4444\t5555",
NULL};
aCertList[0]=sTest;
iRes = RunDialogUnit(aCertList, 500, 300);
return(iRes);
}
