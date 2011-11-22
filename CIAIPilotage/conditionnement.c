#include <msgQLib.h>
#include <semLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <wdLib.h>

#include "conditionnement.h"
#include "communication.h"
#include "traitementCarton.h"
#include "traitementPalette.h"
#include "capteurs.h"

#define MAXFILECONVOYAGE 10

static int tidTraitementCarton;
static int tidTraitementPalette;
static SEM_ID semCapteurCarton;
static SEM_ID semEtatImp;
static SEM_ID semCapteurPalette;
static SEM_ID semEtatEmb;
static SEM_ID semCompteurPalette;
static SEM_ID semClapet;
static MSG_Q_ID fileConvoyage;
static WDOG_ID wdReparation;

volatile int compteurPalette;

int initConditionnement(Lot* premierLot) {
	semCapteurCarton = semMCreate(SEM_Q_FIFO);
	semEtatImp = semMCreate(SEM_Q_FIFO);
	semCapteurPalette = semMCreate(SEM_Q_FIFO);
	semEtatEmb = semMCreate(SEM_Q_FIFO);
	semCompteurPalette = semMCreate(SEM_Q_FIFO);
	semClapet = semMCreate(SEM_Q_FIFO);

	fileConvoyage = msgQCreate(MAXFILECONVOYAGE, sizeof(Carton), MSG_Q_FIFO
			| MSG_Q_INTERRUPTIBLE);

	tidTraitementCarton = taskSpawn("traitement_carton", 100, 0, 128,
			(FUNCPTR) traitementCarton, (int) fileConvoyage,
			(int) semCapteurCarton, (int) semEtatImp, (int) semCompteurPalette,
			(int) premierLot, 0, 0, 0, 0, 0);
	tidTraitementPalette = taskSpawn("traitement_palette", 100, 0, 128,
			(FUNCPTR) traitementPalette, (int) fileConvoyage,
			(int) semCapteurPalette, (int) semEtatEmb,
			(int) semCompteurPalette, 0, 0, 0, 0, 0, 0);

	wdReparation = wdCreate();

	return OK;
}

int arretConditionnement() {
	wdDelete(wdReparation);

	taskDelete(tidTraitementPalette);
	taskDelete(tidTraitementCarton);

	msgQDelete(fileConvoyage);

	semDelete(semClapet);
	semDelete(semCompteurPalette);
	semDelete(semEtatEmb);
	semDelete(semCapteurPalette);
	semDelete(semEtatImp);
	semDelete(semCapteurCarton);

	return OK;
}

int pauseConditionnement() {
	int ret1;
	int ret2;
	semTake(semClapet, WAIT_FOREVER);
	clapet = FERME;
	semGive(semClapet);

	message(CLAPET_FERME, NULL, NULL, NULL);

	// On suspend la tache en erreur :
	ret1 = taskSuspend(0);

	return OK;
}

int repriseConditionnement() {

	// On reprends l'exécution des deux tâches :
	taskResume(tidTraitementCarton);
	taskResume(tidTraitementPalette);

	semTake(semClapet, WAIT_FOREVER);
	clapet = OUVERT;
	semGive(semClapet);

	message(CLAPET_OUVERT, NULL, NULL, NULL);

	return OK;
}

// Watchdog
void surveilReparation(CODE_ERREUR codeErreur) {

	BOOL erreurReparee;
	int val;

	switch (codeErreur) {
	case ABSENCE_CARTON:
		val = capteurCarton;
		break;
	case PIECES_DEFECTUEUSES:
		val = OK; // On peut relancer le conditionnement dans tous les cas
		break;
	case PANNE_IMPRIMANTE:
		val = etatImp;
		break;
	case FILE_CARTONS_PLEINE:
		val = OK; // On peut relancer le conditionnement dans tous les cas
		break;
	case ABSENCE_PALETTE:
		val = capteurPalette;
		break;
	case PIECES_TIMEOUT:
		val = OK; // On peut relancer le conditionnement dans tous les cas
		break;
	case ERREUR_FILMAGE:
		val = etatEmb;
		break;
	default:
		val = OK;
	}

	if (val == OK) {
		message(ERREUR_REPAREE, codeErreur, NULL, NULL);
	} else {
		wdStart(wdReparation, sysClkRateGet() * 3, (FUNCPTR) surveilReparation,
				codeErreur);
	}
}

void surveillerReparation(CODE_ERREUR codeErreur) {
	wdStart(wdReparation, sysClkRateGet() * 3, (FUNCPTR) surveilReparation,
			codeErreur);
}
