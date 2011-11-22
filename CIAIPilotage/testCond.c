#include <vxworks.h>
#include <stdio.h>
#include <semLib.h>
#include <sysLib.h>
#include <taskLib.h>

#include "conditionnement.h"
#include "traitementCarton.h"
#include "traitementPalette.h"
#include "communication.h"
#include "capteurs.h"

#define TEST_TIMEOUT 1

static Lot l1;
static Lot l2;
static SEM_ID semAttenteStockage;
static CODE_ERREUR derniereErreur;

void setUp() {
	semAttenteStockage = semBCreate(0, SEM_EMPTY);

	capteurDim = OK;
	capteurCarton = OK;
	etatImp = OK;
	capteurPalette = OK;
	etatEmb = OK;
	clapet = OUVERT;

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
	taskDelay(sysClkRateGet() * 1);
}

void tearDown() {
	semDelete(semAttenteStockage);

	arretConditionnement();
}

int testStockerPalette() {
	int ret;
	int success = FALSE;

	nbPiecesParCarton = 1;
	nbCartonsParPalette = 1;
	seuilPiecesDefectueuses = 10;

	ITCapteurDim();

	ret = semTake(semAttenteStockage, TEST_TIMEOUT * sysClkRateGet());
	if (ret == OK) {
		success = TRUE;
	}

	return success;
}

int testPanne(CODE_ERREUR c) {
	int ret;
	int success = FALSE;

	arretConditionnement(); // On veut d'abord arrêter le conditionnement en cours

	// On initialise la panne :
	switch (c) {
	case ABSENCE_CARTON:
		capteurCarton = ERROR;
		break;
	case PANNE_IMPRIMANTE:
		etatImp = ERROR;
		break;
	case ABSENCE_PALETTE:
		capteurPalette = ERROR;
		break;
	case ERREUR_FILMAGE:
		etatEmb = ERROR;
		break;
	}

	initConditionnement(&l1); // On relance le conditionnement
	taskDelay(sysClkRateGet() * 1);

	nbPiecesParCarton = 1;
	nbCartonsParPalette = 1;
	seuilPiecesDefectueuses = 10;

	ITCapteurDim();

	ret = semTake(semAttenteStockage, TEST_TIMEOUT * sysClkRateGet());
	if (ret == ERROR) {
		if (derniereErreur == c)
			success = TRUE;
	}

	return success;
}

int testPanneCarton() {
	return testPanne(ABSENCE_CARTON);
}

int testPannePalette() {
	return testPanne(ABSENCE_PALETTE);
}

int testPanneImprimante() {
	return testPanne(PANNE_IMPRIMANTE);
}

int testPanneFilmage() {
	return testPanne(ERREUR_FILMAGE);
}

int testSeuilPiecesDefectueuses() {
	int ret;
		int success = FALSE;

		nbPiecesParCarton = 1;
		nbCartonsParPalette = 1;
		seuilPiecesDefectueuses = 1;

		// On initialise la panne :
		capteurDim = ERROR;
		ITCapteurDim();
		taskDelay(sysClkRateGet() * 1);
		
		capteurDim = OK;
		ITCapteurDim();

		ret = semTake(semAttenteStockage, TEST_TIMEOUT * sysClkRateGet());
		if (ret == ERROR) {
			if (derniereErreur == PIECES_DEFECTUEUSES)
				success = TRUE;
		}

		return success;
}

void log(CODE_EVENEMENT c, int v1, int v2, int v3) {
	printf("[%d] Palette stockee lot %d (%d)\n", v3, v1, v2);
}

void erreur(CODE_ERREUR c) {
	printf("ERREUR !\n");
	derniereErreur = c;
	pauseConditionnement();
}

void message(CODE_EVENEMENT c, int v1, int v2, int v3) {
	printf("MESSAGE !\n");
}

void stockerPalette(int v1, int v2) {
	printf("ON STOCKE !\n");
	semGive(semAttenteStockage);
}
