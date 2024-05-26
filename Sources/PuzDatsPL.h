#include <intuition/intuition.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/dos.h>


/********************
*	GLOBAL VARIABLES
*********************/

/*
 * definicje identyfikatorow dla gadgetow
 */
#define	XYPropID 0x0001
#define SlideID		0x0002
#define ShowPicID 0x0003
#define ScrollID	0x0004
#define PauzeID		0x0005

#define	UseID 		0x0006
#define CancelID	0x0007

#define TheEndID 50
#define SetUpID 51

#define NewGameID 61
#define QuitID 		62

/************************************************************************/

#define	HOW_MANY_PICS 100
char *NamesOfPics[HOW_MANY_PICS];

CPTR ChipAdr[HOW_MANY_PICS];
ULONG *ExtraChip;

struct BitMap DataOfBitMaps[HOW_MANY_PICS];
/************************************************************************/
#define RED(red)			((red)>>0x08)&0x00f
#define GREEN(green) ((green)>>0x04)&0x00f
#define BLUE(blue)		(blue)&0x00f

#define WIDTH	340 /* 320 pixels wide (low resolution)				 */
#define HEIGHT 266 /* 256 lines high (non interlaced PAL display) */
#define DEPTH	  5 /* 5 BitPlanes should be used, gives 32 colours. */
#define COLOURS 32 /* 2^5 = 32												  */

/* ponizsze stale dotycza ladowanych obrazkow */
#define	SizeX 176
#define	SizeY 204
#define	Depth 4
#define Colours 16

/* ile jest wszystkich kawalkow */
/* AmountOfCovers = AmtHor * AmtVer */
#define AmountOfCovers 12

/* ilosc kawalkow odpowiednio w poziomie i w pionie */
#define AmtVer 4
#define AmtHor 3

/* dlugosci odstepow miedzy kawalkami na ekranie */
#define XSpaceBetween 3
#define YSpaceBetween 3

/* wymiary kawalka obrazka */
#define PieceWidth 56		/* PictureWidth  / 3 */
#define PieceHeight 51		/* PictureHeight / 3 */

#define PictureWidth 168
#define PictureHeight 204
/************************************************************************/
#define Xtrans 6
#define Ytrans 18

#define Xchoose 279
#define Ychoose 126

#define Xnext 204
#define Ynext 126

#define Xfirst 258
#define Yfirst 202

#define RP Window->RPort

struct Screen *Screen;
struct Window *Window;
struct Window *StartWdw;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct BitMap my_bit_map, SourceBitMap;
struct IntuiMessage *message = NULL;

/************************************************************************/
#define Xgad 8
#define Ygad 10

extern UWORD NonPicsColors[32];

extern struct Image MarbleImg;
extern struct Image GadgetSelImage;
extern struct Image GadgetImage;
extern struct Image Left_img;
extern struct Image Down_img;
extern struct Image Right_img;
extern struct Image Up_img;
extern struct Image TheEndsel_img;
extern struct Image TheEndnor_img;
extern struct Image SetUpsel_img;
extern struct Image SetUpnor_img;



extern struct Image StartLogo_img;

struct TextAttr AFont={
	(UBYTE *)"topaz.font",
	8,0,FPF_ROMFONT};

struct NewScreen ekran={
	0,0,WIDTH,HEIGHT,DEPTH,0,1,0,WBENCHSCREEN,
	&AFont,(UBYTE *)"Puzzland v2.0",0,0};

struct NewWindow okno={
	0,0,WIDTH,HEIGHT,0,1,GADGETUP | MOUSEBUTTONS,
	ACTIVATE|BACKDROP|BORDERLESS|RMBTRAP, /* Flags */
	NULL,0,0,0,0,
	0,0,0,0,CUSTOMSCREEN};

struct NewWindow StartNewWdw={
	68,78,204,110,0,0,NULL, ACTIVATE|BORDERLESS|RMBTRAP, /* Flags */
	NULL,0,0,0,0,0,0,0,0,CUSTOMSCREEN};



/*
 * kolory do rysowania tekstow w okienku config
 */
#define BackTextColor 15
#define FrontTextColor 9

#define BackColour 15
#define FrontColour 9

/************************************************************************/


struct timerequest *TimerRequest;
struct timeval StartTime;
struct timeval StopTime;
struct timeval GameStartTime;
struct MsgPort *TimerPort = NULL;
struct Library *TimerBase;

short OpenTimer = NULL;



/*
 * structury opisujacae gadgety SetUP i TheEnd
 */
struct Gadget SetUp_gad =
{
	NULL, 197, 88, 134, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &SetUpnor_img,
	(APTR) &SetUpsel_img, NULL, NULL, NULL, SetUpID, NULL
};
struct Gadget TheEnd_gad=
{
	NULL, 197, 67, 134, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &TheEndnor_img,
	(APTR) &TheEndsel_img, NULL, NULL, NULL, TheEndID, NULL
};


/************************************************************************/




/*----------------------------------------------------------------------*/


UWORD NonPicsColors[32] =
{
/* 		0xAAA,0x000,0xFFF,0x6B8,0x888,0x222,0xCCC,0xFFF, */
		0x088,0x0CC,0x0FF,0x7FF,0x888,0x333,0xCCC,0xFFF,
		0xEEE,0xCCC,0xAAA,0x888,0x666,0x444,0x222,0x000,

		0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,
		0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,0xCCC,0xCCC
};
/************************************************************************/


/*  w tablicy PicsColors znajduja sie kolory obrazkow  */
/* 	tylko 'gorne' 16 kolorow */
UWORD PicsColors[HOW_MANY_PICS][16];
/************************************************************************/


/*
 * kolory do rysowania tekstow w okienku config
 */
#define BackTextColor 15
#define FrontTextColor 9

char ScoreLine[20] = {"Punkty:"};

struct IntuiText Score_txt=
{
  BackTextColor, 0, JAM1, 0, 0, &AFont,
  ScoreLine, NULL,
};

BYTE HowManyPics = 0; /* zmienna zwiekszana przez ReadDir() - ilosc obrzkow */

/*
 * zmienne dla nowego loadera obrazkow;
 */

BOOL SmallMem = FALSE; /* znacznik dla GameOver() - aktywny gadget "Nowe obrazki" */

SHORT HowManyLoad = 0; /*zmienna dla LoadPics() ile zaladowac */

SHORT PicsPack = 0; /*ilosc zestawow obrazkow */

#define ONE_PACK 5 /* minimalna ilosc obrazkow w Pack'u (zestawie) */


long random_variable;
int CoverPiecesVec[AmountOfCovers-1];
int HowManyCovers;
int WhichPic;
struct Gadget GadgetsArray[AmountOfCovers];

/*----------------------------------------------------------------------*/

#define SlideMax 25
#define SlideMin 2
/*---------------*/
#define ShowMax 250
#define ShowMin 25
/*---------------*/
/* dane juz wstepnie przemnozone przez 1000 stad 0.9999[s] = 9999 */
#define ScrollMax 999
#define ScrollMin 1
/*---------------*/
#define PauzeMax 240
#define PauzeMin 15

extern struct Configuration
{
	UWORD SlideSpeed;
	UWORD ShowPic;
	LONG	ScrollSpeed;
	UWORD PauzeTime;
} PuzzleConfig;

