#ifndef TRAITEMENT_CARTON
#define TRAITEMENT_CARTON

#include <time.h>
#include <msgQLib.h>
#include <semLib.h>

#include "conditionnement.h"

typedef struct {
	int numLot;
	int typePiece;
	time_t dateEmballage;
} Carton;

void razNbPiecesDefectueuses();

void ITCapteurDim();

int traitementCarton(MSG_Q_ID fileConvoyage, SEM_ID semCapteurCarton,
		SEM_ID semEtatImp, SEM_ID semCompteurPalette, Lot* lotCourant);

#endif
