
extern void AssRoutine();


/*
 * funkcja zastepuje printf()
 */
void MyPuts (char *str)
{
	Write (Output(), str, strlen(str));
}

void SeekAndDestroy(int Value)
{
	register BYTE i, j;
	 
	for(i=0; i<HowManyCovers; i++)
	{
		if(CoverPiecesVec[i] == Value)
		{
			for(j=i; j<HowManyCovers-1; j++)
			{
				CoverPiecesVec[j] = CoverPiecesVec[j+1];
			}
			HowManyCovers --;
			return;
		}
	}

}

void PrintScore(int DataToPrint)
{
	/* miejse na wynik ma 5 pozycji => max to 99999 */
	(DataToPrint > 99999) ? (DataToPrint = 99999):DataToPrint;
	(DataToPrint < -9999) ? (DataToPrint = -9999):DataToPrint;
		
	sprintf( ScoreLine, "Punkty: %d", DataToPrint);
	
	SetAPen(RP, 0);

	RectFill( RP, 203, 20, 205+119, 20+33 );

	Score_txt.FrontPen = BackTextColor;
	PrintIText( RP, &Score_txt, 213, 32);
	Score_txt.FrontPen = FrontTextColor;
	PrintIText( RP, &Score_txt, 212, 33);
}


/*
 * Inicjacja tablicy GadgetsArray zawierajacej struktury Gadget,
 * ktore opisuja gadgety przykrywajace obrazek
 */
void InitGadgetsArray()
{
	register UBYTE loop;
	register SHORT X, Y, X2, Y2, Xdest, Ydest;

	for(loop = 0; loop < AmountOfCovers; loop++)
	{
/* obliczenie polozenia gadetow na ekranie */
/* stale AmtHor - ilosc gadetow w poziomie */

/*  X = numer_gadgetu (suma modulo) ilosc_gadgetow_w_poziomie	*/
		X = loop % AmtHor;

/* Y = numer_gadgetu (dzielenie calkowite) ilosc_gadgetow_w_poziomie */
		Y = loop / AmtHor;

/* zmienna ...SpaceBetween okresla odstep miedzy fragmentami na ekranie */
		X2 = X * (PieceWidth + XSpaceBetween);
		Y2 = Y * (PieceHeight + YSpaceBetween);

/* stale Xtrans, Ytrans wspolrzedne lewego, gornego rogu */
		Xdest = X2 + Xtrans;
		Ydest = Y2 + Ytrans;

/* inicjacja struktury Gadget loop'ego gadget'u */
		GadgetsArray[loop].NextGadget = NULL;
		GadgetsArray[loop].LeftEdge = Xdest;
		GadgetsArray[loop].TopEdge = Ydest;
		GadgetsArray[loop].Width = PieceWidth;
		GadgetsArray[loop].Height = PieceHeight;
		GadgetsArray[loop].Flags = GADGHIMAGE | GADGIMAGE;
		GadgetsArray[loop].Activation = GADGIMMEDIATE | RELVERIFY;
		GadgetsArray[loop].GadgetType = BOOLGADGET;
		GadgetsArray[loop].GadgetRender = (APTR) &GadgetImage;
		GadgetsArray[loop].SelectRender = (APTR) &GadgetSelImage;
		GadgetsArray[loop].GadgetText = NULL;
		GadgetsArray[loop].MutualExclude = NULL;
		GadgetsArray[loop].SpecialInfo = NULL;
		GadgetsArray[loop].GadgetID = loop;
		GadgetsArray[loop].UserData = NULL;
	}
}
/* End of InitGadgetArray */
/************************************************************************/

/*
 * Rysowanie kraty na rysunku
 */
void Checker()
{
	register UBYTE i, loop;
	register UBYTE BarsColours[] = { 9, 12, 14 };
	register UWORD xp, yp, xk, yk;

/* belki poziome */
	for( loop=0; loop < (AmtVer-1); loop++ )
	{
		xp = Xtrans;
		yp = Ytrans + PieceHeight + loop * (PieceHeight + YSpaceBetween);
		xk = AmtHor * PieceWidth + (AmtHor - 1) * XSpaceBetween + Xtrans;

		for( i=0; i<YSpaceBetween; i++ )
		{
			SetAPen(RP, BarsColours[i] );
			Move(RP, xp, yp + i);
			Draw(RP, xk, yp + i);
		}
	}
/* belki pionowe */
	for( loop=0; loop < (AmtHor-1); loop++ )
	{
		xp = Xtrans + PieceWidth + loop * (PieceWidth+ XSpaceBetween);
		yp = Ytrans;

		yk = AmtVer * PieceHeight + (AmtVer - 1) * YSpaceBetween + Ytrans;

		for( i=0; i<XSpaceBetween; i++ )
		{
			SetAPen(RP, BarsColours[i] );
			Move(RP, xp + i, yp );
			Draw(RP, xp + i, yk );
		}
	}


}
/* End of Checker() */

void Timer_Start(struct timeval *Time)
{
register struct	timerequest *Time_Req;

	Time_Req=TimerRequest;


	Time_Req->tr_node.io_Command=TR_GETSYSTIME;
	Time_Req->tr_node.io_Flags=IOF_QUICK;
	DoIO((struct IORequest *)Time_Req);
	*Time=Time_Req->tr_time;
}

/*
 * This gets the ending time and finds out what the per second speed is...
 */
void Timer_Stop(struct timeval *start, struct timeval *stop)
{
register struct	timerequest *Time_Req;


	Time_Req=TimerRequest;

	Time_Req->tr_node.io_Command=TR_GETSYSTIME;
	Time_Req->tr_node.io_Flags=IOF_QUICK;
	DoIO((struct IORequest *)Time_Req);
	*stop = Time_Req->tr_time;

	SubTime( stop, start);


}
/*
 * Funkcja rysuje kolorowa ramke o podanych parametrach:
 * TopEdge, LeftEdge - wsp wewnetrznego prostokata,
 * Width, Height - wymiary wewnetrznego prostokata,
 * Bar'sWidth - szerokosc kolorowej belki dookola (koncowa wielkosc ramki
 * 							to Width + 2 * Bar'sWidth - szerokosc,
 * 								 Height + 2 * Bar'sWidth - wysokosc,
 * ColNR - wskaznik na tablice zawierajaca numery kolorow (liczone od zera )
 * 					uzytych do rysowania belki ( numeracja od zewnetrznej 
 * 					strony ramki)
 */ 

void Frame( struct RastPort *rp,
						SHORT LeftEdge, SHORT TopEdge, 
						SHORT Width, SHORT Height, 
						BYTE BarsWidth, UBYTE *ColNR)
{
 	register BYTE i;	
	register UWORD X1, Y1, X2, Y2;

	for(i=BarsWidth; i>=0; i--)
	{
		X1 = LeftEdge - i;
		Y1 = TopEdge - i;
		X2 = LeftEdge + Width + i; 
		Y2 = TopEdge + Height + i;
	  SetAPen(rp, ColNR[i]);
		RectFill(rp, X1, Y1, X2, Y2);
	}

}
/* End of Frame() */

void DrawTimeBar()
{

register SHORT i, Y;
register UBYTE SmallFrame[] = { 10, 9, 8};

/* time bar */
	Frame( RP, 77-1, 245-1, 	162, 17, 2, SmallFrame);
  Y = 245;

	for(i=0; i<8; i++)
	{
		SetAPen(RP, (8+i) );
		Move(RP,  77, Y+i );
		Draw(RP, 237, Y+i );

		Move(RP,  77, Y+15-i );
		Draw(RP, 237, Y+15-i );

	}

}

/************************************************************************/
/* dealocate all BitPlanes */
void PlaneFree()
{
	register short loop;

	for( loop = 0; loop < HowManyLoad; loop++ )
	{
		if(ChipAdr[loop]) 
 			FreeMem(ChipAdr[loop], (RASSIZE(SizeX, SizeY)*Depth));
	}
}

/*======================================================================*/
/* Returns all allocated resources: */

void CleanUp( message )
STRPTR message;
{

	PlaneFree();
	if(ExtraChip) FreeMem(ExtraChip, RASSIZE(WIDTH, HEIGHT));
/*----------------------------------------------------------------------*/


  if( GfxBase ) CloseLibrary( GfxBase );

  if( IntuitionBase ) CloseLibrary( IntuitionBase );
		
  if (Window) CloseWindow(Window);
  if (Screen) CloseScreen(Screen);
	
  MyPuts( message ); 
  exit(0);
}
/* End of CleanUp() */


BOOL AllocExtraChip()
{
	register LONG i;

	if(!(ExtraChip = (ULONG *)AllocMem((RASSIZE(WIDTH, HEIGHT)), 
																						MEMF_CHIP|MEMF_CLEAR)))
				return( FALSE );


/* wypelnianie dodatkowego najstarszego bitplanu jedynkami */
/* powoduje to przesuniecie kolorow 16-sto kolorowego obrazka do gory */
	for( i = 0; i < (RASSIZE(WIDTH, HEIGHT)/4); i++)
	{
			*(ExtraChip + i) = 0xffffffff;
	}
	return( TRUE );
}

BOOL PicsPacksMaker()
{
	register short loop;
	SHORT ChipAvail;

	if( !AllocExtraChip() )
		return( FALSE );
	
	ChipAvail = (SHORT)( ( AvailMem( MEMF_CHIP | MEMF_CLEAR ) - 70000 ) / 18000);

	if( ChipAvail < 3 )  return( FALSE );

	if( ChipAvail >= HowManyPics )
	{
/*
 * 	zaladowanie wszystkich obrazkow z device'u Obrazki:
 */
		HowManyLoad = HowManyPics;
		PicsPack = 0;
		SmallMem = FALSE;
		return( TRUE );
	}
	else
		for(loop = 2; loop < (HOW_MANY_PICS / ONE_PACK) ; loop++)
		{
				if( ChipAvail >= HowManyPics / loop )
				{
					HowManyLoad = HowManyPics / loop; /*ile zaladowac na raz */
					PicsPack = loop--;
					SmallMem = TRUE;									/*znacznik dla GameOver()*/
					return( TRUE );
				}
		}

	return( FALSE );
}

BOOL ReadDir()
{ 
	register SHORT i=0;
  struct FileLock *lock;
  struct FileInfoBlock *fib_ptr; 
	STRPTR FileNamePtr;
	char DeviceName[] = "Obrazki:";
	char TempName[100];
	char *TempPtr;
	BOOL Error = FALSE;

  /* Allocate enough memory for a FileInfoBlock structure: */
  fib_ptr = (struct FileInfoBlock *)
            AllocMem( sizeof( struct FileInfoBlock ),
                      MEMF_PUBLIC | MEMF_CLEAR );

  if( fib_ptr == NULL )
		return( FALSE );  

  /* Try to lock the file: */
  if( !(lock = (struct FileLock *) Lock( DeviceName, SHARED_LOCK )) )
  {
    /* Deallocate the memory we have allocated: */
    FreeMem( fib_ptr, sizeof( struct FileInfoBlock ) );
		return( FALSE );   
  }
	
  /* Try to examine the directory/device/(file): */
  if( Examine( lock, fib_ptr ) )
  {
    /* Check if it is a directory/device: */
    if( fib_ptr->fib_DirEntryType > 0 )
    {
      /* As long as we can examine files/directories we continue: */
      while( ExNext( lock, fib_ptr ) )
      {
        /* If it is a file we print out the name with white characters. */
        /* However, if it is a (sub)directory we use orange:            */

        if( fib_ptr->fib_DirEntryType < 0 )
				{
		 		/* File */

					FileNamePtr = (STRPTR)fib_ptr->fib_FileName;
					if( i < HOW_MANY_PICS )
					{
						TempPtr = (char *)strcpy(TempName, DeviceName); 
						strcat(TempPtr, FileNamePtr); 
						NamesOfPics[i++] = strdup( TempName );
					}
					else
						goto Exit;
				}
					
      }

      /* Check what went wrong. If it was not because there were no more */
      /* files in the directory (ERROR_NO_MORE_ENTRIES), something       */
      /* terrible has happened!                                          */
      if( IoErr() != ERROR_NO_MORE_ENTRIES )
				Error = TRUE;
    }
    else
      Error = TRUE;
  }
  else
    Error = TRUE;

Exit:
  /* Unlock the file: */
  UnLock( lock );  

  /* Deallocate the memory we have allocated: */
  FreeMem( fib_ptr, sizeof( struct FileInfoBlock ) );

	HowManyPics = i;
	if( Error )
		return( FALSE );
	else 
		return( TRUE );
}

/************************************************************************/
BOOL PicLoad(char **NamePtr)
{
	register int counter, index, loop, ErrorReadingPic;
	FILE *PicFile;
	

/* PicsLoad */


	counter = index = ErrorReadingPic = 0;

	while( index < HowManyLoad )
	{
			if(!(PicFile = fopen( NamePtr[index++], "rb")))
			{
				ErrorReadingPic ++;
				goto Next;
			}

			if(!(ChipAdr[counter] = (CPTR)AllocMem((RASSIZE(SizeX, SizeY)*Depth), 
																						MEMF_CHIP|MEMF_CLEAR)))
			{
				ErrorReadingPic ++;
				goto Exit;
			}

			if(!(fread(ChipAdr[counter], RASSIZE(SizeX, SizeY), Depth, PicFile)))
			{
				ErrorReadingPic ++;
				goto Next;
			}

 			InitBitMap(&DataOfBitMaps[counter], Depth+1, SizeX, PictureHeight);
			
		  for( loop = 0; loop < Depth; loop++ )
  		{
    			DataOfBitMaps[counter].Planes[loop] =
													 (ChipAdr[counter]+loop*(RASSIZE(SizeX, SizeY)));
  		} 
			
 			DataOfBitMaps[counter].Planes[ Depth ] = ExtraChip;

			if(!(fread( *(PicsColors+counter), sizeof(UWORD), 16, PicFile)))
			{
				ErrorReadingPic ++;
				goto Next;
			}

  		counter++;
			goto Skip;
Next:
			if(ChipAdr[counter]) FreeMem(ChipAdr[loop], (RASSIZE(SizeX, SizeY)*Depth));
Skip:	
  		if( PicFile ) 	fclose(PicFile);	

	}
Exit:
	if(ErrorReadingPic < HowManyLoad)
	{
		HowManyLoad = counter;
		return( TRUE );
	}
	else
			return( FALSE );
}
/* End of PicLoad */


void OpenAll()
{
	long error;
  /* Open the Intuition library: */
  IntuitionBase = (struct IntuitionBase *)
    OpenLibrary( "intuition.library", 0 );
  if( !IntuitionBase )
    CleanUp( "\033[1mPuzzland \033[0m nie mog\353 otworzy\352 Intuition library!\n" );

  /* Open the Graphics library: */
  GfxBase = (struct GfxBase *)
    OpenLibrary( "graphics.library", 0 );
  if( !GfxBase )
    CleanUp( "\033[1mPuzzland \033[0m nie mog\353 otworzy\352 Graphics library!" );

	if( !(TimerPort = (struct MsgPort *)CreatePort("MyPort", 0L)) )
		CleanUp("\033[1mPuzzland \033[0m brak pami\353ci\n");

	if( !(TimerRequest = (struct timereqest *)CreateExtIO(TimerPort,
													(long)sizeof(struct timerequest))) )
		CleanUp("\033[1mPuzzland \033[0m brak pami\353ci\n");


	if(error = OpenDevice("timer.device", UNIT_VBLANK,
													(struct IORequest *)TimerRequest, 0L))
		CleanUp("\033[1mPuzzland \033[0m brak pami\353ci\n");

	TimerBase=(struct Library *)TimerRequest->tr_node.io_Device;



 	PicHAM();

  	
  
  if(!(Screen=(struct Screen *)OpenScreen(&ekran)))
    CleanUp( "\033[1mPuzzland \033[0m nie mog\353 otworzy\352 ekranu!" );

		okno.Screen=Screen;
  if(!(Window=(struct Window *)OpenWindow(&okno))) 
    CleanUp( "\033[1mPuzzland \033[0m nie mog\353 otworzy\352 okna!" );



 
 	my_bit_map = Screen->BitMap;

 	LoadRGB4(&Screen->ViewPort, NonPicsColors, COLOURS);
}
/* End of OpenAll() */
/************************************************************************/
/*----------------------------------------------------------------------*/


int PickThePicture()
{
  ULONG MessageClass;
	register int loop, i, j;

  loop = 0;
  while( (message->Code) != SELECTDOWN)
	{

    loop++;      
		i = loop % HowManyLoad;
		
		for(j=16; j<32; j++)
			SetRGB4( &Screen->ViewPort, j, 
						RED		(PicsColors[i][j - 16]), 
						GREEN	(PicsColors[i][j - 16]), 
						BLUE	(PicsColors[i][j - 16]));
		

		BltBitMap(
        &DataOfBitMaps[i],						 	/* Source                 */
        0, 0,      											/* Position, source.      */
        &my_bit_map, 										/* Destination.           */
        Xtrans + (((AmtHor - 1) * XSpaceBetween) / 2), 
				Ytrans + (((AmtVer - 1) * YSpaceBetween) / 2), /* Position, destination. */
        PictureWidth, PictureHeight,		/* Width and height.      */
        0xC0,               						/* Normal copy.           */
        0xFF,      						  				/* All bitplanes.         */
        NULL );    						  				/* No temporary storage.  */ 

		Delay( PuzzleConfig.SlideSpeed );

    if(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
    {
      MessageClass = message->Class;
      ReplyMsg(message);
    }
		
	}
	return(i);
}
/* End of PickThePicture() */
/************************************************************************/
/*----------------------------------------------------------------------*/
/**********
*  RANDOM
***********/

#define LARGE  100000000
#define SMALL  10000
#define MEDIUM 31415821

long random( upper )
  long upper;
{
  if (upper < 0)
    upper = -upper;  /* force positive */

  random_variable = (random_mult( random_variable, MEDIUM ) + 1) % LARGE;
  upper = ((random_variable / SMALL) * upper) / SMALL;

  return (upper);
}

/***************
*  RANDOM MULT
****************/

long random_mult( a, b )
  long a, b;
{
  register long a0, a1, b0, b1, m;

  a0 = a % SMALL;
  a1 = a / SMALL;
  b0 = b % SMALL;
  b1 = b / SMALL;

  m = (((a0 * b1 + a1 * b0) % SMALL) * SMALL + a0 * b0) % LARGE;

  return (m);
}

/*************
*  RANDOMIZE
**************/

void randomize()
{
  struct DateStamp ds;

  DateStamp( &ds );
  random_variable = ds.ds_Tick;
}
/************************************************************************/

