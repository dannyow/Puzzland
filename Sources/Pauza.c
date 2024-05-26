#include <intuition/intuition.h>

extern struct Screen *Screen;
extern struct Window *Window;
extern struct IntuiMessage *message;
extern int Score;
extern struct Gadget GadgetsArray[];
extern struct BitMap DataOfBitMaps[];
extern struct timeval StopTime;
extern struct timeval GameStartTime;
extern struct BitMap my_bit_map;
extern int HowManyCovers;
extern int WhichPic;
extern int CoverPiecesVec[];
extern struct Configuration
{
  UWORD SlideSpeed;
  UWORD ShowPic;
  LONG  ScrollSpeed;
  UWORD PauzeTime;
} PuzzleConfig;

extern struct Image PauzaLogo_img;
extern struct Image KoniecPauzy_nrm_img;
extern struct Image KoniecPauzy_sel_img;

extern void Timer_Start(struct timeval *);
extern void Timer_Stop(struct timeval *, struct timeval *);

#ifndef PieceWidth
/* ilosc kawalkow odpowiednio w poziomie i w pionie */
  #define AmtVer 4
  #define AmtHor 3

/* dlugosci odstepow miedzy kawalkami na ekranie */
  #define XSpaceBetween 3
  #define YSpaceBetween 3

/* wymiary kawalka obrazka */
  #define PieceWidth 56    /* PictureWidth  / 3 */
  #define PieceHeight 51    /* PictureHeight / 3 */

  #define Xtrans 6
  #define Ytrans 18

  #define Xchoose 279
  #define Ychoose 126

  #define Xnext 204
  #define Ynext 126

  #define Xfirst 258
  #define Yfirst 202

#endif  

#define EndOfPauzaID 100

struct Gadget EndOfPauzaGad=
{
  NULL, 4, 35, 126, 21, GADGHIMAGE | GADGIMAGE,
  GADGIMMEDIATE | RELVERIFY, BOOLGADGET, (APTR) &KoniecPauzy_nrm_img,
  (APTR) &KoniecPauzy_sel_img, NULL, NULL, NULL, EndOfPauzaID, NULL
};

struct NewWindow PauzaNewWdw={
  197, 62, 134, 60, 0, 0, GADGETUP|GADGETDOWN,
  ACTIVATE|BACKDROP|BORDERLESS|RMBTRAP, /* Flags */
  &EndOfPauzaGad,0,0,0,0,0,0,0,0,CUSTOMSCREEN};

struct Window *PauzaWdw;

#define PauzaRP PauzaWdw->RPort
#define RP Window->RPort

/*
 * rezultat funkcij bool -> TRUE  jezeli ostatni fragment ukladanki
 *                           FALSE  w kazdym innym przypadku
 */
BOOL Pauza( USHORT PieceToCompare )
{

  register short loop, j;

  BOOL IsEnd;
  ULONG Class, SigWdw, SigPauz, SigRet;
  struct IntuiMessage *PauzaMsg;
  struct Gadget *PressedGad;
  USHORT Lenght, GadID;
  ULONG MessageClass, Sec, TimeBonus;
  USHORT MessageCode;
  struct Gadget *MessageAddress;
  int Xsource, Ysource,
      Xdest, Ydest,
      X, Y;
  UBYTE PauzaFrame[] = { 0, 11, 10, 9, 8 };

  PauzaNewWdw.Screen=Screen;
  if(!(PauzaWdw=(struct Window *)OpenWindow(&PauzaNewWdw)))
    return( FALSE );

/* udalo sie otworzyc okno no to zmniejsz ilosc punktow */
  Score -= 2;
  PrintScore( Score );

/* ramka wokol okna Pauza */
  Frame( PauzaRP, 4, 4, 134-8-1, 60-8-1, 4, PauzaFrame);

/*
 * powielanie rysunku w oknie po narysowaniu ramek wewnatrz
 * napisy beda zamalowane, gorne ogranicaenie petli to
 * wysokosc okna - grubosc dolnej belki
 */
  for(loop = 4; loop<(60-(PauzaLogo_img.Height+4)); loop+=PauzaLogo_img.Height)
  {
       DrawImage( PauzaRP, &PauzaLogo_img, 4, loop );
  }

  DrawImage( PauzaRP, &KoniecPauzy_nrm_img, 4, 35 );

/* Time bar o dlugosci 100 */
  for(loop = 0; loop<8; loop++)
  {
    SetAPen(PauzaRP, (8+loop) );
    Move(PauzaRP,  17, 20+loop );
    Draw(PauzaRP, 117, 20+loop );

    Move(PauzaRP,  17, 20+15-loop );
    Draw(PauzaRP, 117, 20+15-loop );
  }


/* Obsluga IDCMP */

  IsEnd = FALSE;

  SigWdw =  1 << Window->UserPort->mp_SigBit;
  SigPauz = 1 << PauzaWdw->UserPort->mp_SigBit;

  Timer_Start( &GameStartTime );

  while( IsEnd == FALSE )
  {
/*
 * Obsluga belki z "czasem"
 */
/************************************************************************/
    Timer_Stop( &GameStartTime, &StopTime );
/*     
 * czas 10 sekund dlugosc - 100 pixeli 100/x = 10/1 => x=10
 */
    if( (Sec = StopTime.tv_secs) < 10 )
    {
      SetAPen(PauzaRP, 0);
      Lenght = Sec * (100/10);
      RectFill(  PauzaRP, 17, 20, Lenght + 17, 35);
    }
    else
    {
      SetAPen( PauzaRP, 0);
      RectFill(  PauzaRP, 17, 20, 117, 35);
      Lenght = 100;
      IsEnd = TRUE;
    }

/************************************************************************/

     if( PauzaMsg=(struct IntuiMessage *)GetMsg( PauzaWdw->UserPort) )
     {

        Class = PauzaMsg->Class;      /* Save the IDCMP flag. */
        PressedGad = (struct Gadget *)PauzaMsg->IAddress;
        ReplyMsg( PauzaMsg );

        if( (Class==GADGETUP) && ((PressedGad->GadgetID)==EndOfPauzaID) )
        {
                  DisplayBeep( Screen );
                  IsEnd = TRUE;
        }
     }
    
     if(message = (struct IntuiMessage *)GetMsg(Window->UserPort))
     {
    iLAB:
          MessageClass = message->Class;
          MessageCode = message->Code;
          MessageAddress = message->IAddress;
          ReplyMsg(message);

        if( MessageClass == GADGETUP )
        {
            message = (struct IntuiMessage *)GetMsg(Window->UserPort);
            if( (message->Class) == GADGETUP)
              goto iLAB;
            else
            if((GadID = MessageAddress->GadgetID) == PieceToCompare)
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

              ClipBlit(RP,Xnext,Ynext, RP,Xdest,Ydest, PieceWidth,PieceHeight, 0xc0);

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
                          &my_bit_map, Xdest,Ydest, PieceWidth,PieceHeight,   /* Width and height.     */
                          0xC0, 0xFF, NULL );

                  for(loop=0; loop < (160-Lenght)+1; loop++)
                  {
                    SetAPen(RP, 0);
                    RectFill(  RP, 77, 245, Lenght + 77 + loop, 260);
                  }
                  TimeBonus = (Lenght*8)/PuzzleConfig.PauzeTime;
        
                  Score += TimeBonus;
                  PrintScore( Score );

                  Delay( 50 );
                  CloseWindow(PauzaWdw);
                  return( TRUE );
              }
              IsEnd = TRUE;
          }
          else
          {
            DisplayBeep(Screen);
            Score -= 1;
            PrintScore( Score );
            IsEnd = TRUE;
          }
        }
    }
  }

  CloseWindow(PauzaWdw);
  return( FALSE );
}
