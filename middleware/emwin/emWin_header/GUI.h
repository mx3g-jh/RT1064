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
File        : GUI.h
Purpose     : GUI API include file
---------------------------END-OF-HEADER------------------------------
*/

#ifndef  GUI_H
#define  GUI_H

#include <stddef.h>
#include <string.h>   // for memset()

#include "GUI_ConfDefaults.h"
#include "GUI_Type.h"
#include "GUI_Version.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Disable string function warning with newer MSVC versions
*/
#if defined (_MSC_VER)
  #if (_MSC_VER > 1200)
    #pragma warning( disable : 4996)
  #endif
#endif

/*********************************************************************
*
*       Numeric limits
*/
#define I32_MIN (-2147483647L - 1)
#define I32_MAX   2147483647L

/*********************************************************************
*
*       Macros, function replacement
*/
#define GUI_ABS(a)              (((a) < 0) ? (-(a)) : (a))
#define GUI_COUNTOF(a)          (sizeof(a) / sizeof(a[0]))
#define GUI_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define GUI_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define GUI_ZEROFILL(p, Size)   (memset(p, 0, Size))
#define GUI_ZEROFILL_VAR(var)   (memset(&var,    0, sizeof(var)))
#define GUI_ZEROFILL_ARR(arr)   (memset(&arr[0], 0, sizeof(arr)))
#define GUI_ZEROFILL_PTR(ptr)   (memset(ptr,     0, sizeof(*ptr)))

/*********************************************************************
*
*       Support for multitasking systems (locking)
*/
typedef struct GUI_CONTEXT GUI_CONTEXT;

#if !GUI_OS
  #define GUI_LOCK()
  #define GUI_UNLOCK()
  #define GUITASK_INIT()
  #define GUITASK_COPY_CONTEXT()
#else
  void GUI_Lock(void);
  void GUI_Unlock(void);
  void GUITASK_Init(void);
  void GUITASK_CopyContext(void);
  void GUITASK_SetMaxTask(int MaxTask);
  int  GUITASK_GetMaxTask(void);
  GUI_CONTEXT * GUITASK_GetpContext(int Index);
  #define GUI_LOCK()             GUI_Lock()
  #define GUI_UNLOCK()           GUI_Unlock()
  #define GUITASK_INIT()         GUITASK_Init()
  #define GUITASK_COPY_CONTEXT() GUITASK_CopyContext()
#endif

/*********************************************************************
*
*       API table of a display driver
*/
struct GUI_DEVICE_API {
  //
  // Data
  //
  int DeviceClassIndex;
  //
  // Drawing functions
  //
  void           (* pfDrawBitmap   )(GUI_DEVICE *  pDevice,  int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine, const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void           (* pfDrawHLine    )(GUI_DEVICE *  pDevice,  int x0, int y0,  int x1);
  void           (* pfDrawVLine    )(GUI_DEVICE *  pDevice,  int x , int y0,  int y1);
  void           (* pfFillRect     )(GUI_DEVICE *  pDevice,  int x0, int y0, int x1, int y1);
  LCD_PIXELINDEX (* pfGetPixelIndex)(GUI_DEVICE *  pDevice,  int x, int y);
  void           (* pfSetPixelIndex)(GUI_DEVICE *  pDevice,  int x, int y, LCD_PIXELINDEX ColorIndex);
  void           (* pfXorPixel     )(GUI_DEVICE *  pDevice,  int x, int y);
  //
  // Set origin
  //
  void           (* pfSetOrg       )(GUI_DEVICE *  pDevice,  int x, int y);
  //
  // Request information
  //
  void         (*(* pfGetDevFunc)   (GUI_DEVICE ** ppDevice, int Index))(void);
  I32            (* pfGetDevProp   )(GUI_DEVICE *  pDevice,  int Index);
  const void    *(* pfGetDevData   )(GUI_DEVICE *  pDevice,  int Index);
  void           (* pfGetRect      )(GUI_DEVICE *  pDevice,  LCD_RECT * pRect);
};

/*********************************************************************
*
*       Device classes
*/
typedef enum {
  DEVICE_CLASS_DRIVER = 0,
  DEVICE_CLASS_DRIVER_MODIFIER,   // Zoom or delta-pixel modifier
  DEVICE_CLASS_VNC,
  DEVICE_CLASS_SPRITE,
  DEVICE_CLASS_MEMDEV,
  DEVICE_CLASS_ALPHA,
  DEVICE_CLASS_AUTOALPHA,
  DEVICE_CLASS_MEASDEV
} DEVICE_CLASS;

#define GUI_DEVICE_STAYONTOP 1

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;
//
// Macros to be used in configuration files
//
#define GUIDRV_WIN32        &GUIDRV_Win_API

/*********************************************************************
*
*       Definition of GUI_DEVICE structure
*/
struct GUI_DEVICE {
  //
  // Linking
  //
  GUI_DEVICE * pNext;
  GUI_DEVICE * pPrev;
  //
  // Data
  //
  union {
    GUI_HMEM hContext; // Handle of payload data like sprite- or memory device context
    void   * pContext; // Pointer for context data in a fixed block
  } u;
  //
  // API pointers
  //
  const GUI_DEVICE_API     * pDeviceAPI;
  const LCD_API_COLOR_CONV * pColorConvAPI;
  U16 Flags;
  int LayerIndex;
};

extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_1;
extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_8;
extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_16;
extern const GUI_DEVICE_API GUI_MEMDEV_DEVICE_32;

/*********************************************************************
*
*       GUI_CONTEXT
*
*  This structure is public for one reason only:
*  To allow the application to save and restore the context.
*/
typedef union {
  U8  aColorIndex8[2];
  U16 aColorIndex16[2];
  U32 aColorIndex32[2];
} LCD_COLORINDEX_UNION;

struct GUI_CONTEXT {
  //
  // Variables in LCD module
  //
  LCD_COLORINDEX_UNION uLCD;
  LCD_RECT       ClipRect;
  U8             DrawMode;
  U8             SelLayer;
  U8             TextStyle;
  //
  // Variables in GL module
  //
  GUI_RECT * pClipRect_HL;                 // High level clip rectangle ... Speed optimization so drawing routines can optimize
  U16        PenSize;
  U8         PenShape;
  U8         LineStyle;
  U8         StrikeWidth;
  //
  // Variables in GUICHAR module
  //
  const GUI_FONT * pAFont;
  I16P LBorder;
  I16P DispPosX, DispPosY;
  I16P DrawPosX, DrawPosY;
  I16P TextMode, TextAlign;
  GUI_COLOR Color, BkColor;                // Required only when changing devices and for speed opt (caching)
  //
  // Pointer to  color indices
  //
  LCD_PIXELINDEX * LCD_pBkColorIndex;
  LCD_PIXELINDEX * LCD_pColorIndex;
  //
  // Variables in WM module
  //
  #if GUI_WINSUPPORT
    const GUI_RECT * WM__pUserClipRect;
    GUI_HWIN hAWin;
    int xOff, yOff;
    U8 WM_IsActive;
    U8 DisableCliprect;
    U8 ClipBKActive;
  #endif
  //
  // Array of pointers to device chains
  //
  GUI_DEVICE * apDriver[GUI_NUM_LAYERS];
  //
  // Variables in MEMDEV module (with memory devices only)
  //
  GUI_HMEM    hDevData;
  //
  // Variables in Antialiasing module
  //
  const tLCD_HL_APIList * pLCD_HL;       // Required to reroute drawing (HLine & Pixel) to the AA module
  U8 AA_Factor;
  U8 AA_HiResEnable;
  void (* AA_pfSetPixelAA)(int x, int y, U8 Intens); // Function to be used for drawing a single pixel
  //
  // Used to reference and link in the copyright string for libraries.
  //
  const char * sCopyright;
};

/* Rename GUI_SaveContext in order to avoid crashes if wrong GUIConf is used */
#if (GUI_WINSUPPORT == 1)
  #define GUI_SaveContext GUI_SaveContext_W
#else
  #define GUI_SaveContext GUI_SaveContext_
#endif

/*********************************************************************
*
*       Device management
*/
GUI_DEVICE * GUI_DEVICE_Create          (const GUI_DEVICE_API * pDeviceAPI, const LCD_API_COLOR_CONV * pColorConvAPI, U16 Flags, int LayerIndex);
GUI_DEVICE * GUI_DEVICE_CreateAndLink   (const GUI_DEVICE_API * pDeviceAPI, const LCD_API_COLOR_CONV * pColorConvAPI, U16 Flags, int LayerIndex);
void         GUI_DEVICE_Delete          (GUI_DEVICE * pDevice);
int          GUI_DEVICE_GetDeviceClassEx(int LayerIndex);
int          GUI_DEVICE_GetDeviceClass  (void);
int          GUI_DEVICE_Link            (GUI_DEVICE * pDevice);
void         GUI_DEVICE_Unlink          (GUI_DEVICE * pDevice);
GUI_DEVICE * GUI_DEVICE__GetpDriver     (int LayerIndex);
GUI_DEVICE * GUI_DEVICE__GetpDevice     (int LayerIndex, int DeviceClass);

GUI_DEVICE * GUI_DEVICE_UnlinkTaskDevices(void);
void         GUI_DEVICE_LinkDevices      (GUI_DEVICE * pDevice);

/*********************************************************************
*
*       GUI_DIRTYDEVICE_INFO
*
*   Description
*     Information about the dirty device.
*/
typedef struct {
  void * pData;         // Pointer to the first changed pixel.\sup{*1}
  int    x0;            // Leftmost position of changed area.
  int    y0;            // Topmost position of changed area.
  int    xSize;         // Size in X of changed area.
  int    ySize;         // Size in Y of changed area.
  int    LineOff;       // Number of pixels (stride) from one line to the next line.\sup{*1}
  int    BytesPerPixel; // Number of bytes required per pixel.
  int    IsDirty;       // Indicates if dirty pixels exist.
} GUI_DIRTYDEVICE_INFO;

int GUI_DIRTYDEVICE_Create      (void);
int GUI_DIRTYDEVICE_CreateEx    (int LayerIndex);
int GUI_DIRTYDEVICE_CreateExInfo(GUI_DIRTYDEVICE_INFO * pInfo, int LayerIndex);
int GUI_DIRTYDEVICE_Delete      (void);
int GUI_DIRTYDEVICE_DeleteEx    (int LayerIndex);
int GUI_DIRTYDEVICE_Fetch       (GUI_DIRTYDEVICE_INFO * pInfo);
int GUI_DIRTYDEVICE_FetchEx     (GUI_DIRTYDEVICE_INFO * pInfo, int LayerIndex);

/*********************************************************************
*
*       GUI_GCACHE
*/
//
// GCache modules, do not call directly!
//
int GUI_GCACHE_SetMode1bpp (GUI_DEVICE * pDevice);
int GUI_GCACHE_SetMode4bpp (GUI_DEVICE * pDevice);
int GUI_GCACHE_SetMode16bpp(GUI_DEVICE * pDevice);

/*********************************************************************
*
*       GUI_GCACHE modes
* 
*  Description
*    GUI_GCACHE bit depths to be used for GUI_GCACHE_Create() and the
*    other creation functions.
*/
#define GUI_GCACHE_1   (&GUI_GCACHE_SetMode1bpp)     // Use global cache with 1bpp  color depth.
#define GUI_GCACHE_4   (&GUI_GCACHE_SetMode4bpp)     // Use global cache with 4bpp  color depth.
#define GUI_GCACHE_16  (&GUI_GCACHE_SetMode16bpp)    // Use global cache with 16bpp color depth.
/* emDoc stop mark */

//
// Public functions
//
int GUI_GCACHE_Create      (int (* pfMode)(GUI_DEVICE *), const LCD_API_COLOR_CONV * pColorConvAPI);
int GUI_GCACHE_CreateEx    (int (* pfMode)(GUI_DEVICE *), const LCD_API_COLOR_CONV * pColorConvAPI, int LayerIndex);
int GUI_GCACHE_CreateUserEx(int (* pfMode)(GUI_DEVICE *), const LCD_API_COLOR_CONV * pColorConvAPI, int LayerIndex, void * pUserBuffer);
//
// Compatibility macros
//
#define GUI_GCACHE_1_Create(pColorConvAPI)                 GUI_GCACHE_Create  (GUI_GCACHE_1, pColorConvAPI)
#define GUI_GCACHE_1_CreateEx(LayerIndex, pColorConvAPI)   GUI_GCACHE_CreateEx(GUI_GCACHE_1, LayerIndex, pColorConvAPI)
#define GUI_GCACHE_4_Create(pColorConvAPI)                 GUI_GCACHE_Create  (GUI_GCACHE_4, pColorConvAPI)
#define GUI_GCACHE_4_CreateEx(LayerIndex, pColorConvAPI)   GUI_GCACHE_CreateEx(GUI_GCACHE_4, LayerIndex, pColorConvAPI)

/*********************************************************************
*
*       GUI_DCACHE
*/
void GUI_DCACHE_SetClearCacheHook(void (* pFunc)(U32 LayerMask));
void GUI_DCACHE_Clear            (U32 LayerMask);

extern void (* GUI_DCACHE__pfClearCacheHook)(U32 LayerMask);

/*********************************************************************
*
*       GUI_SOFTLAYER_CONFIG
*
*  Description
*    Data structure used by GUI_SOFTLAYER_Enable() to configurate a
*    soft layer.
*/
typedef struct {
  int xPos;      // X-position.
  int yPos;      // Y-position.
  int xSize;     // X-size.
  int ySize;     // Y-size.
  int Visible;   // 1 = visible, 0 = not visible.
} GUI_SOFTLAYER_CONFIG;

int  GUI_SOFTLAYER_Enable           (GUI_SOFTLAYER_CONFIG * pConfig, int NumLayers, GUI_COLOR CompositeColor);
int  GUI_SOFTLAYER_Refresh          (void);
void GUI_SOFTLAYER_SetCompositeColor(U32 Color);
int  GUI_SOFTLAYER_MULTIBUF_Enable  (int OnOff);

/*********************************************************************
*
*       General routines
*/
void             GUI_Exit                 (void);
const GUI_RECT * GUI_GetClipRect          (void);
GUI_COLOR        GUI_GetDefaultBkColor    (void);
GUI_COLOR        GUI_GetDefaultColor      (void);
const GUI_FONT * GUI_GetDefaultFont       (void);
int              GUI_GetScreenSizeX       (void);
int              GUI_GetScreenSizeY       (void);
const char *     GUI_GetVersionString     (void);
int              GUI_Init                 (void);
int              GUI_IsInitialized        (void);
void             GUI_SetAfterExitHook     (void (* pFunc)(void));
void             GUI_SetAfterInitHook     (void (* pFunc)(void));
void             GUI_SetPreInitHook       (void (* pFunc)(void));
void             GUI_RegisterAfterInitHook(void (* pFunc)(void), GUI_REGISTER_INIT * pRegisterInit);
void             GUI_RestoreContext       (const GUI_CONTEXT * pContext);
void             GUI_SaveContext          (GUI_CONTEXT * pContext);
const GUI_RECT * GUI_SetClipRect          (const GUI_RECT * pRect);
void             GUI_SetControlHook       (void (* pFunc)(int LayerIndex, int Cmd));
void             GUI_SetDefault           (void);
void             GUI_SetDefaultBkColor    (GUI_COLOR Color);
void             GUI_SetDefaultColor      (GUI_COLOR Color);
void             GUI_SetDefaultFont       (const GUI_FONT * pFont);
GUI_DRAWMODE     GUI_SetDrawMode          (GUI_DRAWMODE dm);
void             GUI_SetScreenSizeX       (int xSize);
void             GUI_SetScreenSizeY       (int ySize);
void             GUI_SetRefreshHook       (void (* pFunc)(void));
void             MainTask                 (void);

#define GUI_PID_SetInitFunc(x) GUI_SetAfterInitHook(x)  // Compatibility

/*********************************************************************
*
*       Function replacement
*/
void GUI_SetpfMemset(void * (* pFunc)(void * pDest, int c, size_t Cnt));
void GUI_SetpfMemcpy(void * (* pFunc)(void * pDest, const void * pSrc, size_t Cnt));
void GUI_SetpfStrcmp(int    (* pFunc)(const char *, const char *));
void GUI_SetpfStrlen(size_t (* pFunc)(const char *));
void GUI_SetpfStrcpy(char * (* pFunc)(char *, const char *));

/*********************************************************************
*
*       Rectangle helper functions
*/
void GUI_AddRect        (GUI_RECT * pDest, const GUI_RECT * pRect, int Dist);
int  GUI_RectsIntersect (const GUI_RECT * pr0, const GUI_RECT * pr1);
void GUI_MoveRect       (GUI_RECT * pRect, int x, int y);
void GUI_MergeRect      (GUI_RECT * pDest, const GUI_RECT * pr0, const GUI_RECT * pr1);
int  GUI__IntersectRects(GUI_RECT * pDest, const GUI_RECT * pr0, const GUI_RECT * pr1);
void GUI__IntersectRect (GUI_RECT * pDest, const GUI_RECT * pr0);
void GUI__ReduceRect    (GUI_RECT * pDest, const GUI_RECT * pRect, int Dist);

/*********************************************************************
*
*       Misc helper functions
*/
I32              GUI__ATan2                (I32 x, I32 y, I32 * ph);
I32              GUI__ASinHQ               (I32 SinHQ);
int              GUI__CompactPixelIndices  (LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel);
int              GUI__CompactPixelIndicesEx(LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel, const LCD_API_COLOR_CONV * pColorConvAPI);
int              GUI__ConvertColor2Index   (LCD_PIXELINDEX * pBuffer, int NumPixels, int BitsPerPixel, const LCD_API_COLOR_CONV * pColorConvAPI, void * pResult);
void             GUI__Config               (void);
GUI_HMEM         GUI__CopyText             (GUI_HMEM hItem, const char * pItem);
I32              GUI__CosHQ                (I32 Ang1000);
int              GUI__DivideRound          (int a, int b);
I32              GUI__DivideRound32        (I32 a, I32 b);
void             GUI__DrawTwinArc2         (int xl, int xr, int y0,         int r, GUI_COLOR ColorR0, GUI_COLOR ColorR1, GUI_COLOR ColorFill);
void             GUI__DrawTwinArc4         (int x0, int y0, int x1, int y1, int r, GUI_COLOR ColorR0, GUI_COLOR ColorR1, GUI_COLOR ColorFill);
void             GUI__ExpandPixelIndices   (void * pBuffer, int NumPixels, int BitsPerPixel);
void             GUI__ExpandPixelIndicesEx (void * pBufferSRC, void * pBufferDST, int NumPixels, int BitsPerPixel, const LCD_API_COLOR_CONV * pColorConvAPI);
void             GUI__FillTrippleArc       (int x0, int y0, int Size, GUI_COLOR ColorR0, GUI_COLOR ColorR1, GUI_COLOR ColorR2, GUI_COLOR ColorFill);
const GUI_RECT * GUI__GetClipRect          (void);
void             GUI__RegisterExit         (GUI_REGISTER_EXIT * pRegisterExit);
void             GUI__RegisterInit         (GUI_REGISTER_INIT * pRegisterInit);
int              GUI__SetText              (GUI_HMEM * phText, const char * s);
I32              GUI__SinHQ                (I32 Ang1000);
I32              GUI__sqrt32               (I32 Square);
char           * GUI__itoa                 (int Value, char * s, int Base);


/*********************************************************************
*
*       Optional function replacement
*/
void * GUI__memcpy(void * pDest, const void * pSrc, size_t NumBytes);
void * GUI__memset(void * pDest, int c, size_t Cnt);

/*********************************************************************
*
*       Get / Set Attributes
*/
GUI_COLOR GUI_GetBkColor     (void);
int       GUI_GetBkColorIndex(void);
GUI_COLOR GUI_GetColor       (void);
int       GUI_GetColorIndex  (void);
U8        GUI_GetLineStyle   (void);
U16       GUI_GetPenSize     (void);
U8        GUI_GetPenShape    (void);
unsigned  GUI_GetPixelIndex  (int x, int y);

void      GUI_SetBkColor     (GUI_COLOR);
void      GUI_SetColor       (GUI_COLOR);
void      GUI_SetBkColorIndex(LCD_PIXELINDEX Index);
void      GUI_SetColorIndex  (LCD_PIXELINDEX Index);

GUI_COLOR GUI_SetBlendColorEx  (GUI_COLOR Color0, GUI_COLOR Color1, U16 Intens, U16 IMax);
GUI_COLOR GUI_SetBlendColor    (GUI_COLOR Color0, GUI_COLOR Color1, U8 Intens);
GUI_COLOR GUI_SetBlendBkColorEx(GUI_COLOR Color0, GUI_COLOR Color1, U16 Intens, U16 IMax);
GUI_COLOR GUI_SetBlendBkColor  (GUI_COLOR Color0, GUI_COLOR Color1, U8 Intens);

U16       GUI_SetPenSize  (U16 Size);
U8        GUI_SetPenShape (U8 Shape);
U8        GUI_SetLineStyle(U8 Style);

/* Get/Set Character used as decimal point (usually '.' or ',') */
char      GUI_GetDecChar(void);
char      GUI_SetDecChar(char c);

/*********************************************************************
*
*       Color / Index related functions
*/
GUI_COLOR GUI_BlendColors      (GUI_COLOR Color0, GUI_COLOR Color1, U16 Intens, U16 IMax);
int       GUI_Color2Index      (GUI_COLOR color);
GUI_COLOR GUI_Color2VisColor   (GUI_COLOR color);
char      GUI_ColorIsAvailable (GUI_COLOR color);
int       GUI_ColorIsOpaque    (GUI_COLOR Color);
GUI_COLOR GUI_Index2Color      (int Index);
U32       GUI_CalcColorDist    (GUI_COLOR Color0, GUI_COLOR  Color1);
U32       GUI_CalcVisColorError(GUI_COLOR color);

/*********************************************************************
*
*       Error handler
*/
void GUI_SetOnLogFunc  (void (* pFunc)(const char * s));
void GUI_SetOnWarnFunc (void (* pFunc)(const char * s));
void GUI_SetOnErrorFunc(void (* pFunc)(const char * s));

/*********************************************************************
*
*       Logging (for debugging primarily)
*/
void GUI_Log      (const char * s);
void GUI_Log1     (const char * s, I32 p0);
void GUI_Log2     (const char * s, I32 p0, I32 p1);
void GUI_Log3     (const char * s, I32 p0, I32 p1, I32 p2);
void GUI_Log4     (const char * s, I32 p0, I32 p1, I32 p2,I32 p3);
void GUI_Warn     (const char * s);
void GUI_Warn1    (const char * s, I32 p0);
void GUI_Warn2    (const char * s, I32 p0, I32 p1);
void GUI_Warn3    (const char * s, I32 p0, I32 p1, I32 p2);
void GUI_Warn4    (const char * s, I32 p0, I32 p1, I32 p2, I32 p3);
void GUI_ErrorOut (const char * s);
void GUI_ErrorOut1(const char * s, I32 p0);
void GUI_ErrorOut2(const char * s, I32 p0, I32 p1);
void GUI_ErrorOut3(const char * s, I32 p0, I32 p1, I32 p2);
void GUI_ErrorOut4(const char * s, I32 p0, I32 p1, I32 p2, I32 p3);

/*********************************************************************
*
*       2d - GL
*/
void GUI_Clear                 (void);
void GUI_ClearRect             (int x0, int y0, int x1, int y1);
void GUI_ClearRectEx           (const GUI_RECT * pRect);
void GUI_CopyRect              (int x0, int y0, int x1, int y1, int dx, int dy);
void GUI_DrawArc               (int x0, int y0, int rx, int ry, int a0, int a1);
void GUI_DrawArcHR             (int x0, int y0, int rx, int ry, int a0, int a1);
void GUI_DrawArcHREx           (int x0, int y0, int r, int a0, int a1, int c0, int c1);
void GUI_DrawBitmap            (const GUI_BITMAP * pBM, int x0, int y0);
void GUI_DrawBitmapMag         (const GUI_BITMAP * pBM, int x0, int y0, int XMul, int YMul);
void GUI_DrawBitmapEx          (const GUI_BITMAP * pBM, int x0, int y0, int xCenter, int yCenter, int xMag, int yMag);
void GUI_DrawBitmapExp         (int x0, int y0, int XSize, int YSize, int XMul,  int YMul, int BitsPerPixel, int BytesPerLine, const U8 * pData, const GUI_LOGPALETTE * pPal);
void GUI_DrawBitmapHWAlpha     (const GUI_BITMAP * pBM, int x0, int y0);
void GUI_DrawCircle            (int x0, int y0, int r);
void GUI_DrawEllipse           (int x0, int y0, int rx, int ry);
void GUI_DrawEllipseXL         (int x0, int y0, int rx, int ry);
void GUI_DrawGradientH         (int x0, int y0, int x1, int y1, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientHEx       (const GUI_RECT * pRect, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientMH        (int x0, int y0, int y1, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientMHEx      (const GUI_RECT * pRect, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientMV        (int x0, int y0, int x1, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientMVEx      (const GUI_RECT * pRect, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientV         (int x0, int y0, int x1, int y1, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientVEx       (const GUI_RECT * pRect, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedH  (int x0, int y0, int x1, int y1, int rd, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedMH (int x0, int y0, int y1, int rd, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientRoundedHEx(const GUI_RECT * pRect, int rd, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedMHEx(const GUI_RECT * pRect, int rd, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientRoundedV  (int x0, int y0, int x1, int y1, int rd, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedMV (int x0, int y0, int x1, int rd, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGradientRoundedVEx(const GUI_RECT * pRect, int rd, GUI_COLOR Color0, GUI_COLOR Color1);
void GUI_DrawGradientRoundedMVEx(const GUI_RECT * pRect, int rd, GUI_GRADIENT_INFO * pGradientInfo, int NumColors);
void GUI_DrawGraph             (I16 * pay, int NumPoints, int x0, int y0);
void GUI_DrawGraphEx           (I16 * pay, int NumPoints, int x0, int y0, int Numerator, int Denominator, int MirrorX);
void GUI_DrawHLine             (int y0, int x0, int x1);
void GUI_DrawLine              (int x0, int y0, int x1, int y1);
void GUI_DrawLineRel           (int dx, int dy);
void GUI_DrawLineTo            (int x, int y);
void GUI_DrawPie               (int x0, int y0, int r, int a0, int a1, int Type);
void GUI_DrawPieHR             (int x0, int y0, int r, I32 a0, I32 a1);
void GUI_DrawPixel             (int x, int y);
void GUI_DrawPoint             (int x, int y);
void GUI_DrawPolygon           (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_DrawPolyLine          (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_DrawFocusRect         (const GUI_RECT  * pRect, int Dist);
void GUI_DrawFrame             (int x0, int y0, int x1, int y1, int Size);
void GUI_DrawFrameEx           (const GUI_RECT * pRect, int Size);
void GUI_DrawRect              (int x0, int y0, int x1, int y1);
void GUI_DrawRectEx            (const GUI_RECT * pRect);
void GUI_DrawRoundedFrame      (int x0, int y0, int x1, int y1, int r, int w);
void GUI_DrawRoundedFrameEx    (const GUI_RECT * pRect, int r, int w);
void GUI_DrawRoundedRect       (int x0, int y0, int x1, int y1, int r);
void GUI_DrawRoundedRectEx     (const GUI_RECT * pRect, int r);
void GUI_DrawVLine             (int x0, int y0, int y1);
void GUI_FillCircle            (int x0, int y0, int r);
void GUI_FillEllipse           (int x0, int y0, int rx, int ry);
void GUI_FillEllipseXL         (int x0, int y0, int rx, int ry);
void GUI_FillPolygon           (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
void GUI_FillRect              (int x0, int y0, int x1, int y1);
void GUI_FillRectEx            (const GUI_RECT * pRect);
void GUI_FillRoundedRect       (int x0, int y0, int x1, int y1, int r);
void GUI_FillRoundedRectEx     (const GUI_RECT * pRect, int r);
void GUI_GetClientRect         (GUI_RECT * pRect);
void GUI_InvertRect            (int x0, int y0, int x1, int y1);
void GUI_MoveRel               (int dx, int dy);
void GUI_MoveTo                (int x, int y);
void GUI_SetAlphaMask8888      (U32 OrMask, U32 AndMask);
void GUI_SetFuncFillCircle     (int (* pfFillCircle)(int x0, int y0, int r));
void GUI_SetFuncDrawCircle     (int (* pfDrawCircle)(int x0, int y0, int r));
void GUI_SetFuncDrawLine       (int (* pfDrawLine)(int x0, int y0, int x1, int y1));
void GUI_SetFuncDrawRect       (int (* pfDrawRect)(int x0, int y0, int x1, int y1));
void GUI_SetFuncFillRoundedRect(int (* pfFillRoundedRect)(int x0, int y0, int x1, int y1, int r));
void GUI_SetFuncDrawRoundedRect(int (* pfDrawRoundedRect)(int x0, int y0, int x1, int y1, int r));
void GUI_SetFuncDrawBitmapEx   (int (* pfDrawBitmapEx)(const GUI_BITMAP * pBitmap, int x0, int y0, int xMag, int yMag));


/*********************************************************************
*
*       GUI_GET_DATA_FUNC
* 
*  Function description
*    Abstract function type which may refer to either \uref{GUI_GET_DATA_FUNC_I}
*    or \uref{GUI_GET_DATA_FUNC_II}.
*
*    It should only be used for function pointers which accept both above
*    mentioned functions.
*/
typedef int GUI_GET_DATA_FUNC(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);

/*********************************************************************
*
*       GUI_GET_DATA_FUNC_I
* 
*  Function description
*    A callback which is used for loading external data into RAM.
* 
*    This routine must manage its own buffer to read the external data
*    into and pass the address to this buffer to *ppData.
* 
*  Parameters
*    p:        [IN]     Application defined void pointer, could be e.g.
*                       a file handle.
*    ppData:   [IN/OUT] This function must set ppData to point to the
*                       location where the requested data resides in.
*    NumBytes:          Number of requested bytes.
*    Off:               Defines the offset to use for reading the source data.
* 
*  Return value
*    The number of bytes which were actually read. If the number of read bytes does not match,
*    the drawing function will return immediately.
*/
typedef int GUI_GET_DATA_FUNC_I(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);

/*********************************************************************
*
*       GUI_GET_DATA_FUNC_II
* 
*  Function description
*    A callback which is used for loading external data into RAM.
* 
*    This routine does not manage its own buffer. Instead, it receives a
*    preallocated buffer to load the data into. The address to this buffer
*    is *ppData.
* 
*  Parameters
*    p:        [IN] Application defined void pointer, could be e.g.
*                   a file handle.
*    ppData:   [IN] Points to the location of the buffer address that needs
*                   to be filled with data read by this function.
*    NumBytes:      Number of requested bytes.
*    Off:           Defines the offset to use for reading the source data.
* 
*  Return value
*    The number of bytes which were actually read. If the number of read bytes does not match,
*    the drawing function will return immediately.
*/
typedef int GUI_GET_DATA_FUNC_II(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);


/*********************************************************************
*
*       IMAGE file support
*/
/*********************************************************************
*
*       GIF file support
*/
/*********************************************************************
*
*       GUI_GIF_GET_DATA_FUNC
* 
*  Description
*    GetData function used for the GIF format, for more details see
*    \uref{GUI_GET_DATA_FUNC_I}.
*/
typedef GUI_GET_DATA_FUNC_I    GUI_GIF_GET_DATA_FUNC;

int GUI_GIF_Draw           (const void * pGIF, U32 NumBytes,             int x0, int y0);
int GUI_GIF_DrawEx         (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int GUI_GIF_DrawSub        (const void * pGIF, U32 NumBytes,             int x0, int y0, int Index);
int GUI_GIF_DrawSubEx      (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Index);
int GUI_GIF_DrawSubScaled  (const void * pGIF, U32 NumBytes,             int x0, int y0, int Index, int Num, int Denom);
int GUI_GIF_DrawSubScaledEx(GUI_GIF_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Index, int Num, int Denom);
int GUI_GIF_GetComment     (const void * pGIF, U32 NumBytes,             U8 * pBuffer, int MaxSize, int Index);
int GUI_GIF_GetCommentEx   (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p, U8 * pBuffer, int MaxSize, int Index);
int GUI_GIF_GetImageInfo   (const void * pGIF, U32 NumBytes,             GUI_GIF_IMAGE_INFO * pInfo, int Index);
int GUI_GIF_GetImageInfoEx (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p, GUI_GIF_IMAGE_INFO * pInfo, int Index);
int GUI_GIF_GetInfo        (const void * pGIF, U32 NumBytes,             GUI_GIF_INFO * pInfo);
int GUI_GIF_GetInfoEx      (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p, GUI_GIF_INFO * pInfo);
int GUI_GIF_GetXSize       (const void * pGIF);
int GUI_GIF_GetXSizeEx     (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p);
int GUI_GIF_GetYSize       (const void * pGIF);
int GUI_GIF_GetYSizeEx     (GUI_GIF_GET_DATA_FUNC * pfGetData, void * p);
int GUI_GIF_SetFillTrans   (int OnOff);

/*********************************************************************
*
*       BMP file support
*/
/*********************************************************************
*
*       GUI_BMP_GET_DATA_FUNC
* 
*  Description
*    GetData function used for the BMP format, for more details see
*    \uref{GUI_GET_DATA_FUNC_I}.
*/
typedef GUI_GET_DATA_FUNC_I     GUI_BMP_GET_DATA_FUNC;

int  GUI_BMP_Draw        (const void * pFileData,                      int x0, int y0);
int  GUI_BMP_DrawEx      (GUI_BMP_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int  GUI_BMP_DrawScaled  (const void * pFileData,                      int x0, int y0, int Num, int Denom);
int  GUI_BMP_DrawScaledEx(GUI_BMP_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom);
int  GUI_BMP_GetXSize    (const void * pFileData);
int  GUI_BMP_GetXSizeEx  (GUI_BMP_GET_DATA_FUNC * pfGetData, void * p);
int  GUI_BMP_GetYSize    (const void * pFileData);
int  GUI_BMP_GetYSizeEx  (GUI_BMP_GET_DATA_FUNC * pfGetData, void * p);
void GUI_BMP_EnableAlpha (void);
void GUI_BMP_EnableBE    (int OnOff);
void GUI_BMP_DisableAlpha(void);

/*********************************************************************
*
*       PNG file support
*/
/*********************************************************************
*
*       GUI_PNG_GET_DATA_FUNC
* 
*  Description
*    GetData function used for the PNG format, for more details see
*    \uref{GUI_GET_DATA_FUNC_II}.
*/
typedef GUI_GET_DATA_FUNC_II    GUI_PNG_GET_DATA_FUNC;

int GUI_PNG_Draw      (const void * pFileData, int DataSize,        int x0, int y0);
int GUI_PNG_DrawEx    (GUI_PNG_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int GUI_PNG_GetXSize  (const void * pFileData, int FileSize);
int GUI_PNG_GetXSizeEx(GUI_PNG_GET_DATA_FUNC * pfGetData, void * p);
int GUI_PNG_GetYSize  (const void * pFileData, int FileSize);
int GUI_PNG_GetYSizeEx(GUI_PNG_GET_DATA_FUNC * pfGetData, void * p);

/*********************************************************************
*
*       JPEG file support
*/
/*********************************************************************
*
*       GUI_JPEG_GET_DATA_FUNC
* 
*  Description
*    GetData function used for the JPEG format, for more details see
*    \uref{GUI_GET_DATA_FUNC_I}.
*/
typedef GUI_GET_DATA_FUNC_I     GUI_JPEG_GET_DATA_FUNC;

/*********************************************************************
*
*       GUI_JPEG_INFO
*
*   Description
*     Information about a JPEG image.
*/
typedef struct {
  int XSize;       // X-size of the image.
  int YSize;       // Y-size of the image.
  int Progessive;  // Indicates if JPEG is progressive or not
} GUI_JPEG_INFO;

typedef struct GUI_JPEG_DCONTEXT GUI_JPEG_DCONTEXT;
/*********************************************************************
*
*       GUI_JPEG_WRITECLIPPEDPIXELS_FUNC
*
*   Description
*     Function that retrieves the decoded result of a JPEG and writes
*     the pixels to the display.
* 
*   Parameters
*     x0:            X drawing position of the JPEG.
*     y0:            Y drawing position of the JPEG.
*     xSize:         Width of one line.
*     pColor:        [IN] Pointer to decoded JPEG data.
*     pContext:      [IN] Pointer to JPEG context.
*     pColorConvAPI: [IN] Pointer to selected color conversion API.
* 
*/
typedef void (GUI_JPEG_WRITECLIPPEDPIXELS_FUNC)(int x0, int y0, int xSize, LCD_COLOR * pColor, GUI_JPEG_DCONTEXT * pContext, const LCD_API_COLOR_CONV * pColorConvAPI);

int                                GUI_JPEG_Draw            (const void * pFileData, int DataSize,         int x0, int y0);
int                                GUI_JPEG_DrawEx          (GUI_JPEG_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
int                                GUI_JPEG_DrawScaled      (const void * pFileData, int DataSize,         int x0, int y0, int Num, int Denom);
int                                GUI_JPEG_DrawScaledEx    (GUI_JPEG_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom);
int                                GUI_JPEG_GetInfo         (const void * pFileData, int DataSize,         GUI_JPEG_INFO * pInfo);
int                                GUI_JPEG_GetInfoEx       (GUI_JPEG_GET_DATA_FUNC * pfGetData, void * p, GUI_JPEG_INFO * pInfo);
void                               GUI_JPEG_SetpfDrawEx     (int (* pfDrawEx)(GUI_JPEG_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0));
GUI_JPEG_WRITECLIPPEDPIXELS_FUNC * GUI_JPEG_SetpfWritePixels(GUI_JPEG_WRITECLIPPEDPIXELS_FUNC * pFunc);

/*********************************************************************
*
*       SVG file support
*/
typedef GUI_HMEM    GUI_SVG_Handle;

/*********************************************************************
*
*       GUI_SVG_GET_DATA_FUNC
* 
*  Description
*    GetData function used for the SVG format, for more details see
*    \uref{GUI_GET_DATA_FUNC_II}.
*/
typedef GUI_GET_DATA_FUNC_II    GUI_SVG_GET_DATA_FUNC;

/*********************************************************************
*
*       GUI_POINTF
*
*  Description
*    Defines a point in floating point coordinates.
*/
typedef struct {
  float x; // X coordinate in floating points.
  float y; // Y coordinate in floating points.
} GUI_POINTF;

/*********************************************************************
*
*       GUI_MATRIX
*
*  Description
*    A column-major 3x3 matrix used for all kinds of affine transformations.
*
*  Additional information
*    emWin uses column-major matrices which means that the matrix elements
*    are stored in columns from top to bottom and left to right.
* 
*    The order is the same as in OpenVG, therefore GUI_MATRIX is implicitly
*    convertible to the 3x3 matrix type used in OpenVG.
*/
typedef struct {
  float sx;   // The scaling factor in the x-direction.
  float shy;  // The shearing factor in the y-direction.
  float w0;   // Should be left as \c{0.0F} to ensure affinity.
  float shx;  // The shearing factor in the x-direction.
  float sy;   // Scaling factor in y.
  float w1;   // Should be left as \c{0.0F} to ensure affinity.
  float tx;   // The translation factor in the x-direction.
  float ty;   // The translation factor in the y-direction. 
  float w2;   // Should be left as \c{1.0F} to ensure affinity.
} GUI_MATRIX;

void    GUI_MATRIX_Initialize   (GUI_MATRIX * pMatrix, float sx, float shx, float tx, float shy, float sy, float ty, float w0, float w1, float w2);
void    GUI_MATRIX_Identity     (GUI_MATRIX * pMatrix);
float * GUI_MATRIX_GetCellPtr   (GUI_MATRIX * pMatrix, unsigned Row, unsigned Col);
int     GUI_MATRIX_Equals       (const GUI_MATRIX * p0, const GUI_MATRIX * p1);
void    GUI_MATRIX_Multiply     (GUI_MATRIX * pMatrix, const GUI_MATRIX * pMult);
void    GUI_MATRIX_MultiplyPoint(const GUI_MATRIX * pMatrix, GUI_POINTF * pPoint);
void    GUI_MATRIX_Translate    (GUI_MATRIX * pMatrix, float tx, float ty);
void    GUI_MATRIX_Scale        (GUI_MATRIX * pMatrix, float sx, float sy);
void    GUI_MATRIX_Rotate       (GUI_MATRIX * pMatrix, float a);
void    GUI_MATRIX_Shear        (GUI_MATRIX * pMatrix, float shx, float shy);

/*********************************************************************
*
*       GUI_SVG_VIEWBOX
*
*  Description
*    The SVG's view box, as it is defined by the \c{viewBox} attribute
*    in the \c{<svg>} tag.
*/
typedef struct {
  float x;      // X position of the view box.
  float y;      // Y position of the view box.
  float xSize;  // Width of the view box.
  float ySize;  // Height of the view box.
} GUI_SVG_VIEWBOX;

/*********************************************************************
*
*       GUI_SVG_BBOX
*
*  Description
*    Boundary box of an SVG based on the currently set parameters like position,
*    scaling, rotation, etc.
*/
typedef struct {
  float xMin;  // Minimum X position on the display in floating point coordinates.
  float yMin;  // Minimum Y position on the display in floating point coordinates.
  float xMax;  // Maximum X position on the display in floating point coordinates.
  float yMax;  // Maximum Y position on the display in floating point coordinates.
} GUI_SVG_BBOX;

/*********************************************************************
*
*       GUI_SVG_INFO
*
*  Description
*    Information about an SVG document, generated by the \c{GUI_SVG_GetInfo...()}
*    functions.
* 
*  Additional information
*    In case the \c{"width"} and \c{"height"} attributes of the SVG are in any other
*    unit than pixels, the dimensions will be converted into pixels internally.
*    Therefore, the struct members Width and Height are always in pixels.
*    The conversion from other units into pixels is done using the currently set DPI
*    (see GUI_SVG_SetDPI()).
*/
typedef struct {
  GUI_SVG_VIEWBOX ViewBox;  // Viewbox of the SVG, defines the coordinate range.
  GUI_SVG_BBOX    BBox;     // Boundary box of the SVG, based on the currently set affine transformation matrix.
  float           xSize;    // Viewport width of the SVG, defined by the \c{"width"} attribute in the \c{<svg>} tag.
  float           ySize;    // Viewport height of the SVG, defined by the \c{"height"} attribute in the \c{<svg>} tag.
} GUI_SVG_INFO;

/*********************************************************************
*
*       GUI_SVG_HOOKS
*
*  Description
*    Allows to hook in into various spots during the SVG drawing process.
* 
*    Only the desired hooks need to be set. The hooks can be set with GUI_SVG_SetHooks().
*/
typedef struct {
  U8   (* pfPreInitDriverHook) (void);  // Called before the SVG driver is initialized. Returns 1 on error
                                        // to abort the driver initialization.
  U8   (* pfPostInitDriverHook)(void);  // Called when the SVG driver initialization has finished. Returns 1 on error
                                        // to abort the driver initialization.
  void (* pfDeinitDriverHook)  (void);  // Called after the SVG driver has been de-initialized.
  void (* pfSwitchBufferHook)  (void);  // Called during the drawing process, after the drawing commands have been flushed.
  void (* pfBeginDrawPreHook)  (void);  // Called before the initialization of the drawing process of an SVG.
  void (* pfBeginDrawPostHook) (void);  // Called at the end of the initialization of the drawing process of an SVG.
  void (* pfEndDrawHook)       (void);  // Called once the SVG drawing process has finished.
} GUI_SVG_HOOKS;

/*********************************************************************
*
*       GUI_SVG_LOAD_API_CALLBACK
*
*  Description
*    Callback used to load a given routine. The callback receives the
*    name of the routine to be loaded as a string and should return its
*    address as a void function pointer.
* 
*    The main use case of this callback is to load a function from a DLL.
* 
*  Parameters
*    sFunction: [IN] Name of the routine to be loaded as a zero-terminated
*                    string.
*  
*  Return value
*    Address of the function casted to a void function pointer.
*/
typedef void (* GUI_SVG_LOAD_API_CALLBACK(const char * sFunction))(void);

//
// SVG module related
//
int      GUI_SVG_Enable           (int Enable);
void     GUI_SVG_EnablePNG        (void);
void     GUI_SVG_EnableJPEG       (void);
void     GUI_SVG_EnableGIF        (void);
void     GUI_SVG_EnableBMP        (void);
unsigned GUI_SVG_SetDPI           (unsigned NumDotsPerInch);
unsigned GUI_SVG_SetFileBufferSize(unsigned NumBytes);
//
// SVG drawing API
//
int GUI_SVG_Draw               (const void * pFile, U32 FileSize, float x, float y);
int GUI_SVG_DrawEx             (GUI_SVG_GET_DATA_FUNC * pfGetData, void * p, float x, float y);
int GUI_SVG_DrawScaled         (const void * pFile, U32 FileSize, float x, float y, float Scale);
int GUI_SVG_DrawScaledEx       (GUI_SVG_GET_DATA_FUNC * pfGetData, void * p, float x, float y, float Scale);
int GUI_SVG_DrawScaledRotated  (const void * pFile, U32 FileSize, float x, float y, float Scale, float Angle);
int GUI_SVG_DrawScaledRotatedEx(GUI_SVG_GET_DATA_FUNC * pfGetData, void * p, float x, float y, float Scale, float Angle);
//
// SVG handle API
//
GUI_SVG_Handle GUI_SVG_Create         (const void * pFile, U32 FileSize);
GUI_SVG_Handle GUI_SVG_CreateEx       (GUI_SVG_GET_DATA_FUNC * pfGetData, void * p);
void           GUI_SVG_Delete         (GUI_SVG_Handle hSVG);
int            GUI_SVG_DrawH          (GUI_SVG_Handle hSVG, float x, float y);
void           GUI_SVG_EnableCacheMode(GUI_SVG_Handle hSVG, int Enable);
void           GUI_SVG_Identity       (GUI_SVG_Handle hSVG);
int            GUI_SVG_Render         (GUI_SVG_Handle hSVG);
void           GUI_SVG_Rotate         (GUI_SVG_Handle hSVG, float Angle);
void           GUI_SVG_RotateEx       (GUI_SVG_Handle hSVG, float Angle, float x, float y);
void           GUI_SVG_Scale          (GUI_SVG_Handle hSVG, float xScale, float yScale);
void           GUI_SVG_ScaleEx        (GUI_SVG_Handle hSVG, float xScale, float yScale, float x, float y);
void           GUI_SVG_ScaleToSize    (GUI_SVG_Handle hSVG, float xSize, float ySize);
void           GUI_SVG_SetBkColor     (GUI_SVG_Handle hSVG, GUI_COLOR BkColor);
void           GUI_SVG_Transform      (GUI_SVG_Handle hSVG, const GUI_MATRIX * pMatrix);
void           GUI_SVG_Translate      (GUI_SVG_Handle hSVG, float x, float y);
//
// Info API
//
int GUI_SVG_GetInfo  (const void * pFile, U32 FileSize, GUI_SVG_INFO * pInfo);
int GUI_SVG_GetInfoEx(GUI_SVG_GET_DATA_FUNC * pfGetData, void * p, GUI_SVG_INFO * pInfo);
int GUI_SVG_GetInfoH (GUI_SVG_Handle hSVG, GUI_SVG_INFO * pInfo);

//
// Opaque driver type for public usage.
//
typedef void  GUI_SVG_DRIVER_STRUCT;

/*********************************************************************
*
*       GUI_SVG_DRIVER
* 
*  Description
*    All available SVG drivers for run-time usage with the GUI_SVG_DRIVER_... routines.
*/
typedef const GUI_SVG_DRIVER_STRUCT * (GUI_SVG_DRIVER)(void);

// 
// The pointer passed to identify a driver is a function
// so that only the directly referenced drivers are linked.
//
#define GUI_SVG_DRIVER_OPENVG    (&GUI_SVG_DRIVER_OpenVG)
#define GUI_SVG_DRIVER_VGLITE    (&GUI_SVG_DRIVER_VGLite)
#define GUI_SVG_DRIVER_NEMAVG    (&GUI_SVG_DRIVER_NemaVG)
#define GUI_SVG_DRIVER_NANOVG    (&GUI_SVG_DRIVER_NanoVG)

//
// Not to be documented, above defines are documented.
//
const GUI_SVG_DRIVER_STRUCT * GUI_SVG_DRIVER_OpenVG(void);
const GUI_SVG_DRIVER_STRUCT * GUI_SVG_DRIVER_VGLite(void);
const GUI_SVG_DRIVER_STRUCT * GUI_SVG_DRIVER_NemaVG(void);
const GUI_SVG_DRIVER_STRUCT * GUI_SVG_DRIVER_NanoVG(void);
//
// GUI_SVG_DRIVER API
//
void             GUI_SVG_DRIVER_BindAPI       (GUI_SVG_DRIVER * pDriver, const void * pAPI);
void             GUI_SVG_DRIVER_BindDynamicAPI(GUI_SVG_DRIVER * pDriver, void * pAPI, GUI_SVG_LOAD_API_CALLBACK * cbLoadFunction);
GUI_SVG_DRIVER * GUI_SVG_DRIVER_GetSelected   (void);
int              GUI_SVG_DRIVER_HasBoundAPI   (GUI_SVG_DRIVER * pDriver);
int              GUI_SVG_DRIVER_Select        (GUI_SVG_DRIVER * pDriver);
void             GUI_SVG_DRIVER_SetHooks      (GUI_SVG_DRIVER * pDriver, const GUI_SVG_HOOKS * pHooks);
//
// Compatability macros
//
#define GUI_SVG_SetHooks(pHooks)                           GUI_SVG_DRIVER_SetHooks(GUI_SVG_DRIVER_GetSelected(), pHooks)
#define GUI_SVG_SetAPI_VGLite(pAPI)                        GUI_SVG_DRIVER_BindAPI(GUI_SVG_DRIVER_VGLITE, pAPI); \
                                                           GUI_SVG_DRIVER_Select(GUI_SVG_DRIVER_VGLITE)
#define GUI_SVG_SetAPI_OpenVG(pAPI)                        GUI_SVG_DRIVER_BindAPI(GUI_SVG_DRIVER_OPENVG, pAPI); \
                                                           GUI_SVG_DRIVER_Select(GUI_SVG_DRIVER_OPENVG)
#define GUI_SVG_LoadAPI_OpenVG(pOpenVG, pEGL, cbLoad)      if (pOpenVG) {                                   \
                                                             (pOpenVG)->pEGL = pEGL;                        \
                                                             GUI_SVG_DRIVER_BindDynamicAPI(GUI_SVG_DRIVER_OPENVG, pOpenVG, cbLoad); \
                                                             GUI_SVG_DRIVER_Select(GUI_SVG_DRIVER_OPENVG);  \
                                                           }

/*********************************************************************
*
*       GUI_TIMER module
*/
#define GUI_TIMER_CF_WINDOW (1 << 0)
#define GUI_TIMER_CF_CURSOR (1 << 1)

typedef GUI_HMEM GUI_TIMER_HANDLE;

typedef struct {
  GUI_TIMER_TIME   Time;
  PTR_ADDR         Context;
  GUI_TIMER_HANDLE hTimer;
} GUI_TIMER_MESSAGE;

typedef void GUI_TIMER_CALLBACK(/*const*/ GUI_TIMER_MESSAGE* pTM);

GUI_TIMER_HANDLE GUI_TIMER_Create   (GUI_TIMER_CALLBACK * cb, GUI_TIMER_TIME Time, PTR_ADDR Context, U16 Flags);
void             GUI_TIMER_Delete   (GUI_TIMER_HANDLE hObj);

/* Methods changing properties */
GUI_TIMER_TIME GUI_TIMER_GetPeriod(GUI_TIMER_HANDLE hObj);
void           GUI_TIMER_SetPeriod(GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Period);
void           GUI_TIMER_SetDelay (GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Delay);
void           GUI_TIMER_Restart  (GUI_TIMER_HANDLE hObj);
int            GUI_TIMER_GetFlag  (GUI_TIMER_HANDLE hObj, int Flag); /* Not to be documented */
int            GUI_TIMER_Exec     (void);

/*********************************************************************
*
*       MOVIE file support
*/
/*********************************************************************
*
*       Movie notifications
*
*   Description
*     Notifications sent to the movie's callback function. The
*     callback function can be set with GUI_MOVIE_SetpfNotify().
*/
#define GUI_MOVIE_NOTIFICATION_PREDRAW  0    // Sent immediately before a frame is drawn.
#define GUI_MOVIE_NOTIFICATION_POSTDRAW 1    // Sent immediately after a frame is drawn.
#define GUI_MOVIE_NOTIFICATION_START    2    // Sent when starting to play a movie.
#define GUI_MOVIE_NOTIFICATION_STOP     3    // Sent when the movie has stopped.
#define GUI_MOVIE_NOTIFICATION_DELETE   4    // Sent when the movie has been deleted.

typedef GUI_HMEM GUI_MOVIE_HANDLE;

typedef void GUI_MOVIE_FUNC(GUI_MOVIE_HANDLE hMovie, int Notification, U32 CurrentFrame);

/*********************************************************************
*
*       GUI_MOVIE_INFO
*
*   Description
*     Information about a movie.
*/
typedef struct {
  int xSize;         // Horizontal resolution of the movie in pixels.
  int ySize;         // Vertical resolution of the movie in pixels.
  int msPerFrame;    // Period of one frame in ms.
  U32 NumFrames;     // Number of frames of the movie file.
} GUI_MOVIE_INFO;

/*********************************************************************
*
*       GUI_MOVIE_GET_DATA_FUNC
* 
*  Description
*    GetData function used for emWin movie files (EMF), for more details see
*    \uref{GUI_GET_DATA_FUNC_II}.
*/
typedef  GUI_GET_DATA_FUNC_II    GUI_MOVIE_GET_DATA_FUNC;

GUI_MOVIE_HANDLE GUI_MOVIE_Create         (const void * pFileData, U32 FileSize, GUI_MOVIE_FUNC * pfNotify);
GUI_MOVIE_HANDLE GUI_MOVIE_CreateEx       (GUI_MOVIE_GET_DATA_FUNC * pfGetData, void * pParam, GUI_MOVIE_FUNC * pfNotify);
int              GUI_MOVIE_Delete         (GUI_MOVIE_HANDLE hMovie);
void             GUI_MOVIE_DrawFrame      (GUI_MOVIE_HANDLE hMovie, int Index, int x, int y);
U32              GUI_MOVIE_GetFrameIndex  (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_GetInfo        (const void * pFileData, U32 FileSize, GUI_MOVIE_INFO * pInfo);
int              GUI_MOVIE_GetInfoEx      (GUI_MOVIE_GET_DATA_FUNC * pfGetData, void * pParam, GUI_MOVIE_INFO * pInfo);
int              GUI_MOVIE_GetInfoH       (GUI_MOVIE_HANDLE hMovie, GUI_MOVIE_INFO * pInfo);
int              GUI_MOVIE_GetNumFrames   (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_GetPos         (GUI_MOVIE_HANDLE hMovie, int * pxPos, int * pyPos, int * pxSize, int * pySize);
int              GUI_MOVIE_GotoFrame      (GUI_MOVIE_HANDLE hMovie, U32 Frame);
int              GUI_MOVIE_IsPlaying      (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_Pause          (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_Play           (GUI_MOVIE_HANDLE hMovie);
int              GUI_MOVIE_SetPeriod      (GUI_MOVIE_HANDLE hMovie, unsigned Period);
void             GUI_MOVIE_SetpfNotify    (GUI_MOVIE_FUNC * pfNotify);
int              GUI_MOVIE_SetPos         (GUI_MOVIE_HANDLE hMovie, int xPos, int yPos);
void             GUI_MOVIE_SetSecondHandle(GUI_MOVIE_HANDLE hMovie, void * pParamTable);
int              GUI_MOVIE_Show           (GUI_MOVIE_HANDLE hMovie, int xPos, int yPos, int DoLoop);

void             GUI_MOVIE__OnTimeSlice (GUI_MOVIE_HANDLE hMovie, int Run, int DeactivateWM);
int              GUI_MOVIE__Play        (GUI_MOVIE_HANDLE hMovie, GUI_TIMER_CALLBACK * cbTimer, int DoLoop, GUI_HMEM hExtra);
int              GUI_MOVIE__Pause       (GUI_MOVIE_HANDLE hMovie, int ShowFrame, int DeactivateWM);
void             GUI_MOVIE__SetTimerFlag(GUI_MOVIE_HANDLE hMovie);
GUI_MOVIE_HANDLE GUI_MOVIE__CreateUser  (GUI_MOVIE_GET_DATA_FUNC * pfGetData, void * pParam, GUI_MOVIE_FUNC * pfNotify, void * pVoid);
void           * GUI_MOVIE__GetUserData (GUI_MOVIE_HANDLE hMovie);
void             GUI_MOVIE__SetUserData (GUI_MOVIE_HANDLE hMovie, void * pVoid);

/*********************************************************************
*
*       Splines
*/
GUI_HMEM GUI_SPLINE_Create  (const int * px, const int * py, unsigned NumPoints);
void     GUI_SPLINE_Draw    (GUI_HMEM hSpline, int x, int y);
void     GUI_SPLINE_Delete  (GUI_HMEM hSpline);
I16      GUI_SPLINE_GetY    (GUI_HMEM hSpline, unsigned Index, float * py);
unsigned GUI_SPLINE_GetXSize(GUI_HMEM hSpline);
void     GUI_SPLINE_DrawAA  (GUI_HMEM hSpline, int x, int y, unsigned Width);

/*********************************************************************
*
*       Cursor routines
*/
#define GUI_CURSOR_SHOW 0
#define GUI_CURSOR_HIDE 1

typedef struct {
  const GUI_BITMAP  * pBitmap;
  int                 xHot;
  int                 yHot;
} GUI_CURSOR;

/*********************************************************************
*
*       GUI_CURSOR_ANIM
*
*  Description
*    Structure that stores information about a cursor animation used by GUI_CURSOR_SelectAnim().
*
*  Additional information
*    The bitmaps addressed by ppBM need to fulfill with the following requirements:
*    
*    \item They need to have exactly the same X- and Y-size.
*    \item They should not be compressed.
*    \item They need to be transparent.
*    \item They need to be palette based bitmaps with 1, 2, 4 or 8bpp.
*
*    Other bitmaps or insufficient memory cause the function to fail.
*
*    The pPeriod is only required if the periods for the images are different. If the same
*    period should be used for all images Period should be used instead of pPeriod. In
*    this case pPeriod should be \c{NULL}.
*
*    xHot and yHot determine the hot spot position of the cursor. This means the relative
*    position in X and Y from the upper left corner of the image to the position of the
*    pointer input device.
*
*    Customized cursors can be realized by passing a pointer to a custom defined
*    GUI_CURSOR_ANIM structure.
*/
typedef struct {
  const GUI_BITMAP ** ppBm;       // Pointer to an array of pointers to bitmaps to be used for the animated cursor.
  int                 xHot;       // X-position of hot spot. Details can be found below.
  int                 yHot;       // Y-position of hot spot. Details can be found below.
  unsigned            Period;     // Period to be used to switch between the images.
  const unsigned    * pPeriod;    // Pointer to an array containing the periods to be used to switch between the images.
  int                 NumItems;   // Number of images used for the animation.
} GUI_CURSOR_ANIM;

#if GUI_SUPPORT_CURSOR
  int                GUI_CURSOR_GetState     (void);
  int                GUI_CURSOR_GetStateEx   (int Layer);
  void               GUI_CURSOR_Hide         (void);
  void               GUI_CURSOR_HideEx       (int Layer);
  const GUI_CURSOR * GUI_CURSOR_Select       (const GUI_CURSOR * pCursor);
  const GUI_CURSOR * GUI_CURSOR_SelectEx     (const GUI_CURSOR * pCursor, int Layer);
  int                GUI_CURSOR_SelectAnim   (const GUI_CURSOR_ANIM * pCursorAnim);
  int                GUI_CURSOR_SelectAnimEx (const GUI_CURSOR_ANIM * pCursorAnim, int LayerIndex);
  int                GUI_CURSOR_SetBitmap    (const GUI_BITMAP * pBM);
  int                GUI_CURSOR_SetBitmapEx  (const GUI_BITMAP * pBM, int Layer);
  void               GUI_CURSOR_SetPosition  (int x, int y);
  void               GUI_CURSOR_SetPositionEx(int xNewPos, int yNewPos, int Layer);
  void               GUI_CURSOR_Show         (void);
  void               GUI_CURSOR_ShowEx       (int Layer);
  GUI_HSPRITE        GUI_CURSOR__GetSpriteEx (int LayerIndex, int * pxPos, int * pyPos);
  void               GUI_CURSOR__SetSpriteEx (GUI_HSPRITE hSprite, const GUI_CURSOR * pCursor, int LayerIndex);
#else
  #define GUI_CURSOR_Show();
  #define GUI_CURSOR_Clear();
#endif

/*********************************************************************
*
*       Sprite support
*/
#define GUI_SPRITE_CF_STAYONTOP (1 << 0)
#define GUI_SPRITE_CF_SHOW      (1 << 1)

#define GUI_SPRITE_SHOW 0
#define GUI_SPRITE_HIDE 1

GUI_HSPRITE GUI_SPRITE__CreateEx           (const GUI_BITMAP * pBM, int x, int y, int Layer, U16 Flags); /* Not to be documented, only used by cursor modul */
void        GUI_SPRITE__SetCallback        (GUI_HSPRITE hSprite, GUI_HMEM hContext, void (* pCB)(GUI_HSPRITE, int));
GUI_HSPRITE GUI_SPRITE_Create              (const GUI_BITMAP * pBM, int x, int y);
GUI_HSPRITE GUI_SPRITE_CreateAnim          (const GUI_BITMAP ** ppBm, int x, int y, unsigned Period, const unsigned * pPeriod, int NumItems);
GUI_HSPRITE GUI_SPRITE_CreateEx            (const GUI_BITMAP * pBM, int x, int y, int Layer);
GUI_HSPRITE GUI_SPRITE_CreateExAnim        (const GUI_BITMAP ** ppBm, int x, int y, unsigned Period, const unsigned * pPeriod, int NumItems, int LayerIndex);
GUI_HSPRITE GUI_SPRITE_CreateHidden        (const GUI_BITMAP * pBM, int x, int y);
GUI_HSPRITE GUI_SPRITE_CreateHiddenEx      (const GUI_BITMAP * pBM, int x, int y, int Layer);
void        GUI_SPRITE_Delete              (GUI_HSPRITE hSprite);
int         GUI_SPRITE_GetState            (GUI_HSPRITE hSprite);
void        GUI_SPRITE_Hide                (GUI_HSPRITE hSprite);
int         GUI_SPRITE_SetBitmap           (GUI_HSPRITE hSprite, const GUI_BITMAP * pBM);
int         GUI_SPRITE_SetBitmapAndPosition(GUI_HSPRITE hSprite, const GUI_BITMAP * pBM, int x, int y);
int         GUI_SPRITE_SetLoop             (GUI_HSPRITE hSprite, int OnOff);
void        GUI_SPRITE_SetPosition         (GUI_HSPRITE hSprite, int x, int y);
int         GUI_SPRITE_StartAnim           (GUI_HSPRITE hSprite);
int         GUI_SPRITE_StopAnim            (GUI_HSPRITE hSprite);
void        GUI_SPRITE_Show                (GUI_HSPRITE hSprite);

/*********************************************************************
*
*       Cursors and their bitmaps
*/
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowS,  GUI_CursorArrowSI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowM,  GUI_CursorArrowMI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorArrowL,  GUI_CursorArrowLI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossS,  GUI_CursorCrossSI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossM,  GUI_CursorCrossMI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossL,  GUI_CursorCrossLI;
extern GUI_CONST_STORAGE GUI_CURSOR GUI_CursorHeaderM, GUI_CursorHeaderMI;
extern GUI_CONST_STORAGE GUI_CURSOR_ANIM GUI_CursorAnimHourglassM;

/*********************************************************************
*
*       GUI_WRAPMODE
*
*  Description
*    Configuration how text will be wrapped.
*
*  Additional information
*    If word wrapping should be performed and the given rectangle is too small for a word, char
*    wrapping is executed at this word.
*/
typedef enum {
  GUI_WRAPMODE_NONE,   // No wrapping will be performed.
  GUI_WRAPMODE_WORD,   // Text is wrapped word wise.
  GUI_WRAPMODE_CHAR    // Text is wrapped char wise.
} GUI_WRAPMODE;

/*********************************************************************
*
*       Text related routines
*/
void  GUI_DispCEOL                  (void);
void  GUI_DispChar                  (U16 c);
void  GUI_DispCharAt                (U16 c, I16P x, I16P y);
void  GUI_DispChars                 (U16 c, int Cnt);
void  GUI_DispNextLine              (void);
void  GUI_DispString                (const char * s);
void  GUI_DispStringAt              (const char * s, int x, int y);
void  GUI_DispStringAtCEOL          (const char * s, int x, int y);
void  GUI_DispStringHCenterAt       (const char * s, int x, int y);
void  GUI__DispStringInRect         (const char * s, GUI_RECT * pRect, int TextAlign, int MaxNumChars);
void  GUI_DispStringInRect          (const char * s, GUI_RECT * pRect, int TextAlign);
#if GUI_SUPPORT_ROTATION
void  GUI_DispStringInRectEx        (const char * s, GUI_RECT * pRect, int TextAlign, int MaxLen, const GUI_ROTATION * pLCD_Api);
#endif
void  GUI_DispStringInRectMax       (const char * s, GUI_RECT * pRect, int TextAlign, int MaxLen); /* Not to be doc. */
void  GUI_DispStringInRectWrap      (const char * s, GUI_RECT * pRect, int TextAlign, GUI_WRAPMODE WrapMode); /* Not to be doc. */
#if GUI_SUPPORT_ROTATION
void  GUI_DispStringInRectWrapEx    (const char * s, GUI_RECT * pRect, int TextAlign, GUI_WRAPMODE WrapMode, const GUI_ROTATION * pLCD_Api);
#endif
void  GUI_DispStringLen             (const char * s, int Len);
U16   GUI_GetCharFromPos            (const char * pText, int x, int * pIndex);
const GUI_FONT * GUI_GetFallbackFont(U16 c);
void  GUI_GetTextExtend             (GUI_RECT* pRect, const char * s, int Len);
int   GUI_GetYAdjust                (void);
int   GUI_GetDispPosX               (void);
int   GUI_GetDispPosY               (void);
const GUI_FONT * GUI_GetFont        (void);
int   GUI_GetCharDistX              (U16 c);
int   GUI_GetCharDistXEx            (U16 c, int * pSizeX);
int   GUI_GetStringDistX            (const char * s);
int   GUI_GetStringDistXEx          (const char * s, int n);
GUI_DRAWMODE GUI_GetDrawMode        (void);
int   GUI_GetFontDistY              (void);
int   GUI_GetFontSizeY              (void);
void  GUI_GetFontInfo               (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
void  GUI_GetOrg                    (int * px, int * py);
int   GUI_GetYSizeOfFont            (const GUI_FONT * pFont);
int   GUI_GetYDistOfFont            (const GUI_FONT * pFont);
int   GUI_GetTextAlign              (void);
int   GUI_GetTextMode               (void);
char  GUI_IsInFont                  (const GUI_FONT * pFont, U16 c);
U8    GUI_SetClearTextRectMode      (unsigned OnOff);
const GUI_FONT * GUI_SetFallbackFont(const GUI_FONT * pNewFont);
void  GUI_SetFuncGetFallbackFont    (const GUI_FONT * (* pfcbGetFallbackFont)(U16 c));
int   GUI_SetTextAlign              (int Align);
int   GUI_SetTextMode               (int Mode);
char  GUI_SetTextStyle              (char Style);
int   GUI_SetLBorder                (int x);
U8    GUI_SetStrikeWidth            (U8 StrikeWidth);
const GUI_FONT * GUI_SetFont        (const GUI_FONT * pNewFont);
char  GUI_GotoXY                    (int x, int y);
char  GUI_GotoX                     (int x);
char  GUI_GotoY                     (int y);
void  GUI_ShowMissingCharacters     (int OnOff);
int   GUI_GetShowMissingCharacters  (void);
int   GUI_WrapGetNumLines           (const char * pText, int xSize, GUI_WRAPMODE WrapMode);
int   GUI_WrapGetPositions          (const char * pText, int xSize, GUI_WRAPMODE WrapMode, int * aPos, int NumItems);
void  GUI_WrapSetSeparators         (const U16 * pSep, int NumSeps);

int   GUI_GetLeadingBlankCols       (U16 c);
int   GUI_GetTrailingBlankCols      (U16 c);
int   GUI_GetLeadingBlankRows       (U16 c);
int   GUI_GetTrailingBlankRows      (U16 c);

/*********************************************************************
*
*       System independent fonts (SIF)
*/
void GUI_SIF_CreateFont(const void * pFontData, GUI_FONT * pFont, const GUI_SIF_TYPE * pFontType);
void GUI_SIF_DeleteFont(GUI_FONT * pFont);

/*********************************************************************
*
*       External binary fonts (XBF)
*/
int  GUI_XBF_CreateFont(GUI_FONT * pFont, GUI_XBF_DATA * pXBF, const GUI_XBF_TYPE * pFontType, GUI_XBF_GET_DATA_FUNC * pfGetData, void * pVoid);
void GUI_XBF_DeleteFont(GUI_FONT * pFont);

#define GUI_XBF_HEADER_SIZE    18
#define GUI_XBF_TABLEITEM_SIZE (sizeof(U32) + sizeof(U16))

/*********************************************************************
*
*       FreeType engine
*/
void GUI_FT_DestroyCache       (void);
void GUI_FT_Done               (void);
void GUI_FT_GetMeasurementChars(U16 * pLowerHeight, U16 * pUpperHeight);
void GUI_FT_SetCacheSize       (unsigned MaxFaces, unsigned MaxSizes, U32 MaxBytes);
void GUI_FT_SetMeasurementChars(U16 LowerHeight, U16 UpperHeight);

/*********************************************************************
*
*       FreeType compability macros
*/
#define GUI_TTF_DestroyCache()                             GUI_FT_DestroyCache()
#define GUI_TTF_Done()                                     GUI_FT_Done()
#define GUI_TTF_SetCacheSize(MaxFaces, MaxSizes, MaxBytes) GUI_FT_SetCacheSize(MaxFaces, MaxSizes, MaxBytes)

/*********************************************************************
*
*       TrueType support (TTF)
*/
void GUI_TTF_AddRowBottom  (GUI_FONT * pFont, int NumRows);
void GUI_TTF_AddRowTop     (GUI_FONT * pFont, int NumRows);
int  GUI_TTF_CreateFont    (GUI_FONT * pFont, GUI_TTF_CS * pCS);
int  GUI_TTF_CreateFontAA  (GUI_FONT * pFont, GUI_TTF_CS * pCS);
int  GUI_TTF_EmboldenFont  (GUI_FONT * pFont, U8 Embolden);
int  GUI_TTF_EmboldenFontEx(GUI_FONT * pFont, I32 Strength);
int  GUI_TTF_EnableKerning (GUI_FONT * pFont, int OnOff);
int  GUI_TTF_GetFamilyName (GUI_FONT * pFont, char * pBuffer, int NumBytes);
int  GUI_TTF_GetStyleName  (GUI_FONT * pFont, char * pBuffer, int NumBytes);
int  GUI_TTF_ObliqueFont   (GUI_FONT * pFont, U8 Oblique);
int  GUI_TTF_ObliqueFontEx (GUI_FONT * pFont, I32 Angle);

/*********************************************************************
*
*       Glyph Bitmap Distribution Format support (BDF)
*/
int GUI_BDF_CreateFont      (GUI_FONT * pFont, const U8 * pData, U32 Size);
int GUI_BDF_DeleteFont      (GUI_FONT * pFont);
int GUI_BDF_EmboldenFont    (GUI_FONT * pFont, U8 Embolden);
int GUI_BDF_EmboldenFontEx  (GUI_FONT * pFont, I32 Strength);
int GUI_BDF_GetFamilyName   (GUI_FONT * pFont, char * pBuffer, int NumBytes);
int GUI_BDF_GetStyleName    (GUI_FONT * pFont, char * pBuffer, int NumBytes);
int GUI_BDF_SetLetterSpacing(GUI_FONT * pFont, int Spacing);

/*********************************************************************
*
*       Kerning
*/
void               GUI_AttachKerning (const GUI_FONT * pFont, GUI_KERNING_INFO * pInfo, const U16 * pData, U32 NumItems);
int                GUI_DetachKerning (const GUI_FONT * pFont);
GUI_KERNING_INFO * GUI_GetKerning    (const GUI_FONT * pFont);                    // Not to be documented
I16                GUI_GetKerningDist(GUI_KERNING_INFO * pInfo, U16 cl, U16 cr);  // Not to be documented

/*********************************************************************
*
*       Resource file support
*/
/*********************************************************************
*
*       GUI_LANG_GET_DATA_FUNC
* 
*  Description
*    GetData function used for the language module, for more details see
*    \uref{GUI_GET_DATA_FUNC_II}.
*/
typedef GUI_GET_DATA_FUNC_II     GUI_LANG_GET_DATA_FUNC;

void         GUI_LANG_Clear            (void);
int          GUI_LANG_GetLang          (void);
int          GUI_LANG_GetNumItems      (int IndexLang);
const char * GUI_LANG_GetText          (int IndexText);
int          GUI_LANG_GetTextBuffered  (int IndexText, char * pBuffer, int SizeOfBuffer);
int          GUI_LANG_GetTextBufferedEx(int IndexText, int IndexLang, char * pBuffer, int SizeOfBuffer);
const char * GUI_LANG_GetTextEx        (int IndexText, int IndexLang);
int          GUI_LANG_GetTextLen       (int IndexText);
int          GUI_LANG_GetTextLenEx     (int IndexText, int IndexLang);
int          GUI_LANG_LoadCSV          (U8 * pFileData, U32 FileSize);
int          GUI_LANG_LoadCSVEx        (GUI_LANG_GET_DATA_FUNC * pfGetData, void * p);
int          GUI_LANG_LoadText         (U8 * pFileData, U32 FileSize, int IndexLang);
int          GUI_LANG_LoadTextEx       (GUI_LANG_GET_DATA_FUNC * pfGetData, void * p, int IndexLang);
int          GUI_LANG_SetLang          (int IndexLang);
unsigned     GUI_LANG_SetMaxNumLang    (unsigned MaxNumLang);
U16          GUI_LANG_SetSep           (U16 Sep);

/*********************************************************************
*
*       Unicode support
*/
int  GUI_UC_ConvertUC2UTF8(const U16 * s, int Len, char * pBuffer, int BufferSize);
int  GUI_UC_ConvertUTF82UC(const char * s, int Len, U16 * pBuffer, int BufferSize);
int  GUI_UC_EnableBIDI    (int OnOff);
int  GUI_UC_EnableThai    (int OnOff);
int  GUI_UC_Encode        (char * s, U16 Char);
int  GUI_UC_GetCharSize   (const char * s);
U16  GUI_UC_GetCharCode   (const char * s);
void GUI_UC_SetEncodeNone (void);
void GUI_UC_SetEncodeSJIS (void);
void GUI_UC_SetEncodeUTF8 (void);
void GUI_UC_SetBaseDir    (int Dir);  // Only available with new version of BIDI algorithm (GUI_USE_BIDI2 == 1)
int  GUI_UC_GetBaseDir    (void);     // Only available with new version of BIDI algorithm (GUI_USE_BIDI2 == 1)

void GUI_UC_DispString(const U16 * s);
void GUI_UC2DB (U16 Code, U8 * pOut);
U16  GUI_DB2UC (U8 Byte0, U8 Byte1);

/*********************************************************************
*
*       Bidi support
*/
#define GUI_BIDI_BASEDIR_LTR  0
#define GUI_BIDI_BASEDIR_RTL  1
#define GUI_BIDI_BASEDIR_AUTO 2

#define GUI_BIDI_LOG2VIS_CALC     0
#define GUI_BIDI_LOG2VIS_GETCACHE 1

/*********************************************************************
*
*       Drawing of binary, decimal and hexadecimal values
*/
void GUI_DispBin      (U32  v, U8 Len);
void GUI_DispBinAt    (U32  v, I16P x, I16P y, U8 Len);
void GUI_DispDec      (I32 v, U8 Len);
void GUI_DispDecAt    (I32 v, I16P x, I16P y, U8 Len);
void GUI_DispDecMin   (I32 v);
void GUI_DispDecShift (I32 v, U8 Len, U8 Shift);
void GUI_DispDecSpace (I32 v, U8 MaxDigits);
void GUI_DispHex      (U32 v, U8 Len);
void GUI_DispHexAt    (U32 v, I16P x, I16P y, U8 Len);
void GUI_DispSDec     (I32 v, U8 Len);
void GUI_DispSDecShift(I32 v, U8 Len, U8 Shift);

/*********************************************************************
*
*       Drawing of floating point values
*/
void GUI_DispFloat    (float v, char Len);
void GUI_DispFloatFix (float v, char Len, char Fract);
void GUI_DispFloatMin (float v, char Fract);
void GUI_DispSFloatFix(float v, char Len, char Fract);
void GUI_DispSFloatMin(float v, char Fract);

/*********************************************************************
*
*       Dynamic memory management
*/
typedef struct {
  U32 TotalBytes;
  U32 FreeBytes;
  U32 UsedBytes;
  U32 AllocSize;
  U32 NumFixedBytes;
  U32 MaxUsedBytes;
} GUI_ALLOC_INFO;

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBlocks(void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBytes (void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumUsedBlocks(void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumUsedBytes (void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxUsedBytes (void);

void * GUI_ALLOC_calloc (size_t NumItems, size_t Size);
void   GUI_ALLOC_free   (void * p);
void * GUI_ALLOC_malloc (size_t Size);
void * GUI_ALLOC_realloc(void * p, size_t NewSize);

void GUI_ALLOC_GetMemInfo  (GUI_ALLOC_INFO * pInfo);
void GUI_ALLOC_SuppressPeak(int OnOff);

GUI_HMEM           GUI_ALLOC_AllocInit       (const void * pInitData, GUI_ALLOC_DATATYPE Size);
GUI_HMEM           GUI_ALLOC_AllocNoInit     (GUI_ALLOC_DATATYPE size);
GUI_HMEM           GUI_ALLOC_AllocZero       (GUI_ALLOC_DATATYPE size);
void               GUI_ALLOC_AssignMemory    (void * p, U32 NumBytes);
void               GUI_ALLOC_Free            (GUI_HMEM  hMem);
void               GUI_ALLOC_FreeFixedBlock  (void * p);
void               GUI_ALLOC_FreePtrArray    (GUI_HMEM * pArray, int NumElems);
void               GUI_ALLOC_FreePtr         (GUI_HMEM * phMem);
void *             GUI_ALLOC_GetFixedBlock   (GUI_ALLOC_DATATYPE Size);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxSize      (void);
GUI_ALLOC_DATATYPE GUI_ALLOC_GetSize         (GUI_HMEM  hMem);
void *             GUI_ALLOC_h2p             (GUI_HMEM  hMem);
GUI_HMEM           GUI_ALLOC_p2h             (void * p);
void               GUI_ALLOC_Lock            (void);
void *             GUI_ALLOC_LockH           (GUI_HMEM  hMem);
GUI_HMEM           GUI_ALLOC_Realloc         (GUI_HMEM hOld, int NewSize);
GUI_ALLOC_DATATYPE GUI_ALLOC_RequestSize     (void);
void               GUI_ALLOC_SetAvBlockSize  (U32 BlockSize);
void               GUI_ALLOC_SetShortOfRAM   (void (* pfOnShortOfRAM)(void));
void               GUI_ALLOC_Unlock          (void);
void *             GUI_ALLOC_UnlockH         (void ** pp);
//const void *     GUI_ALLOC_UnlockC         (const void ** pp);
void               GUI_ALLOC_UnlockC         (const void ** pp);
int                GUI_ALLOC_SetMaxPercentage(int MaxPercentage);
void               GUI_ALLOC_Exit            (void);

/*********************************************************************
*
*       Memory device flags
*
*  Description
*    Flags to be used for the creation of a memory device.
*/
#define GUI_MEMDEV_HASTRANS       0    // Default: The Memory Device is created with a transparency flag which ensures that the background will be drawn correctly.
#define GUI_MEMDEV_NOTRANS  (1 << 0)   // Creates a Memory Device without transparency. The user must make sure that the background is drawn correctly.
                                       // This way the Memory Device can be used for non-rectangular areas. An other advantage is the higher speed: Using this
                                       // flag accelerates the Memory Device approx. by 30 - 50%.
/* (ignore - emDoc should stop here) */

typedef GUI_HMEM GUI_MEMDEV_Handle;
typedef void     GUI_CALLBACK_VOID_P        (void * p);
typedef int      GUI_ANIMATION_CALLBACK_FUNC(int TimeRem, void * pVoid);
typedef void     GUI_DRAWMEMDEV_16BPP_FUNC  (void * pDst, const void * pSrc, int xSize, int ySize, int BytesPerLineDst, int BytesPerLineSrc);

#define GUI_DRAWMEMDEV_FUNC GUI_DRAWMEMDEV_16BPP_FUNC

typedef void GUI_DRAWBITMAP_FUNC (int LayerIndex, int x, int y, const void * p, int xSize, int ySize, int BytesPerLine);

int GUI_SetFuncDrawAlpha(GUI_DRAWMEMDEV_FUNC * pfDrawMemdevFunc, GUI_DRAWBITMAP_FUNC * pfDrawBitmapFunc);
int GUI_SetFuncDrawM565 (GUI_DRAWMEMDEV_FUNC * pfDrawMemdevFunc, GUI_DRAWBITMAP_FUNC * pfDrawBitmapFunc);
int GUI_SetFuncDrawA8   (GUI_DRAWMEMDEV_FUNC * pfDrawMemdevFunc, GUI_DRAWBITMAP_FUNC * pfDrawBitmapFunc);

extern GUI_ANIMATION_CALLBACK_FUNC * GUI_MEMDEV__pCbAnimation;
extern void                        * GUI_MEMDEV__pVoid;

extern void (* GUI_MEMDEV__pfMEMDEV_Write)    (GUI_MEMDEV_Handle hMem);
extern void (* GUI_MEMDEV__pfMEMDEV_CopyToLCD)(GUI_MEMDEV_Handle hMem);

typedef struct {
  GUI_RECT rView, rPrev;
  char FirstCall;
} GUI_AUTODEV;

typedef struct {
  char DrawFixed;
  char IsMeasurement;
} GUI_AUTODEV_INFO;

int  GUI_MEMDEV_CreateAuto(GUI_AUTODEV * pAutoDev);
void GUI_MEMDEV_DeleteAuto(GUI_AUTODEV * pAutoDev);
int  GUI_MEMDEV_DrawAuto  (GUI_AUTODEV * pAutoDev, GUI_AUTODEV_INFO * pAutoDevInfo, GUI_CALLBACK_VOID_P * pfDraw, void * pData);

/* Create a memory device which is compatible to the selected LCD */
GUI_MEMDEV_Handle GUI_MEMDEV_Create       (int x0, int y0, int xSize, int ySize);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateCopy   (GUI_MEMDEV_Handle hMemSrc);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateEx     (int x0, int y0, int xSize, int ySize, int Flags);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateFixed  (int x0, int y0, int xSize, int ySize, int Flags,
                                           const GUI_DEVICE_API     * pDeviceAPI,
                                           const LCD_API_COLOR_CONV * pColorConvAPI);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateFixed32(int x0, int y0, int xSize, int ySize);

void                       GUI_MEMDEV_Clear               (GUI_MEMDEV_Handle hMem);
int                        GUI_MEMDEV_ClearAlpha          (GUI_MEMDEV_Handle hMemData, GUI_MEMDEV_Handle hMemMask);
void                       GUI_MEMDEV_CopyFromLCD         (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_CopyFromLCDAA       (GUI_MEMDEV_Handle hMem);
int                        GUI_MEMDEV_CopyRect            (GUI_MEMDEV_Handle hMemSrc, GUI_MEMDEV_Handle hMemDst, const GUI_RECT * pRectSrc, int xDst, int yDst);
void                       GUI_MEMDEV_CopyToLCD           (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_CopyToLCDAA         (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_CopyToLCDAt         (GUI_MEMDEV_Handle hMem, int x, int y);
int                        GUI_MEMDEV_CompareWithLCD      (GUI_MEMDEV_Handle hMem, int * px, int * py, int * pExp, int * pAct);
void                       GUI_MEMDEV_Delete              (GUI_MEMDEV_Handle MemDev);
void                       GUI_MEMDEV_DrawBitmap32HQHR    (const GUI_BITMAP * pBm, int x0HR, int y0HR);
void                       GUI_MEMDEV_DrawPerspectiveX    (GUI_MEMDEV_Handle hMem, int x, int y, int h0, int h1, int dx, int dy);
void                       GUI_MEMDEV_DrawDevice32HQHR    (GUI_MEMDEV_Handle hMemSrc, I32 x0HR, int y0HR);
void                       GUI_MEMDEV_Fill32              (GUI_MEMDEV_Handle hMem, U32 Value);
int                        GUI_MEMDEV_GetXPos             (GUI_MEMDEV_Handle hMem);
int                        GUI_MEMDEV_GetXSize            (GUI_MEMDEV_Handle hMem);
int                        GUI_MEMDEV_GetYPos             (GUI_MEMDEV_Handle hMem);
int                        GUI_MEMDEV_GetYSize            (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_MarkDirty           (GUI_MEMDEV_Handle hMem, int x0, int y0, int x1, int y1);
void                       GUI_MEMDEV_ReduceYSize         (GUI_MEMDEV_Handle hMem, int YSize);
void                       GUI_MEMDEV_Rotate              (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
void                       GUI_MEMDEV_RotateAlpha         (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, U8 Alpha);
void                       GUI_MEMDEV_RotateHR            (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, I32 dx, I32 dy, int a, int Mag);
void                       GUI_MEMDEV_RotateHQ            (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
void                       GUI_MEMDEV_RotateHQAlpha       (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, U8 Alpha);
void                       GUI_MEMDEV_RotateHQHR          (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, I32 dx, I32 dy, int a, int Mag);
void                       GUI_MEMDEV_RotateHQT           (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
void                       GUI_MEMDEV_RotateHQTI          (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
GUI_MEMDEV_Handle          GUI_MEMDEV_Select              (GUI_MEMDEV_Handle hMem);  /* Select (activate) a particular memory device. */
void                       GUI_MEMDEV_SetOrg              (GUI_MEMDEV_Handle hMem, int x0, int y0);
void                       GUI_MEMDEV_WriteAt             (GUI_MEMDEV_Handle hMem, int x, int y);
void                       GUI_MEMDEV_Write               (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_WriteAlphaAt        (GUI_MEMDEV_Handle hMem, int Alpha, int x, int y);
void                       GUI_MEMDEV_WriteAlpha          (GUI_MEMDEV_Handle hMem, int Alpha);
void                       GUI_MEMDEV_WriteExAt           (GUI_MEMDEV_Handle hMem, int x, int y, int xMag, int yMag, int Alpha);
void                       GUI_MEMDEV_WriteEx             (GUI_MEMDEV_Handle hMem, int xMag, int yMag, int Alpha);
void                       GUI_MEMDEV_WriteOpaque         (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_WriteOpaqueAt       (GUI_MEMDEV_Handle hMem, int x, int y);
int                        GUI_MEMDEV_Draw                (GUI_RECT * pRect, GUI_CALLBACK_VOID_P * pfDraw, void * pData, int NumLines, int Flags);
void *                     GUI_MEMDEV_GetDataPtr          (GUI_MEMDEV_Handle hMem);
void                       GUI_MEMDEV_SetColorConv        (GUI_MEMDEV_Handle hMem, const LCD_API_COLOR_CONV * pColorConvAPI);
const LCD_API_COLOR_CONV * GUI_MEMDEV_GetColorConv        (GUI_MEMDEV_Handle hMemDev);
int                        GUI_MEMDEV_GetBitsPerPixel     (GUI_MEMDEV_Handle hMemDev);
GUI_MEMDEV_Handle          GUI_MEMDEV_GetSelMemdev        (void);
int                        GUI_MEMDEV_FadeInDevices       (GUI_MEMDEV_Handle hMem0, GUI_MEMDEV_Handle hMem1, int Period);
int                        GUI_MEMDEV_FadeOutDevices      (GUI_MEMDEV_Handle hMem0, GUI_MEMDEV_Handle hMem1, int Period);
void                       GUI_MEMDEV_SerializeBMP        (GUI_MEMDEV_Handle hDev, GUI_CALLBACK_VOID_U8_P * pfSerialize, void * p);
void                       GUI_MEMDEV_SerializeExBMP      (GUI_MEMDEV_Handle hDev, GUI_CALLBACK_VOID_U8_P * pfSerialize, void * p, int xPos, int yPos, int xSize, int ySize);
void                       GUI_MEMDEV_SetAnimationCallback(GUI_ANIMATION_CALLBACK_FUNC * pCbAnimation, void * pVoid);
int                        GUI_MEMDEV_PunchOutDevice      (GUI_MEMDEV_Handle hMemData, GUI_MEMDEV_Handle hMemMask);
void                       GUI_MEMDEV_SetTimePerFrame     (unsigned TimePerFrame);
int                        GUI_MEMDEV_MULTIBUF_Enable     (int OnOff);

/* Private functions */
void                       GUI_MEMDEV__ClearCanvas        (void);
void                       GUI_MEMDEV__ClearCanvasEx      (U8 SetActive);
void                       GUI_MEMDEV__FadeDevice         (GUI_MEMDEV_Handle hMemWin, GUI_MEMDEV_Handle hMemBk, GUI_MEMDEV_Handle hMemDst, U8 Intens);
void                       GUI_MEMDEV__FadeDeviceEx       (GUI_MEMDEV_Handle hMemWin, GUI_MEMDEV_Handle hMemBk, GUI_MEMDEV_Handle hMemDst, U8 Intens, int xPosWin, int yPosWin);
void                       GUI_MEMDEV__Rotate             (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag, U32 Mask);
void                       GUI_MEMDEV__RotateHR           (GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, I32 dx, I32 dy, int a, int Mag, U32 Mask);
U8                         GUI_MEMDEV__SetCanvas          (GUI_MEMDEV_Handle hMem);

void  GUI_SelectLCD(void);

/* Blurring, dithering and blending */
GUI_MEMDEV_Handle GUI_MEMDEV_CreateBlurredDevice32  (GUI_MEMDEV_Handle hMem, U8 Depth);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateBlurredDevice32HQ(GUI_MEMDEV_Handle hMem, U8 Depth);
GUI_MEMDEV_Handle GUI_MEMDEV_CreateBlurredDevice32LQ(GUI_MEMDEV_Handle hMem, U8 Depth);
void              GUI_MEMDEV_SetBlurHQ              (void);
void              GUI_MEMDEV_SetBlurLQ              (void);
int               GUI_MEMDEV_BlendColor32           (GUI_MEMDEV_Handle hMem, U32 BlendColor, U8 BlendIntens);
int               GUI_MEMDEV_Dither32               (GUI_MEMDEV_Handle hMem, const LCD_API_COLOR_CONV * pColorConvAPI);

/* Optional custom drawing of 16bpp memory devices */
void GUI_MEMDEV_SetDrawMemdev16bppFunc(GUI_DRAWMEMDEV_16BPP_FUNC * pfDrawMemdev16bppFunc);

/* Custom rotate function */
void GUI_MEMDEV_SetRotateFuncLR(int (* pfRotate)(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag));
void GUI_MEMDEV_SetRotateFuncHR(int (* pfRotate)(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag));

void GUI_MEMDEV_SetBlendFunc(int (* pfBlend)(GUI_MEMDEV_Handle hMem, GUI_COLOR Color, U8 BlendIntens));


/*********************************************************************
*
*       GUI_DTA_GET_DATA_FUNC
* 
*  Description
*    GetData function used for streamed bitmaps (DTA), for more details see
*    \uref{GUI_GET_DATA_FUNC_II}.
*/
typedef GUI_GET_DATA_FUNC_II     GUI_DTA_GET_DATA_FUNC;


/*********************************************************************
*
*       Memory bitmaps
*/
typedef struct {
  U8     Cmd;
  int    xSize;
  int    ySize;
  void * pExtra;
} GUI_MBITMAP_PARAM;

typedef struct {
  GUI_BITMAP Bitmap;
  void *     pExtra;
  void *     pData;
} GUI_MBITMAP;

typedef int               GUI_MBITMAP_DRAW_IMAGE   (const void * p, int Size, GUI_MBITMAP_PARAM * pPara);
typedef int               GUI_MBITMAP_DRAW_IMAGE_EX(GUI_GET_DATA_FUNC * pfGetData, void * p, GUI_MBITMAP_PARAM * pPara);
typedef GUI_MEMDEV_Handle GUI_MBITMAP_CREATE_MEMDEV(int xSize, int ySize, void * pExtra);
typedef void              GUI_MBITMAP_DELETE_MEMDEV(GUI_MEMDEV_Handle hMem, void * pExtra);

typedef struct {
  GUI_MBITMAP_CREATE_MEMDEV * pfCreateMemdev;
  GUI_MBITMAP_DELETE_MEMDEV * pfDeleteMemdev;
  void *                      pExtra;
} GUI_MBITMAP_CONFIG;

//
// GUI_MBITMAP prototypes
//
GUI_MBITMAP * GUI_MBITMAP_Create        (GUI_MBITMAP_DRAW_IMAGE * pFuncDraw, const void * pData, int SizeOfData);
GUI_MBITMAP * GUI_MBITMAP_CreateEx      (GUI_MBITMAP_DRAW_IMAGE_EX * pFuncDrawEx, GUI_GET_DATA_FUNC * pfGetData, void * p);
GUI_MBITMAP * GUI_MBITMAP_CreateUser    (GUI_MBITMAP_DRAW_IMAGE * pFuncDraw, const void * pData, int SizeOfData, int xSize, int ySize, GUI_MBITMAP_CONFIG * pConfig);
GUI_MBITMAP * GUI_MBITMAP_CreateUserEx  (GUI_MBITMAP_DRAW_IMAGE_EX * pFuncDrawEx, GUI_GET_DATA_FUNC * pfGetData, void * p, int xSize, int ySize, GUI_MBITMAP_CONFIG * pConfig);
void          GUI_MBITMAP_Delete        (GUI_MBITMAP * pMBitmap);
void          GUI_MBITMAP_SetColorFormat(const GUI_DEVICE_API * pDeviceAPI, const LCD_API_COLOR_CONV * pColorConvAPI);

//
// Drawing functions
//
#define GUI_MBITMAP_DRAW_FUNC(TYPE)     int GUI_MBITMAP_Draw##TYPE(const void * p, int Size, GUI_MBITMAP_PARAM * pPara);
#define GUI_MBITMAP_DRAW_FUNC_EX(TYPE)  int GUI_MBITMAP_Draw##TYPE##Ex(GUI_##TYPE##_GET_DATA_FUNC * pfGetData, void * p, GUI_MBITMAP_PARAM * pPara);

//
// Create prototypes
//
GUI_MBITMAP_DRAW_FUNC(Bitmap)
GUI_MBITMAP_DRAW_FUNC(BMP)
GUI_MBITMAP_DRAW_FUNC(DTA)
GUI_MBITMAP_DRAW_FUNC(GIF)
GUI_MBITMAP_DRAW_FUNC(JPEG)
GUI_MBITMAP_DRAW_FUNC(PNG)
GUI_MBITMAP_DRAW_FUNC(SVG)
GUI_MBITMAP_DRAW_FUNC_EX(BMP)
GUI_MBITMAP_DRAW_FUNC_EX(DTA)
GUI_MBITMAP_DRAW_FUNC_EX(GIF)
GUI_MBITMAP_DRAW_FUNC_EX(JPEG)
GUI_MBITMAP_DRAW_FUNC_EX(PNG)
GUI_MBITMAP_DRAW_FUNC_EX(SVG)

//
//  Allow drawing types are:
//   "Bitmap"
//   "BMP"  - "BMPEx"
//   "DTA"  - "DTAEx"
//   "GIF"  - "GIFEx"
//   "JPEG" - "JPEGEx"
//   "PNG"  - "PNGEx"
//   "SVG"  - "SVGEx"
//
#define GUI_MBITMAP_DRAW(TYPE)   GUI_MBITMAP_Draw##TYPE

#define GUI_MBITMAP_BITMAP  GUI_MBITMAP_DRAW(Bitmap)
#define GUI_MBITMAP_BMP     GUI_MBITMAP_DRAW(BMP)
#define GUI_MBITMAP_DTA     GUI_MBITMAP_DRAW(DTA)
#define GUI_MBITMAP_GIF     GUI_MBITMAP_DRAW(GIF)
#define GUI_MBITMAP_JPEG    GUI_MBITMAP_DRAW(JPEG)
#define GUI_MBITMAP_PNG     GUI_MBITMAP_DRAW(PNG)
#define GUI_MBITMAP_SVG     GUI_MBITMAP_DRAW(SVG)
#define GUI_MBITMAP_BMP_EX  GUI_MBITMAP_DRAW(BMPEx)
#define GUI_MBITMAP_DTA_EX  GUI_MBITMAP_DRAW(DTAEx)
#define GUI_MBITMAP_GIF_EX  GUI_MBITMAP_DRAW(GIFEx)
#define GUI_MBITMAP_JPEG_EX GUI_MBITMAP_DRAW(JPEGEx)
#define GUI_MBITMAP_PNG_EX  GUI_MBITMAP_DRAW(PNGEx)
#define GUI_MBITMAP_SVG_EX  GUI_MBITMAP_DRAW(SVGEx)

//
// Conversion macro from GUI_MBITMAP to GUI_BITMAP
//
#define GUI_MBITMAP_2BITMAP(x)      ((GUI_BITMAP *)x)

//
// Commands used by drawing functions
//
#define GUI_MBITMAP_CMD_XSIZE  (0)
#define GUI_MBITMAP_CMD_YSIZE  (1)
#define GUI_MBITMAP_CMD_DRAW   (2)

/*********************************************************************
*
*       Emscripten related 
*/

/*********************************************************************
*
*       GUI_EMSCRIPTEN_DATE
*/
typedef struct {
  int Year;    // Year (e.g. 2024)
  int Month;   // Month (January == 1, February == 2, ...)
  int Day;     // Day of the month (1st day of month == 1)
  int Weekday; // Day of the week (Monday == 1, Tuesday == 2, ...)
  int Hour;    // Hour (valid range: [0-23]).
  int Minute;  // Minute (valid range: [0-59]).
  int Second;  // Second (valid range: [0-59]).
} GUI_EMSCRIPTEN_DATE;

void GUI_EMSCRIPTEN_GetNow        (GUI_EMSCRIPTEN_DATE * pDate);
void GUI_EMSCRIPTEN_GetNowTimezone(GUI_EMSCRIPTEN_DATE * pDate, const char * sTimezone);

/*********************************************************************
*
*       Alpha blending
*/
/*********************************************************************
*
*       GUI_ALPHA_STATE
*
*   Description
*     Used for storing the alpha value with GUI_SetUserAlpha().
*/
typedef struct {
  U32 UserAlpha;   // Alpha value to be used.
} GUI_ALPHA_STATE;

#define GUI_MAKE_ALPHA(Alpha, Color)  ((U32)(((U32)Alpha << 24) | (Color & 0xFFFFFF)))

unsigned     GUI_EnableAlpha          (unsigned OnOff);
U32          GUI_RestoreUserAlpha     (GUI_ALPHA_STATE * pAlphaState);
U8           GUI_GetAlphaEx           (int LayerIndex);
U8           GUI_GetAlpha             (void);
unsigned     GUI_SetAlpha             (U8 Alpha);
U32          GUI_SetUserAlpha         (GUI_ALPHA_STATE * pAlphaState, U32 UserAlpha);
void      (* GUI_SetFuncAlphaBlending (void (* pfAlphaBlending)(LCD_COLOR *, LCD_COLOR *, LCD_COLOR *, U32)))
                                                               (LCD_COLOR *, LCD_COLOR *, LCD_COLOR *, U32);
LCD_COLOR (* GUI_SetFuncMixColors     (LCD_COLOR (* pFunc)(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens)))
                                                          (LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
void      (* GUI_SetFuncMixColorsBulk (void (* pFunc)(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens)))
                                                     (U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);
unsigned     GUI_PreserveTrans        (unsigned OnOff);
void         GUI_AlphaEnableFillRectHW(int OnOff);


/*********************************************************************
*
*       Multi layer support
*/
unsigned GUI_SelectLayer(unsigned Index);
unsigned GUI_GetSelLayer(void);

int  GUI_SetLayerPosEx  (unsigned Index, int xPos, int yPos);
int  GUI_SetLayerSizeEx (unsigned Index, int xSize, int ySize);
int  GUI_SetLayerVisEx  (unsigned Index, int OnOff);
int  GUI_SetLayerAlphaEx(unsigned Index, int Alpha);
int  GUI_GetLayerPosEx  (unsigned Index, int * pxPos, int * pyPos);

void     GUI_AssignCursorLayer(unsigned Index, unsigned CursorLayer);
unsigned GUI_GetCursorLayer   (unsigned Index);

/*********************************************************************
*
*       Multiple buffers and display origin
*/
void GUI_SetOrg(int x, int y);

void GUI_MULTIBUF_Begin          (void);
void GUI_MULTIBUF_BeginEx        (int LayerIndex);
void GUI_MULTIBUF_End            (void);
void GUI_MULTIBUF_EndEx          (int LayerIndex);
void GUI_MULTIBUF_Config         (int NumBuffers);
void GUI_MULTIBUF_ConfigEx       (int LayerIndex, int NumBuffers);
void GUI_MULTIBUF_Confirm        (int Index);
void GUI_MULTIBUF_ConfirmEx      (int LayerIndex, int BufferIndex);
int  GUI_MULTIBUF_GetNumBuffers  (void);
int  GUI_MULTIBUF_GetNumBuffersEx(int LayerIndex);
void GUI_MULTIBUF_SetLayerMask   (U32 LayerMask);
void GUI_MULTIBUF_UseSingleBuffer(void);

/*********************************************************************
*
*       emWinSPY
*/
int  GUI_SPY_Process       (GUI_tSend pfSend, GUI_tRecv pfRecv, void * pConnectInfo);
void GUI_SPY_SetProcessFunc(int (* pProcess)(U8, void *));
void GUI_SPY_SetMemHandler (GUI_tMalloc pMalloc, GUI_tFree pFree);
int  GUI_SPY_StartServer   (void);
int  GUI_SPY_StartServerEx (int (* pGUI_SPY_X_StartServer)(void));
int  GUI_SPY_X_StartServer (void);

/*********************************************************************
*
*       Barcode types
*
*  Description
*    Type of barcode to be drawn.
*/
#define GUI_BARCODE_ITF 0   // \b{Interleaved 2 of 5 (ITF)} which can display an even amount of digits.
#define GUI_BARCODE_128 1   // \b{Code128} which can display all 128 ASCII characters.

int GUI_BARCODE_Draw    (int xPos, int yPos, int ModuleSize, int ySize, int Type, const char * sBarcode);
int GUI_BARCODE_GetXSize(int Type, int ModuleSize, const char * sBarcode);

/*********************************************************************
*
*       ECC levels for QR codes
*
*  Description
*    Error correction level to be used by the Reed-Solomon error correction
*    for a QR code.
*/
#define GUI_QR_ECLEVEL_L 0   // About 7% or less errors can be corrected.
#define GUI_QR_ECLEVEL_M 1   // About 15% or less errors can be corrected.
#define GUI_QR_ECLEVEL_Q 2   // About 25% or less errors can be corrected.
#define GUI_QR_ECLEVEL_H 3   // About 30% or less errors can be corrected.

/*********************************************************************
*
*       GUI_QR_INFO
*
*   Description
*     Information about a QR code.
*/
typedef struct {
  int Version;   // Version according to QR code documentation.
  int Width;     // Number of 'Modules'.
  int Size;      // Size of bitmap in pixels.
} GUI_QR_INFO;

GUI_HMEM GUI_QR_Create      (const char * pText, int PixelSize, int EccLevel, int Version);
GUI_HMEM GUI_QR_CreateFramed(const char * pText, int PixelSize, int EccLevel, int Version);
void     GUI_QR_Draw        (GUI_HMEM hQR, int xPos, int yPos);
void     GUI_QR_GetInfo     (GUI_HMEM hQR, GUI_QR_INFO * pInfo);
void     GUI_QR_Delete      (GUI_HMEM hQR);

/*********************************************************************
*
*       Basics for animations
*/
#define ANIM_LINEAR     GUI_ANIM__Linear
#define ANIM_ACCEL      GUI_ANIM__Accel
#define ANIM_DECEL      GUI_ANIM__Decel
#define ANIM_ACCELDECEL GUI_ANIM__AccelDecel

/*********************************************************************
*
*       Animation states
*
*   Description
*     Describes the current state of an animation. Sent with the
*     \a{State} member of the GUI_ANIM_INFO structure to an animation callback.
*/
#define GUI_ANIM_START   0   // First execution.
#define GUI_ANIM_RUNNING 1   // Passed to all items which are not the first and not the last.
#define GUI_ANIM_END     2   // Last execution.

/*********************************************************************
*
*       GUI_ANIM_RANGE
*/
#ifndef   GUI_ANIM_RANGE
  #define GUI_ANIM_RANGE 32767
#endif

typedef GUI_HMEM GUI_ANIM_HANDLE;

typedef I32 (* GUI_ANIM_GETPOS_FUNC)(GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);

/*********************************************************************
*
*       GUI_ANIM_INFO
*
*   Description
*     Contains information about the current state of an animation.
*/
typedef struct {
  int Pos;                // Position value calculated by the selected position calculation routine.
  int State;              // State of the animation. See \ref{Animation states} for valid values.
  GUI_ANIM_HANDLE hAnim;  // Handle of the animation object.
  GUI_TIMER_TIME Period;  // Period of the animation object.
  unsigned Index;         // Item index
} GUI_ANIM_INFO;

typedef void GUI_ANIMATION_FUNC(GUI_ANIM_INFO * pInfo, void * pVoid);

I32 GUI_ANIM__Linear    (GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);
I32 GUI_ANIM__Decel     (GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);
I32 GUI_ANIM__Accel     (GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);
I32 GUI_ANIM__AccelDecel(GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow);

int             GUI_ANIM_AddItem          (GUI_ANIM_HANDLE hAnim, GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_ANIM_GETPOS_FUNC pfGetPos, void * pVoid, GUI_ANIMATION_FUNC * pfAnim);
int             GUI_ANIM_AddItemById      (I16 Id,                GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_ANIM_GETPOS_FUNC pfGetPos, void * pVoid, GUI_ANIMATION_FUNC * pfAnim);
GUI_ANIM_HANDLE GUI_ANIM_Create           (GUI_TIMER_TIME Period, unsigned MinTimePerFrame, void * pVoid, void (* pfSlice)(int, void *));
GUI_ANIM_HANDLE GUI_ANIM_CreateWithId     (GUI_TIMER_TIME Period, unsigned MinTimePerSlice, void * pVoid, void (* pfSlice)(int, void *), I16 Id);
void            GUI_ANIM_Delete           (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_DeleteById       (I16 Id);
void            GUI_ANIM_DeleteAll        (void);
GUI_ANIM_HANDLE GUI_ANIM_Get              (I16 Id);
void          * GUI_ANIM_GetData          (GUI_ANIM_HANDLE hAnim);
void          * GUI_ANIM_GetItemDataLocked(GUI_ANIM_HANDLE hAnim, unsigned Index);
GUI_ANIM_HANDLE GUI_ANIM_GetFirst         (void);
void          * GUI_ANIM_GetItemData      (GUI_ANIM_HANDLE hAnim, unsigned Index);
GUI_ANIM_HANDLE GUI_ANIM_GetNext          (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_GetNumItems      (GUI_ANIM_HANDLE hAnim);
GUI_TIMER_TIME  GUI_ANIM_GetPeriod        (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_Exec             (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_IsRunning        (GUI_ANIM_HANDLE hAnim);
void            GUI_ANIM_SetData          (GUI_ANIM_HANDLE hAnim, void * pVoid);
int             GUI_ANIM_SetItemData      (GUI_ANIM_HANDLE hAnim, unsigned Index, const void * pVoid, U32 NumBytes);
void            GUI_ANIM_Start            (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_StartExId        (I16 Id,                int NumLoops, void (* pfOnDelete)(void * pVoid));
void            GUI_ANIM_StartEx          (GUI_ANIM_HANDLE hAnim, int NumLoops, void (* pfOnDelete)(void * pVoid));
void            GUI_ANIM_Stop             (GUI_ANIM_HANDLE hAnim);
int             GUI_ANIM_StopById         (I16 Id);

/*********************************************************************
*
*       YUV device
*/
int   GUI_YUV_Create        (void);
int   GUI_YUV_CreateEx      (int LayerIndex, unsigned Period);
int   GUI_YUV_Delete        (void);
int   GUI_YUV_DeleteEx      (int LayerIndex);
U32 * GUI_YUV_GetpData      (U32 * pSize);
U32 * GUI_YUV_GetpDataEx    (int LayerIndex, U32 * pSize);
void  GUI_YUV_InvalidateArea(int x, int y, int xSize, int ySize);
int   GUI_YUV_SetPeriodEx   (int LayerIndex, unsigned Period);
int   GUI_YUV_SetPeriod     (unsigned Period);

/*********************************************************************
*
*       Display orientation
*/
/*********************************************************************
*
*       GUI_ORIENTATION_API
*/
typedef struct {
  void           (* pfDrawBitmap   )(GUI_DEVICE * pDevice, int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine, const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void           (* pfDrawHLine    )(GUI_DEVICE * pDevice, int x0, int y0,  int x1);
  void           (* pfDrawVLine    )(GUI_DEVICE * pDevice, int x , int y0,  int y1);
  void           (* pfFillRect     )(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1);
  LCD_PIXELINDEX (* pfGetPixelIndex)(GUI_DEVICE * pDevice, int x, int y);
  void           (* pfSetPixelIndex)(GUI_DEVICE * pDevice, int x, int y, LCD_PIXELINDEX ColorIndex);
  void           (* pfXorPixel     )(GUI_DEVICE * pDevice, int x, int y);
  int            BytesPerPixel;
} GUI_ORIENTATION_API;

extern const GUI_ORIENTATION_API GUI_OrientationAPI_C0;
extern const GUI_ORIENTATION_API GUI_OrientationAPI_C8;
extern const GUI_ORIENTATION_API GUI_OrientationAPI_C16;
extern const GUI_ORIENTATION_API GUI_OrientationAPI_C32;

#define GUI_ORIENTATION_C0  &GUI_OrientationAPI_C0
#define GUI_ORIENTATION_C8  &GUI_OrientationAPI_C8
#define GUI_ORIENTATION_C16 &GUI_OrientationAPI_C16
#define GUI_ORIENTATION_C32 &GUI_OrientationAPI_C32

int  GUI_SetOrientation        (int Orientation);
int  GUI_SetOrientationEx      (int Orientation, int LayerIndex);
int  GUI_SetOrientationExCached(int Orientation, int LayerIndex, const GUI_ORIENTATION_API * pAPI);
void GUI_OrientationEnableTouch(void);

/*********************************************************************
*
*       Measure device: GUI_MEASDEV
*/
typedef GUI_HMEM GUI_MEASDEV_Handle;

GUI_MEASDEV_Handle GUI_MEASDEV_Create (void);
void               GUI_MEASDEV_Delete (GUI_MEASDEV_Handle hMemDev);
void               GUI_MEASDEV_Select (GUI_MEASDEV_Handle hMem);
void               GUI_MEASDEV_GetRect(GUI_MEASDEV_Handle hMem, GUI_RECT * pRect);
void               GUI_MEASDEV_ClearRect(GUI_MEASDEV_Handle hMem);

/*********************************************************************
*
*       Polygon helpers
*/
void GUI_RotatePolygon (GUI_POINT * pDest, const GUI_POINT * pSrc, int NumPoints, float Angle);
void GUI_MagnifyPolygon(GUI_POINT * pDest, const GUI_POINT * pSrc, int NumPoints, int Mag);
void GUI_EnlargePolygon(GUI_POINT * pDest, const GUI_POINT * pSrc, int NumPoints, int Len);

/*********************************************************************
*
*       Streamed bitmaps
*/
#define GUI_BITMAPSTREAM_GET_BUFFER     1
#define GUI_BITMAPSTREAM_RELEASE_BUFFER 2
#define GUI_BITMAPSTREAM_MODIFY_PALETTE 3

#define DECLARE_CREATE_FROM_STREAM(ID) int GUI_CreateBitmapFromStream##ID(GUI_BITMAP * pBMP, GUI_LOGPALETTE * pPAL, const void * p);

DECLARE_CREATE_FROM_STREAM(IDX)
DECLARE_CREATE_FROM_STREAM(RLE1)
DECLARE_CREATE_FROM_STREAM(RLE4)
DECLARE_CREATE_FROM_STREAM(RLE8)
DECLARE_CREATE_FROM_STREAM(565)
DECLARE_CREATE_FROM_STREAM(M565)
DECLARE_CREATE_FROM_STREAM(555)
DECLARE_CREATE_FROM_STREAM(M555)
DECLARE_CREATE_FROM_STREAM(A565)
DECLARE_CREATE_FROM_STREAM(AM565)
DECLARE_CREATE_FROM_STREAM(A555)
DECLARE_CREATE_FROM_STREAM(AM555)
DECLARE_CREATE_FROM_STREAM(RLE16)
DECLARE_CREATE_FROM_STREAM(RLEM16)
DECLARE_CREATE_FROM_STREAM(24)
DECLARE_CREATE_FROM_STREAM(A8)
DECLARE_CREATE_FROM_STREAM(Alpha)
DECLARE_CREATE_FROM_STREAM(M8888I)
DECLARE_CREATE_FROM_STREAM(RLEAlpha)
DECLARE_CREATE_FROM_STREAM(RLE32)
DECLARE_CREATE_FROM_STREAM(444_12)
DECLARE_CREATE_FROM_STREAM(M444_12)
DECLARE_CREATE_FROM_STREAM(444_12_1)
DECLARE_CREATE_FROM_STREAM(M444_12_1)
DECLARE_CREATE_FROM_STREAM(444_16)
DECLARE_CREATE_FROM_STREAM(M444_16)

int  GUI_CreateBitmapFromStream       (GUI_BITMAP * pBMP, GUI_LOGPALETTE * pPAL, const void * p);
void GUI_DrawStreamedBitmap           (const void * p, int x, int y);
void GUI_DrawStreamedBitmapAuto       (const void * p, int x, int y);
int  GUI_DrawStreamedBitmapEx         (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapExAuto     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapIDXEx      (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap444_12Ex   (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM444_12Ex  (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap444_12_1Ex (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM444_12_1Ex(GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap444_16Ex   (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM444_16Ex  (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap555Ex      (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM555Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap565Ex      (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM565Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapA555Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapAM555Ex    (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapA565Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapAM565Ex    (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap24Ex       (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmap8888Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapM8888IEx   (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapRLE1Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapRLE4Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapRLE8Ex     (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapRLE16Ex    (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapRLEM16Ex   (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
int  GUI_DrawStreamedBitmapRLE32Ex    (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);
void GUI_GetStreamedBitmapInfo        (const void * p, GUI_BITMAPSTREAM_INFO * pInfo);
int  GUI_GetStreamedBitmapInfoEx      (GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, GUI_BITMAPSTREAM_INFO * pInfo);
void GUI_SetStreamedBitmapHook        (GUI_BITMAPSTREAM_CALLBACK pfStreamedBitmapHook);

void LCD__RLE1_SetFunc (GUI_DTA_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off, const LCD_LOGPALETTE * pLogPal);
void LCD__RLE4_SetFunc (GUI_DTA_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off, const LCD_LOGPALETTE * pLogPal);
void LCD__RLE8_SetFunc (GUI_DTA_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off, const LCD_LOGPALETTE * pLogPal);
void LCD__RLE16_SetFunc(GUI_DTA_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off);
void LCD__RLE32_SetFunc(GUI_DTA_GET_DATA_FUNC * pfGetData, void * pVoid, U32 Off);

/*********************************************************************
*
*       BMP-export
*/
void GUI_BMP_Serialize     (GUI_CALLBACK_VOID_U8_P * pfSerialize, void * p);
void GUI_BMP_SerializeEx   (GUI_CALLBACK_VOID_U8_P * pfSerialize, int x0, int y0, int xSize, int ySize, void * p);
void GUI_BMP_SerializeExBpp(GUI_CALLBACK_VOID_U8_P * pfSerialize, int x0, int y0, int xSize, int ySize, void * p, int BitsPerPixel);

void GUI_BMP__WriteBitmapHeader(GUI_CALLBACK_VOID_U8_P * pfSerialize, int xSize, int ySize, void * p, int BitsPerPixel, int BytesPerLine, int BytesPerPixel, int NumColors, int Padding);

/*********************************************************************
*
*       Time / execution related routines
*/
void           GUI_Delay       (int Period);
GUI_TIMER_TIME GUI_GetTime     (void);
int            GUI_GetTimeSlice(void);
int            GUI_Exec        (void);        /* Execute all jobs ... Return 0 if nothing was done. */
int            GUI_Exec1       (void);        /* Execute one job  ... Return 0 if nothing was done. */
void           GUI_SetTimeSlice(int TimeSlice);

/*********************************************************************
*
*       MessageBox
*/
int     GUI_MessageBox   (const char * sMessage, const char * sCaption, int Flags);
#define GUI_MESSAGEBOX_CF_MOVEABLE (1 << 0)
#define GUI_MESSAGEBOX_CF_MODAL    (1 << 1)

#define GUI_MB_OK                20
#define GUI_MB_WARNING           21

/*********************************************************************
*
*       Anti Aliasing
*/
#define GUI_AA_TRANS   0  // Foreground color mixed up with current content of framebuffer
#define GUI_AA_NOTRANS 1  // Foreground color mixed up with current background color

void GUI_AA_DisableHiRes      (void);
void GUI_AA_EnableHiRes       (void);
int  GUI_AA_IsHiResEnabled    (void);
void GUI_AA_DrawPie           (int x0, int y0, int r, I32 a0, I32 a1);
int  GUI_AA_GetFactor         (void);
void GUI_AA_SetFactor         (int Factor);
void GUI_AA_DrawArc           (int x0, int y0, int rx, int ry, int a0, int a1);
void GUI_AA_DrawArcHR         (int x0, int y0, int rx, int ry, I32 a0, I32 a1);
void GUI_AA_DrawArcEx         (int mx, int my, int r, I32 a0, I32 a1, int c0, int c1);
void GUI_AA_DrawCircle        (int x0, int y0, int r);  // Currently not implemented, only for Dave2D
void GUI_AA_DrawLine          (int x0, int y0, int x1, int y1);
void GUI_AA_DrawPolyOutline   (const GUI_POINT * pSrc, int NumPoints, int Thickness, int x, int y);
void GUI_AA_DrawRoundedFrame  (int x0, int y0, int x1, int y1, int r);
void GUI_AA_DrawRoundedFrameEx(const GUI_RECT * pRect, int r);
void GUI_AA_DrawRoundedRect   (int x0, int y0, int x1, int y1, int r);
void GUI_AA_DrawRoundedRectEx (const GUI_RECT * pRect, int r);
void GUI_AA_FillCircle        (int x0, int y0, int r);
void GUI_AA_FillEllipse       (int x0, int y0, int rx, int ry);
void GUI_AA_FillEllipseXL     (int x0, int y0, int rx, int ry);
void GUI_AA_FillPolygon       (const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
int  GUI_AA_FillRoundedRect   (int x0, int y0, int x1, int y1, int r);
int  GUI_AA_FillRoundedRectEx (const GUI_RECT * pRect, int r);
void GUI_AA_SetBufferSize     (int BufferSize);
int  GUI_AA_SetDrawMode       (int Mode);
void GUI_AA_SetpfDrawCharAA4  (int (* pfDrawChar)(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine));
void GUI_AA_SetGammaAA4       (U8 * pGamma);
void GUI_AA_GetGammaAA4       (U8 * pGamma);
void GUI_AA_EnableGammaAA4    (int OnOff);

void GUI_AA_SetFuncDrawArc        (int (* pfDrawArc)    (int x0, int y0, int rx, int ry, I32 a0, I32 a1));
void GUI_AA_SetFuncDrawCircle     (int (* pfDrawCircle) (int x0, int y0, int r));
void GUI_AA_SetFuncDrawLine       (int (* pfDrawLine)   (int x0, int y0, int x1, int y1));
void GUI_AA_SetFuncDrawPolyOutline(int (* pfDrawPolyOutline)(const GUI_POINT * pSrc, int NumPoints, int Thickness, int x, int y));
void GUI_AA_SetFuncFillCircle     (int (* pfFillCircle) (int x0, int y0, int r));
void GUI_AA_SetFuncFillPolygon    (int (* pfFillPolygon)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0));

//
// Compatibility macros
//
#define GUI_AA_PreserveTrans(OnOff)                                           GUI_PreserveTrans(OnOff)
#define GUI_AA_DrawPolyOutlineEx(pSrc, NumPoints, Thickness, x, y, pBuffer)   GUI_AA_DrawPolyOutline(pSrc, NumPoints, Thickness, x, y)


/*********************************************************************
*
*       Keyboard
*/
/* Message layer */
void GUI_StoreKeyMsg(int Key, int Pressed);
void GUI_SendKeyMsg (int Key, int Pressed);
int  GUI_PollKeyMsg (void);
void GUI_GetKeyState(GUI_KEY_STATE * pState);

void GUI_KEY__SetHook(void (* pfHook)(const GUI_KEY_STATE *));

/* Application layer */
int  GUI_GetKey(void);
int  GUI_WaitKey(void);
void GUI_StoreKey(int c);
void GUI_ClearKeyBuffer(void);
int  GUI_GetKeyPressed(void);

/*********************************************************************
*
*       Task synchronization
*/
void GUI_WaitEvent            (void);
void GUI_SignalEvent          (void);
void GUI_SetSignalEventFunc   (GUI_SIGNAL_EVENT_FUNC     pfSignalEvent);
void GUI_SetWaitEventFunc     (GUI_WAIT_EVENT_FUNC       pfWaitEvent);
void GUI_SetWaitEventTimedFunc(GUI_WAIT_EVENT_TIMED_FUNC pfWaitEventTimed);

/*********************************************************************
*
*       PID  (Pointer input device ... mouse/touch)
*/
#define GUI_PID_RegisterHook GUI_PID_RegisterPreHook
void GUI_PID_StoreState      (const GUI_PID_STATE * pState);
int  GUI_PID_GetState        (      GUI_PID_STATE * pState);
void GUI_PID_GetCurrentState (      GUI_PID_STATE * pState);
int  GUI_PID_IsEmpty         (void);
int  GUI_PID_IsPressed       (void);
void GUI_PID_RegisterPreHook (GUI_REGISTER_HOOK * pRegisterHook);
void GUI_PID_SetHook         (void (* pfHook)(      GUI_PID_STATE *));  // Public
void GUI_PID__SetHook        (void (* pfHook)(const GUI_PID_STATE *));  // Private
void GUI_PID__OrientationHook(GUI_PID_STATE * pState);                  // Private
void GUI_PID_RegisterReadHook(GUI_REGISTER_HOOK * pRegisterReadHook);
U8   GUI_PID__BlockInput     (int OnOff);                               // Private
U8   GUI_PID__IsInputBlocked (void);                                    // Private

/*********************************************************************
*
*       Mouse, generic
*/
int  GUI_MOUSE_GetState  (      GUI_PID_STATE * pState);
void GUI_MOUSE_StoreState(const GUI_PID_STATE * pState);

/*********************************************************************
*
*       TOUCH screen, generic
*/
int  GUI_TOUCH_GetLayer     (void);
int  GUI_TOUCH_GetState     (GUI_PID_STATE * pState);
void GUI_TOUCH_GetUnstable  (int * px, int * py);  /* for diagnostics only */
void GUI_TOUCH_SetLayer     (int Layer);
void GUI_TOUCH_StoreState   (int x, int y);
void GUI_TOUCH_StoreStateEx (const GUI_PID_STATE * pState);
void GUI_TOUCH_StoreUnstable(int x, int y);

/*********************************************************************
*
*       Mouse, PS2 driver
*/
void GUI_MOUSE_DRIVER_PS2_Init(void);               /* optional */
void GUI_MOUSE_DRIVER_PS2_OnRx(unsigned char Data);

/*********************************************************************
*
*       TOUCH screen, analog driver
*/
int  GUI_TOUCH_CalcCoefficients (int NumPoints, int * pxRef, int * pyRef, int * pxSample, int * pySample, int xSize, int ySize);
int  GUI_TOUCH_Calibrate        (int Coord, int Log0, int Log1, int Phys0, int Phys1);
int  GUI_TOUCH_CalibratePoint   (int * px, int * py);
void GUI_TOUCH_EnableCalibration(int OnOff);
void GUI_TOUCH_Exec             (void);
int  GUI_TOUCH_GetxPhys         (void);    /* for diagnostics only */
int  GUI_TOUCH_GetyPhys         (void);    /* for diagnostics only */
void GUI_TOUCH_SetCalibration   (int (* pFunc)(int *, int *)); /* Not to be documented */
void GUI_TOUCH_SetOrientation   (unsigned Orientation);
int  GUI_TOUCH_TransformPoint   (int * px, int * py);          /* Not to be documented */

/*********************************************************************
*
*       TOUCH: imports
*
* Please note: The following functions are required by the module.
* They need to be part of your application software (or rather, part
* of the hardware-layer of your software).
*/
void GUI_TOUCH_X_ActivateX(void);
void GUI_TOUCH_X_ActivateY(void);
void GUI_TOUCH_X_Disable  (void);
int  GUI_TOUCH_X_MeasureX (void);
int  GUI_TOUCH_X_MeasureY (void);

/*********************************************************************
*
*       LCD: Palette conversion table
*
* Please note: These functions were originally in GUI_Private.h but
* were also used in several LCDConf.c. Since GUI_Private.h is not
* shipped any more with object code we moved these functions here.
*/
LCD_PIXELINDEX * LCD_GetpPalConvTable        (const LCD_LOGPALETTE * pLogPal);
LCD_PIXELINDEX * LCD_GetpPalConvTableUncached(const LCD_LOGPALETTE * pLogPal);
LCD_PIXELINDEX * LCD_GetpPalConvTableBM      (const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int LayerIndex);
// Setting a function for converting a color palette to an array of index values
void GUI_SetFuncGetpPalConvTable(LCD_PIXELINDEX * (* pFunc)(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int LayerIndex));

/*********************************************************************
*
*       GUI_X_
*
* Externals, to be defined by application
*
* The externals defined below should be defined by the
* application. They are per default contained in the module
* GUI_X.c.
* Note that a lot if not all of these are not required in most target
* systems.
* For this module, samples are available for configurations
* with or without operating system.
*/
//
// Configuration
//
void GUI_X_Config(void);
void GUI_X_Init  (void);

//
// Timing routines
//
GUI_TIMER_TIME GUI_X_GetTime(void);
void           GUI_X_Delay  (int Period);

//
// Multitask routines - required only if multitasking is used (#define GUI_OS 1)
//
void GUI_X_Unlock   (void);
void GUI_X_Lock     (void);
U32  GUI_X_GetTaskId(void);
void GUI_X_InitOS   (void);

//
// Event driving (optional with multitasking)
//
void GUI_X_ExecIdle      (void);
void GUI_X_WaitEvent     (void);
void GUI_X_WaitEventTimed(int Period);
void GUI_X_SignalEvent   (void);

//
// Recording (logs/warnings and errors) - required only for higher levels
//
void GUI_X_Log     (const char * s);
void GUI_X_Warn    (const char * s);
void GUI_X_ErrorOut(const char * s);

/*********************************************************************
*
*       Constants for fonts and bitmaps
*/
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE1;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE1Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE4;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE4Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE8;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE8_Stream;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE8Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE16_Stream;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE16Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEM16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEM16_Stream;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEM16Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE32;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE32Ex;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsRLEAlpha;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsA8;

extern const GUI_BITMAP_METHODS GUI_BitmapMethods444_12;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM444_12;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods444_12_1;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM444_12_1;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods444_16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM444_16;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods555;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM555;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods24;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods888;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM888;
extern const GUI_BITMAP_METHODS GUI_BitmapMethods8888;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsM8888I;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsA565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsAM565;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsA555;
extern const GUI_BITMAP_METHODS GUI_BitmapMethodsAM555;

#define GUI_COMPRESS_RLE1 0  // Used to initialize GUI_BITMAP struct if saved with the Bitmap Converter as RLE bitmap
#define GUI_COMPRESS_RLE4 0  // Used to initialize GUI_BITMAP struct if saved with the Bitmap Converter as RLE bitmap
#define GUI_COMPRESS_RLE8 0  // Used to initialize GUI_BITMAP struct if saved with the Bitmap Converter as RLE bitmap

#define GUI_DRAW_RLE1          &GUI_BitmapMethodsRLE1           /* Method table ! */
#define GUI_DRAW_RLE4          &GUI_BitmapMethodsRLE4           /* Method table ! */
#define GUI_DRAW_RLE8          &GUI_BitmapMethodsRLE8           /* Method table ! */
#define GUI_DRAW_RLE8_Stream   &GUI_BitmapMethodsRLE8_Stream    /* Method table ! */
#define GUI_DRAW_RLE16         &GUI_BitmapMethodsRLE16          /* Method table ! */
#define GUI_DRAW_RLE16_Stream  &GUI_BitmapMethodsRLE16_Stream   /* Method table ! */
#define GUI_DRAW_RLEM16        &GUI_BitmapMethodsRLEM16         /* Method table ! */
#define GUI_DRAW_RLEM16_Stream &GUI_BitmapMethodsRLEM16_Stream  /* Method table ! */
#define GUI_DRAW_RLE32         &GUI_BitmapMethodsRLE32          /* Method table ! */
#define GUI_DRAW_RLEALPHA      &GUI_BitmapMethodsRLEAlpha       /* Method table ! */
#define GUI_DRAW_A8            &GUI_BitmapMethodsA8             /* Method table ! */
 
#define GUI_DRAW_BMP444_12     &GUI_BitmapMethods444_12         /* Method table ! */
#define GUI_DRAW_BMPM444_12    &GUI_BitmapMethodsM444_12        /* Method table ! */
#define GUI_DRAW_BMP444_12_1   &GUI_BitmapMethods444_12_1       /* Method table ! */
#define GUI_DRAW_BMPM444_12_1  &GUI_BitmapMethodsM444_12_1      /* Method table ! */
#define GUI_DRAW_BMP444_16     &GUI_BitmapMethods444_16         /* Method table ! */
#define GUI_DRAW_BMPM444_16    &GUI_BitmapMethodsM444_16        /* Method table ! */
#define GUI_DRAW_BMP555        &GUI_BitmapMethods555            /* Method table ! */
#define GUI_DRAW_BMPM555       &GUI_BitmapMethodsM555           /* Method table ! */
#define GUI_DRAW_BMP565        &GUI_BitmapMethods565            /* Method table ! */
#define GUI_DRAW_BMPM565       &GUI_BitmapMethodsM565           /* Method table ! */
#define GUI_DRAW_BMP24         &GUI_BitmapMethods24             /* Method table ! */
#define GUI_DRAW_BMP888        &GUI_BitmapMethods888            /* Method table ! */
#define GUI_DRAW_BMPM888       &GUI_BitmapMethodsM888           /* Method table ! */
#define GUI_DRAW_BMP8888       &GUI_BitmapMethods8888           /* Method table ! */
#define GUI_DRAW_BMPM8888I     &GUI_BitmapMethodsM8888I         /* Method table ! */
#define GUI_DRAW_BMPA555       &GUI_BitmapMethodsA555           /* Method table ! */
#define GUI_DRAW_BMPAM555      &GUI_BitmapMethodsAM555          /* Method table ! */
#define GUI_DRAW_BMPA565       &GUI_BitmapMethodsA565           /* Method table ! */
#define GUI_DRAW_BMPAM565      &GUI_BitmapMethodsAM565          /* Method table ! */

extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_Ext;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_Frm;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA2;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA4;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA2_EXT;
extern const tGUI_SIF_APIList GUI_SIF_APIList_Prop_AA4_EXT;

extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_Ext;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_Frm;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_AA2_Ext;
extern const tGUI_XBF_APIList GUI_XBF_APIList_Prop_AA4_Ext;

/*********************************************************************
*
*       GUI_KEY_...
*
* These ID values are basically meant to be used with widgets
* Note that we have chosen the values to be close to existing
* "standards", so do not change them unless forced to.
*
*/
#define GUI_KEY_BACKSPACE         8         /* ASCII: BACKSPACE Crtl-H */
#define GUI_KEY_TAB               9         /* ASCII: TAB       Crtl-I */
#define GUI_KEY_BACKTAB           10
#define GUI_KEY_ENTER             13        /* ASCII: ENTER     Crtl-M */
#define GUI_KEY_ALT               15
#define GUI_KEY_LEFT              16
#define GUI_KEY_UP                17
#define GUI_KEY_RIGHT             18
#define GUI_KEY_DOWN              19
#define GUI_KEY_PGUP              20
#define GUI_KEY_PGDOWN            21
#define GUI_KEY_HOME              23
#define GUI_KEY_END               24
#define GUI_KEY_SHIFT             25
#define GUI_KEY_CONTROL           26
#define GUI_KEY_ESCAPE            27        /* ASCII: ESCAPE    0x1b   */
#define GUI_KEY_INSERT            29
#define GUI_KEY_DELETE            30
#define GUI_KEY_SPACE             32

#define GUI_KEY_F1                40
#define GUI_KEY_F2                41

/*********************************************************************
*
*       Dialog item IDs
*
*  The IDs below are arbitrary values. They just have to be unique
*  within the dialog.
*
*  If you use your own Ids, we recommend to use values above GUI_ID_USER.
*/
#define GUI_ID_OK                1
#define GUI_ID_CANCEL            2
#define GUI_ID_YES               3
#define GUI_ID_NO                4
#define GUI_ID_CLOSE             5
#define GUI_ID_HELP              6
#define GUI_ID_MAXIMIZE          7
#define GUI_ID_MINIMIZE          8

#define GUI_ID_VSCROLL    0xFE
#define GUI_ID_HSCROLL    0xFF

#define GUI_ID_VSCROLLER  0xFFE
#define GUI_ID_HSCROLLER  0xFFF

#define GUI_ID_EDIT0      0x100
#define GUI_ID_EDIT1      0x101
#define GUI_ID_EDIT2      0x102
#define GUI_ID_EDIT3      0x103
#define GUI_ID_EDIT4      0x104
#define GUI_ID_EDIT5      0x105
#define GUI_ID_EDIT6      0x106
#define GUI_ID_EDIT7      0x107
#define GUI_ID_EDIT8      0x108
#define GUI_ID_EDIT9      0x109

#define GUI_ID_LISTBOX0   0x110
#define GUI_ID_LISTBOX1   0x111
#define GUI_ID_LISTBOX2   0x112
#define GUI_ID_LISTBOX3   0x113
#define GUI_ID_LISTBOX4   0x114
#define GUI_ID_LISTBOX5   0x115
#define GUI_ID_LISTBOX6   0x116
#define GUI_ID_LISTBOX7   0x117
#define GUI_ID_LISTBOX8   0x118
#define GUI_ID_LISTBOX9   0x119

#define GUI_ID_CHECK0     0x120
#define GUI_ID_CHECK1     0x121
#define GUI_ID_CHECK2     0x122
#define GUI_ID_CHECK3     0x123
#define GUI_ID_CHECK4     0x124
#define GUI_ID_CHECK5     0x125
#define GUI_ID_CHECK6     0x126
#define GUI_ID_CHECK7     0x127
#define GUI_ID_CHECK8     0x128
#define GUI_ID_CHECK9     0x129

#define GUI_ID_SLIDER0    0x130
#define GUI_ID_SLIDER1    0x131
#define GUI_ID_SLIDER2    0x132
#define GUI_ID_SLIDER3    0x133
#define GUI_ID_SLIDER4    0x134
#define GUI_ID_SLIDER5    0x135
#define GUI_ID_SLIDER6    0x136
#define GUI_ID_SLIDER7    0x137
#define GUI_ID_SLIDER8    0x138
#define GUI_ID_SLIDER9    0x139

#define GUI_ID_SCROLLBAR0 0x140
#define GUI_ID_SCROLLBAR1 0x141
#define GUI_ID_SCROLLBAR2 0x142
#define GUI_ID_SCROLLBAR3 0x143
#define GUI_ID_SCROLLBAR4 0x144
#define GUI_ID_SCROLLBAR5 0x145
#define GUI_ID_SCROLLBAR6 0x146
#define GUI_ID_SCROLLBAR7 0x147
#define GUI_ID_SCROLLBAR8 0x148
#define GUI_ID_SCROLLBAR9 0x149

#define GUI_ID_RADIO0     0x150
#define GUI_ID_RADIO1     0x151
#define GUI_ID_RADIO2     0x152
#define GUI_ID_RADIO3     0x153
#define GUI_ID_RADIO4     0x154
#define GUI_ID_RADIO5     0x155
#define GUI_ID_RADIO6     0x156
#define GUI_ID_RADIO7     0x157
#define GUI_ID_RADIO8     0x158
#define GUI_ID_RADIO9     0x159

#define GUI_ID_TEXT0      0x160
#define GUI_ID_TEXT1      0x161
#define GUI_ID_TEXT2      0x162
#define GUI_ID_TEXT3      0x163
#define GUI_ID_TEXT4      0x164
#define GUI_ID_TEXT5      0x165
#define GUI_ID_TEXT6      0x166
#define GUI_ID_TEXT7      0x167
#define GUI_ID_TEXT8      0x168
#define GUI_ID_TEXT9      0x169

#define GUI_ID_BUTTON0    0x170
#define GUI_ID_BUTTON1    0x171
#define GUI_ID_BUTTON2    0x172
#define GUI_ID_BUTTON3    0x173
#define GUI_ID_BUTTON4    0x174
#define GUI_ID_BUTTON5    0x175
#define GUI_ID_BUTTON6    0x176
#define GUI_ID_BUTTON7    0x177
#define GUI_ID_BUTTON8    0x178
#define GUI_ID_BUTTON9    0x179

#define GUI_ID_DROPDOWN0  0x180
#define GUI_ID_DROPDOWN1  0x181
#define GUI_ID_DROPDOWN2  0x182
#define GUI_ID_DROPDOWN3  0x183
#define GUI_ID_DROPDOWN4  0x184
#define GUI_ID_DROPDOWN5  0x185
#define GUI_ID_DROPDOWN6  0x186
#define GUI_ID_DROPDOWN7  0x187
#define GUI_ID_DROPDOWN8  0x188
#define GUI_ID_DROPDOWN9  0x189

#define GUI_ID_MULTIEDIT0 0x190
#define GUI_ID_MULTIEDIT1 0x191
#define GUI_ID_MULTIEDIT2 0x192
#define GUI_ID_MULTIEDIT3 0x193
#define GUI_ID_MULTIEDIT4 0x194
#define GUI_ID_MULTIEDIT5 0x195
#define GUI_ID_MULTIEDIT6 0x196
#define GUI_ID_MULTIEDIT7 0x197
#define GUI_ID_MULTIEDIT8 0x198
#define GUI_ID_MULTIEDIT9 0x199

#define GUI_ID_LISTVIEW0  0x200
#define GUI_ID_LISTVIEW1  0x201
#define GUI_ID_LISTVIEW2  0x202
#define GUI_ID_LISTVIEW3  0x203
#define GUI_ID_LISTVIEW4  0x204
#define GUI_ID_LISTVIEW5  0x205
#define GUI_ID_LISTVIEW6  0x206
#define GUI_ID_LISTVIEW7  0x207
#define GUI_ID_LISTVIEW8  0x208
#define GUI_ID_LISTVIEW9  0x209

#define GUI_ID_PROGBAR0   0x210
#define GUI_ID_PROGBAR1   0x211
#define GUI_ID_PROGBAR2   0x212
#define GUI_ID_PROGBAR3   0x213
#define GUI_ID_PROGBAR4   0x214
#define GUI_ID_PROGBAR5   0x215
#define GUI_ID_PROGBAR6   0x216
#define GUI_ID_PROGBAR7   0x217
#define GUI_ID_PROGBAR8   0x218
#define GUI_ID_PROGBAR9   0x219

#define GUI_ID_GRAPH0     0x220
#define GUI_ID_GRAPH1     0x221
#define GUI_ID_GRAPH2     0x222
#define GUI_ID_GRAPH3     0x223
#define GUI_ID_GRAPH4     0x224
#define GUI_ID_GRAPH5     0x225
#define GUI_ID_GRAPH6     0x226
#define GUI_ID_GRAPH7     0x227
#define GUI_ID_GRAPH8     0x228
#define GUI_ID_GRAPH9     0x229

#define GUI_ID_MULTIPAGE0 0x230
#define GUI_ID_MULTIPAGE1 0x231
#define GUI_ID_MULTIPAGE2 0x232
#define GUI_ID_MULTIPAGE3 0x233
#define GUI_ID_MULTIPAGE4 0x234
#define GUI_ID_MULTIPAGE5 0x235
#define GUI_ID_MULTIPAGE6 0x236
#define GUI_ID_MULTIPAGE7 0x237
#define GUI_ID_MULTIPAGE8 0x238
#define GUI_ID_MULTIPAGE9 0x239

#define GUI_ID_TREEVIEW0  0x240
#define GUI_ID_TREEVIEW1  0x241
#define GUI_ID_TREEVIEW2  0x242
#define GUI_ID_TREEVIEW3  0x243
#define GUI_ID_TREEVIEW4  0x244
#define GUI_ID_TREEVIEW5  0x245
#define GUI_ID_TREEVIEW6  0x246
#define GUI_ID_TREEVIEW7  0x247
#define GUI_ID_TREEVIEW8  0x248
#define GUI_ID_TREEVIEW9  0x249

#define GUI_ID_ICONVIEW0  0x250
#define GUI_ID_ICONVIEW1  0x251
#define GUI_ID_ICONVIEW2  0x252
#define GUI_ID_ICONVIEW3  0x253
#define GUI_ID_ICONVIEW4  0x254
#define GUI_ID_ICONVIEW5  0x255
#define GUI_ID_ICONVIEW6  0x256
#define GUI_ID_ICONVIEW7  0x257
#define GUI_ID_ICONVIEW8  0x258
#define GUI_ID_ICONVIEW9  0x259

#define GUI_ID_LISTWHEEL0 0x260
#define GUI_ID_LISTWHEEL1 0x261
#define GUI_ID_LISTWHEEL2 0x262
#define GUI_ID_LISTWHEEL3 0x263
#define GUI_ID_LISTWHEEL4 0x264
#define GUI_ID_LISTWHEEL5 0x265
#define GUI_ID_LISTWHEEL6 0x266
#define GUI_ID_LISTWHEEL7 0x267
#define GUI_ID_LISTWHEEL8 0x268
#define GUI_ID_LISTWHEEL9 0x269

#define GUI_ID_IMAGE0     0x270
#define GUI_ID_IMAGE1     0x271
#define GUI_ID_IMAGE2     0x272
#define GUI_ID_IMAGE3     0x273
#define GUI_ID_IMAGE4     0x274
#define GUI_ID_IMAGE5     0x275
#define GUI_ID_IMAGE6     0x276
#define GUI_ID_IMAGE7     0x277
#define GUI_ID_IMAGE8     0x278
#define GUI_ID_IMAGE9     0x279

#define GUI_ID_SPINBOX0   0x280
#define GUI_ID_SPINBOX1   0x281
#define GUI_ID_SPINBOX2   0x282
#define GUI_ID_SPINBOX3   0x283
#define GUI_ID_SPINBOX4   0x284
#define GUI_ID_SPINBOX5   0x285
#define GUI_ID_SPINBOX6   0x286
#define GUI_ID_SPINBOX7   0x287
#define GUI_ID_SPINBOX8   0x288
#define GUI_ID_SPINBOX9   0x289

#define GUI_ID_CALENDAR0  0x290
#define GUI_ID_CALENDAR1  0x291
#define GUI_ID_CALENDAR2  0x292
#define GUI_ID_CALENDAR3  0x293
#define GUI_ID_CALENDAR4  0x294
#define GUI_ID_CALENDAR5  0x295
#define GUI_ID_CALENDAR6  0x296
#define GUI_ID_CALENDAR7  0x297
#define GUI_ID_CALENDAR8  0x298
#define GUI_ID_CALENDAR9  0x299

#define GUI_ID_KNOB0      0x300
#define GUI_ID_KNOB1      0x301
#define GUI_ID_KNOB2      0x302
#define GUI_ID_KNOB3      0x303
#define GUI_ID_KNOB4      0x304
#define GUI_ID_KNOB5      0x305
#define GUI_ID_KNOB6      0x306
#define GUI_ID_KNOB7      0x307
#define GUI_ID_KNOB8      0x308
#define GUI_ID_KNOB9      0x309

#define GUI_ID_ROTARY0    0x310
#define GUI_ID_ROTARY1    0x311
#define GUI_ID_ROTARY2    0x312
#define GUI_ID_ROTARY3    0x313
#define GUI_ID_ROTARY4    0x314
#define GUI_ID_ROTARY5    0x315
#define GUI_ID_ROTARY6    0x316
#define GUI_ID_ROTARY7    0x317
#define GUI_ID_ROTARY8    0x318
#define GUI_ID_ROTARY9    0x319

#define GUI_ID_SWIPELIST0 0x320
#define GUI_ID_SWIPELIST1 0x321
#define GUI_ID_SWIPELIST2 0x322
#define GUI_ID_SWIPELIST3 0x323
#define GUI_ID_SWIPELIST4 0x324
#define GUI_ID_SWIPELIST5 0x325
#define GUI_ID_SWIPELIST6 0x326
#define GUI_ID_SWIPELIST7 0x327
#define GUI_ID_SWIPELIST8 0x328
#define GUI_ID_SWIPELIST9 0x329

#define GUI_ID_SWITCH0    0x330
#define GUI_ID_SWITCH1    0x331
#define GUI_ID_SWITCH2    0x332
#define GUI_ID_SWITCH3    0x333
#define GUI_ID_SWITCH4    0x334
#define GUI_ID_SWITCH5    0x335
#define GUI_ID_SWITCH6    0x336
#define GUI_ID_SWITCH7    0x337
#define GUI_ID_SWITCH8    0x338
#define GUI_ID_SWITCH9    0x339

#define GUI_ID_GAUGE0     0x340
#define GUI_ID_GAUGE1     0x341
#define GUI_ID_GAUGE2     0x342
#define GUI_ID_GAUGE3     0x343
#define GUI_ID_GAUGE4     0x344
#define GUI_ID_GAUGE5     0x345
#define GUI_ID_GAUGE6     0x346
#define GUI_ID_GAUGE7     0x347
#define GUI_ID_GAUGE8     0x348
#define GUI_ID_GAUGE9     0x349

#define GUI_ID_QRCODE0    0x350
#define GUI_ID_QRCODE1    0x351
#define GUI_ID_QRCODE2    0x352
#define GUI_ID_QRCODE3    0x353
#define GUI_ID_QRCODE4    0x354
#define GUI_ID_QRCODE5    0x355
#define GUI_ID_QRCODE6    0x356
#define GUI_ID_QRCODE7    0x357
#define GUI_ID_QRCODE8    0x358
#define GUI_ID_QRCODE9    0x359

#define GUI_ID_KEYBOARD0  0x360
#define GUI_ID_KEYBOARD1  0x361
#define GUI_ID_KEYBOARD2  0x362
#define GUI_ID_KEYBOARD3  0x363
#define GUI_ID_KEYBOARD4  0x364
#define GUI_ID_KEYBOARD5  0x365
#define GUI_ID_KEYBOARD6  0x366
#define GUI_ID_KEYBOARD7  0x367
#define GUI_ID_KEYBOARD8  0x368
#define GUI_ID_KEYBOARD9  0x369

#define GUI_ID_ANIM0      0x370
#define GUI_ID_ANIM1      0x371
#define GUI_ID_ANIM2      0x372
#define GUI_ID_ANIM3      0x373
#define GUI_ID_ANIM4      0x374
#define GUI_ID_ANIM5      0x375
#define GUI_ID_ANIM6      0x376
#define GUI_ID_ANIM7      0x377
#define GUI_ID_ANIM8      0x378
#define GUI_ID_ANIM9      0x379

#define GUI_ID_SCROLLER0  0x380
#define GUI_ID_SCROLLER1  0x381
#define GUI_ID_SCROLLER2  0x382
#define GUI_ID_SCROLLER3  0x383
#define GUI_ID_SCROLLER4  0x384
#define GUI_ID_SCROLLER5  0x385
#define GUI_ID_SCROLLER6  0x386
#define GUI_ID_SCROLLER7  0x387
#define GUI_ID_SCROLLER8  0x388
#define GUI_ID_SCROLLER9  0x389

#define GUI_ID_WHEEL0     0x390
#define GUI_ID_WHEEL1     0x391
#define GUI_ID_WHEEL2     0x392
#define GUI_ID_WHEEL3     0x393
#define GUI_ID_WHEEL4     0x394
#define GUI_ID_WHEEL5     0x395
#define GUI_ID_WHEEL6     0x396
#define GUI_ID_WHEEL7     0x397
#define GUI_ID_WHEEL8     0x398
#define GUI_ID_WHEEL9     0x399

#define GUI_ID_MOVIE0     0x400
#define GUI_ID_MOVIE1     0x401
#define GUI_ID_MOVIE2     0x402
#define GUI_ID_MOVIE3     0x403
#define GUI_ID_MOVIE4     0x404
#define GUI_ID_MOVIE5     0x405
#define GUI_ID_MOVIE6     0x406
#define GUI_ID_MOVIE7     0x407
#define GUI_ID_MOVIE8     0x408
#define GUI_ID_MOVIE9     0x409

#define GUI_ID_TICKER0    0x410
#define GUI_ID_TICKER1    0x411
#define GUI_ID_TICKER2    0x412
#define GUI_ID_TICKER3    0x413
#define GUI_ID_TICKER4    0x414
#define GUI_ID_TICKER5    0x415
#define GUI_ID_TICKER6    0x416
#define GUI_ID_TICKER7    0x417
#define GUI_ID_TICKER8    0x418
#define GUI_ID_TICKER9    0x419

#define GUI_ID_USER       0x800

/*********************************************************************
*
*       Mouse buttons
*/
#define GUI_LBUTTON (1 << 0) /* Left button */
#define GUI_RBUTTON (1 << 1) /* Right button */
#define GUI_MBUTTON (1 << 2) /* Middle button */
#define GUI_DBUTTON (1 << 7) /* Double-click button */

/*********************************************************************
*
*       Text style flags
*
*  Description
*    Text style how a text will be displayed.
*/
#define GUI_TS_NORMAL           (0)        // Renders text normal (default).
#define GUI_TS_UNDERLINE        (1 << 0)   // Renders text underlined.
#define GUI_TS_STRIKETHRU       (1 << 1)   // Renders text in strike through type.
#define GUI_TS_OVERLINE         (1 << 2)   // Renders text in overline type.

/*********************************************************************
*
*       Line styles
*
*  Description
*    Style how a line is drawn.
*/
#define GUI_LS_SOLID        (0)    // Lines are drawn solid (default).
#define GUI_LS_DASH         (1)    // Lines are drawn dashed.
#define GUI_LS_DOT          (2)    // Lines are drawn dotted.
#define GUI_LS_DASHDOT      (3)    // Lines are drawn alternating with dashes and dots.
#define GUI_LS_DASHDOTDOT   (4)    // Lines are drawn alternating with dashes and double dots.

/*********************************************************************
*
*       Pen shapes
*/
#define GUI_PS_ROUND        (0)
#define GUI_PS_FLAT         (1)
#define GUI_PS_SQUARE       (2)

/*********************************************************************
*
*       Standard colors
*/
#define GUI_INVALID_COLOR ((((U32)GUI_TRANS_BYTE) << 24) | 0x00ABCDEFul)  /* Invalid color (transparency + determined color) */

#if (GUI_USE_ARGB)
  #define GUI_MAKE_COLOR(ABGR)  (((((U32)ABGR) & 0xFF000000ul) ^ 0xFF000000ul) | ((((U32)ABGR) & 0x00FF0000ul) >> 16) | (((U32)ABGR) & 0x0000FF00ul) | ((((U32)ABGR) & 0x000000FFul) << 16))
  #define GUI_MAKE_TRANS(Alpha) (255 - (Alpha))
#else
  #define GUI_MAKE_COLOR(ABGR)  (ABGR)
  #define GUI_MAKE_TRANS(Alpha) (Alpha)
#endif

#if (GUI_USE_ARGB)
  #define GUI_TRANS_BYTE 0x00
#else
  #define GUI_TRANS_BYTE 0xFF
#endif

#define GUI_BLUE          GUI_MAKE_COLOR(0x00FF0000)
#define GUI_GREEN         GUI_MAKE_COLOR(0x0000FF00)
#define GUI_RED           GUI_MAKE_COLOR(0x000000FF)
#define GUI_CYAN          GUI_MAKE_COLOR(0x00FFFF00)
#define GUI_MAGENTA       GUI_MAKE_COLOR(0x00FF00FF)
#define GUI_YELLOW        GUI_MAKE_COLOR(0x0000FFFF)
#define GUI_LIGHTBLUE     GUI_MAKE_COLOR(0x00FF8080)
#define GUI_LIGHTGREEN    GUI_MAKE_COLOR(0x0080FF80)
#define GUI_LIGHTRED      GUI_MAKE_COLOR(0x008080FF)
#define GUI_LIGHTCYAN     GUI_MAKE_COLOR(0x00FFFF80)
#define GUI_LIGHTMAGENTA  GUI_MAKE_COLOR(0x00FF80FF)
#define GUI_LIGHTYELLOW   GUI_MAKE_COLOR(0x0080FFFF)
#define GUI_DARKBLUE      GUI_MAKE_COLOR(0x00800000)
#define GUI_DARKGREEN     GUI_MAKE_COLOR(0x00008000)
#define GUI_DARKRED       GUI_MAKE_COLOR(0x00000080)
#define GUI_DARKCYAN      GUI_MAKE_COLOR(0x00808000)
#define GUI_DARKMAGENTA   GUI_MAKE_COLOR(0x00800080)
#define GUI_DARKYELLOW    GUI_MAKE_COLOR(0x00008080)
#define GUI_WHITE         GUI_MAKE_COLOR(0x00FFFFFF)
#define GUI_LIGHTGRAY     GUI_MAKE_COLOR(0x00D3D3D3)
#define GUI_GRAY          GUI_MAKE_COLOR(0x00808080)
#define GUI_DARKGRAY      GUI_MAKE_COLOR(0x00404040)
#define GUI_BLACK         GUI_MAKE_COLOR(0x00000000)
#define GUI_BROWN         GUI_MAKE_COLOR(0x002A2AA5)
#define GUI_ORANGE        GUI_MAKE_COLOR(0x0000A5FF)
#define GUI_PINK          GUI_MAKE_COLOR(0x00C00FFC)

#define GUI_TRANSPARENT   GUI_MAKE_COLOR(0xFF000000)

#define GUI_GRAY_3F       GUI_MAKE_COLOR(0x003F3F3F)
#define GUI_GRAY_50       GUI_MAKE_COLOR(0x00505050)
#define GUI_GRAY_55       GUI_MAKE_COLOR(0x00555555)
#define GUI_GRAY_60       GUI_MAKE_COLOR(0x00606060)
#define GUI_GRAY_7C       GUI_MAKE_COLOR(0x007C7C7C)
#define GUI_GRAY_9A       GUI_MAKE_COLOR(0x009A9A9A)
#define GUI_GRAY_AA       GUI_MAKE_COLOR(0x00AAAAAA)
#define GUI_GRAY_C0       GUI_MAKE_COLOR(0x00C0C0C0)
#define GUI_GRAY_C8       GUI_MAKE_COLOR(0x00C8C8C8)
#define GUI_GRAY_D0       GUI_MAKE_COLOR(0x00D0D0D0)
#define GUI_GRAY_E7       GUI_MAKE_COLOR(0x00E7E7E7)

#define GUI_BLUE_98       GUI_MAKE_COLOR(0x00980000)

/*********************************************************************
*
*       MultiTouch-Support
*/
/*********************************************************************
*
*       MultiTouch flags
*
*  Description
*    Data structure used by GUI_MTOUCH_GetEvent() to store a multi touch
*    event in.
*/
#define GUI_MTOUCH_FLAG_DOWN (1 << 0)    // New touch point has touched the surface.
#define GUI_MTOUCH_FLAG_MOVE (1 << 1)    // Touch point has been moved.
#define GUI_MTOUCH_FLAG_UP   (1 << 2)    // Touch point has released the surface.

/*********************************************************************
*
*       Variables
*/
extern T_GUI_MTOUCH_STOREEVENT GUI_MTOUCH__pStoreEvent;

/*********************************************************************
*
*       Interface
*/
void GUI_MTOUCH_Enable          (int OnOff);
int  GUI_MTOUCH_GetEvent        (GUI_MTOUCH_EVENT * pEvent);
int  GUI_MTOUCH_GetTouchInput   (GUI_MTOUCH_EVENT * pEvent, GUI_MTOUCH_INPUT * pBuffer, unsigned Index);
int  GUI_MTOUCH_IsEmpty         (void);
void GUI_MTOUCH_SetOrientation  (int Orientation);
void GUI_MTOUCH_SetOrientationEx(int Orientation, int LayerIndex);
void GUI_MTOUCH_StoreEvent      (GUI_MTOUCH_EVENT * pEvent, GUI_MTOUCH_INPUT * pInput);

/*********************************************************************
*
*       Axis values
* 
*  Description
*    Defines to distinguish between the X and Y axis. Used in various
*    emWin functions.
*/
#define GUI_COORD_X     0     // X axis.
#define GUI_COORD_Y     1     // Y axis.

/*********************************************************************
*
*       Horizontal / Vertical
*/
#define GUI_HORIZONTAL (1 << GUI_COORD_X)
#define GUI_VERTICAL   (1 << GUI_COORD_Y)

/*********************************************************************
*
*       Addresses of standard fonts
*/
//
// Proportional fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8_ASCII,        GUI_Font8_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font10S_ASCII,      GUI_Font10S_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font10_ASCII,       GUI_Font10_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13_ASCII,       GUI_Font13_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13B_ASCII,      GUI_Font13B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13H_ASCII,      GUI_Font13H_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font13HB_ASCII,     GUI_Font13HB_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font16_ASCII,       GUI_Font16_1,       GUI_Font16_HK,    GUI_Font16_1HK;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font16B_ASCII,      GUI_Font16B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font20_ASCII,       GUI_Font20_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font20B_ASCII,      GUI_Font20B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font24_ASCII,       GUI_Font24_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font24B_ASCII,      GUI_Font24B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font32_ASCII,       GUI_Font32_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font32B_ASCII,      GUI_Font32B_1;

//
// Proportional fonts, framed
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font20F_ASCII;

//
// Monospaced
//
extern GUI_CONST_STORAGE GUI_FONT GUI_Font4x6;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font6x8,            GUI_Font6x9;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font6x8_ASCII,      GUI_Font6x8_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x8,            GUI_Font8x9;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x8_ASCII,      GUI_Font8x8_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x10_ASCII;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x12_ASCII;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x13_ASCII,     GUI_Font8x13_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x15B_ASCII,    GUI_Font8x15B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x16,           GUI_Font8x17,       GUI_Font8x18;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x16x1x2,       GUI_Font8x16x2x2,   GUI_Font8x16x3x3;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font8x16_ASCII,     GUI_Font8x16_1;

//
// Digits
//
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD24x32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD36x48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD48;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD48x64;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD64;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD60x80;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontD80;

//
// Comic fonts
//
extern GUI_CONST_STORAGE GUI_FONT GUI_FontComic18B_ASCII, GUI_FontComic18B_1;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontComic24B_ASCII, GUI_FontComic24B_1;

/*********************************************************************
*
*       Macros to be used
*/
//
// Proportional fonts
//
#define GUI_FONT_8_ASCII        &GUI_Font8_ASCII
#define GUI_FONT_8_1            &GUI_Font8_1
#define GUI_FONT_10S_ASCII      &GUI_Font10S_ASCII
#define GUI_FONT_10S_1          &GUI_Font10S_1
#define GUI_FONT_10_ASCII       &GUI_Font10_ASCII
#define GUI_FONT_10_1           &GUI_Font10_1
#define GUI_FONT_13_ASCII       &GUI_Font13_ASCII
#define GUI_FONT_13_1           &GUI_Font13_1
#define GUI_FONT_13B_ASCII      &GUI_Font13B_ASCII
#define GUI_FONT_13B_1          &GUI_Font13B_1
#define GUI_FONT_13H_ASCII      &GUI_Font13H_ASCII
#define GUI_FONT_13H_1          &GUI_Font13H_1
#define GUI_FONT_13HB_ASCII     &GUI_Font13HB_ASCII
#define GUI_FONT_13HB_1         &GUI_Font13HB_1
#define GUI_FONT_16_ASCII       &GUI_Font16_ASCII
#define GUI_FONT_16_1           &GUI_Font16_1
#define GUI_FONT_16_HK          &GUI_Font16_HK
#define GUI_FONT_16_1HK         &GUI_Font16_1HK
#define GUI_FONT_16B_ASCII      &GUI_Font16B_ASCII
#define GUI_FONT_16B_1          &GUI_Font16B_1
#define GUI_FONT_20_ASCII       &GUI_Font20_ASCII
#define GUI_FONT_20_1           &GUI_Font20_1
#define GUI_FONT_20B_ASCII      &GUI_Font20B_ASCII
#define GUI_FONT_20B_1          &GUI_Font20B_1
#define GUI_FONT_24_ASCII       &GUI_Font24_ASCII
#define GUI_FONT_24_1           &GUI_Font24_1
#define GUI_FONT_24B_ASCII      &GUI_Font24B_ASCII
#define GUI_FONT_24B_1          &GUI_Font24B_1
#define GUI_FONT_32_ASCII       &GUI_Font32_ASCII
#define GUI_FONT_32_1           &GUI_Font32_1
#define GUI_FONT_32B_ASCII      &GUI_Font32B_ASCII
#define GUI_FONT_32B_1          &GUI_Font32B_1

//
// Proportional fonts, framed
//
#define GUI_FONT_20F_ASCII      &GUI_Font20F_ASCII

//
// Monospaced
//
#define GUI_FONT_4X6            &GUI_Font4x6
#define GUI_FONT_6X8            &GUI_Font6x8
#define GUI_FONT_6X8_ASCII      &GUI_Font6x8_ASCII
#define GUI_FONT_6X8_1          &GUI_Font6x8_1
#define GUI_FONT_6X9            &GUI_Font6x9
#define GUI_FONT_8X8            &GUI_Font8x8
#define GUI_FONT_8X8_ASCII      &GUI_Font8x8_ASCII
#define GUI_FONT_8X8_1          &GUI_Font8x8_1
#define GUI_FONT_8X9            &GUI_Font8x9
#define GUI_FONT_8X10_ASCII     &GUI_Font8x10_ASCII
#define GUI_FONT_8X12_ASCII     &GUI_Font8x12_ASCII
#define GUI_FONT_8X13_ASCII     &GUI_Font8x13_ASCII
#define GUI_FONT_8X13_1         &GUI_Font8x13_1
#define GUI_FONT_8X15B_ASCII    &GUI_Font8x15B_ASCII
#define GUI_FONT_8X15B_1        &GUI_Font8x15B_1
#define GUI_FONT_8X16           &GUI_Font8x16
#define GUI_FONT_8X17           &GUI_Font8x17
#define GUI_FONT_8X18           &GUI_Font8x18
#define GUI_FONT_8X16X1X2       &GUI_Font8x16x1x2
#define GUI_FONT_8X16X2X2       &GUI_Font8x16x2x2
#define GUI_FONT_8X16X3X3       &GUI_Font8x16x3x3
#define GUI_FONT_8X16_ASCII     &GUI_Font8x16_ASCII
#define GUI_FONT_8X16_1         &GUI_Font8x16_1

//
// Digits
//
#define GUI_FONT_D24X32         &GUI_FontD24x32
#define GUI_FONT_D32            &GUI_FontD32
#define GUI_FONT_D36X48         &GUI_FontD36x48
#define GUI_FONT_D48            &GUI_FontD48
#define GUI_FONT_D48X64         &GUI_FontD48x64
#define GUI_FONT_D64            &GUI_FontD64
#define GUI_FONT_D60X80         &GUI_FontD60x80
#define GUI_FONT_D80            &GUI_FontD80

//
// Comic fonts
//
#define GUI_FONT_COMIC18B_ASCII &GUI_FontComic18B_ASCII
#define GUI_FONT_COMIC18B_1     &GUI_FontComic18B_1
#define GUI_FONT_COMIC24B_ASCII &GUI_FontComic24B_ASCII
#define GUI_FONT_COMIC24B_1     &GUI_FontComic24B_1

/*********************************************************************
*
*       Text and drawing modes
*
* These defines come in two flavors: the long version (.._DRAWMODE_..)
* and the short ones (.._DM_..). They are identical, feel free to use
* which ever one you like best.
*/
#define GUI_DRAWMODE_NORMAL  LCD_DRAWMODE_NORMAL
#define GUI_DRAWMODE_XOR     LCD_DRAWMODE_XOR
#define GUI_DRAWMODE_TRANS   LCD_DRAWMODE_TRANS
#define GUI_DRAWMODE_REV     LCD_DRAWMODE_REV
#define GUI_DM_NORMAL        LCD_DRAWMODE_NORMAL
#define GUI_DM_XOR           LCD_DRAWMODE_XOR
#define GUI_DM_TRANS         LCD_DRAWMODE_TRANS
#define GUI_DM_REV           LCD_DRAWMODE_REV

#define GUI_TEXTMODE_NORMAL  LCD_DRAWMODE_NORMAL
#define GUI_TEXTMODE_XOR     LCD_DRAWMODE_XOR
#define GUI_TEXTMODE_TRANS   LCD_DRAWMODE_TRANS
#define GUI_TEXTMODE_REV     LCD_DRAWMODE_REV
#define GUI_TM_NORMAL        LCD_DRAWMODE_NORMAL
#define GUI_TM_XOR           LCD_DRAWMODE_XOR
#define GUI_TM_TRANS         LCD_DRAWMODE_TRANS
#define GUI_TM_REV           LCD_DRAWMODE_REV

/* Text alignment flags, horizontal */
#define GUI_TA_LEFT       (0)
#define GUI_TA_HORIZONTAL (3u)
#define GUI_TA_RIGHT      (1u)
#define GUI_TA_CENTER     (2u)
#define GUI_TA_HCENTER    GUI_TA_CENTER  /* easier to remember :-)  */

/* Text alignment flags, vertical */
#define GUI_TA_TOP        (0)
#define GUI_TA_VERTICAL   (3u << 2)
#define GUI_TA_BOTTOM     (1u << 2)
#define GUI_TA_BASELINE   (2u << 2)
#define GUI_TA_VCENTER    (3u << 2)

/* General alignment flags */
#define GUI_ALIGN_LEFT       GUI_TA_LEFT
#define GUI_ALIGN_HCENTER    GUI_TA_HCENTER
#define GUI_ALIGN_RIGHT      GUI_TA_RIGHT
#define GUI_ALIGN_TOP        GUI_TA_TOP
#define GUI_ALIGN_VCENTER    GUI_TA_VCENTER
#define GUI_ALIGN_BOTTOM     GUI_TA_BOTTOM
#define GUI_ALIGN_HORIZONTAL GUI_TA_HORIZONTAL
#define GUI_ALIGN_VERTICAL   GUI_TA_VERTICAL
#define GUI_ALIGN_CENTER     (GUI_TA_HCENTER | GUI_TA_VCENTER)

/* General orientation flags */
#define GUI_MIRROR_X (1u)
#define GUI_MIRROR_Y (1u << 1)
#define GUI_SWAP_XY  (1u << 2)

#define GUI_ROTATION_0     (0)
#define GUI_ROTATION_CW    (GUI_MIRROR_X | GUI_SWAP_XY)
#define GUI_ROTATION_180   (GUI_MIRROR_X | GUI_MIRROR_Y)
#define GUI_ROTATION_CCW   (GUI_MIRROR_Y | GUI_SWAP_XY)
#define GUI_ROTATION_LEFT  (1u << 3)
#define GUI_ROTATION_RIGHT (1u << 4)

/*********************************************************************
*
*       Min/Max coordinates
*
* Define minimum and maximum coordinates in x and y
*/
#define GUI_XMIN -16383
#define GUI_XMAX  16383
#define GUI_YMIN -16383
#define GUI_YMAX  16383

/*********************************************************************
*
*       Defines for constants
*/
#define ________ 0x0
#define _______X 0x1
#define ______X_ 0x2
#define ______XX 0x3
#define _____X__ 0x4
#define _____X_X 0x5
#define _____XX_ 0x6
#define _____XXX 0x7
#define ____X___ 0x8
#define ____X__X 0x9
#define ____X_X_ 0xa
#define ____X_XX 0xb
#define ____XX__ 0xc
#define ____XX_X 0xd
#define ____XXX_ 0xe
#define ____XXXX 0xf
#define ___X____ 0x10
#define ___X___X 0x11
#define ___X__X_ 0x12
#define ___X__XX 0x13
#define ___X_X__ 0x14
#define ___X_X_X 0x15
#define ___X_XX_ 0x16
#define ___X_XXX 0x17
#define ___XX___ 0x18
#define ___XX__X 0x19
#define ___XX_X_ 0x1a
#define ___XX_XX 0x1b
#define ___XXX__ 0x1c
#define ___XXX_X 0x1d
#define ___XXXX_ 0x1e
#define ___XXXXX 0x1f
#define __X_____ 0x20
#define __X____X 0x21
#define __X___X_ 0x22
#define __X___XX 0x23
#define __X__X__ 0x24
#define __X__X_X 0x25
#define __X__XX_ 0x26
#define __X__XXX 0x27
#define __X_X___ 0x28
#define __X_X__X 0x29
#define __X_X_X_ 0x2a
#define __X_X_XX 0x2b
#define __X_XX__ 0x2c
#define __X_XX_X 0x2d
#define __X_XXX_ 0x2e
#define __X_XXXX 0x2f
#define __XX____ 0x30
#define __XX___X 0x31
#define __XX__X_ 0x32
#define __XX__XX 0x33
#define __XX_X__ 0x34
#define __XX_X_X 0x35
#define __XX_XX_ 0x36
#define __XX_XXX 0x37
#define __XXX___ 0x38
#define __XXX__X 0x39
#define __XXX_X_ 0x3a
#define __XXX_XX 0x3b
#define __XXXX__ 0x3c
#define __XXXX_X 0x3d
#define __XXXXX_ 0x3e
#define __XXXXXX 0x3f
#define _X______ 0x40
#define _X_____X 0x41
#define _X____X_ 0x42
#define _X____XX 0x43
#define _X___X__ 0x44
#define _X___X_X 0x45
#define _X___XX_ 0x46
#define _X___XXX 0x47
#define _X__X___ 0x48
#define _X__X__X 0x49
#define _X__X_X_ 0x4a
#define _X__X_XX 0x4b
#define _X__XX__ 0x4c
#define _X__XX_X 0x4d
#define _X__XXX_ 0x4e
#define _X__XXXX 0x4f
#define _X_X____ 0x50
#define _X_X___X 0x51
#define _X_X__X_ 0x52
#define _X_X__XX 0x53
#define _X_X_X__ 0x54
#define _X_X_X_X 0x55
#define _X_X_XX_ 0x56
#define _X_X_XXX 0x57
#define _X_XX___ 0x58
#define _X_XX__X 0x59
#define _X_XX_X_ 0x5a
#define _X_XX_XX 0x5b
#define _X_XXX__ 0x5c
#define _X_XXX_X 0x5d
#define _X_XXXX_ 0x5e
#define _X_XXXXX 0x5f
#define _XX_____ 0x60
#define _XX____X 0x61
#define _XX___X_ 0x62
#define _XX___XX 0x63
#define _XX__X__ 0x64
#define _XX__X_X 0x65
#define _XX__XX_ 0x66
#define _XX__XXX 0x67
#define _XX_X___ 0x68
#define _XX_X__X 0x69
#define _XX_X_X_ 0x6a
#define _XX_X_XX 0x6b
#define _XX_XX__ 0x6c
#define _XX_XX_X 0x6d
#define _XX_XXX_ 0x6e
#define _XX_XXXX 0x6f
#define _XXX____ 0x70
#define _XXX___X 0x71
#define _XXX__X_ 0x72
#define _XXX__XX 0x73
#define _XXX_X__ 0x74
#define _XXX_X_X 0x75
#define _XXX_XX_ 0x76
#define _XXX_XXX 0x77
#define _XXXX___ 0x78
#define _XXXX__X 0x79
#define _XXXX_X_ 0x7a
#define _XXXX_XX 0x7b
#define _XXXXX__ 0x7c
#define _XXXXX_X 0x7d
#define _XXXXXX_ 0x7e
#define _XXXXXXX 0x7f
#define X_______ 0x80
#define X______X 0x81
#define X_____X_ 0x82
#define X_____XX 0x83
#define X____X__ 0x84
#define X____X_X 0x85
#define X____XX_ 0x86
#define X____XXX 0x87
#define X___X___ 0x88
#define X___X__X 0x89
#define X___X_X_ 0x8a
#define X___X_XX 0x8b
#define X___XX__ 0x8c
#define X___XX_X 0x8d
#define X___XXX_ 0x8e
#define X___XXXX 0x8f
#define X__X____ 0x90
#define X__X___X 0x91
#define X__X__X_ 0x92
#define X__X__XX 0x93
#define X__X_X__ 0x94
#define X__X_X_X 0x95
#define X__X_XX_ 0x96
#define X__X_XXX 0x97
#define X__XX___ 0x98
#define X__XX__X 0x99
#define X__XX_X_ 0x9a
#define X__XX_XX 0x9b
#define X__XXX__ 0x9c
#define X__XXX_X 0x9d
#define X__XXXX_ 0x9e
#define X__XXXXX 0x9f
#define X_X_____ 0xa0
#define X_X____X 0xa1
#define X_X___X_ 0xa2
#define X_X___XX 0xa3
#define X_X__X__ 0xa4
#define X_X__X_X 0xa5
#define X_X__XX_ 0xa6
#define X_X__XXX 0xa7
#define X_X_X___ 0xa8
#define X_X_X__X 0xa9
#define X_X_X_X_ 0xaa
#define X_X_X_XX 0xab
#define X_X_XX__ 0xac
#define X_X_XX_X 0xad
#define X_X_XXX_ 0xae
#define X_X_XXXX 0xaf
#define X_XX____ 0xb0
#define X_XX___X 0xb1
#define X_XX__X_ 0xb2
#define X_XX__XX 0xb3
#define X_XX_X__ 0xb4
#define X_XX_X_X 0xb5
#define X_XX_XX_ 0xb6
#define X_XX_XXX 0xb7
#define X_XXX___ 0xb8
#define X_XXX__X 0xb9
#define X_XXX_X_ 0xba
#define X_XXX_XX 0xbb
#define X_XXXX__ 0xbc
#define X_XXXX_X 0xbd
#define X_XXXXX_ 0xbe
#define X_XXXXXX 0xbf
#define XX______ 0xc0
#define XX_____X 0xc1
#define XX____X_ 0xc2
#define XX____XX 0xc3
#define XX___X__ 0xc4
#define XX___X_X 0xc5
#define XX___XX_ 0xc6
#define XX___XXX 0xc7
#define XX__X___ 0xc8
#define XX__X__X 0xc9
#define XX__X_X_ 0xca
#define XX__X_XX 0xcb
#define XX__XX__ 0xcc
#define XX__XX_X 0xcd
#define XX__XXX_ 0xce
#define XX__XXXX 0xcf
#define XX_X____ 0xd0
#define XX_X___X 0xd1
#define XX_X__X_ 0xd2
#define XX_X__XX 0xd3
#define XX_X_X__ 0xd4
#define XX_X_X_X 0xd5
#define XX_X_XX_ 0xd6
#define XX_X_XXX 0xd7
#define XX_XX___ 0xd8
#define XX_XX__X 0xd9
#define XX_XX_X_ 0xda
#define XX_XX_XX 0xdb
#define XX_XXX__ 0xdc
#define XX_XXX_X 0xdd
#define XX_XXXX_ 0xde
#define XX_XXXXX 0xdf
#define XXX_____ 0xe0
#define XXX____X 0xe1
#define XXX___X_ 0xe2
#define XXX___XX 0xe3
#define XXX__X__ 0xe4
#define XXX__X_X 0xe5
#define XXX__XX_ 0xe6
#define XXX__XXX 0xe7
#define XXX_X___ 0xe8
#define XXX_X__X 0xe9
#define XXX_X_X_ 0xea
#define XXX_X_XX 0xeb
#define XXX_XX__ 0xec
#define XXX_XX_X 0xed
#define XXX_XXX_ 0xee
#define XXX_XXXX 0xef
#define XXXX____ 0xf0
#define XXXX___X 0xf1
#define XXXX__X_ 0xf2
#define XXXX__XX 0xf3
#define XXXX_X__ 0xf4
#define XXXX_X_X 0xf5
#define XXXX_XX_ 0xf6
#define XXXX_XXX 0xf7
#define XXXXX___ 0xf8
#define XXXXX__X 0xf9
#define XXXXX_X_ 0xfa
#define XXXXX_XX 0xfb
#define XXXXXX__ 0xfc
#define XXXXXX_X 0xfd
#define XXXXXXX_ 0xfe
#define XXXXXXXX 0xff

/*********************************************************************
*
*       Compatibility with older versions
*/
#define GUI_DispString_UC      GUI_UC_DispString
#define TOUCH_X_ActivateX      GUI_TOUCH_X_ActivateX
#define TOUCH_X_ActivateY      GUI_TOUCH_X_ActivateY
#define TOUCH_X_Disable        GUI_TOUCH_X_Disable
#define TOUCH_X_MeasureX       GUI_TOUCH_X_MeasureX
#define TOUCH_X_MeasureY       GUI_TOUCH_X_MeasureY
#define GUI_SelLayer           GUI_SelectLayer
#define GUI_MEMDEV_FadeDevices GUI_MEMDEV_FadeInDevices
#if defined(__cplusplus)
}
#endif

#endif   /* ifdef GUI_H */

/*************************** End of file ****************************/
