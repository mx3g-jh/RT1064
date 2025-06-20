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
File        : GUI_JPEG_Private.h
Purpose     : Private header
----------------------------------------------------------------------

Explanation of terms:
  
  Component   - Color channel, e.g., Red or Luminance.
  Sample      - Single component value (i.e., one number in the image data).
  Coefficient - Frequency coefficient (a DCT transform output number).
  Block       - 8x8 group of samples or coefficients.
  MCU         - (Minimum Coded Unit) is an interleaved set of blocks of size
                determined by the sampling factors, or a single block in a
                noninterleaved scan.

---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_JPEG_PRIVATE_H
#define GUI_JPEG_PRIVATE_H

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/
#define HUFF_TABLE_BITS 10   // 8 was old value. 10 seems best from a performance perspective

#ifndef   GUI_JPEG_USE_CC_TABLE
  #define GUI_JPEG_USE_CC_TABLE    0
#endif
#ifndef   GUI_JPEG_USE_LIMIT_TABLE
  #define GUI_JPEG_USE_LIMIT_TABLE 0
#endif
#ifndef   GUI_JPEG_USE_WORD_WRITE
#define   GUI_JPEG_USE_WORD_WRITE  1
#endif
#ifndef   GUI_JPEG_USE_HELIUM_H2V2
  #define GUI_JPEG_USE_HELIUM_H2V2 0
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define JPEG_LOCK_H(h) (GUI_JPEG_DCONTEXT *)GUI_LOCK_H(h)

//
// Function inlining
//
#ifndef   GUI_ALWAYS_INLINE
  #include "SEGGER.h" // For INLINE define.
  #define GUI_ALWAYS_INLINE  INLINE
#endif
#ifndef   GUI_NEVER_INLINE
#define   GUI_NEVER_INLINE
#endif
//
// Macros for color conversion
//
#if defined(GUI_SAT_INT_TO_U8)
#define ASSIGN_SATURATED(x, v)  x = (U8)GUI_SAT_INT_TO_U8(v)
#undef  GUI_JPEG_USE_LIMIT_TABLE
#define GUI_JPEG_USE_LIMIT_TABLE 0
#elif GUI_JPEG_USE_LIMIT_TABLE
#define ASSIGN_SATURATED(x, v)  x = _aLimit[v + 0x100]
#else
#define ASSIGN_SATURATED(x, v) \
    do {                         \
      int s = v;                 \
      if (s > 255) { s = 255; }  \
      if (s < 0) { s = 0; }      \
      x = s;                     \
    } while (0)
#endif
//
// Macros for IDCT
//
#define DESCALE(x, n)  (((x) + (((I32)1) << ((n) - 1))) >> (n))
#define MULTIPLY(v, c) ((v) * (c))
//
// Constants for IDCT
//
#define CONST_BITS 13
#define PASS1_BITS  2

/*********************************************************************
*
*       Scan types
*/
#define GRAYSCALE 0
#define YH1V1     1
#define YH2V1     2
#define YH1V2     3
#define YH2V2     4

/*********************************************************************
*
*       Maximum number of...
*/
#define MAX_COMPONENTS  4
#define MAX_HUFFTABLES  8
#define MAX_QUANTTABLES 4
#define MAX_COMPSINSCAN 4
#define MAX_BLOCKSPERMCU 10

/*********************************************************************
*
*       Marker definitions
*/
#define M_SOF0  0xc0 // Start Of Frame, non-differential, Huffman coding, Baseline DCT
#define M_SOF1  0xc1
#define M_SOF2  0xc2
#define M_SOF3  0xc3

#define M_SOF5  0xc5
#define M_SOF6  0xc6
#define M_SOF7  0xc7

#define M_JPG   0xc8
#define M_SOF9  0xc9
#define M_SOF10 0xca
#define M_SOF11 0xcb

#define M_SOF13 0xcd
#define M_SOF14 0xce
#define M_SOF15 0xcf

#define M_DHT   0xc4 // Define Huffman Table

#define M_DAC   0xcc // Define Arithmetic Coding Table

#define M_RST0  0xd0
#define M_RST1  0xd1
#define M_RST2  0xd2
#define M_RST3  0xd3
#define M_RST4  0xd4
#define M_RST5  0xd5
#define M_RST6  0xd6
#define M_RST7  0xd7

#define M_SOI   0xd8 // Start Of Image
#define M_EOI   0xd9
#define M_SOS   0xda // Start Of Scan
#define M_DQT   0xdb // Define Quantisation Table
#define M_DRI   0xdd // Define Restart Interval

#define M_APP0  0xe0 // Application Usage
#define M_APP14 0xee // Adobe marker

#define M_TEM   0x01

#define ADOBE_TRANSFORM_RGB   0
#define ADOBE_TRANSFORM_YCBCR 1

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// Default parameter structure for reading data from memory
//
typedef struct {
  const U8 * pFileData;
  I32   FileSize;
} GUI_JPEG_PARAM;

//
// Huffman table definition
//
typedef struct {
  U8    aCodeSize[256];                  // Size of symbol: How many bits are needed to (en/de)code it
  I16   aLookUp[1 << HUFF_TABLE_BITS];   // Quick lookup so we do not need to go through tree for first 7-9 bits
  I16   aTree[512];
} HUFF_TABLE;

//
// Coefficient buffer used for progressive JPEGs
//
typedef struct {
  int NumBlocksX;
  int NumBlocksY;
  unsigned BlockSize;
  GUI_HMEM hData;
} COEFF_BUFFER;

typedef struct {
  //
  // Function pointer for reading one byte
  //
  int                   (* pfGetU8)(GUI_JPEG_DCONTEXT * pContext, U8 * pByte);
  GUI_JPEG_GET_DATA_FUNC * pfGetData; // 'GetData' Function pointer
  void * pParam;                 // Pointer passed to 'GetData' function
  U32    Off;                    // Data pointer
  //
  // Image size
  //
  U16 xSize;
  U16 ySize;
  //
  // Input buffer
  //
  const U8 * pBuffer;
  unsigned   NumBytesInBuffer;
  U8         StartOfFile;
  U8         IsProgressive;    // Flag is set to 1 if JPEG is progressive
  unsigned PushBack;            // Push-back buffer, up to 2 bytes pushed back
} GUI_JPEG_GETINFO;

struct GUI_JPEG_DCONTEXT {
  GUI_JPEG_GETINFO Info;
  //
  // Bit buffer
  //
  U32 BitBuffer;
  int NumBitsLeft;
  //
  // Common
  //
  U8 TransformationRequired;
  U8 ScanType;                  // Gray, Yh1v1, Yh1v2, Yh2v1, Yh2v2
  int MaxMCUsPerRow;            // Maximum number of MCUs per row
  int MaxMCUsPerCol;            // Maximum number of MCUs per column
  int MaxBlocksPerMCU;          // Maximum number of blocks per MCU
  int MaxBlocksPerRow;          // Maximum number of blocks per row
  int MaxMCU_xSize;             // MCU's max. X size in pixels
  int MaxMCU_ySize;             // MCU's max. Y size in pixels
  int DestBytesPerPixel;        // 4 (RGB) or 1 (Y)
  int DestBytesPerScanline;     // Rounded up
  int RealDestBytesPerScanline; // Actual bytes
  int EOB_Run;                  // 'End Of Band' run
  int RestartInterval;
  int RestartsLeft;
  int NextRestartNum;
  int MCUsPerRow;
  int MCUsPerCol;
  int NumBlocksPerMCU;
  int aMCU_Org[MAX_BLOCKSPERMCU];
  //
  // Block buffer
  //
  GUI_HMEM hBlocks;
  GUI_HMEM hBlockMaxZagSet;
  //
  // Sample buffer
  //
  GUI_HMEM hSampleBuf;
  U8     * pSampleBuf;
  //
  // Status
  //
  int TotalLinesLeft; // Total number of lines left in image
  int MCULinesLeft;   // Total number of lines left in current MCU
  //
  // Output buffer(s)
  //
  GUI_HMEM hScanLine0; // Buffer 0
  GUI_HMEM hScanLine1; // Buffer 1, only used for V2 sampling factors
  U8 BufferIndex;
  //
  // Huffman tables
  //
  U8 aHuffNumTableAvail[MAX_HUFFTABLES];
  U8 aaHuffNum[MAX_HUFFTABLES][17];   // Pointer to number of Huffman codes per bit size
  U8 aaHuffVal[MAX_HUFFTABLES][256];  // Pointer to Huffman codes
  HUFF_TABLE aHuffTable[MAX_HUFFTABLES];
  HUFF_TABLE * apDC_Huff[MAX_BLOCKSPERMCU];
  HUFF_TABLE * apAC_Huff[MAX_BLOCKSPERMCU];
  //
  // Quantization tables
  //
  U16 aaQuantTbl[MAX_QUANTTABLES][64];
  U16 * apQuantTbl[MAX_QUANTTABLES];
  //
  // Component information
  //
  U8 NumCompsPerFrame;                      // Number of components per frame
  U8 aCompHSamp[MAX_COMPONENTS];            // Component's horizontal sampling factor
  U8 aCompVSamp[MAX_COMPONENTS];            // Component's vertical sampling factor
  U8 aCompQuant[MAX_COMPONENTS];            // Component's quantization table selector
  U8 aCompId   [MAX_COMPONENTS];            // Component's ID
  U8 NumCompsPerScan;                       // Number of components per scan
  U8 aCompList[MAX_COMPSINSCAN];            // Components in this scan
  U8 aCompDC_Tab[MAX_COMPONENTS];           // Component's DC Huffman coding table selector
  U8 aCompAC_Tab[MAX_COMPONENTS];           // Component's AC Huffman coding table selector
  unsigned * apComponent[MAX_BLOCKSPERMCU]; // Points into the table aLastDC_Val[]
  unsigned   aLastDC_Val[MAX_COMPONENTS];   // Table of last DC values
  //
  // Data used for progressive scans
  //
  U8 SpectralStart;                        // Spectral selection start
  U8 SpectralEnd;                          // Spectral selection end
  U8 SuccessiveLow;                        // Successive approximation low
  U8 SuccessiveHigh;                       // Successive approximation high
  COEFF_BUFFER aDC_Coeffs[MAX_COMPONENTS]; // DC coefficient buffer for progressive scan
  COEFF_BUFFER aAC_Coeffs[MAX_COMPONENTS]; // AC coefficient buffer for progressive scan
  int aBlockY_MCU[MAX_COMPONENTS];         // 
};

/*********************************************************************
*
*       Private interface
*
**********************************************************************
*/
int       GUI_JPEG__DecodeLine              (GUI_JPEG_DCONTEXT * pContext);
void      GUI_JPEG__Free                    (GUI_JPEG_DCONTEXT * pContext);
GUI_COLOR GUI_JPEG__GetColorGray            (const U8 ** ppData, unsigned SkipCnt);
GUI_COLOR GUI_JPEG__GetColorRGB             (const U8 ** ppData, unsigned SkipCnt);
int       GUI_JPEG__GetData                 (void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off);
int       GUI_JPEG__InitDraw                (GUI_HMEM hContext);
int       GUI_JPEG__ReadInfo                (GUI_HMEM hContext);
int       GUI_JPEG__ReadUntilSOF            (GUI_HMEM hContext);
int       GUI_JPEG__SkipLine                (GUI_JPEG_DCONTEXT * pContext);
int       GUI_JPEG__TransformRow            (GUI_JPEG_DCONTEXT * pContext);
int       GUI_JPEG__GetInfoEx               (GUI_HMEM hContext, GUI_JPEG_INFO * pInfo);
//
extern const U8 GUI_JPEG__aCoeffOrder[];

#endif

/*************************** End of file ****************************/
