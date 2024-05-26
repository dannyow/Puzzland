/* #include "Config.h" */
#include <intuition/intuition.h>
#include <stdlib.h>list
#include <stdio.h>
#include <string.h>

#define GameOverRP	GameOverWdw->RPort
#define NewGameID		61
#define QuitID 			62
#define ContinueID	63
#define NewPicsID   64

extern int Score;
extern BOOL SmallMem;
extern struct Image Koniec_img;
extern struct Image NoweObrNRM_img;
extern struct Image NoweObrSEL_img;
extern struct Image KontynuacjaNRM_img;
extern struct Image KontynuacjaSEL_img;
extern struct Image NowaGraNRM_img;
extern struct Image NowaGraSEL_img;
extern struct Image WyjscieNRM_img;
extern struct Image WyjscieSEL_img;
extern struct Window *Window;
extern struct Screen *Screen;
extern struct IntuiText DummyIT;

extern void PrintITShadow( struct RastPort *UserRPort,struct IntuiText *ITptr, 
													 short LeftEdge, short TopEdge, char *Message );


/************************************************************************/


struct Window *GameOverWdw;

/* struktury dla boolgadgets  */
struct Gadget NewPics=
{
	NULL, 4, 59, 164, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &NoweObrNRM_img,
	(APTR) &NoweObrSEL_img, NULL, NULL, NULL, NewPicsID, NULL
};
struct Gadget ContinueGad =
{
	&NewPics, 4, 80, 164, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &KontynuacjaNRM_img,
	(APTR) &KontynuacjaSEL_img, NULL, NULL, NULL, ContinueID, NULL
};
struct Gadget NewGameGad =
{
	&ContinueGad, 4, 101, 164, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &NowaGraNRM_img,
	(APTR) &NowaGraSEL_img, NULL, NULL, NULL, NewGameID, NULL
};
struct Gadget QuitGad =
{
	&NewGameGad, 4, 122, 164, 21, GADGHIMAGE | GADGIMAGE,
	GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &WyjscieNRM_img,
	(APTR) &WyjscieSEL_img, NULL, NULL, NULL, QuitID, NULL
};


struct NewWindow GameOverNewWdw={
	84,70,172,147,0,0,GADGETUP|GADGETDOWN,
	ACTIVATE|BORDERLESS|RMBTRAP, /* Flags */
	&QuitGad,0,0,0,0,
	0,0,0,0,CUSTOMSCREEN};
/*----------------------------------------------------------------------*/

/*
 * Okno Game Over
 */
UBYTE GameOver()
{
  struct IntuiMessage *GameOverMsg;
	struct Gadget *PressedGad;
	UBYTE Result;
  ULONG Class;
	UBYTE FrameColours[] = { 11, 10, 9, 8 };
	char buffer[20];

	GameOverNewWdw.Screen=Screen;
  if(!(GameOverWdw=(struct Window *)OpenWindow(&GameOverNewWdw))) 
		return( 0 );

	Frame( GameOverRP, 4, 4, 172-8-1, 147-8-1, 3, FrameColours); 

	DrawImage( GameOverRP, &Koniec_img, 					4, 4 );
	
	sprintf( buffer, "Tw\363j wynik: %d", Score);
	PrintITShadow( GameOverRP, &DummyIT, 26, 45, buffer);

	DrawImage( GameOverRP, &NoweObrNRM_img,			 	4, 59 );
	DrawImage( GameOverRP, &KontynuacjaNRM_img, 	4, 80 );
	DrawImage( GameOverRP, &NowaGraNRM_img, 			4, 101 );
	DrawImage( GameOverRP, &WyjscieNRM_img, 			4, 122 );

	if( !SmallMem ) 
		OffGadget( &NewPics, GameOverWdw, NULL );

	RefreshGadgets(GameOverWdw->FirstGadget, GameOverWdw, NULL);

/************************************************************************/
/* Obsluga IDCMP */
  for(;;)
  {
    Wait( 1 << GameOverWdw->UserPort->mp_SigBit );

    while(GameOverMsg=(struct IntuiMessage *)GetMsg( GameOverWdw->UserPort))
    {

      Class = GameOverMsg->Class;      /* Save the IDCMP flag. */
			PressedGad = (struct Gadget *)GameOverMsg->IAddress;
      ReplyMsg( GameOverMsg );

			if( Class == GADGETDOWN )
      {
				switch( PressedGad->GadgetID )
				{
					case QuitID:
						Result = 0;
						goto Exit;

					case NewGameID:
						Result = 1;
						goto Exit;

					case ContinueID:
						Result = 2;
						goto Exit;

					case NewPicsID:
						Result = 3;
						goto Exit;
      }
    }
  }
}

Exit:
  if (GameOverWdw) CloseWindow(GameOverWdw);
	return( Result );
}
