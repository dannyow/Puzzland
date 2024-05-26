#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <exec/memory.h>
#include <exec/types.h>

#define HAM_WIDTH			320
#define HAM_HEIGHT		256
#define HAM_DEPTH			6 /* HAM - 6 bitplanes */

#define HAMS_NAME	"Puzzland.pic"

#define HAM_SIZE 			61504 /* RASSIZE(HAM_WIDTH, HAM_HEIGHT) * HAM_DEPTH + 32 * 2 */
#define PIECE_WIDTH 	32
#define PIECE_HEIGHT	32

struct NewScreen NewHAMScr=
	{
	0,0, HAM_WIDTH, HAM_HEIGHT, HAM_DEPTH,0,0,HAM, CUSTOMSCREEN, NULL, NULL,0,0
	};

struct NewWindow NewHAMWdw=
	{
	0,0,HAM_WIDTH,HAM_HEIGHT,0,0,MOUSEBUTTONS,
	ACTIVATE|BACKDROP|BORDERLESS|RMBTRAP, /* Flags */
	NULL,0,0,0,0,0,0,0,0,CUSTOMSCREEN
	};

struct Screen *HAMScreen;
struct Window *HAMWindow;

struct BitMap ScreenBitMap, UnPackBitMap;

LONG PackedFileSize = 0;
char *SourcePacked = NULL,
		 *ChipDest = NULL;

extern void UnPack( char *, char * ); /* asm routine */

BOOL ReadPacked()
{
  struct FileLock *lock = NULL;
  struct FileInfoBlock *fib_ptr = NULL; 
  struct FileHandle *file_handle = NULL;
	LONG read_lenght = 0;
	

  /* Allocate enough memory for a FileInfoBlock structure: */
  fib_ptr = (struct FileInfoBlock *)AllocMem( sizeof( struct FileInfoBlock ),
											                      MEMF_PUBLIC | MEMF_CLEAR );

  if( !fib_ptr )
		return( FALSE );
  
  if( !( lock = (struct FileLock *) Lock( HAMS_NAME, SHARED_LOCK )) )
  {
    /* Deallocate the memory we have allocated: */
    FreeMem( fib_ptr, sizeof( struct FileInfoBlock ) );
		return( FALSE );    
  }

  /* Try to examine the file */
  if( Examine( lock, fib_ptr ) )
		PackedFileSize = fib_ptr->fib_Size;
	else
  {
	  /* Unlock the file: */
  	UnLock( lock );  
	  /* Deallocate the memory we have allocated: */
  	FreeMem( fib_ptr, sizeof( struct FileInfoBlock ) );
		return(FALSE);	
	}
  /* Unlock the file: */
 	UnLock( lock );  
  /* Deallocate the memory we have allocated: */
 	FreeMem( fib_ptr, sizeof( struct FileInfoBlock ) );

 /* Allocate memory for packed picture */
	SourcePacked = (char *)AllocMem( PackedFileSize, MEMF_PUBLIC | MEMF_CLEAR );
	if( !SourcePacked )
		return( FALSE );

  /* Have we opened the file successfully? */
  if( !(file_handle = (struct FileHandle *)Open( HAMS_NAME, MODE_OLDFILE )) )
  {
		FreeMem( SourcePacked, PackedFileSize );
		return( FALSE );
  }

  read_lenght = Read( file_handle, SourcePacked, PackedFileSize);
	if( read_lenght != PackedFileSize)
	{
    Close( file_handle );
		FreeMem( SourcePacked, PackedFileSize );
		return( FALSE );
	}

  Close( file_handle );
	return( TRUE );	
}

BOOL PicHAM()
{
	register SHORT loop, x, y;
	UBYTE hash[80] =
	{
	66, 27, 52, 3, 26, 5, 6, 63, 4, 21, 32, 47, 28, 39, 14, 74, 23, 56, 16, 25, 
	48, 72, 18, 36, 22, 55, 8, 77, 59, 12, 58, 31, 51, 17, 69, 67, 60, 37, 61, 46, 
	78, 41, 65, 7, 34, 71, 49, 35, 24, 15, 19, 68, 10, 2, 0, 53, 20, 57, 40, 9, 
	42, 54, 70, 43, 11, 44, 29, 76, 45, 33, 62, 30, 50, 73, 38, 75, 64, 1, 13, 79, 
	};
	BOOL error = TRUE;

	/* open the screen in HAM mode */  
  if(!(HAMScreen=(struct Screen *)OpenScreen(&NewHAMScr)))
		goto Exit;

 	ScreenBitMap = HAMScreen->BitMap;

	NewHAMWdw.Screen=HAMScreen;
	if(!(HAMWindow=(struct Window *)OpenWindow( &NewHAMWdw ))) 
		goto Exit;

	ShowTitle( HAMScreen, FALSE );

	if( !ReadPacked() )
		goto Exit;


	ChipDest = (char *)AllocMem( HAM_SIZE, MEMF_CHIP | MEMF_CLEAR );
	if( !ChipDest )
		goto Exit;


	UnPack( SourcePacked, ChipDest ); 

/*
 * Allocatred in ReadPacked()
 */
	if (SourcePacked) FreeMem( SourcePacked, PackedFileSize );

 	LoadRGB4(&HAMScreen->ViewPort, ChipDest+(HAM_DEPTH*RASSIZE(HAM_WIDTH,HAM_HEIGHT)), 32);

	InitBitMap( &UnPackBitMap, HAM_DEPTH, HAM_WIDTH, HAM_HEIGHT );

  for( loop = 0; loop < HAM_DEPTH; loop++ )
	{
		UnPackBitMap.Planes[loop] =
											( ChipDest + loop * (RASSIZE( HAM_WIDTH, HAM_HEIGHT )));
	} 

	for(loop=0; loop<((HAM_WIDTH/PIECE_WIDTH)*(HAM_HEIGHT/PIECE_HEIGHT)); loop++)
	{
		x = hash[loop]%(HAM_WIDTH/PIECE_WIDTH) * PIECE_WIDTH;
		y = hash[loop]/(HAM_WIDTH/PIECE_WIDTH) * PIECE_HEIGHT;
	
		BltBitMap(&UnPackBitMap, x, y,
						  &ScreenBitMap, x, y, PIECE_WIDTH, PIECE_HEIGHT,
						  0xC0, 0xFF, NULL );

		Delay( 1 );
	}

	Wait( 1<<HAMWindow->UserPort->mp_SigBit );

/* clear destination */
	for(loop=0; loop<((HAM_WIDTH/PIECE_WIDTH)*(HAM_HEIGHT/PIECE_HEIGHT)); loop++)
	{
		x = hash[loop]%(HAM_WIDTH/PIECE_WIDTH) * PIECE_WIDTH;
		y = hash[loop]/(HAM_WIDTH/PIECE_WIDTH) * PIECE_HEIGHT;
	
		BltBitMap(&UnPackBitMap, x, y,
						  &ScreenBitMap, x, y, PIECE_WIDTH, PIECE_HEIGHT,
						  0x00, 0xFF, NULL );
		Delay( 1 );
	}

	error = FALSE;

Exit:
  if (HAMWindow) CloseWindow(HAMWindow);
  if (HAMScreen) CloseScreen(HAMScreen);
	if (ChipDest) FreeMem( ChipDest, HAM_SIZE );
	if ( error ) 
		return(FALSE);
	else
		return( TRUE );
}
