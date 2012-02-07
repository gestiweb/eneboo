/***************************************************************************
 AQS_p.h
 -------------------
 begin                : 29/01/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
 email                : mail@infosial.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
 Este  programa es software libre. Puede redistribuirlo y/o modificarlo
 bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
 versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef AQS_P_H_
#define AQS_P_H_

#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <qdom.h>
#include <qsinterpreter.h>
#include <qmetaobject.h>
#include <qptrdict.h>
#include <qevent.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qdragobject.h>
#include <qiconview.h>
#include <qiconset.h>
#include <qtooltip.h>
#include <qtextcodec.h>
#include <qurl.h>
#include <qnetworkprotocol.h>
#include <qurlinfo.h>
#include <quuid.h>
#include <qsobjectfactory.h>
#include <qswrapperfactory.h>

#ifdef AQ_DEBUG
#define AQS_IF_DEBUG(T) T
#else
#define AQS_IF_DEBUG(T)
#endif

class AQS;
class AQSBaseObject;
class AQSObject;
class AQSColor;
class AQSImage;
class AQSPixmap;
class AQSUrlInfo;

extern AQS *globalAQS;
extern QSInterpreter *globalAQSInterpreter;
extern QSObjectFactory *globalAQSFactory;
extern QSWrapperFactory *globalAQSWrapper;

typedef QValueList<QVariant> QVariantList;

class AQS : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int NoButton READ NoButton)
  Q_PROPERTY(int LeftButton READ LeftButton)
  Q_PROPERTY(int RightButton READ RightButton)
  Q_PROPERTY(int MidButton READ MidButton)
  Q_PROPERTY(int MouseButtonMask READ MouseButtonMask)
  Q_PROPERTY(int ShiftButton READ ShiftButton)
  Q_PROPERTY(int ControlButton READ ControlButton)
  Q_PROPERTY(int AltButton READ AltButton)
  Q_PROPERTY(int MetaButton READ MetaButton)
  Q_PROPERTY(int KeyButtonMask READ KeyButtonMask)
  Q_PROPERTY(int Horizontal READ Horizontal)
  Q_PROPERTY(int AlignAuto READ AlignAuto)
  Q_PROPERTY(int AlignLeft READ AlignLeft)
  Q_PROPERTY(int AlignRight READ AlignRight)
  Q_PROPERTY(int AlignHCenter READ AlignHCenter)
  Q_PROPERTY(int AlignJustify READ AlignJustify)
  Q_PROPERTY(int AlignHorizontal_Mask READ AlignHorizontal_Mask)
  Q_PROPERTY(int AlignTop READ AlignTop)
  Q_PROPERTY(int AlignBottom READ AlignBottom)
  Q_PROPERTY(int AlignVCenter READ AlignVCenter)
  Q_PROPERTY(int AlignVertical_Mask READ AlignVertical_Mask)
  Q_PROPERTY(int SingleLine READ SingleLine)
  Q_PROPERTY(int DontClip READ DontClip)
  Q_PROPERTY(int ExpandTabs READ ExpandTabs)
  Q_PROPERTY(int ShowPrefix READ ShowPrefix)
  Q_PROPERTY(int WordBreak READ WordBreak)
  Q_PROPERTY(int BreakAnywhere READ BreakAnywhere)
  Q_PROPERTY(int DontPrint READ DontPrint)
  Q_PROPERTY(int WState_Created READ WState_Created)
  Q_PROPERTY(int WState_Disabled READ WState_Disabled)
  Q_PROPERTY(int WState_Visible READ WState_Visible)
  Q_PROPERTY(int WState_ForceHide READ WState_ForceHide)
  Q_PROPERTY(int WState_OwnCursor READ WState_OwnCursor)
  Q_PROPERTY(int WState_MouseTracking READ WState_MouseTracking)
  Q_PROPERTY(int WState_CompressKeys READ WState_CompressKeys)
  Q_PROPERTY(int WState_BlockUpdates READ WState_BlockUpdates)
  Q_PROPERTY(int WState_InPaintEvent READ WState_InPaintEvent)
  Q_PROPERTY(int WState_Reparented READ WState_Reparented)
  Q_PROPERTY(int WState_ConfigPending READ WState_ConfigPending)
  Q_PROPERTY(int WState_Resized READ WState_Resized)
  Q_PROPERTY(int WState_AutoMask READ WState_AutoMask)
  Q_PROPERTY(int WState_Polished READ WState_Polished)
  Q_PROPERTY(int WState_DND READ WState_DND)
  Q_PROPERTY(int WState_Reserved0 READ WState_Reserved0)
  Q_PROPERTY(int WState_Reserved1 READ WState_Reserved1)
  Q_PROPERTY(int WState_Reserved2 READ WState_Reserved2)
  Q_PROPERTY(int WState_Reserved3 READ WState_Reserved3)
  Q_PROPERTY(int WState_Maximized READ WState_Maximized)
  Q_PROPERTY(int WState_Minimized READ WState_Minimized)
  Q_PROPERTY(int WState_ForceDisabled READ WState_ForceDisabled)
  Q_PROPERTY(int WState_Exposed READ WState_Exposed)
  Q_PROPERTY(int WType_TopLevel READ WType_TopLevel)
  Q_PROPERTY(int WType_Dialog READ WType_Dialog)
  Q_PROPERTY(int WType_Popup READ WType_Popup)
  Q_PROPERTY(int WType_Desktop READ WType_Desktop)
  Q_PROPERTY(int WType_Mask READ WType_Mask)
  Q_PROPERTY(int WStyle_Customize READ WStyle_Customize)
  Q_PROPERTY(int WStyle_NormalBorder READ WStyle_NormalBorder)
  Q_PROPERTY(int WStyle_DialogBorder READ WStyle_DialogBorder)
  Q_PROPERTY(int WStyle_NoBorder READ WStyle_NoBorder)
  Q_PROPERTY(int WStyle_Title READ WStyle_Title)
  Q_PROPERTY(int WStyle_SysMenu READ WStyle_SysMenu)
  Q_PROPERTY(int WStyle_Minimize READ WStyle_Minimize)
  Q_PROPERTY(int WStyle_Maximize READ WStyle_Maximize)
  Q_PROPERTY(int WStyle_MinMax READ WStyle_MinMax)
  Q_PROPERTY(int WStyle_Tool READ WStyle_Tool)
  Q_PROPERTY(int WStyle_StaysOnTop READ WStyle_StaysOnTop)
  Q_PROPERTY(int WStyle_ContextHelp READ WStyle_ContextHelp)
  Q_PROPERTY(int WStyle_Reserved READ WStyle_Reserved)
  Q_PROPERTY(int WStyle_Mask READ WStyle_Mask)
  Q_PROPERTY(int WDestructiveClose READ WDestructiveClose)
  Q_PROPERTY(int WPaintDesktop READ WPaintDesktop)
  Q_PROPERTY(int WPaintUnclipped READ WPaintUnclipped)
  Q_PROPERTY(int WPaintClever READ WPaintClever)
  Q_PROPERTY(int WResizeNoErase READ WResizeNoErase)
  Q_PROPERTY(int WMouseNoMask READ WMouseNoMask)
  Q_PROPERTY(int WStaticContents READ WStaticContents)
  Q_PROPERTY(int WRepaintNoErase READ WRepaintNoErase)
  Q_PROPERTY(int WWinOwnDC READ WWinOwnDC)
  Q_PROPERTY(int WShowModal READ WShowModal)
  Q_PROPERTY(int WNoMousePropagation READ WNoMousePropagation)
  Q_PROPERTY(int WSubWindow READ WSubWindow)
  Q_PROPERTY(int WNorthWestGravity READ WNorthWestGravity)
  Q_PROPERTY(int WType_Modal READ WType_Modal)
  Q_PROPERTY(int WStyle_Dialog READ WStyle_Dialog)
  Q_PROPERTY(int ColorMode_Mask READ ColorMode_Mask)
  Q_PROPERTY(int AutoColor READ AutoColor)
  Q_PROPERTY(int ColorOnly READ ColorOnly)
  Q_PROPERTY(int MonoOnly READ MonoOnly)
  Q_PROPERTY(int AlphaDither_Mask READ AlphaDither_Mask)
  Q_PROPERTY(int ThresholdAlphaDither READ ThresholdAlphaDither)
  Q_PROPERTY(int OrderedAlphaDither READ OrderedAlphaDither)
  Q_PROPERTY(int DiffuseAlphaDither READ DiffuseAlphaDither)
  Q_PROPERTY(int NoAlpha READ NoAlpha)
  Q_PROPERTY(int Dither_Mask READ Dither_Mask)
  Q_PROPERTY(int DiffuseDither READ DiffuseDither)
  Q_PROPERTY(int OrderedDither READ OrderedDither)
  Q_PROPERTY(int ThresholdDither READ ThresholdDither)
  Q_PROPERTY(int DitherMode_Mask READ DitherMode_Mask)
  Q_PROPERTY(int AutoDither READ AutoDither)
  Q_PROPERTY(int PreferDither READ PreferDither)
  Q_PROPERTY(int TransparentMode READ TransparentMode)
  Q_PROPERTY(int PixelUnit READ PixelUnit)
  Q_PROPERTY(int LoMetricUnit READ LoMetricUnit)
  Q_PROPERTY(int HiMetricUnit READ HiMetricUnit)
  Q_PROPERTY(int LoEnglishUnit READ LoEnglishUnit)
  Q_PROPERTY(int HiEnglishUnit READ HiEnglishUnit)
  Q_PROPERTY(int WindowsStyle READ WindowsStyle)
  Q_PROPERTY(int MacStyle READ MacStyle)
  Q_PROPERTY(int Win3Style READ Win3Style)
  Q_PROPERTY(int PMStyle READ PMStyle)
  Q_PROPERTY(int SHIFT READ SHIFT)
  Q_PROPERTY(int CTRL READ CTRL)
  Q_PROPERTY(int ALT READ ALT)
  Q_PROPERTY(int MODIFIER_MASK READ MODIFIER_MASK)
  Q_PROPERTY(int UNICODE_ACCEL READ UNICODE_ACCEL)
  Q_PROPERTY(int Key_Escape READ Key_Escape)
  Q_PROPERTY(int Key_Tab READ Key_Tab)
  Q_PROPERTY(int Key_Backtab READ Key_Backtab)
  Q_PROPERTY(int Key_BackTab READ Key_BackTab)
  Q_PROPERTY(int Key_Backspace READ Key_Backspace)
  Q_PROPERTY(int Key_BackSpace READ Key_BackSpace)
  Q_PROPERTY(int Key_Return READ Key_Return)
  Q_PROPERTY(int Key_Enter READ Key_Enter)
  Q_PROPERTY(int Key_Insert READ Key_Insert)
  Q_PROPERTY(int Key_Delete READ Key_Delete)
  Q_PROPERTY(int Key_Pause READ Key_Pause)
  Q_PROPERTY(int Key_Print READ Key_Print)
  Q_PROPERTY(int Key_SysReq READ Key_SysReq)
  Q_PROPERTY(int Key_Home READ Key_Home)
  Q_PROPERTY(int Key_End READ Key_End)
  Q_PROPERTY(int Key_Left READ Key_Left)
  Q_PROPERTY(int Key_Up READ Key_Up)
  Q_PROPERTY(int Key_Right READ Key_Right)
  Q_PROPERTY(int Key_Down READ Key_Down)
  Q_PROPERTY(int Key_Prior READ Key_Prior)
  Q_PROPERTY(int Key_PageUp READ Key_PageUp)
  Q_PROPERTY(int Key_Next READ Key_Next)
  Q_PROPERTY(int Key_PageDown READ Key_PageDown)
  Q_PROPERTY(int Key_Shift READ Key_Shift)
  Q_PROPERTY(int Key_Control READ Key_Control)
  Q_PROPERTY(int Key_Meta READ Key_Meta)
  Q_PROPERTY(int Key_Alt READ Key_Alt)
  Q_PROPERTY(int Key_CapsLock READ Key_CapsLock)
  Q_PROPERTY(int Key_NumLock READ Key_NumLock)
  Q_PROPERTY(int Key_ScrollLock READ Key_ScrollLock)
  Q_PROPERTY(int Key_F1 READ Key_F1)
  Q_PROPERTY(int Key_F2 READ Key_F2)
  Q_PROPERTY(int Key_F3 READ Key_F3)
  Q_PROPERTY(int Key_F4 READ Key_F4)
  Q_PROPERTY(int Key_F5 READ Key_F5)
  Q_PROPERTY(int Key_F6 READ Key_F6)
  Q_PROPERTY(int Key_F7 READ Key_F7)
  Q_PROPERTY(int Key_F8 READ Key_F8)
  Q_PROPERTY(int Key_F9 READ Key_F9)
  Q_PROPERTY(int Key_F10 READ Key_F10)
  Q_PROPERTY(int Key_F11 READ Key_F11)
  Q_PROPERTY(int Key_F12 READ Key_F12)
  Q_PROPERTY(int Key_F13 READ Key_F13)
  Q_PROPERTY(int Key_F14 READ Key_F14)
  Q_PROPERTY(int Key_F15 READ Key_F15)
  Q_PROPERTY(int Key_F16 READ Key_F16)
  Q_PROPERTY(int Key_F17 READ Key_F17)
  Q_PROPERTY(int Key_F18 READ Key_F18)
  Q_PROPERTY(int Key_F19 READ Key_F19)
  Q_PROPERTY(int Key_F20 READ Key_F20)
  Q_PROPERTY(int Key_F21 READ Key_F21)
  Q_PROPERTY(int Key_F22 READ Key_F22)
  Q_PROPERTY(int Key_F23 READ Key_F23)
  Q_PROPERTY(int Key_F24 READ Key_F24)
  Q_PROPERTY(int Key_F25 READ Key_F25)
  Q_PROPERTY(int Key_F26 READ Key_F26)
  Q_PROPERTY(int Key_F27 READ Key_F27)
  Q_PROPERTY(int Key_F28 READ Key_F28)
  Q_PROPERTY(int Key_F29 READ Key_F29)
  Q_PROPERTY(int Key_F30 READ Key_F30)
  Q_PROPERTY(int Key_F31 READ Key_F31)
  Q_PROPERTY(int Key_F32 READ Key_F32)
  Q_PROPERTY(int Key_F33 READ Key_F33)
  Q_PROPERTY(int Key_F34 READ Key_F34)
  Q_PROPERTY(int Key_F35 READ Key_F35)
  Q_PROPERTY(int Key_Super_L READ Key_Super_L)
  Q_PROPERTY(int Key_Super_R READ Key_Super_R)
  Q_PROPERTY(int Key_Menu READ Key_Menu)
  Q_PROPERTY(int Key_Hyper_L READ Key_Hyper_L)
  Q_PROPERTY(int Key_Hyper_R READ Key_Hyper_R)
  Q_PROPERTY(int Key_Help READ Key_Help)
  Q_PROPERTY(int Key_Direction_L READ Key_Direction_L)
  Q_PROPERTY(int Key_Direction_R READ Key_Direction_R)
  Q_PROPERTY(int Key_Space READ Key_Space)
  Q_PROPERTY(int Key_Any READ Key_Any)
  Q_PROPERTY(int Key_Exclam READ Key_Exclam)
  Q_PROPERTY(int Key_QuoteDbl READ Key_QuoteDbl)
  Q_PROPERTY(int Key_NumberSign READ Key_NumberSign)
  Q_PROPERTY(int Key_Dollar READ Key_Dollar)
  Q_PROPERTY(int Key_Percent READ Key_Percent)
  Q_PROPERTY(int Key_Ampersand READ Key_Ampersand)
  Q_PROPERTY(int Key_Apostrophe READ Key_Apostrophe)
  Q_PROPERTY(int Key_ParenLeft READ Key_ParenLeft)
  Q_PROPERTY(int Key_ParenRight READ Key_ParenRight)
  Q_PROPERTY(int Key_Asterisk READ Key_Asterisk)
  Q_PROPERTY(int Key_Plus READ Key_Plus)
  Q_PROPERTY(int Key_Comma READ Key_Comma)
  Q_PROPERTY(int Key_Minus READ Key_Minus)
  Q_PROPERTY(int Key_Period READ Key_Period)
  Q_PROPERTY(int Key_Slash READ Key_Slash)
  Q_PROPERTY(int Key_0 READ Key_0)
  Q_PROPERTY(int Key_1 READ Key_1)
  Q_PROPERTY(int Key_2 READ Key_2)
  Q_PROPERTY(int Key_3 READ Key_3)
  Q_PROPERTY(int Key_4 READ Key_4)
  Q_PROPERTY(int Key_5 READ Key_5)
  Q_PROPERTY(int Key_6 READ Key_6)
  Q_PROPERTY(int Key_7 READ Key_7)
  Q_PROPERTY(int Key_8 READ Key_8)
  Q_PROPERTY(int Key_9 READ Key_9)
  Q_PROPERTY(int Key_Colon READ Key_Colon)
  Q_PROPERTY(int Key_Semicolon READ Key_Semicolon)
  Q_PROPERTY(int Key_Less READ Key_Less)
  Q_PROPERTY(int Key_Equal READ Key_Equal)
  Q_PROPERTY(int Key_Greater READ Key_Greater)
  Q_PROPERTY(int Key_Question READ Key_Question)
  Q_PROPERTY(int Key_At READ Key_At)
  Q_PROPERTY(int Key_A READ Key_A)
  Q_PROPERTY(int Key_B READ Key_B)
  Q_PROPERTY(int Key_C READ Key_C)
  Q_PROPERTY(int Key_D READ Key_D)
  Q_PROPERTY(int Key_E READ Key_E)
  Q_PROPERTY(int Key_F READ Key_F)
  Q_PROPERTY(int Key_G READ Key_G)
  Q_PROPERTY(int Key_H READ Key_H)
  Q_PROPERTY(int Key_I READ Key_I)
  Q_PROPERTY(int Key_J READ Key_J)
  Q_PROPERTY(int Key_K READ Key_K)
  Q_PROPERTY(int Key_L READ Key_L)
  Q_PROPERTY(int Key_M READ Key_M)
  Q_PROPERTY(int Key_N READ Key_N)
  Q_PROPERTY(int Key_O READ Key_O)
  Q_PROPERTY(int Key_P READ Key_P)
  Q_PROPERTY(int Key_Q READ Key_Q)
  Q_PROPERTY(int Key_R READ Key_R)
  Q_PROPERTY(int Key_S READ Key_S)
  Q_PROPERTY(int Key_T READ Key_T)
  Q_PROPERTY(int Key_U READ Key_U)
  Q_PROPERTY(int Key_V READ Key_V)
  Q_PROPERTY(int Key_W READ Key_W)
  Q_PROPERTY(int Key_X READ Key_X)
  Q_PROPERTY(int Key_Y READ Key_Y)
  Q_PROPERTY(int Key_Z READ Key_Z)
  Q_PROPERTY(int Key_BracketLeft READ Key_BracketLeft)
  Q_PROPERTY(int Key_Backslash READ Key_Backslash)
  Q_PROPERTY(int Key_BracketRight READ Key_BracketRight)
  Q_PROPERTY(int Key_AsciiCircum READ Key_AsciiCircum)
  Q_PROPERTY(int Key_Underscore READ Key_Underscore)
  Q_PROPERTY(int Key_QuoteLeft READ Key_QuoteLeft)
  Q_PROPERTY(int Key_BraceLeft READ Key_BraceLeft)
  Q_PROPERTY(int Key_Bar READ Key_Bar)
  Q_PROPERTY(int Key_BraceRight READ Key_BraceRight)
  Q_PROPERTY(int Key_AsciiTilde READ Key_AsciiTilde)
  Q_PROPERTY(int Key_nobreakspace READ Key_nobreakspace)
  Q_PROPERTY(int Key_exclamdown READ Key_exclamdown)
  Q_PROPERTY(int Key_cent READ Key_cent)
  Q_PROPERTY(int Key_sterling READ Key_sterling)
  Q_PROPERTY(int Key_currency READ Key_currency)
  Q_PROPERTY(int Key_yen READ Key_yen)
  Q_PROPERTY(int Key_brokenbar READ Key_brokenbar)
  Q_PROPERTY(int Key_section READ Key_section)
  Q_PROPERTY(int Key_diaeresis READ Key_diaeresis)
  Q_PROPERTY(int Key_copyright READ Key_copyright)
  Q_PROPERTY(int Key_ordfeminine READ Key_ordfeminine)
  Q_PROPERTY(int Key_guillemotleft READ Key_guillemotleft)
  Q_PROPERTY(int Key_notsign READ Key_notsign)
  Q_PROPERTY(int Key_hyphen READ Key_hyphen)
  Q_PROPERTY(int Key_registered READ Key_registered)
  Q_PROPERTY(int Key_macron READ Key_macron)
  Q_PROPERTY(int Key_degree READ Key_degree)
  Q_PROPERTY(int Key_plusminus READ Key_plusminus)
  Q_PROPERTY(int Key_twosuperior READ Key_twosuperior)
  Q_PROPERTY(int Key_threesuperior READ Key_threesuperior)
  Q_PROPERTY(int Key_acute READ Key_acute)
  Q_PROPERTY(int Key_mu READ Key_mu)
  Q_PROPERTY(int Key_paragraph READ Key_paragraph)
  Q_PROPERTY(int Key_periodcentered READ Key_periodcentered)
  Q_PROPERTY(int Key_cedilla READ Key_cedilla)
  Q_PROPERTY(int Key_onesuperior READ Key_onesuperior)
  Q_PROPERTY(int Key_masculine READ Key_masculine)
  Q_PROPERTY(int Key_guillemotright READ Key_guillemotright)
  Q_PROPERTY(int Key_onequarter READ Key_onequarter)
  Q_PROPERTY(int Key_onehalf READ Key_onehalf)
  Q_PROPERTY(int Key_threequarters READ Key_threequarters)
  Q_PROPERTY(int Key_questiondown READ Key_questiondown)
  Q_PROPERTY(int Key_Agrave READ Key_Agrave)
  Q_PROPERTY(int Key_Aacute READ Key_Aacute)
  Q_PROPERTY(int Key_Acircumflex READ Key_Acircumflex)
  Q_PROPERTY(int Key_Atilde READ Key_Atilde)
  Q_PROPERTY(int Key_Adiaeresis READ Key_Adiaeresis)
  Q_PROPERTY(int Key_Aring READ Key_Aring)
  Q_PROPERTY(int Key_AE READ Key_AE)
  Q_PROPERTY(int Key_Ccedilla READ Key_Ccedilla)
  Q_PROPERTY(int Key_Egrave READ Key_Egrave)
  Q_PROPERTY(int Key_Eacute READ Key_Eacute)
  Q_PROPERTY(int Key_Ecircumflex READ Key_Ecircumflex)
  Q_PROPERTY(int Key_Ediaeresis READ Key_Ediaeresis)
  Q_PROPERTY(int Key_Igrave READ Key_Igrave)
  Q_PROPERTY(int Key_Iacute READ Key_Iacute)
  Q_PROPERTY(int Key_Icircumflex READ Key_Icircumflex)
  Q_PROPERTY(int Key_Idiaeresis READ Key_Idiaeresis)
  Q_PROPERTY(int Key_ETH READ Key_ETH)
  Q_PROPERTY(int Key_Ntilde READ Key_Ntilde)
  Q_PROPERTY(int Key_Ograve READ Key_Ograve)
  Q_PROPERTY(int Key_Oacute READ Key_Oacute)
  Q_PROPERTY(int Key_Ocircumflex READ Key_Ocircumflex)
  Q_PROPERTY(int Key_Otilde READ Key_Otilde)
  Q_PROPERTY(int Key_Odiaeresis READ Key_Odiaeresis)
  Q_PROPERTY(int Key_multiply READ Key_multiply)
  Q_PROPERTY(int Key_Ooblique READ Key_Ooblique)
  Q_PROPERTY(int Key_Ugrave READ Key_Ugrave)
  Q_PROPERTY(int Key_Uacute READ Key_Uacute)
  Q_PROPERTY(int Key_Ucircumflex READ Key_Ucircumflex)
  Q_PROPERTY(int Key_Udiaeresis READ Key_Udiaeresis)
  Q_PROPERTY(int Key_Yacute READ Key_Yacute)
  Q_PROPERTY(int Key_THORN READ Key_THORN)
  Q_PROPERTY(int Key_ssharp READ Key_ssharp)
  Q_PROPERTY(int Key_agrave READ Key_agrave)
  Q_PROPERTY(int Key_aacute READ Key_aacute)
  Q_PROPERTY(int Key_acircumflex READ Key_acircumflex)
  Q_PROPERTY(int Key_atilde READ Key_atilde)
  Q_PROPERTY(int Key_adiaeresis READ Key_adiaeresis)
  Q_PROPERTY(int Key_aring READ Key_aring)
  Q_PROPERTY(int Key_ae READ Key_ae)
  Q_PROPERTY(int Key_ccedilla READ Key_ccedilla)
  Q_PROPERTY(int Key_egrave READ Key_egrave)
  Q_PROPERTY(int Key_eacute READ Key_eacute)
  Q_PROPERTY(int Key_ecircumflex READ Key_ecircumflex)
  Q_PROPERTY(int Key_ediaeresis READ Key_ediaeresis)
  Q_PROPERTY(int Key_igrave READ Key_igrave)
  Q_PROPERTY(int Key_iacute READ Key_iacute)
  Q_PROPERTY(int Key_icircumflex READ Key_icircumflex)
  Q_PROPERTY(int Key_idiaeresis READ Key_idiaeresis)
  Q_PROPERTY(int Key_eth READ Key_eth)
  Q_PROPERTY(int Key_ntilde READ Key_ntilde)
  Q_PROPERTY(int Key_ograve READ Key_ograve)
  Q_PROPERTY(int Key_oacute READ Key_oacute)
  Q_PROPERTY(int Key_ocircumflex READ Key_ocircumflex)
  Q_PROPERTY(int Key_otilde READ Key_otilde)
  Q_PROPERTY(int Key_odiaeresis READ Key_odiaeresis)
  Q_PROPERTY(int Key_division READ Key_division)
  Q_PROPERTY(int Key_oslash READ Key_oslash)
  Q_PROPERTY(int Key_ugrave READ Key_ugrave)
  Q_PROPERTY(int Key_uacute READ Key_uacute)
  Q_PROPERTY(int Key_ucircumflex READ Key_ucircumflex)
  Q_PROPERTY(int Key_udiaeresis READ Key_udiaeresis)
  Q_PROPERTY(int Key_yacute READ Key_yacute)
  Q_PROPERTY(int Key_thorn READ Key_thorn)
  Q_PROPERTY(int Key_ydiaeresis READ Key_ydiaeresis)
  Q_PROPERTY(int Key_Back READ Key_Back)
  Q_PROPERTY(int Key_Forward READ Key_Forward)
  Q_PROPERTY(int Key_Stop READ Key_Stop)
  Q_PROPERTY(int Key_Refresh READ Key_Refresh)
  Q_PROPERTY(int Key_VolumeDown READ Key_VolumeDown)
  Q_PROPERTY(int Key_VolumeMute READ Key_VolumeMute)
  Q_PROPERTY(int Key_VolumeUp READ Key_VolumeUp)
  Q_PROPERTY(int Key_BassBoost READ Key_BassBoost)
  Q_PROPERTY(int Key_BassUp READ Key_BassUp)
  Q_PROPERTY(int Key_BassDown READ Key_BassDown)
  Q_PROPERTY(int Key_TrebleUp READ Key_TrebleUp)
  Q_PROPERTY(int Key_TrebleDown READ Key_TrebleDown)
  Q_PROPERTY(int Key_MediaPlay READ Key_MediaPlay)
  Q_PROPERTY(int Key_MediaStop READ Key_MediaStop)
  Q_PROPERTY(int Key_MediaPrev READ Key_MediaPrev)
  Q_PROPERTY(int Key_MediaNext READ Key_MediaNext)
  Q_PROPERTY(int Key_MediaRecord READ Key_MediaRecord)
  Q_PROPERTY(int Key_HomePage READ Key_HomePage)
  Q_PROPERTY(int Key_Favorites READ Key_Favorites)
  Q_PROPERTY(int Key_Search READ Key_Search)
  Q_PROPERTY(int Key_Standby READ Key_Standby)
  Q_PROPERTY(int Key_OpenUrl READ Key_OpenUrl)
  Q_PROPERTY(int Key_LaunchMail READ Key_LaunchMail)
  Q_PROPERTY(int Key_LaunchMedia READ Key_LaunchMedia)
  Q_PROPERTY(int Key_Launch0 READ Key_Launch0)
  Q_PROPERTY(int Key_Launch1 READ Key_Launch1)
  Q_PROPERTY(int Key_Launch2 READ Key_Launch2)
  Q_PROPERTY(int Key_Launch3 READ Key_Launch3)
  Q_PROPERTY(int Key_Launch4 READ Key_Launch4)
  Q_PROPERTY(int Key_Launch5 READ Key_Launch5)
  Q_PROPERTY(int Key_Launch6 READ Key_Launch6)
  Q_PROPERTY(int Key_Launch7 READ Key_Launch7)
  Q_PROPERTY(int Key_Launch8 READ Key_Launch8)
  Q_PROPERTY(int Key_Launch9 READ Key_Launch9)
  Q_PROPERTY(int Key_LaunchA READ Key_LaunchA)
  Q_PROPERTY(int Key_LaunchB READ Key_LaunchB)
  Q_PROPERTY(int Key_LaunchC READ Key_LaunchC)
  Q_PROPERTY(int Key_LaunchD READ Key_LaunchD)
  Q_PROPERTY(int Key_LaunchE READ Key_LaunchE)
  Q_PROPERTY(int Key_LaunchF READ Key_LaunchF)
  Q_PROPERTY(int Key_MediaLast READ Key_MediaLast)
  Q_PROPERTY(int UpArrow READ UpArrow)
  Q_PROPERTY(int DownArrow READ DownArrow)
  Q_PROPERTY(int LeftArrow READ LeftArrow)
  Q_PROPERTY(int CopyROP READ CopyROP)
  Q_PROPERTY(int OrROP READ OrROP)
  Q_PROPERTY(int XorROP READ XorROP)
  Q_PROPERTY(int NotAndROP READ NotAndROP)
  Q_PROPERTY(int EraseROP READ EraseROP)
  Q_PROPERTY(int NotCopyROP READ NotCopyROP)
  Q_PROPERTY(int NotOrROP READ NotOrROP)
  Q_PROPERTY(int NotXorROP READ NotXorROP)
  Q_PROPERTY(int AndROP READ AndROP)
  Q_PROPERTY(int NotEraseROP READ NotEraseROP)
  Q_PROPERTY(int NotROP READ NotROP)
  Q_PROPERTY(int ClearROP READ ClearROP)
  Q_PROPERTY(int SetROP READ SetROP)
  Q_PROPERTY(int NopROP READ NopROP)
  Q_PROPERTY(int AndNotROP READ AndNotROP)
  Q_PROPERTY(int OrNotROP READ OrNotROP)
  Q_PROPERTY(int NandROP READ NandROP)
  Q_PROPERTY(int NorROP READ NorROP)
  Q_PROPERTY(int NoPen READ NoPen)
  Q_PROPERTY(int SolidLine READ SolidLine)
  Q_PROPERTY(int DashLine READ DashLine)
  Q_PROPERTY(int DotLine READ DotLine)
  Q_PROPERTY(int DashDotLine READ DashDotLine)
  Q_PROPERTY(int DashDotDotLine READ DashDotDotLine)
  Q_PROPERTY(int FlatCap READ FlatCap)
  Q_PROPERTY(int SquareCap READ SquareCap)
  Q_PROPERTY(int RoundCap READ RoundCap)
  Q_PROPERTY(int MiterJoin READ MiterJoin)
  Q_PROPERTY(int BevelJoin READ BevelJoin)
  Q_PROPERTY(int RoundJoin READ RoundJoin)
  Q_PROPERTY(int NoBrush READ NoBrush)
  Q_PROPERTY(int SolidPattern READ SolidPattern)
  Q_PROPERTY(int Dense1Pattern READ Dense1Pattern)
  Q_PROPERTY(int Dense2Pattern READ Dense2Pattern)
  Q_PROPERTY(int Dense3Pattern READ Dense3Pattern)
  Q_PROPERTY(int Dense4Pattern READ Dense4Pattern)
  Q_PROPERTY(int Dense5Pattern READ Dense5Pattern)
  Q_PROPERTY(int Dense6Pattern READ Dense6Pattern)
  Q_PROPERTY(int Dense7Pattern READ Dense7Pattern)
  Q_PROPERTY(int HorPattern READ HorPattern)
  Q_PROPERTY(int VerPattern READ VerPattern)
  Q_PROPERTY(int CrossPattern READ CrossPattern)
  Q_PROPERTY(int BDiagPattern READ BDiagPattern)
  Q_PROPERTY(int FDiagPattern READ FDiagPattern)
  Q_PROPERTY(int DiagCrossPattern READ DiagCrossPattern)
  Q_PROPERTY(int WV_32s READ WV_32s)
  Q_PROPERTY(int WV_95 READ WV_95)
  Q_PROPERTY(int WV_98 READ WV_98)
  Q_PROPERTY(int WV_Me READ WV_Me)
  Q_PROPERTY(int WV_DOS_based READ WV_DOS_based)
  Q_PROPERTY(int WV_NT READ WV_NT)
  Q_PROPERTY(int WV_2000 READ WV_2000)
  Q_PROPERTY(int WV_XP READ WV_XP)
  Q_PROPERTY(int UI_General READ UI_General)
  Q_PROPERTY(int UI_AnimateMenu READ UI_AnimateMenu)
  Q_PROPERTY(int UI_FadeMenu READ UI_FadeMenu)
  Q_PROPERTY(int UI_AnimateCombo READ UI_AnimateCombo)
  Q_PROPERTY(int UI_AnimateTooltip READ UI_AnimateTooltip)
  Q_PROPERTY(int ArrowCursor READ ArrowCursor)
  Q_PROPERTY(int UpArrowCursor READ UpArrowCursor)
  Q_PROPERTY(int CrossCursor READ CrossCursor)
  Q_PROPERTY(int WaitCursor READ WaitCursor)
  Q_PROPERTY(int IbeamCursor READ IbeamCursor)
  Q_PROPERTY(int SizeVerCursor READ SizeVerCursor)
  Q_PROPERTY(int SizeHorCursor READ SizeHorCursor)
  Q_PROPERTY(int SizeBDiagCursor READ SizeBDiagCursor)
  Q_PROPERTY(int SizeFDiagCursor READ SizeFDiagCursor)
  Q_PROPERTY(int SizeAllCursor READ SizeAllCursor)
  Q_PROPERTY(int BlankCursor READ BlankCursor)
  Q_PROPERTY(int SplitVCursor READ SplitVCursor)
  Q_PROPERTY(int SplitHCursor READ SplitHCursor)
  Q_PROPERTY(int PointingHandCursor READ PointingHandCursor)
  Q_PROPERTY(int ForbiddenCursor READ ForbiddenCursor)
  Q_PROPERTY(int WhatsThisCursor READ WhatsThisCursor)
  Q_PROPERTY(int LastCursor READ LastCursor)
  Q_PROPERTY(int PlainText READ PlainText)
  Q_PROPERTY(int RichText READ RichText)
  Q_PROPERTY(int TextDate READ TextDate)
  Q_PROPERTY(int ISODate READ ISODate)
  Q_PROPERTY(int FixedColor READ FixedColor)
  Q_PROPERTY(int FixedPixmap READ FixedPixmap)
  Q_PROPERTY(int NoBackground READ NoBackground)
  Q_PROPERTY(int PaletteForeground READ PaletteForeground)
  Q_PROPERTY(int PaletteButton READ PaletteButton)
  Q_PROPERTY(int PaletteLight READ PaletteLight)
  Q_PROPERTY(int PaletteMidlight READ PaletteMidlight)
  Q_PROPERTY(int PaletteDark READ PaletteDark)
  Q_PROPERTY(int PaletteMid READ PaletteMid)
  Q_PROPERTY(int PaletteText READ PaletteText)
  Q_PROPERTY(int PaletteBrightText READ PaletteBrightText)
  Q_PROPERTY(int PaletteBase READ PaletteBase)
  Q_PROPERTY(int PaletteBackground READ PaletteBackground)
  Q_PROPERTY(int PaletteShadow READ PaletteShadow)
  Q_PROPERTY(int PaletteHighlight READ PaletteHighlight)
  Q_PROPERTY(int PaletteHighlightedText READ PaletteHighlightedText)
  Q_PROPERTY(int PaletteButtonText READ PaletteButtonText)
  Q_PROPERTY(int PaletteLink READ PaletteLink)
  Q_PROPERTY(int PaletteLinkVisited READ PaletteLinkVisited)
  Q_PROPERTY(int CaseSensitive READ CaseSensitive)
  Q_PROPERTY(int BeginsWith READ BeginsWith)
  Q_PROPERTY(int EndsWith READ EndsWith)
  Q_PROPERTY(int Contains READ Contains)
  Q_PROPERTY(int UndefinedDevice READ UndefinedDevice)
  Q_PROPERTY(int Widget READ Widget)
  Q_PROPERTY(int Printer READ Printer)
  Q_PROPERTY(int Picture READ Picture)
  Q_PROPERTY(int DeviceTypeMask READ DeviceTypeMask)
  Q_PROPERTY(int ExternalDevice READ ExternalDevice)

  Q_ENUMS(EventType)
  Q_ENUMS(Shape)
  Q_ENUMS(Shadow)
  Q_ENUMS(CoordinateMode)
  Q_ENUMS(ColorMode)
  Q_ENUMS(Optimization)
  Q_ENUMS(PrinterMode PrinterOrientation PageSize PageOrder PrinterColorMode PaperSource PrintRange PrinterOption)
  Q_ENUMS(NodeType)
  Q_ENUMS(RegionType)
  Q_ENUMS(ColorSpec)
  Q_ENUMS(ContextMenuReason)
  Q_ENUMS(DragDropAction)
  Q_ENUMS(DragMode)
  Q_ENUMS(Endian DataStreamByteOrder)
  Q_ENUMS(ScaleMode)
  Q_ENUMS(ProcessEvents)
  Q_ENUMS(SocketNotifierType)
  Q_ENUMS(Encoding TextStreamEncoding)
  Q_ENUMS(NetState)
  Q_ENUMS(NetOperation)
  Q_ENUMS(NetConnectionState)
  Q_ENUMS(NetError)
  Q_ENUMS(DockCloseMode DockPlace DockHandlePosition DockWindows Dock)
  Q_ENUMS(IconSetSize IconSetMode IconSetState)
  Q_ENUMS(ToolButtonTextPosition)
  Q_ENUMS(Corner)
  Q_ENUMS(HttpState HttpError)
  Q_ENUMS(CanvasRttiValues CanvasFrameAnimationType)
  Q_ENUMS(RenderReportFlags)
  Q_ENUMS(AQReportsMsgType)
  Q_ENUMS(SplitterResizeMode)
  Q_ENUMS(FilterSpec SortSpec PermissionSpec)
  Q_ENUMS(Communication)
  Q_ENUMS(SizeType)
  Q_ENUMS(ZipStatus ZipCompressionPolicy)
  Q_ENUMS(OdsStyleFlags)
  Q_ENUMS(ClassFlags FunctionFlags)

public:

  enum EventType {
    None = 0,       // invalid event
    Timer = 1,        // timer event
    MouseButtonPress = 2,     // mouse button pressed
    MouseButtonRelease = 3,     // mouse button released
    MouseButtonDblClick = 4,    // mouse button double click
    MouseMove = 5,        // mouse move
    KeyPress = 6,       // key pressed
    KeyRelease = 7,       // key released
    FocusIn = 8,        // keyboard focus received
    FocusOut = 9,       // keyboard focus lost
    Enter = 10,       // mouse enters widget
    Leave = 11,       // mouse leaves widget
    Paint = 12,       // paint widget
    Move = 13,        // move widget
    Resize = 14,        // resize widget
    Create = 15,        // after object creation
    Destroy = 16,       // during object destruction
    Show = 17,        // widget is shown
    Hide = 18,        // widget is hidden
    Close = 19,       // request to close widget
    Quit = 20,        // request to quit application
    Reparent = 21,        // widget has been reparented
    ShowMinimized = 22,           // widget is shown minimized
    ShowNormal = 23,            // widget is shown normal
    WindowActivate = 24,            // window was activated
    WindowDeactivate = 25,            // window was deactivated
    ShowToParent = 26,            // widget is shown to parent
    HideToParent = 27,            // widget is hidden to parent
    ShowMaximized = 28,           // widget is shown maximized
    ShowFullScreen = 29,      // widget is shown full-screen
    Accel = 30,       // accelerator event
    Wheel = 31,       // wheel event
    AccelAvailable = 32,      // accelerator available event
    CaptionChange = 33,     // caption changed
    IconChange = 34,      // icon changed
    ParentFontChange = 35,      // parent font changed
    ApplicationFontChange = 36,   // application font changed
    ParentPaletteChange = 37,   // parent palette changed
    ApplicationPaletteChange = 38,    // application palette changed
    PaletteChange = 39,     // widget palette changed
    Clipboard = 40,       // internal clipboard event
    Speech = 42,        // reserved for speech input
    SockAct = 50,       // socket activation
    AccelOverride = 51,     // accelerator override event
    DeferredDelete = 52,      // deferred delete event
    DragEnter = 60,       // drag moves into widget
    DragMove = 61,        // drag moves in widget
    DragLeave = 62,       // drag leaves or is cancelled
    Drop = 63,        // actual drop
    DragResponse = 64,      // drag accepted/rejected
    ChildInserted = 70,     // new child widget
    ChildRemoved = 71,      // deleted child widget
    LayoutHint = 72,      // child min/max size changed
    ShowWindowRequest = 73,     // widget's window should be mapped
    WindowBlocked = 74,     // window is about to be blocked modally
    WindowUnblocked = 75,     // windows modal blocking has ended
    ActivateControl = 80,     // ActiveX activation
    DeactivateControl = 81,     // ActiveX deactivation
    ContextMenu = 82,     // context popup menu
    IMStart = 83,       // input method composition start
    IMCompose = 84,       // input method composition
    IMEnd = 85,       // input method composition end
    Accessibility = 86,     // accessibility information is requested
    TabletMove = 87,      // Wacom tablet event
    LocaleChange = 88,      // the system locale changed
    LanguageChange = 89,      // the application language changed
    LayoutDirectionChange = 90,   // the layout direction changed
    Style = 91,       // internal style event
    TabletPress = 92,     // tablet press
    TabletRelease = 93,     // tablet release
    OkRequest = 94,       // CE (Ok) button pressed
    HelpRequest = 95,     // CE (?)  button pressed
    WindowStateChange = 96,     // window state has changed
    IconDrag = 97
  };

  enum ContextMenuReason {
    Mouse,
    Keyboard,
    Other
  };

  enum Shape {
    NoFrame = 0, Box = 0x0001, Panel = 0x0002, WinPanel = 0x0003,
    HLine = 0x0004, VLine = 0x0005, StyledPanel = 0x0006, PopupPanel = 0x0007,
    MenuBarPanel = 0x0008, ToolBarPanel = 0x0009, LineEditPanel = 0x000a,
    TabWidgetPanel = 0x000b, GroupBoxPanel = 0x000c, MShape = 0x000f
  };

  enum Shadow {
    Plain = 0x0010, Raised = 0x0020, Sunken = 0x0030, MShadow = 0x00f0
  };

  enum CoordinateMode {
    CoordDevice, CoordPainter
  };

  enum ColorMode {
    Auto, Color, Mono
  };

  enum Optimization {
    DefaultOptim, NoOptim, MemoryOptim = NoOptim, NormalOptim, BestOptim
  };

  enum PrinterMode {
    ScreenResolution, PrinterResolution, HighResolution, Compatible
  };
  enum PrinterOrientation {
    Portrait, Landscape
  };
  enum PageSize {
    A4, B5, Letter, Legal, Executive, A0, A1, A2, A3,
    A5, A6, A7, A8, A9, B0, B1, B10, B2, B3, B4, B6, B7, B8, B9,
    C5E, Comm10E, DLE, Folio, Ledger, Tabloid, Custom, NPageSize = Custom
  };
  enum PageOrder {
    FirstPageFirst, LastPageFirst
  };
  enum PrinterColorMode {
    PrintGrayScale, PrintColor
  };
  enum PaperSource {
    OnlyOne, Lower, Middle, Manual, Envelope, EnvelopeManual, PrintAuto,
    Tractor, SmallFormat, LargeFormat, LargeCapacity, Cassette, FormSource
  };
  enum PrintRange {
    AllPages, Selection, PageRange
  };
  enum PrinterOption {
    PrintToFile, PrintSelection, PrintPageRange
  };

  enum NodeType {
    ElementNode = 1, AttributeNode = 2, TextNode = 3, CDATASectionNode = 4,
    EntityReferenceNode = 5, EntityNode = 6, ProcessingInstructionNode = 7, CommentNode = 8,
    DocumentNode = 9, DocumentTypeNode = 10, DocumentFragmentNode = 11, NotationNode = 12,
    BaseNode = 21, CharacterDataNode = 22
  };

  enum RegionType {
    Rectangle, Ellipse
  };

  enum ColorSpec {
    Rgb, Hsv
  };

  enum DragDropAction {
    DDCopy, DDLink, DDMove, DDPrivate, DDUserAction = 100
  };

  enum DragMode {
    DDragDefault, DDragCopy, DDragMove, DDragLink, DDragCopyOrMove
  };

  enum Endian {
    IgnoreEndian, BigEndian, LittleEndian
  };

  enum DataStreamByteOrder {
    DataStreamBigEndian, DataStreamLittleEndian
  };

  enum ScaleMode {
    ScaleFree, ScaleMin, ScaleMax
  };

  enum ProcessEvents {
    AllEvents = 0x00, ExcludeUserInput = 0x01,
    ExcludeSocketNotifiers = 0x02, WaitForMore = 0x04
  };

  enum SocketNotifierType {
    Read, Write, Exception
  };

  enum Encoding {
    DefaultCodec, UnicodeUTF8
  };

  enum TextStreamEncoding {
    Locale, Latin1, Unicode, UnicodeNetworkOrder,
    UnicodeReverse, RawUnicode, TextStreamUTF8
  };

  enum NetState {
    StWaiting = 0,
    StInProgress,
    StDone,
    StFailed,
    StStopped
  };

  enum NetOperation {
    OpListChildren = 1,
    OpMkDir = 2,
    OpMkdir = OpMkDir,
    OpRemove = 4,
    OpRename = 8,
    OpGet = 32,
    OpPut = 64
  };

  enum NetConnectionState {
    ConHostFound,
    ConConnected,
    ConClosed
  };

  enum NetError {
    // no error
    NoError = 0,
    // general errors
    ErrValid,
    ErrUnknownProtocol,
    ErrUnsupported,
    ErrParse,
    // errors on connect
    ErrLoginIncorrect,
    ErrHostNotFound,
    // protocol errors
    ErrListChildren,
    ErrListChlidren = ErrListChildren,
    ErrMkDir,
    ErrMkdir = ErrMkDir,
    ErrRemove,
    ErrRename,
    ErrGet,
    ErrPut,
    ErrFileNotExisting,
    ErrPermissionDenied
  };

  enum DockPlace {
    InDock,
    OutsideDock
  };

  enum DockCloseMode {
    Never = 0,
    Docked = 1,
    Undocked = 2,
    Always = Docked | Undocked
  };

  enum DockHandlePosition {
    Normal, Reverse
  };

  enum DockWindows {
    OnlyToolBars,
    NoToolBars,
    AllDockWindows
  };

  enum Dock {
    DockUnmanaged,
    DockTornOff,
    DockTop,
    DockBottom,
    DockRight,
    DockLeft,
    DockMinimized,
    Unmanaged = DockUnmanaged,
    TornOff = DockTornOff,
    Top = DockTop,
    Bottom = DockBottom,
    Right = DockRight,
    Left = DockLeft,
    Minimized = DockMinimized
  };

  enum IconSetSize {
    IconSetAutomatic,
    IconSetSmall,
    IconSetLarge
  };

  enum IconSetMode {
    IconSetNormal,
    IconSetDisabled,
    IconSetActive
  };

  enum IconSetState {
    IconSetOn,
    IconSetOff
  };

  enum ToolButtonTextPosition {
    BesideIcon,
    BelowIcon,
    ToolButtonRight = BesideIcon,
    ToolButtonUnder = BelowIcon
  };

  enum Corner {
    TopLeft     = 0x00000,
    TopRight    = 0x00001,
    BottomLeft  = 0x00002,
    BottomRight = 0x00003
  };

  enum HttpState {
    HttpUnconnected,
    HttpHostLookup,
    HttpConnecting,
    HttpSending,
    HttpReading,
    HttpConnected,
    HttpClosing
  };

  enum HttpError {
    HttpNoError,
    HttpUnknownError,
    HttpHostNotFound,
    HttpConnectionRefused,
    HttpUnexpectedClose,
    HttpInvalidResponseHeader,
    HttpWrongContentLength,
    HttpAborted
  };

  enum CanvasRttiValues {
    CanvasRtti_Item = 0,
    CanvasRtti_Sprite = 1,
    CanvasRtti_PolygonalItem = 2,
    CanvasRtti_Text = 3,
    CanvasRtti_Polygon = 4,
    CanvasRtti_Rectangle = 5,
    CanvasRtti_Ellipse = 6,
    CanvasRtti_Line = 7,
    CanvasRtti_Spline = 8
  };

  enum CanvasFrameAnimationType {
    CanvasCycle,
    CanvasOscillate
  };

  enum RenderReportFlags {
    Append        = 0x00000001,
    Display       = 0x00000002,
    PageBreak     = 0x00000004,
    FillRecords   = 0x00000008
  };

  enum AQReportsMsgType {
    AQErrorMsg,
    AQLogMsg,
    AQSysMsg
  };

  enum SplitterResizeMode {
    SplitterStretch,
    SplitterKeepSize,
    SplitterFollowSizeHint,
    SplitterAuto
  };

  enum PermissionSpec {
    ReadOwner = 00400,
    WriteOwner = 00200,
    ExeOwner = 00100,
    ReadGroup = 00040,
    WriteGroup = 00020,
    ExeGroup = 00010,
    ReadOther = 00004,
    WriteOther = 00002,
    ExeOther = 00001
  };

  enum FilterSpec {
    Dirs         = 0x001,
    Files        = 0x002,
    Drives       = 0x004,
    NoSymLinks   = 0x008,
    All          = 0x007,
    TypeMask     = 0x00F,
    Readable     = 0x010,
    Writable     = 0x020,
    Executable   = 0x040,
    RWEMask      = 0x070,
    Modified     = 0x080,
    Hidden       = 0x100,
    System       = 0x200,
    AccessMask   = 0x3F0,
    DefaultFilter = -1
  };

  enum SortSpec   {
    Name         = 0x00,
    Time         = 0x01,
    Size         = 0x02,
    Unsorted     = 0x03,
    SortByMask   = 0x03,
    DirsFirst    = 0x04,
    Reversed     = 0x08,
    IgnoreCase   = 0x10,
    LocaleAwar   = 0x20,
    DefaultSort  = -1
  };

  enum Communication {
    Stdin = 0x01,
    Stdout = 0x02,
    Stderr = 0x04,
    DupStderr = 0x08
  };

  enum SizePolicy_Internal {
    HSize = 6,
    HMask = 0x3f,
    VMask = HMask << HSize,
    MayGrow = 1,
    ExpMask = 2,
    MayShrink = 4
  };

  enum SizeType {
    Fixed = 0,
    Minimum = MayGrow,
    Maximum = MayShrink,
    Preferred = MayGrow | MayShrink,
    MinimumExpanding = MayGrow | ExpMask,
    Expanding = MayGrow | MayShrink | ExpMask,
    Ignored = ExpMask
  };

  enum ZipStatus {
    ZipNoError,
    ZipFileWriteError,
    ZipFileOpenError,
    ZipFilePermissionsError,
    ZipFileError
  };

  enum ZipCompressionPolicy {
    ZipAlwaysCompress,
    ZipNeverCompress,
    ZipAutoCompress
  };

  enum OdsStyleFlags {
    ODS_NONE           = 0,
    ODS_BORDER_BOTTOM  = (1 <<  0),
    ODS_BORDER_LEFT    = (1 <<  1),
    ODS_BORDER_RIGHT   = (1 <<  2),
    ODS_BORDER_TOP     = (1 <<  3),
    ODS_ALIGN_LEFT     = (1 <<  4),
    ODS_ALIGN_CENTER   = (1 <<  5),
    ODS_ALIGN_RIGHT    = (1 <<  6),
    ODS_TEXT_BOLD      = (1 <<  7),
    ODS_TEXT_ITALIC    = (1 <<  8),
    ODS_TEXT_UNDERLINE = (1 <<  9),
  };

  enum ClassFlags {
    AllClasses,
    GlobalClasses
  };

  enum FunctionFlags {
    FunctionNames = 0,
    FunctionSignatures = 1,
    IncludeMemberFunctions = 2
  };


  AQS() : QObject(0, "aqs_namespace"), objectsCount_(0) {
    AQS_IF_DEBUG(printf("Global AQS Namespace created\n"));
  }

  ~AQS() {
    AQS_IF_DEBUG(printf("Global AQS Namespace destroyed\nAQS Objects count %d\n", objectsCount_));
  }

  int NoButton() const {
    return 0x0000;
  }
  int LeftButton() const {
    return 0x0001;
  }
  int RightButton() const {
    return 0x0002;
  }
  int MidButton() const {
    return 0x0004;
  }
  int MouseButtonMask() const {
    return 0x00ff;
  }
  int ShiftButton() const {
    return 0x0100;
  }
  int ControlButton() const {
    return 0x0200;
  }
  int AltButton() const {
    return 0x0400;
  }
  int MetaButton() const {
    return 0x0800;
  }
  int KeyButtonMask() const {
    return 0x0fff;
  }
  int Horizontal() const {
    return 0;
  }
  int AlignAuto() const {
    return 0x0000;
  }
  int AlignLeft() const {
    return 0x0001;
  }
  int AlignRight() const {
    return 0x0002;
  }
  int AlignHCenter() const {
    return 0x0004;
  }
  int AlignJustify() const {
    return 0x0008;
  }
  int AlignHorizontal_Mask() const {
    return AlignLeft();
  }
  int AlignTop() const {
    return 0x0010;
  }
  int AlignBottom() const {
    return 0x0020;
  }
  int AlignVCenter() const {
    return 0x0040;
  }
  int AlignVertical_Mask() const {
    return AlignTop();
  }
  int SingleLine() const {
    return 0x0080;
  }
  int DontClip() const {
    return 0x0100;
  }
  int ExpandTabs() const {
    return 0x0200;
  }
  int ShowPrefix() const {
    return 0x0400;
  }
  int WordBreak() const {
    return 0x0800;
  }
  int BreakAnywhere() const {
    return 0x1000;
  }
  int DontPrint() const {
    return 0x2000;
  }
  int WState_Created() const {
    return 0x00000001;
  }
  int WState_Disabled() const {
    return 0x00000002;
  }
  int WState_Visible() const {
    return 0x00000004;
  }
  int WState_ForceHide() const {
    return 0x00000008;
  }
  int WState_OwnCursor() const {
    return 0x00000010;
  }
  int WState_MouseTracking() const {
    return 0x00000020;
  }
  int WState_CompressKeys() const {
    return 0x00000040;
  }
  int WState_BlockUpdates() const {
    return 0x00000080;
  }
  int WState_InPaintEvent() const {
    return 0x00000100;
  }
  int WState_Reparented() const {
    return 0x00000200;
  }
  int WState_ConfigPending() const {
    return 0x00000400;
  }
  int WState_Resized() const {
    return 0x00000800;
  }
  int WState_AutoMask() const {
    return 0x00001000;
  }
  int WState_Polished() const {
    return 0x00002000;
  }
  int WState_DND() const {
    return 0x00004000;
  }
  int WState_Reserved0() const {
    return 0x00008000;
  }
  int WState_Reserved1() const {
    return 0x00010000;
  }
  int WState_Reserved2() const {
    return 0x00020000;
  }
  int WState_Reserved3() const {
    return 0x00040000;
  }
  int WState_Maximized() const {
    return 0x00080000;
  }
  int WState_Minimized() const {
    return 0x00100000;
  }
  int WState_ForceDisabled() const {
    return 0x00200000;
  }
  int WState_Exposed() const {
    return 0x00400000;
  }
  int WType_TopLevel() const {
    return 0x00000001;
  }
  int WType_Dialog() const {
    return 0x00000002;
  }
  int WType_Popup() const {
    return 0x00000004;
  }
  int WType_Desktop() const {
    return 0x00000008;
  }
  int WType_Mask() const {
    return 0x0000000f;
  }
  int WStyle_Customize() const {
    return 0x00000010;
  }
  int WStyle_NormalBorder() const {
    return 0x00000020;
  }
  int WStyle_DialogBorder() const {
    return 0x00000040;
  }
  int WStyle_NoBorder() const {
    return 0x00002000;
  }
  int WStyle_Title() const {
    return 0x00000080;
  }
  int WStyle_SysMenu() const {
    return 0x00000100;
  }
  int WStyle_Minimize() const {
    return 0x00000200;
  }
  int WStyle_Maximize() const {
    return 0x00000400;
  }
  int WStyle_MinMax() const {
    return WStyle_Minimize();
  }
  int WStyle_Tool() const {
    return 0x00000800;
  }
  int WStyle_StaysOnTop() const {
    return 0x00001000;
  }
  int WStyle_ContextHelp() const {
    return 0x00004000;
  }
  int WStyle_Reserved() const {
    return 0x00008000;
  }
  int WStyle_Mask() const {
    return 0x0000fff0;
  }
  int WDestructiveClose() const {
    return 0x00010000;
  }
  int WPaintDesktop() const {
    return 0x00020000;
  }
  int WPaintUnclipped() const {
    return 0x00040000;
  }
  int WPaintClever() const {
    return 0x00080000;
  }
  int WResizeNoErase() const {
    return 0x00100000;
  }
  int WMouseNoMask() const {
    return 0x00200000;
  }
  int WStaticContents() const {
    return 0x00400000;
  }
  int WRepaintNoErase() const {
    return 0x00800000;
  }
  int WWinOwnDC() const {
    return 0x00000000;
  }
  int WShowModal() const {
    return 0x04000000;
  }
  int WNoMousePropagation() const {
    return 0x08000000;
  }
  int WSubWindow() const {
    return 0x10000000;
  }
  int WNorthWestGravity() const {
    return WStaticContents();
  }
  int WType_Modal() const {
    return WType_Dialog();
  }
  int WStyle_Dialog() const {
    return WType_Dialog();
  }
  int ColorMode_Mask() const {
    return 0x00000003;
  }
  int AutoColor() const {
    return 0x00000000;
  }
  int ColorOnly() const {
    return 0x00000003;
  }
  int MonoOnly() const {
    return 0x00000002;
  }
  int AlphaDither_Mask() const {
    return 0x0000000c;
  }
  int ThresholdAlphaDither() const {
    return 0x00000000;
  }
  int OrderedAlphaDither() const {
    return 0x00000004;
  }
  int DiffuseAlphaDither() const {
    return 0x00000008;
  }
  int NoAlpha() const {
    return 0x0000000c;
  }
  int Dither_Mask() const {
    return 0x00000030;
  }
  int DiffuseDither() const {
    return 0x00000000;
  }
  int OrderedDither() const {
    return 0x00000010;
  }
  int ThresholdDither() const {
    return 0x00000020;
  }
  int DitherMode_Mask() const {
    return 0x000000c0;
  }
  int AutoDither() const {
    return 0x00000000;
  }
  int PreferDither() const {
    return 0x00000040;
  }
  int TransparentMode() const {
    return 0;
  }
  int PixelUnit() const {
    return 0;
  }
  int LoMetricUnit() const {
    return 1;
  }
  int HiMetricUnit() const {
    return 2;
  }
  int LoEnglishUnit() const {
    return 3;
  }
  int HiEnglishUnit() const {
    return 4;
  }
  int WindowsStyle() const {
    return 1;
  }
  int MacStyle() const {
    return 0;
  }
  int Win3Style() const {
    return 1;
  }
  int PMStyle() const {
    return 2;
  }
  int SHIFT() const {
    return 0x00200000;
  }
  int CTRL() const {
    return 0x00400000;
  }
  int ALT() const {
    return 0x00800000;
  }
  int MODIFIER_MASK() const {
    return 0x00e00000;
  }
  int UNICODE_ACCEL() const {
    return 0x10000000;
  }
  int Key_Escape() const {
    return 0x1000;
  }
  int Key_Tab() const {
    return 0x1001;
  }
  int Key_Backtab() const {
    return 0x1002;
  }
  int Key_BackTab() const {
    return Key_Backtab();
  }
  int Key_Backspace() const {
    return 0x1003;
  }
  int Key_BackSpace() const {
    return Key_Backspace();
  }
  int Key_Return() const {
    return 0x1004;
  }
  int Key_Enter() const {
    return 0x1005;
  }
  int Key_Insert() const {
    return 0x1006;
  }
  int Key_Delete() const {
    return 0x1007;
  }
  int Key_Pause() const {
    return 0x1008;
  }
  int Key_Print() const {
    return 0x1009;
  }
  int Key_SysReq() const {
    return 0x100a;
  }
  int Key_Home() const {
    return 0x1010;
  }
  int Key_End() const {
    return 0x1011;
  }
  int Key_Left() const {
    return 0x1012;
  }
  int Key_Up() const {
    return 0x1013;
  }
  int Key_Right() const {
    return 0x1014;
  }
  int Key_Down() const {
    return 0x1015;
  }
  int Key_Prior() const {
    return 0x1016;
  }
  int Key_PageUp() const {
    return Key_Prior();
  }
  int Key_Next() const {
    return 0x1017;
  }
  int Key_PageDown() const {
    return Key_Next();
  }
  int Key_Shift() const {
    return 0x1020;
  }
  int Key_Control() const {
    return 0x1021;
  }
  int Key_Meta() const {
    return 0x1022;
  }
  int Key_Alt() const {
    return 0x1023;
  }
  int Key_CapsLock() const {
    return 0x1024;
  }
  int Key_NumLock() const {
    return 0x1025;
  }
  int Key_ScrollLock() const {
    return 0x1026;
  }
  int Key_F1() const {
    return 0x1030;
  }
  int Key_F2() const {
    return 0x1031;
  }
  int Key_F3() const {
    return 0x1032;
  }
  int Key_F4() const {
    return 0x1033;
  }
  int Key_F5() const {
    return 0x1034;
  }
  int Key_F6() const {
    return 0x1035;
  }
  int Key_F7() const {
    return 0x1036;
  }
  int Key_F8() const {
    return 0x1037;
  }
  int Key_F9() const {
    return 0x1038;
  }
  int Key_F10() const {
    return 0x1039;
  }
  int Key_F11() const {
    return 0x103a;
  }
  int Key_F12() const {
    return 0x103b;
  }
  int Key_F13() const {
    return 0x103c;
  }
  int Key_F14() const {
    return 0x103d;
  }
  int Key_F15() const {
    return 0x103e;
  }
  int Key_F16() const {
    return 0x103f;
  }
  int Key_F17() const {
    return 0x1040;
  }
  int Key_F18() const {
    return 0x1041;
  }
  int Key_F19() const {
    return 0x1042;
  }
  int Key_F20() const {
    return 0x1043;
  }
  int Key_F21() const {
    return 0x1044;
  }
  int Key_F22() const {
    return 0x1045;
  }
  int Key_F23() const {
    return 0x1046;
  }
  int Key_F24() const {
    return 0x1047;
  }
  int Key_F25() const {
    return 0x1048;
  }
  int Key_F26() const {
    return 0x1049;
  }
  int Key_F27() const {
    return 0x104a;
  }
  int Key_F28() const {
    return 0x104b;
  }
  int Key_F29() const {
    return 0x104c;
  }
  int Key_F30() const {
    return 0x104d;
  }
  int Key_F31() const {
    return 0x104e;
  }
  int Key_F32() const {
    return 0x104f;
  }
  int Key_F33() const {
    return 0x1050;
  }
  int Key_F34() const {
    return 0x1051;
  }
  int Key_F35() const {
    return 0x1052;
  }
  int Key_Super_L() const {
    return 0x1053;
  }
  int Key_Super_R() const {
    return 0x1054;
  }
  int Key_Menu() const {
    return 0x1055;
  }
  int Key_Hyper_L() const {
    return 0x1056;
  }
  int Key_Hyper_R() const {
    return 0x1057;
  }
  int Key_Help() const {
    return 0x1058;
  }
  int Key_Direction_L() const {
    return 0x1059;
  }
  int Key_Direction_R() const {
    return 0x1060;
  }
  int Key_Space() const {
    return 0x20;
  }
  int Key_Any() const {
    return Key_Space();
  }
  int Key_Exclam() const {
    return 0x21;
  }
  int Key_QuoteDbl() const {
    return 0x22;
  }
  int Key_NumberSign() const {
    return 0x23;
  }
  int Key_Dollar() const {
    return 0x24;
  }
  int Key_Percent() const {
    return 0x25;
  }
  int Key_Ampersand() const {
    return 0x26;
  }
  int Key_Apostrophe() const {
    return 0x27;
  }
  int Key_ParenLeft() const {
    return 0x28;
  }
  int Key_ParenRight() const {
    return 0x29;
  }
  int Key_Asterisk() const {
    return 0x2a;
  }
  int Key_Plus() const {
    return 0x2b;
  }
  int Key_Comma() const {
    return 0x2c;
  }
  int Key_Minus() const {
    return 0x2d;
  }
  int Key_Period() const {
    return 0x2e;
  }
  int Key_Slash() const {
    return 0x2f;
  }
  int Key_0() const {
    return 0x30;
  }
  int Key_1() const {
    return 0x31;
  }
  int Key_2() const {
    return 0x32;
  }
  int Key_3() const {
    return 0x33;
  }
  int Key_4() const {
    return 0x34;
  }
  int Key_5() const {
    return 0x35;
  }
  int Key_6() const {
    return 0x36;
  }
  int Key_7() const {
    return 0x37;
  }
  int Key_8() const {
    return 0x38;
  }
  int Key_9() const {
    return 0x39;
  }
  int Key_Colon() const {
    return 0x3a;
  }
  int Key_Semicolon() const {
    return 0x3b;
  }
  int Key_Less() const {
    return 0x3c;
  }
  int Key_Equal() const {
    return 0x3d;
  }
  int Key_Greater() const {
    return 0x3e;
  }
  int Key_Question() const {
    return 0x3f;
  }
  int Key_At() const {
    return 0x40;
  }
  int Key_A() const {
    return 0x41;
  }
  int Key_B() const {
    return 0x42;
  }
  int Key_C() const {
    return 0x43;
  }
  int Key_D() const {
    return 0x44;
  }
  int Key_E() const {
    return 0x45;
  }
  int Key_F() const {
    return 0x46;
  }
  int Key_G() const {
    return 0x47;
  }
  int Key_H() const {
    return 0x48;
  }
  int Key_I() const {
    return 0x49;
  }
  int Key_J() const {
    return 0x4a;
  }
  int Key_K() const {
    return 0x4b;
  }
  int Key_L() const {
    return 0x4c;
  }
  int Key_M() const {
    return 0x4d;
  }
  int Key_N() const {
    return 0x4e;
  }
  int Key_O() const {
    return 0x4f;
  }
  int Key_P() const {
    return 0x50;
  }
  int Key_Q() const {
    return 0x51;
  }
  int Key_R() const {
    return 0x52;
  }
  int Key_S() const {
    return 0x53;
  }
  int Key_T() const {
    return 0x54;
  }
  int Key_U() const {
    return 0x55;
  }
  int Key_V() const {
    return 0x56;
  }
  int Key_W() const {
    return 0x57;
  }
  int Key_X() const {
    return 0x58;
  }
  int Key_Y() const {
    return 0x59;
  }
  int Key_Z() const {
    return 0x5a;
  }
  int Key_BracketLeft() const {
    return 0x5b;
  }
  int Key_Backslash() const {
    return 0x5c;
  }
  int Key_BracketRight() const {
    return 0x5d;
  }
  int Key_AsciiCircum() const {
    return 0x5e;
  }
  int Key_Underscore() const {
    return 0x5f;
  }
  int Key_QuoteLeft() const {
    return 0x60;
  }
  int Key_BraceLeft() const {
    return 0x7b;
  }
  int Key_Bar() const {
    return 0x7c;
  }
  int Key_BraceRight() const {
    return 0x7d;
  }
  int Key_AsciiTilde() const {
    return 0x7e;
  }
  int Key_nobreakspace() const {
    return 0x0a0;
  }
  int Key_exclamdown() const {
    return 0x0a1;
  }
  int Key_cent() const {
    return 0x0a2;
  }
  int Key_sterling() const {
    return 0x0a3;
  }
  int Key_currency() const {
    return 0x0a4;
  }
  int Key_yen() const {
    return 0x0a5;
  }
  int Key_brokenbar() const {
    return 0x0a6;
  }
  int Key_section() const {
    return 0x0a7;
  }
  int Key_diaeresis() const {
    return 0x0a8;
  }
  int Key_copyright() const {
    return 0x0a9;
  }
  int Key_ordfeminine() const {
    return 0x0aa;
  }
  int Key_guillemotleft() const {
    return 0x0ab;
  }
  int Key_notsign() const {
    return 0x0ac;
  }
  int Key_hyphen() const {
    return 0x0ad;
  }
  int Key_registered() const {
    return 0x0ae;
  }
  int Key_macron() const {
    return 0x0af;
  }
  int Key_degree() const {
    return 0x0b0;
  }
  int Key_plusminus() const {
    return 0x0b1;
  }
  int Key_twosuperior() const {
    return 0x0b2;
  }
  int Key_threesuperior() const {
    return 0x0b3;
  }
  int Key_acute() const {
    return 0x0b4;
  }
  int Key_mu() const {
    return 0x0b5;
  }
  int Key_paragraph() const {
    return 0x0b6;
  }
  int Key_periodcentered() const {
    return 0x0b7;
  }
  int Key_cedilla() const {
    return 0x0b8;
  }
  int Key_onesuperior() const {
    return 0x0b9;
  }
  int Key_masculine() const {
    return 0x0ba;
  }
  int Key_guillemotright() const {
    return 0x0bb;
  }
  int Key_onequarter() const {
    return 0x0bc;
  }
  int Key_onehalf() const {
    return 0x0bd;
  }
  int Key_threequarters() const {
    return 0x0be;
  }
  int Key_questiondown() const {
    return 0x0bf;
  }
  int Key_Agrave() const {
    return 0x0c0;
  }
  int Key_Aacute() const {
    return 0x0c1;
  }
  int Key_Acircumflex() const {
    return 0x0c2;
  }
  int Key_Atilde() const {
    return 0x0c3;
  }
  int Key_Adiaeresis() const {
    return 0x0c4;
  }
  int Key_Aring() const {
    return 0x0c5;
  }
  int Key_AE() const {
    return 0x0c6;
  }
  int Key_Ccedilla() const {
    return 0x0c7;
  }
  int Key_Egrave() const {
    return 0x0c8;
  }
  int Key_Eacute() const {
    return 0x0c9;
  }
  int Key_Ecircumflex() const {
    return 0x0ca;
  }
  int Key_Ediaeresis() const {
    return 0x0cb;
  }
  int Key_Igrave() const {
    return 0x0cc;
  }
  int Key_Iacute() const {
    return 0x0cd;
  }
  int Key_Icircumflex() const {
    return 0x0ce;
  }
  int Key_Idiaeresis() const {
    return 0x0cf;
  }
  int Key_ETH() const {
    return 0x0d0;
  }
  int Key_Ntilde() const {
    return 0x0d1;
  }
  int Key_Ograve() const {
    return 0x0d2;
  }
  int Key_Oacute() const {
    return 0x0d3;
  }
  int Key_Ocircumflex() const {
    return 0x0d4;
  }
  int Key_Otilde() const {
    return 0x0d5;
  }
  int Key_Odiaeresis() const {
    return 0x0d6;
  }
  int Key_multiply() const {
    return 0x0d7;
  }
  int Key_Ooblique() const {
    return 0x0d8;
  }
  int Key_Ugrave() const {
    return 0x0d9;
  }
  int Key_Uacute() const {
    return 0x0da;
  }
  int Key_Ucircumflex() const {
    return 0x0db;
  }
  int Key_Udiaeresis() const {
    return 0x0dc;
  }
  int Key_Yacute() const {
    return 0x0dd;
  }
  int Key_THORN() const {
    return 0x0de;
  }
  int Key_ssharp() const {
    return 0x0df;
  }
  int Key_agrave() const {
    return 0x0e0;
  }
  int Key_aacute() const {
    return 0x0e1;
  }
  int Key_acircumflex() const {
    return 0x0e2;
  }
  int Key_atilde() const {
    return 0x0e3;
  }
  int Key_adiaeresis() const {
    return 0x0e4;
  }
  int Key_aring() const {
    return 0x0e5;
  }
  int Key_ae() const {
    return 0x0e6;
  }
  int Key_ccedilla() const {
    return 0x0e7;
  }
  int Key_egrave() const {
    return 0x0e8;
  }
  int Key_eacute() const {
    return 0x0e9;
  }
  int Key_ecircumflex() const {
    return 0x0ea;
  }
  int Key_ediaeresis() const {
    return 0x0eb;
  }
  int Key_igrave() const {
    return 0x0ec;
  }
  int Key_iacute() const {
    return 0x0ed;
  }
  int Key_icircumflex() const {
    return 0x0ee;
  }
  int Key_idiaeresis() const {
    return 0x0ef;
  }
  int Key_eth() const {
    return 0x0f0;
  }
  int Key_ntilde() const {
    return 0x0f1;
  }
  int Key_ograve() const {
    return 0x0f2;
  }
  int Key_oacute() const {
    return 0x0f3;
  }
  int Key_ocircumflex() const {
    return 0x0f4;
  }
  int Key_otilde() const {
    return 0x0f5;
  }
  int Key_odiaeresis() const {
    return 0x0f6;
  }
  int Key_division() const {
    return 0x0f7;
  }
  int Key_oslash() const {
    return 0x0f8;
  }
  int Key_ugrave() const {
    return 0x0f9;
  }
  int Key_uacute() const {
    return 0x0fa;
  }
  int Key_ucircumflex() const {
    return 0x0fb;
  }
  int Key_udiaeresis() const {
    return 0x0fc;
  }
  int Key_yacute() const {
    return 0x0fd;
  }
  int Key_thorn() const {
    return 0x0fe;
  }
  int Key_ydiaeresis() const {
    return 0x0ff;
  }
  int Key_Back() const {
    return 0x1061;
  }
  int Key_Forward() const {
    return 0x1062;
  }
  int Key_Stop() const {
    return 0x1063;
  }
  int Key_Refresh() const {
    return 0x1064;
  }
  int Key_VolumeDown() const {
    return 0x1070;
  }
  int Key_VolumeMute() const {
    return 0x1071;
  }
  int Key_VolumeUp() const {
    return 0x1072;
  }
  int Key_BassBoost() const {
    return 0x1073;
  }
  int Key_BassUp() const {
    return 0x1074;
  }
  int Key_BassDown() const {
    return 0x1075;
  }
  int Key_TrebleUp() const {
    return 0x1076;
  }
  int Key_TrebleDown() const {
    return 0x1077;
  }
  int Key_MediaPlay() const {
    return 0x1080;
  }
  int Key_MediaStop() const {
    return 0x1081;
  }
  int Key_MediaPrev() const {
    return 0x1082;
  }
  int Key_MediaNext() const {
    return 0x1083;
  }
  int Key_MediaRecord() const {
    return 0x1084;
  }
  int Key_HomePage() const {
    return 0x1090;
  }
  int Key_Favorites() const {
    return 0x1091;
  }
  int Key_Search() const {
    return 0x1092;
  }
  int Key_Standby() const {
    return 0x1093;
  }
  int Key_OpenUrl() const {
    return 0x1094;
  }
  int Key_LaunchMail() const {
    return 0x10a0;
  }
  int Key_LaunchMedia() const {
    return 0x10a1;
  }
  int Key_Launch0() const {
    return 0x10a2;
  }
  int Key_Launch1() const {
    return 0x10a3;
  }
  int Key_Launch2() const {
    return 0x10a4;
  }
  int Key_Launch3() const {
    return 0x10a5;
  }
  int Key_Launch4() const {
    return 0x10a6;
  }
  int Key_Launch5() const {
    return 0x10a7;
  }
  int Key_Launch6() const {
    return 0x10a8;
  }
  int Key_Launch7() const {
    return 0x10a9;
  }
  int Key_Launch8() const {
    return 0x10aa;
  }
  int Key_Launch9() const {
    return 0x10ab;
  }
  int Key_LaunchA() const {
    return 0x10ac;
  }
  int Key_LaunchB() const {
    return 0x10ad;
  }
  int Key_LaunchC() const {
    return 0x10ae;
  }
  int Key_LaunchD() const {
    return 0x10af;
  }
  int Key_LaunchE() const {
    return 0x10b0;
  }
  int Key_LaunchF() const {
    return 0x10b1;
  }
  int Key_MediaLast() const {
    return 0x1fff;
  }
  int UpArrow() const {
    return 0;
  }
  int DownArrow() const {
    return 1;
  }
  int LeftArrow() const {
    return 2;
  }
  int CopyROP() const {
    return 0;
  }
  int OrROP() const {
    return 1;
  }
  int XorROP() const {
    return 2;
  }
  int NotAndROP() const {
    return 3;
  }
  int EraseROP() const {
    return NotAndROP();
  }
  int NotCopyROP() const {
    return 1;
  }
  int NotOrROP() const {
    return 2;
  }
  int NotXorROP() const {
    return 3;
  }
  int AndROP() const {
    return 4;
  }
  int NotEraseROP() const {
    return AndROP();
  }
  int NotROP() const {
    return 1;
  }
  int ClearROP() const {
    return 2;
  }
  int SetROP() const {
    return 3;
  }
  int NopROP() const {
    return 4;
  }
  int AndNotROP() const {
    return 5;
  }
  int OrNotROP() const {
    return 6;
  }
  int NandROP() const {
    return 7;
  }
  int NorROP() const {
    return 8;
  }
  int NoPen() const {
    return 0;
  }
  int SolidLine() const {
    return 1;
  }
  int DashLine() const {
    return 2;
  }
  int DotLine() const {
    return 3;
  }
  int DashDotLine() const {
    return 4;
  }
  int DashDotDotLine() const {
    return 5;
  }
  int FlatCap() const {
    return 0x00;
  }
  int SquareCap() const {
    return 0x10;
  }
  int RoundCap() const {
    return 0x20;
  }
  int MiterJoin() const {
    return 0x00;
  }
  int BevelJoin() const {
    return 0x40;
  }
  int RoundJoin() const {
    return 0x80;
  }
  int NoBrush() const {
    return 0;
  }
  int SolidPattern() const {
    return 1;
  }
  int Dense1Pattern() const {
    return 2;
  }
  int Dense2Pattern() const {
    return 3;
  }
  int Dense3Pattern() const {
    return 4;
  }
  int Dense4Pattern() const {
    return 5;
  }
  int Dense5Pattern() const {
    return 6;
  }
  int Dense6Pattern() const {
    return 7;
  }
  int Dense7Pattern() const {
    return 8;
  }
  int HorPattern() const {
    return 9;
  }
  int VerPattern() const {
    return 10;
  }
  int CrossPattern() const {
    return 11;
  }
  int BDiagPattern() const {
    return 12;
  }
  int FDiagPattern() const {
    return 13;
  }
  int DiagCrossPattern() const {
    return 14;
  }
  int WV_32s() const {
    return 0x0001;
  }
  int WV_95() const {
    return 0x0002;
  }
  int WV_98() const {
    return 0x0003;
  }
  int WV_Me() const {
    return 0x0004;
  }
  int WV_DOS_based() const {
    return 0x000f;
  }
  int WV_NT() const {
    return 0x0010;
  }
  int WV_2000() const {
    return 0x0020;
  }
  int WV_XP() const {
    return 0x0030;
  }
  int UI_General() const {
    return 0;
  }
  int UI_AnimateMenu() const {
    return 1;
  }
  int UI_FadeMenu() const {
    return 2;
  }
  int UI_AnimateCombo() const {
    return 3;
  }
  int UI_AnimateTooltip() const {
    return 4;
  }
  int ArrowCursor() const {
    return 0;
  }
  int UpArrowCursor() const {
    return 1;
  }
  int CrossCursor() const {
    return 2;
  }
  int WaitCursor() const {
    return 3;
  }
  int IbeamCursor() const {
    return 4;
  }
  int SizeVerCursor() const {
    return 5;
  }
  int SizeHorCursor() const {
    return 6;
  }
  int SizeBDiagCursor() const {
    return 7;
  }
  int SizeFDiagCursor() const {
    return 8;
  }
  int SizeAllCursor() const {
    return 9;
  }
  int BlankCursor() const {
    return 10;
  }
  int SplitVCursor() const {
    return 11;
  }
  int SplitHCursor() const {
    return 12;
  }
  int PointingHandCursor() const {
    return 13;
  }
  int ForbiddenCursor() const {
    return 14;
  }
  int WhatsThisCursor() const {
    return 15;
  }
  int LastCursor() const {
    return WhatsThisCursor();
  }
  int PlainText() const {
    return 0;
  }
  int RichText() const {
    return 1;
  }
  int TextDate() const {
    return 0;
  }
  int ISODate() const {
    return 1;
  }
  int FixedColor() const {
    return 0;
  }
  int FixedPixmap() const {
    return 1;
  }
  int NoBackground() const {
    return 2;
  }
  int PaletteForeground() const {
    return 3;
  }
  int PaletteButton() const {
    return 4;
  }
  int PaletteLight() const {
    return 5;
  }
  int PaletteMidlight() const {
    return 6;
  }
  int PaletteDark() const {
    return 7;
  }
  int PaletteMid() const {
    return 8;
  }
  int PaletteText() const {
    return 9;
  }
  int PaletteBrightText() const {
    return 10;
  }
  int PaletteBase() const {
    return 11;
  }
  int PaletteBackground() const {
    return 12;
  }
  int PaletteShadow() const {
    return 13;
  }
  int PaletteHighlight() const {
    return 14;
  }
  int PaletteHighlightedText() const {
    return 15;
  }
  int PaletteButtonText() const {
    return 16;
  }
  int PaletteLink() const {
    return 17;
  }
  int PaletteLinkVisited() const {
    return 18;
  }
  int CaseSensitive() const {
    return 0x00001;
  }
  int BeginsWith() const {
    return 0x00002;
  }
  int EndsWith() const {
    return 0x00004;
  }
  int Contains() const {
    return 0x00008;
  }
  int UndefinedDevice() const {
    return 0x00;
  }
  int Widget() const {
    return 0x01;
  }
  int Printer() const {
    return 0x03;
  }
  int Picture() const {
    return 0x04;
  }
  int DeviceTypeMask() const {
    return 0x0f;
  }
  int ExternalDevice() const {
    return 0x10;
  }

  int objectsCount() const {
    return objectsCount_;
  }

  inline AQSBaseObject *findAQSObject(void *ptr) const;

public slots:
  QColor ColorDialog_getColor(const QColor &initial = white,
                              QWidget *parent = 0,
                              const QString &name = QString::null) {
    return QColorDialog::getColor(initial, parent, name);
  }
  QColor ColorDialog_getColor(QColor *initial,
                              QWidget *parent = 0,
                              const QString &name = QString::null) {
    return QColorDialog::getColor(*initial, parent, name);
  }
  int ColorDialog_customCount() {
    return QColorDialog::customCount();
  }
  QRgb ColorDialog_customColor(int i) {
    return QColorDialog::customColor(i);
  }
  void ColorDialog_setCustomColor(int i, QRgb c) {
    QColorDialog::setCustomColor(i, c);
  }
  void ColorDialog_setStandardColor(int i, QRgb c) {
    QColorDialog::setStandardColor(i, c);
  }

  QFont FontDialog_getFont(const QFont &initial,
                           QWidget *parent = 0,
                           const QString &name = QString::null) {
    QFontDialog::getFont(0, initial, parent, name);
  }
  QFont FontDialog_getFont(QWidget *parent = 0,
                           const QString &name = QString::null) {
    QFontDialog::getFont(0, parent, name);
  }

  QWidget *DragObject_target() {
    return QDragObject::target();
  }

  void IconSet_setIconSize(QIconSet::Size which, const QSize &size) {
    QIconSet::setIconSize(which, size);
  }
  const QSize &IconSet_iconSize(QIconSet::Size which) {
    return QIconSet::iconSize(which);
  }

  QPixmap Pixmap_fromMimeSource(const QString &abs_name) {
    return QPixmap::fromMimeSource(abs_name);
  }
  QPixmap Pixmap_grabWindow(WId window, int x = 0, int y = 0, int w = -1, int h = -1) {
    return QPixmap::grabWindow(window, x, y, w, h);
  }
  QPixmap Pixmap_grabWidget(QWidget *widget, int x = 0, int y = 0, int w = -1, int h = -1) {
    return QPixmap::grabWidget(widget, x, y, w, h);
  }
  QString Pixmap_imageFormat(const QString &fileName) {
    return QPixmap::imageFormat(fileName);
  }

  QImage Image_fromMimeSource(const QString &abs_name) {
    return QImage::fromMimeSource(abs_name);
  }
  uint Image_systemBitOrder() {
    return QImage::systemBitOrder();
  }
  uint Image_systemByteOrder() {
    return QImage::systemByteOrder();
  }
  QString Image_imageFormat(const QString &fileName) {
    return QImage::imageFormat(fileName);
  }
  QStringList Image_inputFormatList() {
    return QImage::inputFormatList();
  }
  QStringList Image_outputFormatList() {
    return QImage::outputFormatList();
  }
  void Image_bitBlt(QImage *dst, int dx, int dy, QImage *src,
                    int sx = 0, int sy = 0, int sw = -1, int sh = -1,
                    int conversion_flags = 0) {
    bitBlt(dst, dx, dy, src, sx, sy, sw, sh, conversion_flags);
  }

  void PaintDevice_bitBlt(QPaintDevice *dst, int dx, int dy, QPaintDevice *src,
                          int sx = 0, int sy = 0, int sw = -1, int sh = -1,
                          int rop = Qt::CopyROP, bool ignoreMask = false) {
    bitBlt(dst, dx, dy, src, sx, sy, sw, sh, (Qt::RasterOp)rop, ignoreMask);
  }
  void PaintDevice_bitBlt(QPaintDevice *dst, const QPoint &dp,
                          QPaintDevice *src, const QRect &sr = QRect(0, 0, -1, -1),
                          int rop = Qt::CopyROP, bool ignoreMask = false) {
    bitBlt(dst, dp, src, sr, (Qt::RasterOp)rop, ignoreMask);
  }

  bool ColorDrag_canDecode(QMimeSource *e) {
    return QColorDrag::canDecode(e);
  }
  bool ColorDrag_decode(QMimeSource *e, AQSColor *col);
  bool IconDrag_canDecode(QMimeSource *e) {
    return QIconDrag::canDecode(e);
  }
  bool ImageDrag_canDecode(QMimeSource *e) {
    return QImageDrag::canDecode(e);
  }
  bool ImageDrag_decode(QMimeSource *e, AQSImage *img);
  bool ImageDrag_decode(QMimeSource *e, AQSPixmap *pm);
  bool TextDrag_canDecode(QMimeSource *e) {
    return QTextDrag::canDecode(e);
  }
  bool TextDrag_decode(QMimeSource *e, QObject *o) {
    if (!o)
      return false;
    QString str;
    bool ret = QTextDrag::decode(e, str);
    o->setName(str);
    return ret;
  }
  bool UriDrag_canDecode(QMimeSource *e) {
    return QUriDrag::canDecode(e);
  }
  bool UriDrag_decodeToUnicodeUris(QMimeSource *e, QObject *o) {
    if (!o)
      return false;
    QStringList l;
    bool ret = QUriDrag::decodeToUnicodeUris(e, l);
    o->setName(l.join(","));
    return ret;
  }
  bool UriDrag_decodeLocalFiles(QMimeSource *e, QObject *o) {
    if (!o)
      return false;
    QStringList l;
    bool ret = QUriDrag::decodeLocalFiles(e, l);
    o->setName(l.join(","));
    return ret;
  }

  QTextCodec *TextCodec_loadCharmap(QIODevice *iod) {
    return QTextCodec::loadCharmap(iod);
  }

  QTextCodec *TextCodec_loadCharmapFile(QString filename) {
    return QTextCodec::loadCharmapFile(filename);
  }

  QTextCodec *TextCodec_codecForMib(int mib) {
    return QTextCodec::codecForMib(mib);
  }

  QTextCodec *TextCodec_codecForName(const char *name, int accuracy = 0) {
    return QTextCodec::codecForName(name, accuracy);
  }

  QTextCodec *TextCodec_codecForContent(const char *chars, int len) {
    return QTextCodec::codecForContent(chars, len);
  }

  QTextCodec *TextCodec_codecForIndex(int i) {
    return QTextCodec::codecForIndex(i);
  }

  QTextCodec *TextCodec_codecForLocale() {
    return QTextCodec::codecForLocale();
  }

  void TextCodec_setCodecForLocale(QTextCodec *c) {
    QTextCodec::setCodecForLocale(c);
  }

  QTextCodec *TextCodec_codecForTr() {
    return QTextCodec::codecForTr();
  }

  void TextCodec_setCodecForTr(QTextCodec *c) {
    QTextCodec::setCodecForTr(c);
  }

  QTextCodec *TextCodec_codecForCStrings() {
    return QTextCodec::codecForCStrings();
  }

  void TextCodec_setCodecForCStrings(QTextCodec *c) {
    QTextCodec::setCodecForCStrings(c);
  }

  void TextCodec_deleteAllCodecs() {
    QTextCodec::deleteAllCodecs();
  }

  const char *TextCodec_locale() {
    return QTextCodec::locale();
  }

  void ToolTip_add(QWidget *widget, const QString &text) {
    QToolTip::add(widget, text);
  }

  QString Url_decode(QString &url) {
    QUrl::decode(url);
    return url;
  }

  QString Url_encode(QString &url) {
    QUrl::encode(url);
    return url;
  }

  bool Url_isRelativeUrl(const QString &url) {
    return QUrl::isRelativeUrl(url);
  }

  QNetworkProtocol *NetworkProtocol_getNetworkProtocol(const QString &protocol) {
    return QNetworkProtocol::getNetworkProtocol(protocol);
  }

  bool NetworkProtocol_hasOnlyLocalFileSystem() {
    return QNetworkProtocol::hasOnlyLocalFileSystem();
  }

  bool UrlInfo_greaterThan(QUrlInfo *i1, QUrlInfo *i2, int sortBy) {
    return (i1 && i2) ? QUrlInfo::greaterThan(*i1, *i2, sortBy) : false;
  }
  bool UrlInfo_greaterThan(AQSUrlInfo *i1, AQSUrlInfo *i2, int sortBy);

  bool UrlInfo_lessThan(QUrlInfo *i1, QUrlInfo *i2, int sortBy) {
    return (i1 && i2) ? QUrlInfo::lessThan(*i1, *i2, sortBy) : false;
  }
  bool UrlInfo_lessThan(AQSUrlInfo *i1, AQSUrlInfo *i2, int sortBy);

  bool UrlInfo_equal(QUrlInfo *i1, QUrlInfo *i2, int sortBy) {
    return (i1 && i2) ? QUrlInfo::equal(*i1, *i2, sortBy) : true;
  }
  bool UrlInfo_equal(AQSUrlInfo *i1, AQSUrlInfo *i2, int sortBy);

  QString createUuid() const {
    return QUuid::createUuid().toString();
  }

  QDomNode *toXml(AQSObject *o, bool includeChildren = true,
                  bool includeComplexTypes = false) const;
  QDomNode *toXml(QObject *o, bool includeChildren = true,
                  bool includeComplexTypes = false) const;

  QByteArray toHex(QByteArray *ba) const;
  QByteArray fromHex(QByteArray *ba) const;
  QByteArray toBase64(QByteArray *ba) const;
  QByteArray fromBase64(QByteArray *ba) const;
  QByteArray compress(QByteArray *ba) const;
  QByteArray uncompress(QByteArray *ba) const;
  QByteArray encryptInternal(QByteArray *ba) const;
  QByteArray decryptInternal(QByteArray *ba) const;
  QString sha1(QByteArray *ba) const;

  int xsltproc(const QStringList &args) const;
  QByteArray xsltproc(QByteArray *xslt, QByteArray *xml) const;

private:

  inline void insertAQSObject(AQSBaseObject *aqo);
  inline void removeAQSObject(AQSBaseObject *aqo);

  int objectsCount_;
  QPtrDict<AQSBaseObject> objectsCache_;

  friend class AQSBaseObject;
};

#define AQ_QUOTEME(x) #x
#define AQ_CALL_VOID(F) if (o_) o_->F
#define AQ_CALL_RET(F) return (o_ ? o_->F : 0)
#define AQ_CALL_RET_V(F,V) return (o_ ? o_->F : V())
#define AQ_CALL_RET_PTR(F,V) return (o_ ? new V ( o_->F ) : 0)

#define AQ_STATIC_CONSTRUCTOR(Prefix,Class) \
  static Prefix##Class *Prefix##Class##_(const QSArgumentList &args) { \
    Prefix##Class *co = reinterpret_cast<Prefix##Class *>(specializedConstruct(construct(args))); \
    if (!co) { \
      AQS_IF_DEBUG(printf("Failed to construct %s(%s)\n", \
                          AQ_QUOTEME(Prefix##Class), argsSignature(args).latin1())); \
    } \
    return co; \
  }

#define AQ_SKIP_DECLARE(Class,BaseClass)

#define AQ_DECLARE_AQS(Class) \
  private: \
  void init(void *vo) { \
    if (!vo) { \
      o_ = 0; \
      AQS_IF_DEBUG(printf("%s\n", \
                          AQ_QUOTEME(AQS##Class must be initialized with a valid pointer) \
                         )); \
    } else { \
      internalInit(vo); \
      AQS_IF_DEBUG(printf("%s init %p\n", AQ_QUOTEME(AQS##Class), o_)); \
      specializedInternalInit(); \
    } \
  } \
  void init(QObject *qo) { \
    if (!qo) { \
      o_ = 0; \
      AQS_IF_DEBUG(printf("%s\n", \
                          AQ_QUOTEME(AQS##Class must be initialized with a valid QObject) \
                         )); \
    } else { \
      internalInit(qo); \
      AQS_IF_DEBUG(printf("%s init %p\n", AQ_QUOTEME(AQS##Class), o_)); \
      specializedInternalInit(); \
    } \
  } \
  void finish() { \
    if (!finished_) { \
      AQS_IF_DEBUG(printf("%s finish %p\n", AQ_QUOTEME(AQS##Class), o_)); \
      finished_ = true; \
      specializedInternalFinish(); \
    } \
  } \
  public: \
  virtual const char *RTTI() const { return #Class; } \
  AQS##Class() : AQSBaseObject (), o_(0) {} \
  virtual ~AQS##Class() { \
    finish(); \
  }

static inline QString argsSignature(const QSArgumentList &args)
{
  QString ret;
  for (QSArgumentList::const_iterator it = args.begin(); it != args.end(); ++it) {
    QSArgument arg(*it);

    if (!ret.isEmpty())
      ret.append(",");

    if (arg.type() == QSArgument::QObjectPtr || arg.type() == QSArgument::VoidPointer) {
      QString typeName(arg.typeName());
      if (typeName.startsWith("AQS")) {
        if (globalAQSFactory->instanceDescriptors()[typeName].isEmpty())
          typeName = QString::fromLatin1("AQ") + typeName.mid(3);
      }
      ret.append(typeName);
      ret.append("*");
    } else
      ret.append(arg.typeName());
  }
  return ret;
}

static inline bool canCastSignature(const QString &signature, const QString &candidate)
{
  QStringList splitSignature(QStringList::split(",", signature, true));
  QStringList splitCandidate(QStringList::split(",", candidate, true));

  int argsCount = splitSignature.size();
  if (argsCount != splitCandidate.size())
    return false;

  for (int i = 0; i < argsCount; ++i) {
    QString cdt(splitCandidate[i]);
    QString sgt(splitSignature[i]);

    if (cdt == sgt)
      continue;

    bool isCdtPtr = (cdt.at(cdt.length() - 1) == '*');
    bool isSgtPtr = (sgt.at(sgt.length() - 1) == '*');

    if (isCdtPtr && !isSgtPtr && cdt.left(cdt.length() - 1) == sgt)
      continue;
    if (sgt.isEmpty() && isCdtPtr)
      continue;
    if (sgt == "Pointer*" && isCdtPtr)
      continue;
    if (!isCdtPtr && !isSgtPtr) {
      if ((sgt == "double" || sgt == "bool" || sgt == "char") &&
          (cdt.contains("::") || cdt == "int" || cdt == "uint" ||
           cdt == "float" || cdt == "bool" || cdt == "char" ||
           cdt == "short" || cdt == "ushort" || cdt == "long" ||
           cdt == "ulong" || cdt == "WFlags"))
        continue;
      return false;
    }
    if (!isCdtPtr || !isSgtPtr)
      return false;

    QMetaObject *meta = QMetaObject::metaObject(sgt.left(sgt.length() - 1).latin1());
    if (!meta || !meta->inherits(cdt.left(cdt.length() - 1).latin1()))
      return false;
  }

  return true;
}

static inline QString castArgsSignature(const QSArgumentList &args,
                                        const QMap<int, QStringList> &candidates)
{
  int argsCount = args.count();
  if (argsCount == 0 || !candidates.contains(argsCount))
    return QString::null;

  QStringList candidate(candidates[argsCount]);
  QString signature(argsSignature(args));

  if (candidate.contains(signature))
    return signature;

  for (QStringList::const_iterator it = candidate.begin(); it != candidate.end(); ++it) {
    if (canCastSignature(signature, *it)) {
      AQS_IF_DEBUG(printf("Auto Cast %s --> %s\n", signature.latin1(), (*it).latin1()));
      return *it;
    }
  }

  return QString::null;
}

static inline QByteArray byteArrayToHex(QByteArray *ba)
{
  QByteArray hex(ba->size() * 2);
  char *hexData = hex.data();
  const uchar *data = (const uchar *)ba->data();
  for (int i = 0; i < ba->size(); ++i) {
    int j = (data[i] >> 4) & 0xf;
    if (j <= 9)
      hexData[i * 2] = (j + '0');
    else
      hexData[i * 2] = (j + 'a' - 10);
    j = data[i] & 0xf;
    if (j <= 9)
      hexData[i * 2 + 1] = (j + '0');
    else
      hexData[i * 2 + 1] = (j + 'a' - 10);
  }
  return hex;
}

static inline QByteArray byteArrayFromHex(QByteArray *ba)
{
  QByteArray res((ba->size() + 1) / 2);
  uchar *result = (uchar *)res.data() + res.size();
  bool odd_digit = true;
  for (int i = ba->size() - 1; i >= 0; --i) {
    int ch = ba->at(i);
    int tmp;
    if (ch >= '0' && ch <= '9')
      tmp = ch - '0';
    else if (ch >= 'a' && ch <= 'f')
      tmp = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
      tmp = ch - 'A' + 10;
    else
      continue;
    if (odd_digit) {
      --result;
      *result = tmp;
      odd_digit = false;
    } else {
      *result |= tmp << 4;
      odd_digit = true;
    }
  }
  return res;
}

static inline QByteArray byteArrayToBase64(QByteArray *ba)
{
  const char alphabet[] = "ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
                          "ghijklmn" "opqrstuv" "wxyz0123" "456789+/";
  const char padchar = '=';
  int padlen = 0;
  QByteArray tmp((ba->size() * 4) / 3 + 3);
  int i = 0;
  char *out = tmp.data();
  const uchar *data = (const uchar *)ba->data();
  while (i < ba->size()) {
    int chunk = 0;
    chunk |= int(uchar(data[i++])) << 16;
    if (i == ba->size()) {
      padlen = 2;
    } else {
      chunk |= int(uchar(data[i++])) << 8;
      if (i == ba->size()) padlen = 1;
      else chunk |= int(uchar(data[i++]));
    }

    int j = (chunk & 0x00fc0000) >> 18;
    int k = (chunk & 0x0003f000) >> 12;
    int l = (chunk & 0x00000fc0) >> 6;
    int m = (chunk & 0x0000003f);
    *out++ = alphabet[j];
    *out++ = alphabet[k];
    if (padlen > 1) *out++ = padchar;
    else *out++ = alphabet[l];
    if (padlen > 0) *out++ = padchar;
    else *out++ = alphabet[m];
  }
  tmp.truncate(out - tmp.data());
  return tmp;
}

static inline QByteArray byteArrayFromBase64(QByteArray *ba)
{
  unsigned int buf = 0;
  int nbits = 0;
  QByteArray tmp((ba->size() * 3) / 4);
  int offset = 0;
  for (int i = 0; i < ba->size(); ++i) {
    int ch = ba->at(i);
    int d;

    if (ch >= 'A' && ch <= 'Z')
      d = ch - 'A';
    else if (ch >= 'a' && ch <= 'z')
      d = ch - 'a' + 26;
    else if (ch >= '0' && ch <= '9')
      d = ch - '0' + 52;
    else if (ch == '+')
      d = 62;
    else if (ch == '/')
      d = 63;
    else
      d = -1;

    if (d != -1) {
      buf = (buf << 6) | d;
      nbits += 6;
      if (nbits >= 8) {
        nbits -= 8;
        tmp[offset++] = buf >> nbits;
        buf &= (1 << nbits) - 1;
      }
    }
  }
  tmp.truncate(offset);
  return tmp;
}

// Ver openssl/RAND_bytes
static inline int rand_bytes(QByteArray *ba)
{
  return RAND_bytes((uchar *)ba->data(), ba->size());
}

// Ver openssl/RAND_pseudo_bytes
static inline int rand_pseudo_bytes(QByteArray *ba)
{
  return RAND_pseudo_bytes((uchar *)ba->data(), ba->size());
}

static inline QByteArray aes_256_encrypt(QByteArray *ba,
                                         const QByteArray &key, // Espera 256 bits (32 bytes)
                                         const QByteArray &iv // Espera 256 bits (32 bytes)
                                        )
{
  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);

  if (!EVP_EncryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL,
                          (uchar *)key.data(), (uchar *)iv.data())) {
    ERR_print_errors_fp(stderr);
    AQS_IF_DEBUG(printf("AQS_p.h aes_256_encrypt: ERROR in EVP_EncryptInit_ex \n"));
    return QByteArray();
  }

  int len = ba->size();
  int c_len = len + ctx.cipher->block_size - 1;
  int f_len = 0;
  QByteArray res(c_len);
  uchar *ciphertext = (uchar *)res.data();
  uchar *plaintext = (uchar *)ba->data();

  if (!EVP_EncryptUpdate(&ctx, ciphertext, &c_len, plaintext, len)) {
    ERR_print_errors_fp(stderr);
    AQS_IF_DEBUG(printf("AQS_p.h aes_256_encrypt: ERROR in EVP_EncryptUpdate \n"));
    EVP_CIPHER_CTX_cleanup(&ctx);
    return QByteArray();
  }

  if (!EVP_EncryptFinal_ex(&ctx, ciphertext + c_len, &f_len)) {
    ERR_print_errors_fp(stderr);
    AQS_IF_DEBUG(printf("AQS_p.h aes_256_encrypt: ERROR in EVP_EncryptFinal_ex \n"));
    EVP_CIPHER_CTX_cleanup(&ctx);
    return QByteArray();
  }

  EVP_CIPHER_CTX_cleanup(&ctx);
  res.truncate(c_len + f_len);
  return res;
}

static inline QByteArray aes_256_decrypt(QByteArray *ba,
                                         const QByteArray &key, // Espera 256 bits (32 bytes)
                                         const QByteArray &iv // Espera 256 bits (32 bytes)
                                        )
{
  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);

  if (!EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL,
                          (uchar *)key.data(), (uchar *)iv.data())) {
    ERR_print_errors_fp(stderr);
    AQS_IF_DEBUG(printf("AQS_p.h aes_256_encrypt: ERROR in EVP_DecryptInit_ex \n"));
    return QByteArray();
  }

  int len = ba->size();
  int p_len = len;
  int f_len = 0;
  QByteArray res(p_len);
  uchar *plaintext = (uchar *)res.data();
  uchar *ciphertext = (uchar *)ba->data();

  if (!EVP_DecryptUpdate(&ctx,  plaintext, &p_len, ciphertext, len)) {
    ERR_print_errors_fp(stderr);
    AQS_IF_DEBUG(printf("AQS_p.h aes_256_encrypt: ERROR in EVP_DecryptUpdate \n"));
    EVP_CIPHER_CTX_cleanup(&ctx);
    return QByteArray();
  }

  if (!EVP_DecryptFinal_ex(&ctx, plaintext + p_len, &f_len)) {
    ERR_print_errors_fp(stderr);
    AQS_IF_DEBUG(printf("AQS_p.h aes_256_encrypt: ERROR in EVP_DecryptFinal_ex \n"));
    EVP_CIPHER_CTX_cleanup(&ctx);
    return QByteArray();
  }

  EVP_CIPHER_CTX_cleanup(&ctx);
  res.truncate(p_len + f_len);
  return res;
}

class AQSBaseObject : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool isValid READ isValid)

public:
  AQSBaseObject() :
    QObject(), o_(0), finished_(false), wrap_(false) {
  }
  virtual ~AQSBaseObject() {
    finish();
  }

  bool isValid() const {
    return (o_ != 0);
  }

  void *o() const {
    return o_;
  }

  void setWrap() {
    wrap_ = true;
  }

  virtual const char *RTTI() const = 0;

  static QMap<int, QStringList> candidateConstructors() {
    return QMap<int, QStringList>();
  }

public slots:
  virtual QDomNode *toXml(bool includeComplexTypes = false) {
    return &xml_;
  }

protected:
  void specializedInternalInit() {}
  void specializedInternalFinish() {}
  inline void internalInit(void *o);
  inline void internalInit(QObject *o);

  static void *specializedConstruct(void *co) {
    return co;
  }
  static void *construct(const QSArgumentList &args) {
    return 0;
  }

  bool finished_;
  bool wrap_;
  QDomDocument xml_;

private:
  inline void finish();

  void *o_;
};

inline void AQS::insertAQSObject(AQSBaseObject *aqo)
{
  objectsCache_.replace(aqo->o(), aqo);
}

inline void AQS::removeAQSObject(AQSBaseObject *aqo)
{
  objectsCache_.remove(aqo->o());
}

inline AQSBaseObject *AQS::findAQSObject(void *ptr) const
{
  return objectsCache_.find(ptr);
}

inline void AQSBaseObject::internalInit(void *o)
{
  o_ = o;
  globalAQS->insertAQSObject(this);
  AQS_IF_DEBUG(globalAQS->objectsCount_++);
}

inline void AQSBaseObject::internalInit(QObject *o)
{
  o->insertChild(this);
  internalInit(static_cast<void *>(o));
}

inline void AQSBaseObject::finish()
{
  globalAQS->removeAQSObject(this);
  finished_ = true;
  o_ = 0;
  AQS_IF_DEBUG(if (globalAQS->objectsCount_ > 0) globalAQS->objectsCount_--);
}


template <class T>
static inline T argValue(const QSArgument &arg)
{
  if (arg.type() == QSArgument::Variant)
    return static_cast<T>(arg.variant().rawAccess());

  QObject *aqo = (arg.type() == QSArgument::QObjectPtr ? arg.qobject() : 0);
  if (aqo && aqo->inherits("AQSVoidPtr"))
    return static_cast<T>(static_cast<AQSBaseObject *>(aqo)->o());

  return static_cast<T>(aqo ? aqo : arg.ptr());
}

#endif /* AQS_P_H_ */
