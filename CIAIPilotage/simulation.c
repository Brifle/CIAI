#include <vxworks.h>
#include <wdLib.h>
#include <sysLib.h>
#include <time.h>
#include <stdlib.h>

#include "capteurs.h"
#include "traitementCarton.h"

#define TAUX_PIECE_DEFECTUEUSE 10
#define VITESSE_ARRIVEE_PIECE 2
#define TAUX_PANNE_IMPRIMANTE 1000
#define TAUX_REPARATION_IMPRIMANTE 60
#define TAUX_PANNE_EMBALLAGE 1000
#define TAUX_REPARATION_EMBALLAGE 60
#define TAUX_ABS_CARTON 1000
#define TAUX_PRES_CARTON 60
#define TAUX_ABS_PALETTE 1000
#define TAUX_PRES_PALETTE 60

static WDOG_ID wdProdPiece;
static WDOG_ID wdPanneImprimante;
static WDOG_ID wdAbscenceCarton;
static WDOG_ID wdPanneEmballage;
static WDOG_ID wdAbscencePalette;

void productionPiece() {
	int val;

	if (clapet == OUVERT) {
		capteurDim = ((rand() % TAUX_PIECE_DEFECTUEUSE) == 0);
		ITCapteurDim();
	}

	//Temps avant la prochaine piece
	val = rand() % VITESSE_ARRIVEE_PIECE;
	wdStart(wdProdPiece, sysClkRateGet() * val, (FUNCPTR) productionPiece, NULL);
}

void panneImprimante() {
	int val;

	etatImp = !etatImp;

	if (etatImp) {
		val = rand() % TAUX_PANNE_IMPRIMANTE;
	} else {
		val = rand() % TAUX_REPARATION_IMPRIMANTE;
	}

	wdStart(wdPanneImprimante, sysClkRateGet() * val,
			(FUNCPTR) panneImprimante, NULL);
}

void panneEmballage() {
	int val;

	etatEmb = !etatEmb;

	if (etatEmb) {
		val = rand() % TAUX_PANNE_EMBALLAGE;
	} else {
		val = rand() % TAUX_REPARATION_EMBALLAGE;
	}

	wdStart(wdPanneEmballage, sysClkRateGet() * val, (FUNCPTR) panneEmballage,
			NULL);
}

void absenceCarton() {
	int val;

	capteurCarton = !capteurCarton;

	if (capteurCarton) {
		val = rand() % TAUX_ABS_CARTON;
	} else {
		val = rand() % TAUX_PRES_CARTON;
	}

	wdStart(wdAbscenceCarton, sysClkRateGet() * val, (FUNCPTR) absenceCarton,
			NULL);
}

void absencePalette() {
	int val;

	capteurPalette = !capteurPalette;

	if (capteurPalette) {
		val = rand() % TAUX_ABS_PALETTE;
	} else {
		val = rand() % TAUX_PRES_PALETTE;
	}

	wdStart(wdAbscencePalette, sysClkRateGet() * val, (FUNCPTR) absencePalette,
			NULL);
}

void initSimulation() {
	int val;

	srand(time(NULL));
	
	wdProdPiece = wdCreate();
	wdPanneImprimante = wdCreate();
	wdAbscenceCarton = wdCreate();
	wdPanneEmballage = wdCreate();
	wdAbscencePalette = wdCreate();

	val = rand() % VITESSE_ARRIVEE_PIECE;
	wdStart(wdProdPiece, sysClkRateGet() * val, (FUNCPTR) productionPiece, NULL);

	val = rand() % TAUX_PANNE_IMPRIMANTE;
	wdStart(wdPanneImprimante, sysClkRateGet() * val,
			(FUNCPTR) panneImprimante, NULL);

	val = rand() % TAUX_PANNE_EMBALLAGE;
	wdStart(wdPanneEmballage, sysClkRateGet() * val, (FUNCPTR) panneEmballage,
			NULL);

	val = rand() % TAUX_ABS_CARTON;
	wdStart(wdAbscenceCarton, sysClkRateGet() * val, (FUNCPTR) absenceCarton,
			NULL);

	val = rand() % TAUX_ABS_PALETTE;
	wdStart(wdAbscencePalette, sysClkRateGet() * val, (FUNCPTR) absencePalette,
			NULL);
}

void arretSimulation() {
	wdDelete(wdProdPiece);
	wdDelete(wdPanneImprimante);
	wdDelete(wdAbscenceCarton);
	wdDelete(wdPanneEmballage);
	wdDelete(wdAbscencePalette);
}


