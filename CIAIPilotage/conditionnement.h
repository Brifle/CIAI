
#ifndef CONDITIONNEMENT
#define CONDITIONNEMENT

typedef struct _Lot {
	int numLot;
	int typePiece;
	int nbPalettesMax;
	int nbPalettesCourant;
	struct _Lot* lotSuivant;
} Lot;

volatile int nbPiecesParCarton;

volatile int nbCartonsParPalette;

volatile int seuilPiecesDefectueuses;

volatile int codeOperateur;

int initConditionnement(Lot* premierLot);

int arretConditionnement();

int pauseConditionnement();

int repriseConditionnement();

void surveillerReparation(int codeErreur);

#endif
