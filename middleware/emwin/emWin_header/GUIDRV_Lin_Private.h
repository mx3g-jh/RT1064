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
File        : GUIDRV_Lin_Private.h
Purpose     : Common definitions and common code for all LIN-drivers
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_LIN_PRIVATE_H
#define GUIDRV_LIN_PRIVATE_H

#include <string.h>

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Common definitions for all variants of the LIN driver
*
**********************************************************************
*/
#if defined(WIN32)
  //
  // Simulation prototypes
  //
  U16    SIM_Lin_ReadMem16  (unsigned int Off);
  U32    SIM_Lin_ReadMem32  (unsigned int Off);
  U8     SIM_Lin_ReadMem08p (U8  * p);
  U32    SIM_Lin_ReadMem32p (U32 * p);
  void   SIM_Lin_WriteMem16 (unsigned int Off, U16 Data);
  void   SIM_Lin_WriteMem32 (unsigned int Off, U32 Data);
  void   SIM_Lin_WriteMem08p(U8  * p, U8 Data);
  void   SIM_Lin_WriteMem16p(U16 * p, U16 Data);
  void   SIM_Lin_WriteMem32p(U32 * p, U32 Data);
  void   SIM_Lin_memcpy     (void * pDst, const void * pSrc, int Len);
  void   SIM_Lin_memset     (void * pDst, U8 Value, U32 Len);
  void   SIM_Lin_SetVRAMAddr(int LayerIndex, void * pVRAM);
  void   SIM_Lin_SetVRAMSize(int LayerIndex, int vxSize, int vySize, int xSize, int ySize);
  void   SIM_Lin_CopyBuffer (int IndexSrc, int IndexDst);
  void   SIM_Lin_ShowBuffer (int Index);
  void * SIM_Lin_GetVRAMAddr(int LayerIndex);
  //
  // Access macro definition for internal simulation
  //
  #define LCD_READ_MEM16(VRAMAddr, Off)        SIM_Lin_ReadMem16(Off)
  #define LCD_READ_MEM32(VRAMAddr, Off)        SIM_Lin_ReadMem32(Off)
  #define LCD_READ_MEM08P(p)                   SIM_Lin_ReadMem08p(p)
  #define LCD_READ_MEM32P(p)                   SIM_Lin_ReadMem32p(p)
  #define LCD_WRITE_MEM16(VRAMAddr, Off, Data) SIM_Lin_WriteMem16(Off, Data)
  #define LCD_WRITE_MEM32(VRAMAddr, Off, Data) SIM_Lin_WriteMem32(Off, Data)
  #define LCD_WRITE_MEM08P(p, Data)            SIM_Lin_WriteMem08p(p, Data)
  #define LCD_WRITE_MEM16P(p, Data)            SIM_Lin_WriteMem16p(p, Data)
  #define LCD_WRITE_MEM32P(p, Data)            SIM_Lin_WriteMem32p(p, Data)
  #undef  GUI__MEMCPY
  #define GUI__MEMCPY(pDst, pSrc, Len)         SIM_Lin_memcpy(pDst, pSrc, Len)
  #undef  GUI__MEMSET
  #define GUI__MEMSET(pDst, Value, Len)        SIM_Lin_memset(pDst, Value, Len)
  //
  // Get VRAM address
  //
  #define GET_VRAM_ADDRESS SIM_Lin_GetVRAMAddr(pDevice->LayerIndex)
#else
  //
  // Access macro definition for hardware
  //
  #define LCD_READ_MEM16(VRAMAddr, Off)        (*((U16 *)VRAMAddr + (U32)Off))
  #define LCD_READ_MEM32(VRAMAddr, Off)        (*((U32 *)VRAMAddr + (U32)Off))
  #define LCD_READ_MEM08P(p)                   (*((U8  *)p))
  #define LCD_READ_MEM32P(p)                   (*((U32 *)p))
  #define LCD_WRITE_MEM16(VRAMAddr, Off, Data) *((U16 *)VRAMAddr + (U32)Off) = (U16)(Data)
  #define LCD_WRITE_MEM32(VRAMAddr, Off, Data) *((U32 *)VRAMAddr + (U32)Off) = Data
  #define LCD_WRITE_MEM08P(p, Data)            *((U8  *)p) = (U8)(Data)
  #define LCD_WRITE_MEM16P(p, Data)            *((U16 *)p) = (U16)(Data)
  #define LCD_WRITE_MEM32P(p, Data)            *((U32 *)p) = Data
  //
  // Get VRAM address
  //
  #define GET_VRAM_ADDRESS (void *)pContext->VRAMAddr
#endif

#ifndef   WRITE_MEM16
  #define WRITE_MEM16(VRAMAddr, Off, Data) LCD_WRITE_MEM16(VRAMAddr, Off, Data)
#endif
#ifndef   WRITE_MEM32
  #define WRITE_MEM32(VRAMAddr, Off, Data) LCD_WRITE_MEM32(VRAMAddr, Off, Data)
#endif
#ifndef   READ_MEM08P
  #define READ_MEM08P(p)                   LCD_READ_MEM08P(p)
#endif
#ifndef   READ_MEM16
  #define READ_MEM16(VRAMAddr, Off)        LCD_READ_MEM16(VRAMAddr, Off)
#endif
#ifndef   READ_MEM32
  #define READ_MEM32(VRAMAddr, Off)        LCD_READ_MEM32(VRAMAddr, Off)
#endif
#ifndef   READ_MEM32P
  #define READ_MEM32P(p)                   LCD_READ_MEM32P(p)
#endif
#ifndef   WRITE_MEM08P
  #define WRITE_MEM08P(p, Data)            LCD_WRITE_MEM08P(p, Data)
#endif
#ifndef   WRITE_MEM16P
  #define WRITE_MEM16P(p, Data)            LCD_WRITE_MEM16P(p, Data)
#endif
#ifndef   WRITE_MEM32P
  #define WRITE_MEM32P(p, Data)            LCD_WRITE_MEM32P(p, Data)
#endif

//
// Private functions
//
#define LCD_DEVFUNC_SET_THRESHOLD (LCD_DEVFUNC_INIT_PRIVATE + 1)

//
// Set to 1 if FillRect should be used in DrawHLine and DrawVLine
//
#ifndef   GUIDRV_LIN_USE_FILLRECT
  #define GUIDRV_LIN_USE_FILLRECT 0
#endif

#define OFF2PTR08(VRAMAddr, Off)     (U8  *)((U8 *)VRAMAddr + (Off     ))
#define OFF2PTR16(VRAMAddr, Off)     (U16 *)((U8 *)VRAMAddr + (Off << 1))
#define OFF2PTR32(VRAMAddr, Off)     (U32 *)((U8 *)VRAMAddr + (Off << 2))

//
// Macro to add a proper value as alpha channel if LIN24 is used
//
#define GUIDRV_LIN_24_ADD_ALPHA(x) ( x^= (*GUI_pContext->LCD_pColorIndex & 0xFF000000))

//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_LIN

//
// Definition of default members for DRIVER_CONTEXT structure
//
#define DEFAULT_CONTEXT_MEMBERS                               \
  void * VRAMAddr;                                            \
  void * BaseAddr;                                            \
  void ** aBufferPTR;                                         \
  int BufferIndex;                                            \
  int xSize, ySize;                                           \
  int vxSize, vySize;                                         \
  int vxSizePhys;                                             \
  int xPos, yPos;                                             \
  int Alpha;                                                  \
  int IsVisible;                                              \
  int Threshold;                                              \
  void (* pfFillRect)  (int /* LayerIndex */,                 \
                        int /* x0 */,                         \
                        int /* y0 */,                         \
                        int /* x1 */,                         \
                        int /* y1 */,                         \
                        U32 /* PixelIndex */);                \
  void (* pfCopyBuffer)(int /* LayerIndex */,                 \
                        int /* IndexSrc */,                   \
                        int /* IndexDst */);                  \
  void (* pfDrawBMP1)  (int /* LayerIndex */,                 \
                        int /* x */,                          \
                        int /* y */,                          \
                        U8 const * /* p */,                   \
                        int /* Diff */,                       \
                        int /* xSize */,                      \
                        int /* ySize */,                      \
                        int /* BytesPerLine */,               \
                        const LCD_PIXELINDEX * /* pTrans */); \
  void (* pfDrawBMP8)  (int /* LayerIndex */,                 \
                        int /* x */,                          \
                        int /* y */,                          \
                        U8 const * /* p */,                   \
                        int /* xSize */,                      \
                        int /* ySize */,                      \
                        int /* BytesPerLine */,               \
                        const LCD_PIXELINDEX * /* pTrans */); \
  void (* pfCopyRect)  (int /* LayerIndex */,                 \
                        int /* x0 */,                         \
                        int /* y0 */,                         \
                        int /* x1 */,                         \
                        int /* y1 */,                         \
                        int /* xSize */,                      \
                        int /* ySize */);                     \
  void (* pfSetPos)    (int /* LayerIndex */,                 \
                        int /* xPos */,                       \
                        int /* yPos */);                      \
  void (* pfRefresh)   (int /* LayerIndex */);

#ifndef   PRIVATE_CONTEXT_MEMBERS
  #define PRIVATE_CONTEXT_MEMBERS
#endif

//
// Definition of default function management for _GetDevFunc()
//
#define DEFAULT_MANAGEMENT_GETDEVFUNC()                                             \
  case LCD_DEVFUNC_SET_VRAM_ADDR:                                                   \
    return (void (*)(void))_SetVRAMAddr;                                            \
  case LCD_DEVFUNC_SET_BUFFERPTR:                                                   \
    return (void (*)(void))_SetVRAM_BufferPTR;                                      \
  case LCD_DEVFUNC_SET_VSIZE:                                                       \
    return (void (*)(void))_SetVSize;                                               \
  case LCD_DEVFUNC_SET_SIZE:                                                        \
    return (void (*)(void))_SetSize;                                                \
  case LCD_DEVFUNC_SETPOS:                                                          \
    return (void (*)(void))_SetPos;                                                 \
  case LCD_DEVFUNC_GETPOS:                                                          \
    return (void (*)(void))_GetPos;                                                 \
  case LCD_DEVFUNC_SETALPHA:                                                        \
    return (void (*)(void))_SetAlpha;                                               \
  case LCD_DEVFUNC_SETVIS:                                                          \
    return (void (*)(void))_SetVis;                                                 \
  case LCD_DEVFUNC_INIT:                                                            \
    return (void (*)(void))_Init;                                                   \
  case LCD_DEVFUNC_EXIT:                                                            \
    return (void (*)(void))_OnExit;                                                 \
  case LCD_DEVFUNC_ON:                                                              \
    return (void (*)(void))_On;                                                     \
  case LCD_DEVFUNC_OFF:                                                             \
    return (void (*)(void))_Off;                                                    \
  case LCD_DEVFUNC_ALPHAMODE:                                                       \
    return (void (*)(void))_SetAlphaMode;                                           \
  case LCD_DEVFUNC_CHROMAMODE:                                                      \
    return (void (*)(void))_SetChromaMode;                                          \
  case LCD_DEVFUNC_CHROMA:                                                          \
    return (void (*)(void))_SetChroma;                                              \
  case LCD_DEVFUNC_COPYBUFFER:                                                      \
    return (void (*)(void))_CopyBuffer;                                             \
  case LCD_DEVFUNC_SHOWBUFFER:                                                      \
    return (void (*)(void))_ShowBuffer;                                             \
  case LCD_DEVFUNC_SETFUNC:                                                         \
    return (void (*)(void))_SetDevFunc;                                             \
  case LCD_DEVFUNC_REFRESH:                                                         \
    return (void (*)(void))_Refresh;                                                \
  case LCD_DEVFUNC_FILLRECT:                                                        \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfFillRect; \
  case LCD_DEVFUNC_DRAWBMP_1BPP:                                                    \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfDrawBMP1; \
  case LCD_DEVFUNC_DRAWBMP_8BPP:                                                    \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfDrawBMP8; \
  case LCD_DEVFUNC_COPYRECT:                                                        \
    return (void (*)(void))((DRIVER_CONTEXT *)(*ppDevice)->u.pContext)->pfCopyRect; \
  case LCD_DEVFUNC_SETTHRESHOLD:                                                    \
    return (void (*)(void))GUIDRV_Lin_SetThreshold;

//
// Definition of default function management for _GetDevProp()
//
#define DEFAULT_MANAGEMENT_GETDEVPROP() \
  case LCD_DEVCAP_XSIZE:                \
    return pContext->xSize;             \
  case LCD_DEVCAP_YSIZE:                \
    return pContext->ySize;             \
  case LCD_DEVCAP_VXSIZE:               \
    return pContext->vxSize;            \
  case LCD_DEVCAP_VYSIZE:               \
    return pContext->vySize;            \
  case LCD_DEVCAP_THRESHOLD:            \
    return pContext->Threshold;

//
// Definition of default function management for _GetDevData()
//
#define DEFAULT_MANAGEMENT_GETDEVDATA() \
  case LCD_DEVDATA_VRAMADDR:            \
    return GET_VRAM_ADDRESS;

//
// Definition of private function management for _GetDevFunc()
//
#ifndef   PRIVATE_MANAGEMENT_GETDEVFUNC
  #define PRIVATE_MANAGEMENT_GETDEVFUNC()
#endif

//
// Definition of default function management for _SetDevFunc()
//
#define DEFAULT_MANAGEMENT_SETDEVFUNC()                                                                                                                                      \
    case LCD_DEVFUNC_FILLRECT:                                                                                                                                               \
      pContext->pfFillRect   = (void (*)(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex))pFunc;                                                              \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_COPYBUFFER:                                                                                                                                             \
      pContext->pfCopyBuffer = (void (*)(int LayerIndex, int IndexSrc, int IndexDst))pFunc;                                                                                  \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_DRAWBMP_1BPP:                                                                                                                                           \
      pContext->pfDrawBMP1   = (void (*)(int LayerIndex, int x, int y, U8 const * p, int Diff, int xSize, int ySize, int BytesPerLine, const LCD_PIXELINDEX * pTrans))pFunc; \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_DRAWBMP_8BPP:                                                                                                                                           \
      pContext->pfDrawBMP8   = (void (*)(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine, const LCD_PIXELINDEX * pTrans))pFunc;           \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_COPYRECT:                                                                                                                                               \
      pContext->pfCopyRect   = (void (*)(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize))pFunc;                                                        \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_REFRESH:                                                                                                                                                \
      pContext->pfRefresh    = (void (*)(int LayerIndex))pFunc;                                                                                                              \
      break;                                                                                                                                                                 \
    case LCD_DEVFUNC_SETPOS:                                                                                                                                                 \
      pContext->pfSetPos     = (void (*)(int LayerIndex, int xPos, int yPos))pFunc;                                                                                          \
      break;

//
// Definition of private function management for _GetDevFunc()
//
#ifndef   PRIVATE_MANAGEMENT_SETDEVFUNC
  #define PRIVATE_MANAGEMENT_SETDEVFUNC()
#endif

//
// Endian related definitions
//
#ifndef   LCD_MIRROR
  #define LCD_MIRROR 0
#endif

#if (LCD_MIRROR == 2)
#define MIRROR(x) x = ((x & 0x000000ffUL) <<  8) \
                    | ((x & 0x0000ff00UL) >>  8) \
                    | ((x & 0x00ff0000UL) <<  8) \
                    | ((x & 0xff000000UL) >>  8)
#else
#define MIRROR(x) x = ((x & 0x000000ffUL) << 24) \
                    | ((x & 0x0000ff00UL) <<  8) \
                    | ((x & 0x00ff0000UL) >>  8) \
                    | ((x & 0xff000000UL) >> 24)
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// DRIVER_CONTEXT structure consisting of default and private members
//
typedef struct {
  DEFAULT_CONTEXT_MEMBERS
  PRIVATE_CONTEXT_MEMBERS
} DRIVER_CONTEXT;

/*********************************************************************
*
*       Static code (common for all)
*
**********************************************************************
*/
#if !defined(GUIDRV_LIN_EXCLUDE_CODE)

static I32 _GetDevProp(GUI_DEVICE * pDevice, int Index);

/*********************************************************************
*
*       _InitOnce
*
* Purpose:
*   Allocates a fixed block for the context of the driver
*
* Return value:
*   0 on success, 1 on error
*/
static int _InitOnce(GUI_DEVICE * pDevice) {
  void (* pFunc)(GUI_DEVICE *);

  if (pDevice->u.pContext == NULL) {
    //
    // Allocate memory for context
    //
    pDevice->u.pContext = GUI_ALLOC_GetFixedBlock(sizeof(DRIVER_CONTEXT));
    GUI__memset((U8 *)pDevice->u.pContext, 0, sizeof(DRIVER_CONTEXT));
    //
    // Optional extended initialization of driver variants
    //
    pFunc = (void (*)(GUI_DEVICE *))pDevice->pDeviceAPI->pfGetDevFunc(&pDevice, LCD_DEVFUNC_INIT_PRIVATE);
    if (pFunc) {
      pFunc(pDevice);
    }
  }
  return pDevice->u.pContext ? 0 : 1;
}

/*********************************************************************
*
*       _GetRect
*
* Purpose:
*   Returns the display size.
*/
static void _GetRect(GUI_DEVICE * pDevice, LCD_RECT * pRect) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  pRect->x0 = 0;
  pRect->y0 = 0;
  pRect->x1 = pContext->vxSize - 1;
  pRect->y1 = pContext->vySize - 1;
}

/*********************************************************************
*
*       _SetVis
*
* Purpose:
*   Sets the visibility of the given layer by sending a LCD_X_SETVIS command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetVis(GUI_DEVICE * pDevice, int OnOff) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETVIS_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->IsVisible = OnOff;
    Data.OnOff = OnOff;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETVIS, (void *)&Data);
  }
}

/*********************************************************************
*
*       _SetPos
*
* Purpose:
*   Sets the position of the given layer by sending a LCD_X_SETPOS command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetPos(GUI_DEVICE * pDevice, int xPos, int yPos) {
  DRIVER_CONTEXT * pContext;
  int xSizeDisplay, ySizeDisplay, xSizeLayer, ySizeLayer, BitsPerPixel;
  LCD_X_SETPOS_INFO PosInfo = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (pContext->pfSetPos) {
      //
      // Use custom callback to setlayer position
      //
      pContext->pfSetPos(pDevice->LayerIndex, xPos, yPos);
    } else {
      //
      // Calculate xPos/yPos and new layer size
      //
      pContext->xPos = xPos;
      pContext->yPos = yPos;

      xSizeDisplay  = LCD_GetXSizeDisplay();
      ySizeDisplay  = LCD_GetYSizeDisplay();
      xSizeLayer    = pContext->xSize;
      ySizeLayer    = pContext->ySize;
      BitsPerPixel  = pDevice->pDeviceAPI->pfGetDevProp(pDevice, LCD_DEVCAP_BITSPERPIXEL);
      PosInfo.BytesPerPixel = (BitsPerPixel + 7) / 8;
      if (xPos < 0) {
        PosInfo.Off -= xPos * PosInfo.BytesPerPixel;
        PosInfo.xPos = 0;
        PosInfo.xLen = xSizeLayer + xPos;
      } else {
        PosInfo.xPos = xPos;
        PosInfo.xLen = xSizeLayer;
        if ((PosInfo.xPos + PosInfo.xLen) > xSizeDisplay) {
          PosInfo.xLen = xSizeDisplay - xPos;
        }
      }
      if (yPos < 0) {
        PosInfo.Off -= yPos * PosInfo.BytesPerPixel * xSizeLayer;
        PosInfo.yPos = 0;
        PosInfo.yLen = ySizeLayer + yPos;
      } else {
        PosInfo.yPos = yPos;
        PosInfo.yLen = ySizeLayer;
        if ((PosInfo.yPos + PosInfo.yLen) > ySizeDisplay) {
          PosInfo.yLen = ySizeDisplay - yPos;
        }
      }
      if ((PosInfo.xLen <= 0) || (PosInfo.yLen <= 0) || (PosInfo.xPos >= xSizeDisplay) || (PosInfo.yPos >= ySizeDisplay)) {
        if (pContext->IsVisible == 1) {
          _SetVis(pDevice, 0);
        }
        return;
      }
      LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETPOS, (void *)&PosInfo);
    }
  }
}

/*********************************************************************
*
*       _GetPos
*
* Purpose:
*   Returns the position of the given layer.
*/
static void _GetPos(GUI_DEVICE * pDevice, int * pxPos, int * pyPos) {
  DRIVER_CONTEXT * pContext;

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    *pxPos = pContext->xPos;
    *pyPos = pContext->yPos;
  }
}

/*********************************************************************
*
*       _SetAlpha
*
* Purpose:
*   Sets the alpha value of the given layer by sending a LCD_X_SETALPHA command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetAlpha(GUI_DEVICE * pDevice, int Alpha) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETALPHA_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->Alpha = Alpha;
    Data.Alpha = Alpha;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETALPHA, (void *)&Data);
  }
}

/*********************************************************************
*
*       _Init
*
* Purpose:
*   Called during the initialization process of emWin.
*/
static int  _Init(GUI_DEVICE * pDevice) {
  int r;

  r = _InitOnce(pDevice);
  r |= LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_INITCONTROLLER, NULL);
  return r;
}

/*********************************************************************
*
*       _OnExit
*
* Purpose:
*   Called during the deinitialization of the driver.
*/
static int  _OnExit(GUI_DEVICE * pDevice) {
  DRIVER_CONTEXT * pContext;

  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
  if (pContext) {
    if (pDevice->u.pContext) {
      GUI_ALLOC_FreeFixedBlock(pDevice->u.pContext);
      pDevice->u.pContext = NULL;
    }
  }
  return 0;
}

/*********************************************************************
*
*       _On
*
* Purpose:
*   Sends a LCD_X_ON command to LCD_X_DisplayDriver().
*/
static void _On (GUI_DEVICE * pDevice) {
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_ON, NULL);
}

/*********************************************************************
*
*       _Off
*
* Purpose:
*   Sends a LCD_X_OFF command to LCD_X_DisplayDriver().
*/
static void _Off (GUI_DEVICE * pDevice) {
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_OFF, NULL);
}

/*********************************************************************
*
*       _SetAlphaMode
*
* Purpose:
*   Sets the alpha mode of the given layer by sending a LCD_X_SETALPHAMODE command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetAlphaMode(GUI_DEVICE * pDevice, int AlphaMode) {
  LCD_X_SETALPHAMODE_INFO Data = {0};

  Data.AlphaMode = AlphaMode;
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETALPHAMODE, (void *)&Data);
}

/*********************************************************************
*
*       _SetChromaMode
*
* Purpose:
*   Sets the chroma mode of the given layer by sending a LCD_X_SETCHROMAMODE command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetChromaMode(GUI_DEVICE * pDevice, int ChromaMode) {
  LCD_X_SETCHROMAMODE_INFO Data = {0};

  Data.ChromaMode = ChromaMode;
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETCHROMAMODE, (void *)&Data);
}

/*********************************************************************
*
*       _SetChroma
*
* Purpose:
*   Sets the chroma values of the given layer by sending a LCD_X_SETCHROMA command to LCD_X_DisplayDriver()
*   (Requires special hardware support.)
*/
static void _SetChroma(GUI_DEVICE * pDevice, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax) {
  LCD_X_SETCHROMA_INFO Data = {0};

  Data.ChromaMin = ChromaMin;
  Data.ChromaMax = ChromaMax;
  LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETCHROMA, (void *)&Data);
}

/*********************************************************************
*
*       _CopyBuffer
*
* Purpose:
*   Copies the source buffer to the destination buffer and routes
*   further drawing operations to the destination buffer.
*
*   (Required for using multiple buffers)
*/
static void _CopyBuffer(GUI_DEVICE * pDevice, int IndexSrc, int IndexDst) {
  DRIVER_CONTEXT * pContext;
  #if (!defined(WIN32))
    void * pSrc;
    void * pDst;
    I32 BufferSize;
    int BitsPerPixel;
  #endif

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (IndexSrc != IndexDst) {
      #if defined(WIN32)
        SIM_Lin_CopyBuffer(IndexSrc, IndexDst);
      #else
        BitsPerPixel = pDevice->pDeviceAPI->pfGetDevProp(pDevice, LCD_DEVCAP_BITSPERPIXEL);
        BufferSize = (((U32)pContext->vxSize * pContext->ySize * BitsPerPixel) >> 3);
        if (pContext->aBufferPTR) {
          pSrc = pContext->aBufferPTR[IndexSrc];
          pDst = pContext->aBufferPTR[IndexDst];
        } else {
          pSrc = (U8 *)pContext->BaseAddr + BufferSize * IndexSrc;
          pDst = (U8 *)pContext->BaseAddr + BufferSize * IndexDst;
        }
        if (pContext->pfCopyBuffer) {
          //
          // Use custom callback function for copy operation
          //
          pContext->pfCopyBuffer(pDevice->LayerIndex, IndexSrc, IndexDst);
        } else {
          //
          // Calculate pointers for copy operation
          //
          GUI__MEMCPY(pDst, pSrc, BufferSize);
        }
        //
        // Set destination buffer as target for further drawing operations
        //
        pContext->VRAMAddr = pDst;
      #endif
    }
  }
}

/*********************************************************************
*
*       _ShowBuffer
*
* Purpose:
*   Sends a LCD_X_SHOWBUFFER command to LCD_X_DisplayDriver() to make the given buffer visible.
*
*   (Required for using multiple buffers)
*/
static void _ShowBuffer(GUI_DEVICE * pDevice, int Index) {
  LCD_X_SHOWBUFFER_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    #if defined(WIN32)
      SIM_Lin_ShowBuffer(Index);
    #else
      Data.Index = Index;
      LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SHOWBUFFER, (void *)&Data);
    #endif
  }
}

/*********************************************************************
*
*       _Refresh
*/
static void _Refresh(GUI_DEVICE * pDevice) {
  #if (!defined(WIN32))
    DRIVER_CONTEXT * pContext;
    
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (pContext->pfRefresh) {
      pContext->pfRefresh(pDevice->LayerIndex);
    }
  #else
    GUI_USE_PARA(pDevice);
  #endif
}

/*********************************************************************
*
*       _SetOrg
*
* Purpose:
*   Calls the driver callback function with the display origin to be set
*/
static void _SetOrg(GUI_DEVICE * pDevice, int x, int y) {
  #if (!defined(WIN32))
    DRIVER_CONTEXT * pContext;
    int Orientation;
  #endif
  LCD_X_SETORG_INFO Data = {0};

  #if defined(WIN32)
    LCDSIM_SetOrg(x, y, pDevice->LayerIndex);
  #else
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    Orientation  = LCD_GetMirrorXEx(pDevice->LayerIndex) * GUI_MIRROR_X;
    Orientation |= LCD_GetMirrorYEx(pDevice->LayerIndex) * GUI_MIRROR_Y;
    Orientation |= LCD_GetSwapXYEx (pDevice->LayerIndex) * GUI_SWAP_XY;
    switch (Orientation) {
    case 0:
      Data.xPos = x;
      Data.yPos = y;
      break;
    case GUI_MIRROR_X:
      Data.xPos = pContext->vxSize - pContext->xSize - x;
      Data.yPos = y;
      break;
    case GUI_MIRROR_Y:
      Data.xPos = x;
      Data.yPos = pContext->vySize - pContext->ySize - y;
      break;
    case GUI_MIRROR_X | GUI_MIRROR_Y:
      Data.xPos = pContext->vxSize - pContext->xSize - x;
      Data.yPos = pContext->vySize - pContext->ySize - y;
      break;
    case GUI_SWAP_XY:
      Data.xPos = y;
      Data.yPos = x;
      break;
    case GUI_SWAP_XY | GUI_MIRROR_X:
      Data.xPos = pContext->vySize - pContext->ySize  - y;
      Data.yPos = x;
      break;
    case GUI_SWAP_XY | GUI_MIRROR_Y:
      Data.xPos = y;
      Data.yPos = pContext->vxSize - pContext->xSize  - x;
      break;
    case GUI_SWAP_XY | GUI_MIRROR_X | GUI_MIRROR_Y:
      Data.xPos = pContext->vySize - pContext->ySize  - y;
      Data.yPos = pContext->vxSize - pContext->xSize  - x;
      break;
    default:
      break;
    }
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETORG, (void *)&Data);
  #endif
}

/*********************************************************************
*
*       _SetVRAMAddr
*/
static void _SetVRAMAddr(GUI_DEVICE * pDevice, void * pVRAM) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETVRAMADDR_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->VRAMAddr = pContext->BaseAddr = (void *)pVRAM;
    Data.pVRAM = pVRAM;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETVRAMADDR, (void *)&Data);
  }
  #if defined(WIN32)
    SIM_Lin_SetVRAMAddr(pDevice->LayerIndex, pVRAM);
  #endif
}

/*********************************************************************
*
*       _SetVRAM_BufferPTR
*/
static void _SetVRAM_BufferPTR(GUI_DEVICE * pDevice, void ** pBufferPTR) {
  DRIVER_CONTEXT * pContext;

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    pContext->aBufferPTR = pBufferPTR;
    pContext->VRAMAddr = *(pBufferPTR + 0);
  }
}

/*********************************************************************
*
*       _SetVSize
*/
static void _SetVSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
  DRIVER_CONTEXT * pContext;
  #if defined(WIN32)
    int NumBuffers;

    pContext = NULL;
  #endif
  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    #if defined(WIN32)
      NumBuffers = GUI_MULTIBUF_GetNumBuffers();
    #endif
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (_GetDevProp(pDevice, LCD_DEVCAP_SWAP_XY)) {
      #if defined(WIN32)
        pContext->vxSize = xSize * NumBuffers;
      #else
        pContext->vxSize = xSize;
      #endif
      pContext->vySize = ySize;
      pContext->vxSizePhys = ySize;
    } else {
      pContext->vxSize = xSize;
      #if defined(WIN32)
        pContext->vySize = ySize * NumBuffers;
      #else
        pContext->vySize = ySize;
      #endif
      pContext->vxSizePhys = xSize;
    }
  }
  #if defined(WIN32)
    if (pContext) {
      SIM_Lin_SetVRAMSize(pDevice->LayerIndex, pContext->vxSize, pContext->vySize, pContext->xSize, pContext->ySize);
    }
  #endif
}

/*********************************************************************
*
*       _SetSize
*/
static void _SetSize(GUI_DEVICE * pDevice, int xSize, int ySize) {
  DRIVER_CONTEXT * pContext;
  LCD_X_SETSIZE_INFO Data = {0};

  _InitOnce(pDevice);
  if (pDevice->u.pContext) {
    pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;
    if (pContext->vxSizePhys == 0) {
      if (_GetDevProp(pDevice, LCD_DEVCAP_SWAP_XY)) {
        pContext->vxSizePhys = ySize;
      } else {
        pContext->vxSizePhys = xSize;
      }
    }
    pContext->xSize = xSize;
    pContext->ySize = ySize;
    Data.xSize = xSize;
    Data.ySize = ySize;
    LCD_X_DisplayDriver(pDevice->LayerIndex, LCD_X_SETSIZE, (void *)&Data);
  }
}

#endif  // GUIDRV_LIN_EXCLUDE_CODE

#if defined(__cplusplus)
}
#endif

#endif /* GUIDRV_LIN_PRIVATE_H */

/*************************** End of file ****************************/
