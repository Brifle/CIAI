#ifndef TRAITEMENT_PALETTE
#define TRAITEMENT_PALETTE

#include <time.h>
#include <msgQLib.h>
#include <semLib.h>

int traitementPalette(MSG_Q_ID fileConvoyage, SEM_ID semCapteurPalette, SEM_ID semEtatEmb,
		SEM_ID semCompteurPalette);

#endif
