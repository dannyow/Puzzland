#include <intuition/intuition.h>
#include <stdlib.h>
#include <stdio.h>
#include <convert.h>


#define FrontColour	9
#define BackColour 	15

/*
 * HiScore gadgets ID
 */
#define NagranieID	81
#define OkID				82

extern int Score;
extern struct Screen *Screen;
extern struct Window *Window;
extern struct IntuiText DummyIT;
void PrintITShadow( struct RastPort *UserRPort, struct IntuiText *ITptr, 
										short LeftEdge, short TopEdge, char *Message );

extern struct Image HiScoreLogo_img;
extern struct Image PodajImie_img;
extern struct Image Przegrales_img;
extern struct Image Nagranie_norm_img;
extern struct Image Nagranie_Sel_img;
extern struct Image OK_norm_img;
extern struct Image OK_sel_img;

char *HiScoreLines[10] = 
{
	"Daniel",
	"->EMU<-",
	"Amiga",
	"Valar",
	"Paul",
	"Jessica",
	"Stilgar",
	"Chani",
	"Alia",
	"Herbert",
};

int HiScoreNumbs[10] =
{
	1000, 500, 400, 300, 250, 200, 150, 100, 75, 50
};

FILE *HiScoreFile;

#define AmountOfChars 17

/* bufory dla String Gdaget'u */
UBYTE NameBuffer[AmountOfChars]; /* AmountOfChars characters including the NULL-sign. */
UBYTE UndoNameBuffer[AmountOfChars]; /* Must be at least as big as NameBuffer. */
/* THE STRING GADGET's STRUCTURES: */
struct StringInfo HiScore_string_info=
{
  NameBuffer, UndoNameBuffer,
	0, AmountOfChars, /* MaxChars, AmountOfChars characters + null-sign ('\0'). */
  0, 0, 0, 0, 0, 0,
  NULL, NULL, NULL
};

struct Gadget HiScoreName_gad=
{
  NULL,  18,  164, 136, 8,
	GADGHCOMP, STRINGCENTER|RELVERIFY|GADGIMMEDIATE, STRGADGET,
  NULL /*(APTR) &HiScoreNameBorder*/ , NULL, NULL, NULL,
  (APTR) &HiScore_string_info, 0, NULL
};

/*
 * GADGETY -Nagranie- i -OK-
 */

struct Gadget NagranieGad=
{
	NULL, 4, 176, 82, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &Nagranie_norm_img,
	(APTR) &Nagranie_Sel_img, NULL, NULL, NULL, NagranieID, NULL
};
struct Gadget OkGad=
{
	&NagranieGad, 86, 176, 82, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &OK_norm_img,
	(APTR) &OK_sel_img, NULL, NULL, NULL, OkID, NULL
};

struct NewWindow HiScoreNewWdw={
	84, 32, 172, 201, 0, 0, GADGETUP|GADGETDOWN,
	ACTIVATE|BACKDROP|BORDERLESS|RMBTRAP, /* Flags */
	&OkGad,0,0,0,0,0,0,0,0,CUSTOMSCREEN};

struct Window *HiScoreWdw;


#define HiScoreRP HiScoreWdw->RPort

void ReadHiScore()
{

	register short loop;
	int tmp;
	char buffer[20];
	char *ptr;

	if( !(HiScoreFile = fopen("S:Puzzle.hiscore","r")) )
		return;

	for(loop=0; loop<10; loop++)
	{
		if( !(fscanf(HiScoreFile, "%d", &tmp)) )
			goto Exit;
	
		/* miejse na wynik ma 5 pozycji -> max to 99999 */
		(tmp > 99999) ?(tmp=99999):tmp;
		(tmp < -99999) ?(tmp=-9999):tmp;

		HiScoreNumbs[loop] = tmp;		

		strset( buffer, 0 );

		if( !(fgets(buffer, 10, HiScoreFile)) )
			goto Exit;

		ptr = (char *)strchr(buffer,  '\n');
 		*ptr = '\0';

		HiScoreLines[loop] = (char *)strdup(buffer);

	}

Exit:
		if(HiScoreFile) fclose(HiScoreFile);
		return;

}

BOOL WriteHiScore()
{
	register short loop;
	BOOL Error = FALSE;

	if( !(HiScoreFile = fopen("S:Puzzle.hiscore","w")) )
		return(FALSE);

	for(loop=0; loop<10; loop++)
	{

		if( !(fprintf(HiScoreFile, 
								"%d%s\n", HiScoreNumbs[loop], HiScoreLines[loop])) )
		{
			Error = TRUE;
			goto Exit;
		}

	}

Exit:
		if(HiScoreFile) fclose(HiScoreFile);
		if(Error)
			return( FALSE );
		else
			return( TRUE );

}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void HiScore()
{
  register short loop = 0, ypos, PlayerPlace;
	BOOL IsEnd;

	LONG Class;
  struct IntuiMessage *HiScoreMsg;
	struct Gadget *PressedGad;

	UBYTE HiScoreFrame[] = { 12, 11, 10, 9, 8 };
	char TmpBuff[20];

/* odczyt hiscore z pliku S:Puzzle.hiscore */
/*eadHiScore(*/

	HiScoreNewWdw.Screen=Screen;
  if(!(HiScoreWdw=(struct Window *)OpenWindow(&HiScoreNewWdw)))
		return;

/* ramka wokol okna HiScore */
	Frame( HiScoreRP, 4, 4, 172-8-1, 201-8-1, 4, HiScoreFrame);

/*
 * powielanie rysunku w oknie po narysowaniu ramek wewnatrz
 * napisy beda zamalowane, gorne ogranicaenie petli to
 * wysokosc okna - grubosc dolnej belki
 */
	for(loop = 4; loop<(201-(HiScoreLogo_img.Height+4)); loop+=HiScoreLogo_img.Height)
	{
   		DrawImage( HiScoreRP, &HiScoreLogo_img, 4, loop );
	}

/* ramka wokol tekstow z imionami i punktacja */
	Frame( HiScoreRP, 18, 35, 135, 105, 4, HiScoreFrame);
	
/* wszystko juz narysowane	 */

/* 	przeglad tablicy wynikow i porownanie jej zawartosci ze Score */

	loop = 0;
	while((Score < HiScoreNumbs[loop]) && (loop < 10 ))		loop++;

	if( loop < 10 )
	{
/* spoko score miesci sie w zakresie wynikow bedzie wpis */

		PlayerPlace = loop;
		for(loop = 8; loop>=PlayerPlace; loop--)
		{
			HiScoreNumbs[loop+1] = HiScoreNumbs[loop];
			HiScoreLines[loop+1] = HiScoreLines[loop];
		}
		HiScoreNumbs[PlayerPlace] = Score;
		HiScoreLines[PlayerPlace] = "          ";
		/* rysowanie imaga z napisem Podaj swe imie? */
		DrawImage( HiScoreRP, &PodajImie_img, 	4, 145 );

		/* ramka wokol String Gadget'u dla podania imienia */
		Frame( HiScoreRP, 18, 164, 135, 7, 4, HiScoreFrame);
		/* dodanie String Gadgetu do listy zwiazanej z oknem	 */
		AddGadget( HiScoreWdw, &HiScoreName_gad, NULL );

		ActivateGadget( &HiScoreName_gad, HiScoreWdw, NULL );
	}
  else
	{
/* cienizna nici z wpisu rysuj PRZEGRALES !!! */
			DrawImage( HiScoreRP, &Przegrales_img, 	4, 145 );
	}

/* odswiezenie wszystkich gadgetow zwiazanych z oknem */
	RefreshGadgets( HiScoreWdw->FirstGadget, HiScoreWdw, NULL );


/*
 * wydruk zawartosci tablicy HiScoreLines - imiona HiScor'owcow
 */
	ypos = 39;
	for(loop = 0; loop<10; loop++)
	{
		PrintITShadow( HiScoreRP, &DummyIT, 21, ypos, HiScoreLines[loop] );

/* 		zamiana liczby Score na ASCII i zapisanie w buforze */
		ltos( HiScoreNumbs[loop], TmpBuff, 10);
		PrintITShadow( HiScoreRP, &DummyIT, 112, ypos, TmpBuff );
		ypos += 10;
	}

/*
 * 	DrawImage( HiScoreRP, &Nagranie_norm_img, 	4, 176 );
 * 	DrawImage( HiScoreRP, &OK_norm_img, 	86, 176 );
 */

/************************************************************************/
/* Obsluga IDCMP */

  IsEnd = FALSE;

  while( IsEnd == FALSE )
  {
    Wait( 1 << HiScoreWdw->UserPort->mp_SigBit );

    while(HiScoreMsg=(struct IntuiMessage *)GetMsg( HiScoreWdw->UserPort))
    {

      Class = HiScoreMsg->Class;      /* Save the IDCMP flag. */
			PressedGad = (struct Gadget *)HiScoreMsg->IAddress;
      ReplyMsg( HiScoreMsg );

      if( Class == GADGETUP )
			{
					/* wypelnienie bufora zerami - bezpiecznie					 */
					/* 					strset( TmpBuff, 0 ); */

					TmpBuff[10] = '\0'; 
					/* zkopiowanie max 10 znakow do bufora				   */
					strncpy( TmpBuff, NameBuffer, 10);
				
					/* czy w tekscie od user'a sa cyfry				 */
		      if( !strpbrk( TmpBuff, (char *)"0123456789" ) )
					{
						/* nie a wiec wszystko w porzo */
							HiScoreLines[ PlayerPlace ] = (char *)strdup(TmpBuff);

							ypos = 10*PlayerPlace + 39;	
							PrintITShadow( HiScoreRP, &DummyIT, 
													21, ypos, HiScoreLines[PlayerPlace] );
							ModifyIDCMP( HiScoreWdw, GADGETDOWN);
					}
					else
					{
					/* oj cyferek sie zachciewa, a nie ma */
						DisplayBeep( Screen );
					}
			}
			else
			if( Class == GADGETDOWN )
      {
				switch( PressedGad->GadgetID )
				{
					case OkID:
								IsEnd = TRUE;
								break;

					case NagranieID:

								if(! WriteHiScore())
									DisplayBeep( Screen );

								IsEnd = TRUE;
								break;
      	}
    	}
  	}
	}

	CloseWindow(HiScoreWdw);
	return;
}
