/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.46 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  NXP Semiconductors USA, Inc.  whose
registered  office  is  situated  at 411 E. Plumeria Drive, San  Jose,
CA 95134, USA  solely for  the  purposes  of  creating  libraries  for
NXPs M0, M3/M4 and  ARM7/9 processor-based  devices,  sublicensed  and
distributed under the terms and conditions of the NXP End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              NXP Semiconductors, 1109 McKay Dr, M/S 76, San Jose, CA 95131, USA
Licensed SEGGER software: emWin
License number:           GUI-00186
License model:            emWin License Agreement, dated August 20th 2011 and Amendment No. 1, dated October 17th 2017 and Amendment No. 2, dated December 18th 2018
Licensed platform:        NXP's ARM 7/9, Cortex-M0, M3, M4, M7, A7, M33
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2011-08-19 - 2025-09-02
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : EDIT.h
Purpose     : EDIT include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef EDIT_H
#define EDIT_H

#include "WM.h"
#include "DIALOG_Type.h" // Required for Create indirect data structure

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {             // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defaults for configuration switches
*
*  The following are defaults for config switches which affect the
*  interface specified in this module
*
**********************************************************************
*/
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Create / Status flags
//
#define EDIT_CF_LEFT    GUI_TA_LEFT
#define EDIT_CF_RIGHT   GUI_TA_RIGHT
#define EDIT_CF_HCENTER GUI_TA_HCENTER
#define EDIT_CF_VCENTER GUI_TA_VCENTER
#define EDIT_CF_TOP     GUI_TA_TOP
#define EDIT_CF_BOTTOM  GUI_TA_BOTTOM

/*********************************************************************
*
*       EDIT color indexes
*
*  Description
*    Color indexes for EDIT widget.
*/
#define EDIT_CI_DISABLED 0    // Color index for the disabled state.
#define EDIT_CI_ENABLED  1    // Color index for the enabled state.
#define EDIT_CI_CURSOR   2    // Color to be used for the cursor. This is only taken into account if the cursor is not in inversion mode (\c{EDIT_EnableInversion(0)}).

/*********************************************************************
*
*       EDIT flags
*
*  Description
*    These flags are used if the EDIT widget is in decimal or float mode.
*    This can be activated by calling EDIT_SetDecMode() or EDIT_SetFloatMode().
*    These flags are OR-combinable.
*/
#define GUI_EDIT_NORMAL                  (0 << 0)    // Edit in normal mode. A sign is displayed only if the value is negative.
#define GUI_EDIT_SIGNED                  (1 << 0)    // "+" and "-" sign is displayed.
#define GUI_EDIT_SUPPRESS_LEADING_ZEROES (1 << 1)    // Does not show leading zeroes.
/* emDoc mark */

//
// Cursor coloring
//
#define GUI_EDIT_SHOWCURSOR              (1 << 2)
#define GUI_EDIT_CUSTCOLORMODE           (1 << 3)
#define GUI_EDIT_CURSORBLINK             (1 << 4)
//
// Automatic text scrolling
//
#define GUI_EDIT_AUTOSCROLL              (1 << 5)
//
// Password mode
//
#define GUI_EDIT_PASSWORD_MODE           (1 << 6)

//
// Edit modes
//
#define GUI_EDIT_MODE_INSERT    0
#define GUI_EDIT_MODE_OVERWRITE 1

//
// Compatibility macros
//
#define EDIT_CI_DISABELD EDIT_CI_DISABLED
#define EDIT_CI_ENABELD  EDIT_CI_ENABLED

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM EDIT_Handle;
typedef int  tEDIT_AddKeyEx    (EDIT_Handle hObj, int Key);
typedef void tEDIT_UpdateBuffer(EDIT_Handle hObj);

/*********************************************************************
*
*             Create functions
*/
EDIT_Handle EDIT_Create        (int x0, int y0, int xSize, int ySize, int Id, int MaxLen, int Flags);
EDIT_Handle EDIT_CreateAsChild (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags, int MaxLen);
EDIT_Handle EDIT_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int MaxLen);
EDIT_Handle EDIT_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int MaxLen, int NumExtraBytes);
EDIT_Handle EDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void EDIT_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
void EDIT_SetDefaultBkColor  (unsigned int Index, GUI_COLOR Color);
void EDIT_SetDefaultFont     (const GUI_FONT * pFont);
void EDIT_SetDefaultTextAlign(int Align);
void EDIT_SetDefaultTextColor(unsigned int Index, GUI_COLOR Color);
U16  EDIT_SetPasswordChar    (U16 PasswordChar);

/*********************************************************************
*
*       Individual member functions
*/
//
// Query preferences
//
GUI_COLOR        EDIT_GetDefaultBkColor(unsigned int Index);
const GUI_FONT * EDIT_GetDefaultFont(void);
int              EDIT_GetDefaultTextAlign(void);
GUI_COLOR        EDIT_GetDefaultTextColor(unsigned int Index);
//
// Methods changing properties
//
void             EDIT_AddKey           (EDIT_Handle hObj, int Key);
void             EDIT_EnableAutoScroll (EDIT_Handle hObj, int OnOff);
void             EDIT_EnableBlink      (EDIT_Handle hObj, int Period, int OnOff);
GUI_COLOR        EDIT_GetBkColor       (EDIT_Handle hObj, unsigned int Index);
void             EDIT_SetBkColor       (EDIT_Handle hObj, unsigned int Index, GUI_COLOR color);
void             EDIT_SetBorderSize    (EDIT_Handle hObj, int Border);
int              EDIT_GetBorderSize    (EDIT_Handle hObj);
void             EDIT_SetCursorAtChar  (EDIT_Handle hObj, int Pos);
void             EDIT_SetCursorAtPixel (EDIT_Handle hObj, int xPos);
void             EDIT_SetFont          (EDIT_Handle hObj, const GUI_FONT * pFont);
int              EDIT_SetInsertMode    (EDIT_Handle hObj, int OnOff);
void             EDIT_SetMaxLen        (EDIT_Handle hObj, int MaxLen);
void             EDIT_SetPasswordMode  (EDIT_Handle hObj, int OnOff);
void             EDIT_SetpfAddKeyEx    (EDIT_Handle hObj, tEDIT_AddKeyEx * pfAddKeyEx);
void             EDIT_SetpfUpdateBuffer(EDIT_Handle hObj, tEDIT_UpdateBuffer * pfUpdateBuffer);
void             EDIT_SetText          (EDIT_Handle hObj, const char * s);
void             EDIT_SetTextAlign     (EDIT_Handle hObj, int Align);
GUI_COLOR        EDIT_GetTextColor     (EDIT_Handle hObj, unsigned int Index);
void             EDIT_SetTextColor     (EDIT_Handle hObj, unsigned int Index, GUI_COLOR Color);
void             EDIT_SetSel           (EDIT_Handle hObj, int FirstChar, int LastChar);
int              EDIT_SetUserData      (EDIT_Handle hObj, const void * pSrc, int NumBytes);
int              EDIT_EnableInversion  (EDIT_Handle hObj, int OnOff);
//
// Get/Set user input
//
U16              EDIT_GetCharAtPixel   (EDIT_Handle hObj, int x, int y, int * pIndex);
int              EDIT_GetCursorCharPos (EDIT_Handle hObj);
void             EDIT_GetCursorPixelPos(EDIT_Handle hObj, int * pxPos, int * pyPos);
float            EDIT_GetFloatValue    (EDIT_Handle hObj);
const GUI_FONT * EDIT_GetFont          (EDIT_Handle hObj);
int              EDIT_GetMaxLen        (EDIT_Handle hObj);
void             EDIT_GetMinMax        (EDIT_Handle hObj, int * pMin, int * pMax);
int              EDIT_GetNumChars      (EDIT_Handle hObj);
void             EDIT_GetText          (EDIT_Handle hObj, char * sDest, int MaxLen);
int              EDIT_GetTextAlign     (EDIT_Handle hObj);
I32              EDIT_GetValue         (EDIT_Handle hObj);
void             EDIT_SetFloatValue    (EDIT_Handle hObj, float Value);
void             EDIT_GetSel           (EDIT_Handle hObj, int * pFirstChar, int * pLastChar);
void             EDIT_GetSelText       (EDIT_Handle hObj, char * sDest, int MaxLen);
int              EDIT_GetUserData      (EDIT_Handle hObj, void * pDest, int NumBytes);
void             EDIT_SetValue         (EDIT_Handle hObj, I32 Value);

/*********************************************************************
*
*       Defines for compatability
*/
#define EDIT_SetFocussable EDIT_SetFocusable
#define EDIT_SetFocusable  WIDGET_SetFocusable

/*********************************************************************
*
*       Routines for editing values
*
**********************************************************************
*/
void  EDIT_SetHexMode  (EDIT_Handle hEdit, U32 Value, U32 Min, U32 Max);
void  EDIT_SetBinMode  (EDIT_Handle hEdit, U32 Value, U32 Min, U32 Max);
void  EDIT_SetDecMode  (EDIT_Handle hEdit, I32 Value, I32 Min, I32 Max, int Shift, U8 Flags);
void  EDIT_SetFloatMode(EDIT_Handle hEdit, float Value, float Min, float Max, int Shift, U8 Flags);
void  EDIT_SetTextMode (EDIT_Handle hEdit);
void  EDIT_SetUlongMode(EDIT_Handle hEdit, U32 Value, U32 Min, U32 Max);

U32   GUI_EditHex      (U32 Value, U32 Min, U32 Max, int Len, int xSize);
U32   GUI_EditBin      (U32 Value, U32 Min, U32 Max, int Len, int xSize);
I32   GUI_EditDec      (I32 Value, I32 Min, I32 Max, int Len, int xSize, int Shift, U8 Flags);
float GUI_EditFloat    (float Value, float Min, float Max, int Len, int xSize, int Shift, U8 Flags);
void  GUI_EditString   (char * pString, int Len, int xSize);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // EDIT_H

/*************************** End of file ****************************/
