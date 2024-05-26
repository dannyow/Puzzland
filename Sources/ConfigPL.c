#include <intuition/intuition.h>
#include <stdio.h>

#define Xgad 8
#define Ygad 10
/*
 * definicje identyfikatorow dla gadgetow 
 */
#define	XYPropID	0x0001
#define SlideID		0x0002
#define ShowPicID 0x0003
#define ScrollID	0x0004
#define PauzeID		0x0005

#define	UseID			0x0006
#define CancelID	0x0007
#define SaveID		0x0008
/*
 * struktury image wraz z danymi znajduja sie w pliku Gads.c
 * kompilacja tego pliku lc Gads,
 * linkowanie z tym plikiem:
 * Lc -L+ram:Gads.o This.c
 */
extern UWORD NonPicsColors[32];
extern struct Image Cancel_gad;
extern struct Image Cancel_sel;
extern struct Image Use_gad;
extern struct Image Use_sel;
extern struct Image Save_gad;
extern struct Image Save_sel;
extern struct Image Suwak1;
extern struct Image Suwak2;
extern struct Image Suwak3;
extern struct Image Suwak4;
extern struct Image ConfigLogo_img;
extern struct Image Min_img;
extern struct Image Max_img;

extern struct Screen *Screen;
extern struct Window *Window;
extern struct TextAttr AFont;
extern struct IntuiText DummyIT;
extern void PrintITShadow( struct RastPort *UserRPort, struct IntuiText *ITptr, 
										short LeftEdge, short TopEdge, char *Message );

#ifndef HOW_MANY_PICS
#define	HOW_MANY_PICS 20
#endif	

extern UWORD PicsColors[HOW_MANY_PICS][16];
extern int WhichPic;

/************************************************************************/

struct Window *ConfigWdw;

#define ConfigRP ConfigWdw->RPort

#define SlideMax 25 
#define SlideMin 2
/*---------------*/
#define ShowMax 250
#define ShowMin 25
/*---------------*/
/* dane juz wstepnie przemnozone przez 100000 stad 0.9999[s] = 9999 */
#define ScrollMax 999
#define ScrollMin 1
/*---------------*/
#define PauzeMax 240
#define PauzeMin 15
#define ConfigPath "S:Puzzle.config"

FILE *ConfigFile;

struct Configuration
{
	UWORD SlideSpeed;
	UWORD ShowPic;
	LONG  ScrollSpeed; 
	UWORD PauzeTime;
} PuzzleConfig = {5, 100, 700, 20};

void Configuration();
/************************************************************************/

void ReadConfig()
{

	if( !(ConfigFile = fopen( ConfigPath, "r" )) )
	{
		Delay( 20 );
 		DisplayBeep( Screen );
		return;
	}

	if( !(fscanf(ConfigFile, "%hu_%hu_%ld_%hu",
														&PuzzleConfig.SlideSpeed,
														&PuzzleConfig.ShowPic,
														&PuzzleConfig.ScrollSpeed,
														&PuzzleConfig.PauzeTime )) )
		{
					Delay( 20 );
					DisplayBeep( Screen );
		}

		if(ConfigFile) fclose(ConfigFile);
		return;

}

BOOL WriteConfig()
{
	BOOL Error = FALSE;

	if( !(ConfigFile = fopen( ConfigPath, "w" )) )
		return(FALSE);

	if( !(fprintf(ConfigFile,"%u_%u_%ld_%u",
															PuzzleConfig.SlideSpeed,
															PuzzleConfig.ShowPic,
															PuzzleConfig.ScrollSpeed,
															PuzzleConfig.PauzeTime )) )
		{
					Delay( 20 );
 					DisplayBeep( Screen );
					Error = TRUE;
		}

		if(ConfigFile) fclose(ConfigFile);
		if(Error) 	return( FALSE );
		else
			return( TRUE );

}

/************************************************************************/


/*
 * KONFIGURACJA
 */
UWORD chip XYProp_data[] =
	{
		0xFFFF,0x0000,0xFFFE,0x8000,0xE499,0x8000,0xF889,0x8000,0xC881,0x8000,
		0xC88F,0x8000,0xCE7B,0x8000,0xD941,0x8000,0xE4C1,0x8000,0xC50B,0x8000,
		0xC41B,0x8000,0xC833,0x8000,0xF1E3,0x8000,0xC28F,0x8000,0xC489,0x8000,
		0xBFFF,0x8000,0x7FFF,0x8000,0xFFFF,0x8000,0xFFFF,0x0000,0xEED8,0x0000,
		0xFDBA,0x0000,0xEBBA,0x0000,0xDBBE,0x0000,0xFFFA,0x0000,0xF9C2,0x0000,
		0xF7DE,0x0000,0xED9E,0x0000,0xDD7A,0x0000,0xFDF2,0x0000,0xFBE6,0x0000,
		0xE78E,0x0000,0xDEB8,0x0000,0xC000,0x0000,0x8000,0x0000
  };

struct Image XYProp_image =
	{
		0,0,17,17,2,
		&XYProp_data[0],
		0x03,0x04,NULL
	};

struct PropInfo XYProp_info=
{
  FREEHORIZ|FREEVERT, 0, 0,
  MAXBODY * 1/31, MAXBODY * 1/31, 

  /* These variables are initialized and maintained by Intuition: */

  0,              /* CWidth */
  0,              /* CHeight */
  0, 0,           /* HPotRes, VPotRes */
  0,              /* LeftBorder */
  0               /* TopBorder */
};
/************************************************************************/


struct Gadget XYPropGad=
{
  NULL,            /* NextGadget, no more gadgets in the list. */
  Xgad,              /* LeftEdge, 10 pixels out. */
  Ygad,              /* TopEdge, 20 lines down. */
  132,             /* Width, always 150 pixels less than the wind. size. */
  70,             /* Height, always 70 lines less than the wind. size. */
  GADGHNONE,       /* Flags, complement the colours. */
  GADGIMMEDIATE|   /* Activation, our program will recieve a message */
  RELVERIFY,       /* when the user has selected this gadget, and when */
                   /* the user has released it. */
  PROPGADGET,      /* GadgetType, a Proportional gadget. */
  (APTR) &XYProp_image,/* GadgetRender, a pointer to our Image structure. */
                   /* (Intuition will take care of the knob image) */
                   /* (See chapter 3 GRAPHICS for more information) */
  NULL,            /* SelectRender, NULL since we do not supply the */
                   /* gadget with an alternative image. */
  NULL,            /* GadgetText, no text. */
  NULL,            /* MutualExclude, no mutual exclude. */
  (APTR) &XYProp_info, /* SpecialInfo, pointer to a PropInfo structure. */
  XYPropID,        /* GadgetID */
  NULL             /* UserData, no user data connected to the gadget. */
};
/*
 * gadgety proporcionalne poziome
 */				

/*----------------------------------------------------------------------*/
/* Slideshow speed: */
struct PropInfo Slide_info=
{
  FREEHORIZ, 0, 0, 	0x400	, 0, /* MAXBODY * 1/64*/
  /* These variables are initialized and maintained by Intuition: */
  0, 0, 0, 0, 0, 0
};
struct Gadget Slide_gad=
{
  &XYPropGad, 122, 95, 127, 8,
  GADGHNONE, GADGIMMEDIATE|RELVERIFY, PROPGADGET,
	(APTR) &Suwak1, NULL, NULL, NULL,
  (APTR) &Slide_info, SlideID, NULL
};

/*----------------------------------------------------------------------*/
/* ShowPic Time: */
struct PropInfo ShowPic_info=
{
  FREEHORIZ, 0, 0, 	0x400	, 0, /* MAXBODY * 1/64*/
  /* These variables are initialized and maintained by Intuition: */
  0, 0, 0, 0, 0, 0
};
struct Gadget ShowPic_gad=
{
  &Slide_gad, 122, 117, 127, 8,
  GADGHNONE, GADGIMMEDIATE|RELVERIFY, PROPGADGET,
	(APTR) &Suwak2, NULL, NULL, NULL,
  (APTR) &ShowPic_info, ShowPicID, NULL
};

/*----------------------------------------------------------------------*/
/* Scroll Speed Time: */
struct PropInfo Scroll_info=
{
  FREEHORIZ, 0, 0, 	0x400	, 0, /* MAXBODY * 1/64*/
  /* These variables are initialized and maintained by Intuition: */
  0, 0, 0, 0, 0, 0
};
struct Gadget Scroll_gad=
{
  &ShowPic_gad, 122, 139, 127, 8,
  GADGHNONE, GADGIMMEDIATE|RELVERIFY, PROPGADGET,
	(APTR) &Suwak3, NULL, NULL, NULL,
  (APTR) &Scroll_info, ScrollID, NULL
};

/*----------------------------------------------------------------------*/
/* Pauze Time: */
struct PropInfo Pauze_info=
{
  FREEHORIZ, 0, 0, 	0x400	, 0, /* MAXBODY * 1/64*/
  /* These variables are initialized and maintained by Intuition: */
  0, 0, 0, 0, 0, 0
};
struct Gadget Pauze_gad=
{
  &Scroll_gad, 122, 161, 127, 8,
  GADGHNONE, GADGIMMEDIATE|RELVERIFY, PROPGADGET,
	(APTR) &Suwak4, NULL, NULL, NULL,
  (APTR) &Pauze_info, PauzeID, NULL
};

/*----------------------------------------------------------------------*/
/*
 * struktury dla boolgadgets Use i Cancel
 */
struct Gadget Save =
{
	&Pauze_gad, 8, 171, 88, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &Save_gad,
	(APTR) &Save_sel, NULL, NULL, NULL, SaveID, NULL
};

struct Gadget Use =
{
	&Save, 96, 171, 88, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &Use_gad,
	(APTR) &Use_sel, NULL, NULL, NULL, UseID, NULL
};

struct Gadget Cancel =
{
	&Use, 184, 171, 88, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &Cancel_gad,
	(APTR) &Cancel_sel, NULL, NULL, NULL, CancelID, NULL
};


struct NewWindow ConfigNewWdw={
	30,33,280,200,0,0,GADGETUP|GADGETDOWN,
	ACTIVATE|BORDERLESS|RMBTRAP, /* Flags */
/* 	&Cancel*/ NULL,0,0,0,0,
	0,0,0,0,CUSTOMSCREEN};
/************************************************************************/

/************************************************************************/
/*
 * Funkcja rysuje kolorowa ramke dookola gadgetow proporcjonalnych,
 * 	(nie wypelnia wnetrza ramki)
 * Rp	- rastport w ktorym ma byc rysowana ramka,
 * LeftEdge, TopEdge 	- lewy, gorny rog ramki,
 */

void FrameForProp(struct RastPort *Rp, 
									SHORT LeftEdge, SHORT TopEdge,
									SHORT Width, SHORT Height,
									BYTE BarWidth, BYTE *ColNR )
{
	register SHORT i, factor, x, y;

	for(i=0; i<BarWidth; i++)
	{

		x = LeftEdge-i;
		y = TopEdge-i;
		factor = -4 + (i<<1);

		SetAPen( Rp, ColNR[i]);
		Move( Rp, x+3, y+1);
		Draw( Rp, x+factor+Width, y+1);
		Draw( Rp, x+factor+Width, y+(factor+2)+Height);
		Draw( Rp, x+3, y+(factor+2)+Height);
		Draw( Rp, x+3, y+1);
	}
}

void Configuration()
{
  struct Preferences pref;
  struct IntuiMessage *ConfigMsg;
	struct Gadget *PressedGad;
	struct PropInfo *PropInfoPtr;
	struct Configuration CurrentConfig = PuzzleConfig;

	float FactorPauze, FactorShow, FactorSlide, FactorScroll;
	LONG PauzePot, ShowPot, SlidePot, ScrollPot;

	BOOL EndConfig;
  ULONG Class;
	UWORD XOffset,YOffset;
	BYTE NewOffsetX, NewOffsetY, OldXOffset, OldYOffset;
	UBYTE ConfigFrame[] = { 0, 15, 14, 13, 12, 11, 10, 9, 8 };
	UBYTE ConfigFrame2[] = { 12, 11, 10, 9};
	UBYTE SmallFrame[] = {10, 9};
/*
 * Okno konfiguracji
 */
	ConfigNewWdw.Screen=Screen;
  if(!(ConfigWdw=(struct Window *)OpenWindow(&ConfigNewWdw)))
	{
		DisplayBeep( Screen );
		return;
	}

/* 	ReadConfig(); */

	Frame( ConfigRP, 8, 8, 280-16-1, 200-16-1, 8, ConfigFrame);

/* 	RefreshGadgets( &Cancel, ConfigWdw, NULL); */
  AddGList( ConfigWdw, &Cancel, 0, 8, NULL);
	RefreshGadgets( &Cancel, ConfigWdw, NULL);

/* wczytanie preferencji systemowych */
	if( GetPrefs( &pref, sizeof(pref) ) == NULL )
  {
	 /* Could not get a copy of the preferences! */
		goto Exit;
  }

/* zmiana kolorow w strukturce Preferences Is it strange? - Hej */
	pref.color0 =	NonPicsColors[0];
	pref.color1 =	NonPicsColors[1];
	pref.color2 =	NonPicsColors[2];
	pref.color3 =	NonPicsColors[3];


	pref.color17 = PicsColors[WhichPic][1];
	pref.color18 = PicsColors[WhichPic][2];
	pref.color19 = PicsColors[WhichPic][3];




	SetPrefs( &pref, sizeof(pref), TRUE);
/*
 * zapamietanie odczytanych ViewOffsets - wykorzystane pozniej
 * przy nacisnieciu Cancel
 */
	OldXOffset = pref.ViewXOffset;
	OldYOffset = pref.ViewYOffset;

	XOffset = ((float)OldXOffset+31)/31 * MAXPOT;
  YOffset = ((float)OldYOffset+19)/31 * MAXPOT;

/*
 * modyfikacja polorzenia gadgetu X-Y dla danych
 * pobranych z preferencji systemowych
 */
  ModifyProp( &XYPropGad, ConfigWdw, NULL, FREEHORIZ|FREEVERT,
							XOffset, YOffset, MAXBODY * 1/31, MAXBODY * 1/31);


/*  PauzePot, ShowPot, SlidePot, ScrollPot; */

	SlidePot = (LONG)( (1-(float)(PuzzleConfig.SlideSpeed-SlideMin)/
																						(SlideMax-SlideMin)) * MAXBODY);

	ShowPot = (LONG)( (float)(PuzzleConfig.ShowPic-ShowMin)/
																						(ShowMax-ShowMin) * MAXBODY);

	ScrollPot = (LONG)( (1-(float)(PuzzleConfig.ScrollSpeed-ScrollMin)/
																						(ScrollMax-ScrollMin)) * MAXBODY);


	PauzePot = (LONG)( (float)(PuzzleConfig.PauzeTime-PauzeMin)/
																						(PauzeMax-PauzeMin) * MAXBODY);


  ModifyProp( &Slide_gad, ConfigWdw, NULL, FREEHORIZ, SlidePot, NULL,
																									MAXBODY * 1/64, NULL);

  ModifyProp( &ShowPic_gad, ConfigWdw, NULL, FREEHORIZ, ShowPot, NULL,
																									MAXBODY * 1/64, NULL);

  ModifyProp( &Scroll_gad, ConfigWdw, NULL, FREEHORIZ, ScrollPot, NULL,
																									MAXBODY * 1/64, NULL);

  ModifyProp( &Pauze_gad, ConfigWdw, NULL, FREEHORIZ, PauzePot, NULL,
																									MAXBODY * 1/64, NULL);


/* ramka dookola gadgetu proporcionalnego X-Y	 */
	FrameForProp( ConfigRP, Xgad, Ygad, 132, 70, 4, ConfigFrame2);

/* ramki dla gadgetow proporcjonalnych X */
	FrameForProp( ConfigRP, 122, 95,  127, 8, 2, SmallFrame);
	FrameForProp( ConfigRP, 122, 117, 127, 8, 2, SmallFrame);
	FrameForProp( ConfigRP, 122, 139, 127, 8, 2, SmallFrame);
	FrameForProp( ConfigRP, 122, 161, 127, 8, 2, SmallFrame);

/*
 * wydruk tekstow (z cieniem) do gadget'ow 
 */
	PrintITShadow( ConfigRP, &DummyIT, 14, 84, "tempo zmiany obrazk\363w:");
	PrintITShadow( ConfigRP, &DummyIT, 14, 106, "moment ods\356ony ca\356ego obrazka:");
	PrintITShadow( ConfigRP, &DummyIT, 14, 128, "tempo przesuwania fragment\363w:");
	PrintITShadow( ConfigRP, &DummyIT, 14, 150, "czas trwania gry:");



	DrawImage( ConfigRP, &ConfigLogo_img, 140, 8 );
	
	DrawImage( ConfigRP, &Min_img, 107, 95 );
	DrawImage( ConfigRP, &Max_img, 250, 95 );
	DrawImage( ConfigRP, &Min_img, 107, 117 );
	DrawImage( ConfigRP, &Max_img, 250, 117 );
	DrawImage( ConfigRP, &Min_img, 107, 139 );
	DrawImage( ConfigRP, &Max_img, 250, 139 );
	DrawImage( ConfigRP, &Min_img, 107, 161 );
	DrawImage( ConfigRP, &Max_img, 250, 161 );

/************************************************************************/
/* Obsluga IDCMP */

  EndConfig = FALSE;

  while( EndConfig == FALSE )
  {
	 Wait( 1 << ConfigWdw->UserPort->mp_SigBit );

	 while(ConfigMsg=(struct IntuiMessage *)GetMsg( ConfigWdw->UserPort))
	 {

		Class = ConfigMsg->Class;		 /* Save the IDCMP flag. */
			PressedGad = (struct Gadget *)ConfigMsg->IAddress;
		ReplyMsg( ConfigMsg );

			PropInfoPtr = (struct PropInfo *)PressedGad->SpecialInfo;

		if( Class == GADGETUP )
			{
				switch( PressedGad->GadgetID )
				{
					case XYPropID:

						NewOffsetX = (BYTE)((float)PropInfoPtr->HorizPot / MAXPOT*31)-31;
						NewOffsetY = (BYTE)((float)PropInfoPtr->VertPot / MAXPOT*31)-19;

						pref.ViewXOffset = NewOffsetX;
						pref.ViewYOffset = NewOffsetY;
						SetPrefs( &pref, sizeof(pref), FALSE);
						break;

					case SlideID:
						FactorSlide= (float)(SlideMax - SlideMin)/64;
						CurrentConfig.SlideSpeed=
								(UWORD)( SlideMax - (float)PropInfoPtr->HorizPot / MAXPOT*64 * FactorSlide);
						break;

					case ShowPicID:
						FactorShow= (float)(ShowMax - ShowMin)/64;
						CurrentConfig.ShowPic=
								(UWORD)( (float)PropInfoPtr->HorizPot / MAXPOT*64 * FactorShow)+ShowMin;

						break;

					case ScrollID:
						FactorScroll = (float)(ScrollMax - ScrollMin)/64;
						CurrentConfig.ScrollSpeed=
							(UWORD)( ScrollMax - (float)PropInfoPtr->HorizPot / MAXPOT*64 * FactorScroll);

						break;

					case PauzeID:

						FactorPauze = (float)(PauzeMax - PauzeMin)/64;
						CurrentConfig.PauzeTime =
								(UWORD)( (float)PropInfoPtr->HorizPot / MAXPOT*64 * FactorPauze)+PauzeMin;
						break;

					case UseID:

/* ustaw preferncje,  wyslij komunikat IDCMP NEWPREF. i wychodz */
						SetPrefs( &pref, sizeof(pref), TRUE);
						PuzzleConfig = CurrentConfig;

						EndConfig = TRUE;
						break;
	
  				case SaveID:

						SetPrefs( &pref, sizeof(pref), TRUE);
						PuzzleConfig = CurrentConfig;
/* zapisanie preferencji */
						WriteConfig();
						EndConfig = TRUE;
						break;

					case CancelID:

/* odtworzenie ViewOffsets i wyjscie */
						pref.ViewXOffset = OldXOffset;
						pref.ViewYOffset = OldYOffset;
						SetPrefs( &pref, sizeof(pref), FALSE);


						EndConfig = TRUE;
						break;
		}
	 }


  }
}
Exit:
  RemoveGList( ConfigWdw, &Cancel, 8);
  if (ConfigWdw) CloseWindow(ConfigWdw);
	return;
}