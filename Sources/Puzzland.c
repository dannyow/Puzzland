#include "PuzDatsPL.h"
#include "PuzFunsPL.h"
 
/*
 * poprawiny SCROLL
 */
extern void ReadHiScore();
extern void	ReadConfig();
/************************************************************************/
extern struct Image PauzaLogo_img;
extern struct Image KoniecPauzy_nrm_img;
extern struct Image KoniecPauzy_sel_img;


int Score = 0;

/*
 * deklaracje struktoryrt IntuiText za pomoca ktorej drukowane sa
 * napisy pola FronPen i IText inicjowane sa przed wydrukiem przez
 * funkcje PrintITShadow
 */
struct IntuiText DummyIT =
{
  0 /*Front Pen */, 0, JAM1, 0, 0, &AFont, NULL /* IText */, NULL,
};
/************************************************************************/
/*
 * funkcja wypisuje tekst podany w strukturze ItemText,
 * 	wraz z cieniem (ten sam napis przesuniety w dol i w prawo o jeden).
 * 
 * UserRPort - wskaznik na rastport w ktorym rysujemy napis,
 * ITptr - wskaznik na strukture IntuiText wykorzystywana jako
 * 					szkielet rysowanych napisow,
 * LeftEdge, TopEdge - wspolrzedne napisu "gornego"
 * Message - wskaznik na tekst do wydruku
 */
void PrintITShadow( struct RastPort *UserRPort, struct IntuiText *ITptr, 
										short LeftEdge, short TopEdge, char *Message )
{
	ITptr->IText = Message;

	ITptr->FrontPen = BackColour;
	PrintIText( UserRPort, ITptr, LeftEdge+1, TopEdge+1);

	ITptr->FrontPen = FrontColour;
	PrintIText( UserRPort, ITptr, LeftEdge, TopEdge);


}


/************************************************************************/
/*
 * DO NOT USE prinf()
 */
void _main()
{
	UBYTE Return;
	register int i, j, k, loop=0, counter=0, temp, error_load_count;
	USHORT WhichPiece, OldWhichPiece, OlderWhichPiece, 
				 Lenght, index, GadID;
  ULONG MessageClass, MicroSec, Sec, TimeBonus;
	USHORT MessageCode;
	struct Gadget *MessageAddress;
  BOOL load_ret = FALSE, 	one_time = TRUE;

	int Xsource, Ysource,
			Xdest, Ydest,
			X, Y;
	UBYTE FrameColours[] = { 15, 14, 13, 12, 11, 10, 9, 8 };
	UBYTE ScoreFrame[] = { 0, 14, 13, 12, 11, 10, 9, 8 };
	UBYTE SmallFrame[] = { 10, 9, 8};
	char Error_msg[] =
	{
		"\000\040\020 Puzzland's meditation: #?$@!^&+, #!_*&^$@?\00\01"
		"\000\245\035 -> Nie wczytalem ZADNEGO obrazka!!! <-\00\01"
		"\000\350\052 Prosze popraw to!!!\000\000"
	};

	AssRoutine();
	OpenAll();

/* Background  */
	for(i=0; i<4; i++)
		for(j=0; j<5; j++)
			DrawImage(RP, (APTR) &MarbleImg, i*85, j*64);

/************************************************************************/
/* inicjacja random-generator */
	randomize();
	InitGadgetsArray();
	ModifyIDCMP(Window, NULL);
/************************************************************************/
/* ramka dookola obrazka */
	Frame( RP, (Xtrans+1), (Ytrans), 
					(AmtHor * PieceWidth + (AmtHor - 1) * XSpaceBetween),
				 	AmtVer * PieceHeight + (AmtVer - 1) * YSpaceBetween,
					7, FrameColours); 

/* ramki dookola fragmentow */
	Frame( RP, Xchoose-1, Ychoose-1, 	PieceWidth+1, PieceHeight+1, 2, SmallFrame);
	Frame( RP, Xnext-1, 	Ynext-1, 		PieceWidth+1, PieceHeight+1, 2, SmallFrame);
	Frame( RP, Xfirst-1, 	Yfirst-1, 	PieceWidth+1, PieceHeight+1, 2, SmallFrame);

/* ramka dookola score */
	Frame( RP, 204, 19, 119, 35, 7, ScoreFrame);
	PrintScore( Score );

/* strzalki */
	DrawImage( RP, &Right_img, 	190, 142);
	DrawImage( RP, &Up_img, 		222, 181);
	DrawImage( RP, &Left_img, 	264, 142);
	DrawImage( RP, &Down_img, 	222, 112);
 	DrawImage( RP, &Up_img, 		299, 187); 
 	DrawImage( RP, &Up_img, 		299, 181); 

	AddGadget(Window, &SetUp_gad, 0);
	AddGadget(Window, &TheEnd_gad , 0);

	OffGadget(&SetUp_gad, Window, 0);
	OffGadget(&TheEnd_gad, Window, 0);

	RefreshGadgets( Window->FirstGadget, Window, NULL);

	DrawTimeBar();

	one_time = TRUE;


About:
/*
 * okno startowe About
 */

	StartNewWdw.Screen=Screen;
  if(!(StartWdw=(struct Window *)OpenWindow(&StartNewWdw))) 
		CleanUp("\033[1mPuzzland -\033[0m Nie mog\353 otworzy\352 okna!\n");

	DrawImage( StartWdw->RPort, &StartLogo_img,	0, 0);
	
	 /*
 		* odczytanie zawartosci Obrazki: tulko jeden raz
	 	*/
	if( one_time )
	{
		one_time = FALSE;
		if(!ReadDir())
		{
		  if( StartWdw )
		  	CloseWindow(StartWdw); 
			DisplayAlert( RECOVERY_ALERT, Error_msg, 60 );
			CleanUp("\033[1mPuzzland - koniec :-(\033[0m\n");
		}

		if( !PicsPacksMaker() )
		{
		  if( StartWdw )
		  	CloseWindow(StartWdw); 
			DisplayAlert( RECOVERY_ALERT, Error_msg, 60 );
			CleanUp("\033[1mPuzzland - koniec :-(\033[0m\n");
		}
	/*
	 * odczyt konfiguracji i HiScore
	 */
		ReadHiScore();
		ReadConfig();
	}

	error_load_count = 0; /* wewnetrzny licznik niepowodzen */
load:
	load_ret = PicLoad( &NamesOfPics[ counter*HowManyLoad ] );
  if( !load_ret  )	
	{
		error_load_count++;
		if( error_load_count > PicsPack )
		{
			if( StartWdw )  	CloseWindow(StartWdw); 
			CleanUp("\033[1mPuzzland - nie wczyta\356em obrazk\363w\033[0m\n");
		}
		counter++;
		counter %= PicsPack;
		goto load;
	}

/************************************************************************/


  if( StartWdw )  	CloseWindow(StartWdw); 


/*----------------------------------------------------------------------*/

Restart:

	OffGadget(&SetUp_gad, Window, 0);
	OffGadget(&TheEnd_gad, Window, 0);
	RefreshGadgets( Window->FirstGadget, Window, NULL);

/*----------------------------------------------------------------------*/
	DrawTimeBar();

	ModifyIDCMP(Window, GADGETUP | MOUSEBUTTONS);

	WhichPic = PickThePicture();

	Delay(PuzzleConfig.ShowPic);
	WhichPiece = random(AmountOfCovers);

/*----------------------------------------------------------------------*/

	for(i=0; i<WhichPiece; i++)
		CoverPiecesVec[i] = i;

	for(j=WhichPiece+1; j<AmountOfCovers; j++)
		CoverPiecesVec[i++] = j;

	HowManyCovers = i;
	
/*----------------------------------------------------------------------*/
	X = WhichPiece % AmtHor;
	Y = WhichPiece / AmtHor;

	Xsource = X * PieceWidth;
	Ysource = Y * PieceHeight;

	Xdest = (X * (PieceWidth + XSpaceBetween)) + Xtrans;
	Ydest = (Y * (PieceHeight + YSpaceBetween)) + Ytrans;

	BltBitMap(
		  &DataOfBitMaps[ WhichPic ], /* Source					  */
		  Xsource, Ysource,					/* Position, source. 	  */
		  &my_bit_map, 								/* Destination.			  */
		  Xdest, Ydest,							/* Position, destination. */
		  PieceWidth, PieceHeight, 	/* Width and height. 	  */
		  0xC0,									/* Normal copy.			  */
		  0xFF,											/* All bitplanes. 		  */
		  NULL );										/* No temporary storage.  */


/* kratkowanie  */
	Checker();

	for(i=0; i<HowManyCovers; i++)
	{
		j = CoverPiecesVec[i];
		AddGadget(Window, &GadgetsArray[j], 0);
	}
	RefreshGadgets( Window->FirstGadget, Window, NULL);

/* swap zamienia 6 losowych wartosci z tablicy przykrytych kawalkow */
	for(i=0; i<6; i++)
	{
		j = random(HowManyCovers);
		k = random(HowManyCovers);
		temp = CoverPiecesVec[j];
		CoverPiecesVec[j] = CoverPiecesVec[k];
		CoverPiecesVec[k] = temp;
	}




/************************************************************************/

	OnGadget(&SetUp_gad, Window, 0);
	OnGadget(&TheEnd_gad, Window, 0);
	RefreshGadgets( Window->FirstGadget, Window, NULL);

	Timer_Start( &StartTime );
  GameStartTime = StartTime;

/* nie siagalna wartosc */
	OlderWhichPiece = 13;
	loop = 2;

  for( ;; )
	{

		Timer_Stop( &GameStartTime, &StopTime );
/* 		
 * czas 20sekund dlugosc - 160 pixeli 160/x = 20/1 => x=8
 */
		if( (Sec = StopTime.tv_secs) < PuzzleConfig.PauzeTime)
		{
			SetAPen(RP, 0);
			Lenght = Sec * (160/PuzzleConfig.PauzeTime);
			RectFill(	RP, 77, 245, Lenght + 77, 260);
		}
		else
		{
			SetAPen(RP, 0);
			RectFill(	RP, 77, 245, 237, 260);
			Lenght = 160;
			goto Exit;
		}
		if(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
	  {
loopLAB:
		  MessageClass = message->Class;
			MessageCode = message->Code;
			MessageAddress = message->IAddress;
		  ReplyMsg(message);

			switch( MessageClass )
			{
				case GADGETUP:
					message = (struct IntuiMessage *)GetMsg(Window->UserPort);
					if( (message->Class) == GADGETUP)
						goto loopLAB;
					else
					if((GadID = MessageAddress->GadgetID) == OlderWhichPiece)
					{
						MessageClass = NULL;
						RemoveGadget(Window, &GadgetsArray[GadID]);
						RefreshGadgets( &GadgetsArray[GadID], Window, NULL);

						X = GadID % AmtHor;
						Y = GadID / AmtHor;

						Xsource = X * (PieceWidth + XSpaceBetween);
						Ysource = Y * (PieceHeight + YSpaceBetween);

						Xdest = Xsource + Xtrans;
						Ydest = Ysource + Ytrans;

						ClipBlit(	RP, Xnext, Ynext,
											RP, Xdest, Ydest,
											PieceWidth, PieceHeight, 0xc0);

						SeekAndDestroy(GadID);
						loop--;
						Score += 5;
						PrintScore( Score );


						if( HowManyCovers == 1 )
						{		
						/* ostatni kawalek przykryty gadgetem */
								j = CoverPiecesVec[0];
								RemoveGadget(Window, &GadgetsArray[j]);
								RefreshGadgets( &GadgetsArray[j], Window, NULL);
								X = j % AmtHor;
								Y = j / AmtHor;

								Xsource = X * PieceWidth;
								Ysource = Y * PieceHeight;

								Xdest = (X * (PieceWidth + XSpaceBetween))  + Xtrans;
								Ydest = (Y * (PieceHeight + YSpaceBetween)) + Ytrans;
								
						/* przemiesienie ostatniego kawalka na obrazek */
								BltBitMap(&DataOfBitMaps[ WhichPic ], Xsource,Ysource,
												  &my_bit_map, Xdest,Ydest, PieceWidth,PieceHeight, 	/* Width and height. 	  */
												  0xC0, 0xFF, NULL );

								for(loop=0; loop < (160-Lenght)+1; loop++)
								{
								SetAPen(RP, 0);
								RectFill(	RP, 77, 245, Lenght + 77 + loop, 260);
								}
								TimeBonus = (Lenght*8)/PuzzleConfig.PauzeTime;
				
								Score += TimeBonus;
								PrintScore( Score );

								Delay( 50 );
								goto Exit;
						}
					}
					else
					if( GadID == TheEndID )
					{
						goto Exit;
					}
					else
					if( GadID == SetUpID )
					{
 						Configuration();
						Timer_Start( &GameStartTime );
					}
					else
					{
						DisplayBeep(Screen);
						Score -= 1;
						PrintScore( Score );
					}
				case MOUSEBUTTONS:
					if( (message->Code) == MENUDOWN)
					{
						if( Pauza( OlderWhichPiece ) )
							goto Exit;
						else
						 Timer_Start( &GameStartTime );
					}						

			}
		}

/*
 * 			Scroll of pieces
 */
	 Timer_Stop( &StartTime, &StopTime );

		if(StopTime.tv_micro > (PuzzleConfig.ScrollSpeed*1000) )
		{

			Timer_Start( &StartTime );

			loop++;

			index = loop % HowManyCovers;
			WhichPiece = CoverPiecesVec[index];

			index = (loop-1) % HowManyCovers;
			OldWhichPiece = CoverPiecesVec[index];
	
			index = (loop-2) % HowManyCovers;
			OlderWhichPiece = CoverPiecesVec[index];
	
  		X = WhichPiece % AmtHor;
			Y = WhichPiece / AmtHor;

			Xsource = X * PieceWidth;
			Ysource = Y * PieceHeight;

			BltBitMap(&DataOfBitMaps[ WhichPic ], Xsource, Ysource,
							  &my_bit_map, Xfirst, Yfirst,
							  PieceWidth, PieceHeight, 0xC0, 0xFF, NULL );										/* No temporary storage.  */

  		X = OldWhichPiece % AmtHor;
			Y = OldWhichPiece / AmtHor;

			Xsource = X * PieceWidth;
			Ysource = Y * PieceHeight;

			BltBitMap(&DataOfBitMaps[ WhichPic ], Xsource, Ysource,
								&my_bit_map, Xchoose, Ychoose, 
								PieceWidth, PieceHeight, 0xC0, 0xFF, NULL );									

  		X = OlderWhichPiece % AmtHor;
			Y = OlderWhichPiece / AmtHor;

			Xsource = X * PieceWidth;
			Ysource = Y * PieceHeight;

			BltBitMap(&DataOfBitMaps[ WhichPic ], Xsource, Ysource,
							  &my_bit_map, Xnext, Ynext,			
							  PieceWidth, PieceHeight, 0xC0, 0xFF, NULL );									

	 }
	}
/************************************************************************/

Exit:

		Return = GameOver();

		switch(Return)
		{
			case 0:
					HiScore();
					CleanUp("\033[1mPuzzland - to ju\373 koniec :-)\033[0m\n");
					break;
			case 1:
			case 2:
			case 3: /* nowe obrazki */
				if(HowManyCovers > 1)
		  		for(i=0; i<HowManyCovers; i++)
					{
						j = CoverPiecesVec[i];
						RemoveGadget(Window, &GadgetsArray[j]);
						RefreshGadgets( &GadgetsArray[j], Window, NULL);
					}
			/* ramki dookola fragmentow */
				Frame( RP, Xchoose-1, Ychoose-1, 	PieceWidth+1, PieceHeight+1, 2, SmallFrame);
				Frame( RP, Xnext-1, 	Ynext-1, 		PieceWidth+1, PieceHeight+1, 2, SmallFrame);
				Frame( RP, Xfirst-1, 	Yfirst-1, 	PieceWidth+1, PieceHeight+1, 2, SmallFrame);

				SetAPen( RP, 15);
				RectFill( RP, Xtrans, Ytrans, 
									Xtrans+AmtHor*PieceWidth+(AmtHor-1)*XSpaceBetween,
									Ytrans+AmtVer*PieceHeight+(AmtVer-1)*YSpaceBetween);
				if(Return == 1)
				{
					HiScore();
					Score = 0;
					PrintScore( Score );
					goto Restart;
				}
				else
				if( Return == 2 )
			  {
					goto Restart;
				}
				else 
				if( Return == 3 )
				{
					PlaneFree();
					counter++;
					counter %= PicsPack;
					goto About;
				}
			}
}
