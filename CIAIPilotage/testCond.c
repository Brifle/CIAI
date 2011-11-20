
#include <vxworks.h>
#include <stdio.h>
#include "conditionnement.h"
#include "communication.h"

Lot l1;
Lot l2;

void test() {
	
	nbPiecesParCarton = 1;
	nbCartonsParPalette = 2;
	seuilPiecesDefectueuses = 10;
	
	l1.numLot = 1;
	l1.nbPalettesCourant = 0;
	l1.nbPalettesMax = 1;
	l1.typePiece = 0;
	l1.lotSuivant = &l2;
	l2.numLot = 2;
	l2.nbPalettesCourant = 0;
	l2.nbPalettesMax = 1;
	l2.typePiece = 1;
	l2.lotSuivant = NULL;
	
	initConditionnement(&l1);
	
}

void fin() {
	arretConditionnement();
}

void log(CODE_EVENEMENT c, int v1, int v2, int v3) {
	printf("[%d] Palette stockee lot %d (%d)\n", v3, v1, v2);
}

void erreur(CODE_ERREUR c) {
	printf("ERREUR !\n");
}

void message(CODE_EVENEMENT c, int v1, int v2, int v3) {
	printf("MESSAGE !\n");
}

void stockerPalette(int v1, int v2) {
	printf("ON STOCKE !\n");
}
