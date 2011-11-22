#include <vxworks.h>
#include <stdio.h>
#include <semLib.h>
#include <time.h>
#include <taskLib.h>

#include "capteurs.h"
#include "conditionnement.h"
#include "communication.h"
#include "traitementCarton.h"

#define TIMEOUT WAIT_FOREVER

static int compteurCarton;
static int nbPiecesDefectueuses = 0;
static SEM_ID attentePiece;
static int numLotCourant;
static int typePieceLotCourant;

extern int compteurPalette;

void imprimerCarton(Carton c) {
	printf("[Imprimante] %d - Lot : %d (%d)\n", c.dateEmballage, c.numLot,
			c.typePiece);
}

void razNbPiecesDefectueuses() {
	nbPiecesDefectueuses = 0;
}

int traitementCarton(MSG_Q_ID fileConvoyage, SEM_ID semCapteurCarton,
		SEM_ID semEtatImp, SEM_ID semCompteurPalette, Lot* lotCourant) {

	int nbCartonsLotCourant = 0;
	attentePiece = semBCreate(0, SEM_EMPTY);

	FOREVER {

		int _capteurCarton;
		int _etatImp;
		int ret;
		Carton cartonCourant;
		int _compteurPalette;

		numLotCourant = lotCourant->numLot;
		typePieceLotCourant = lotCourant->typePiece;

		// Vérifier la présence d'un carton :

		semTake(semCapteurCarton, WAIT_FOREVER);
		_capteurCarton = capteurCarton;
		semGive(semCapteurCarton);

		if (_capteurCarton == OK) {
			compteurCarton = 0;
			message(CARTON_DEBUT, lotCourant->numLot, lotCourant->typePiece,
					NULL);
		} else {
			erreur(ABSENCE_CARTON);
		}

		// Attendre le remplissage du carton :

		ret = semTake(attentePiece, TIMEOUT);
		if (ret == ERROR && errno == S_objLib_OBJ_TIMEOUT) {
			erreur(PIECES_TIMEOUT);
		}

		message(CARTON_PLEIN, lotCourant->numLot, lotCourant->typePiece, NULL);

		nbCartonsLotCourant++;
		
		// Vérifier le seuil des pieces déféctueuses :
		if (nbPiecesDefectueuses >= seuilPiecesDefectueuses) {
			erreur(PIECES_DEFECTUEUSES);
		}

		// Vérifier l'état de l'imprimante :

		semTake(semEtatImp, WAIT_FOREVER);
		_etatImp = etatImp;
		semGive(semEtatImp);

		if (_etatImp != OK) {
			erreur(PANNE_IMPRIMANTE);
		}

		cartonCourant.numLot = lotCourant->numLot;
		cartonCourant.typePiece = lotCourant->typePiece;
		cartonCourant.dateEmballage = time(NULL);

		imprimerCarton(cartonCourant);
		message(CARTON_IMPRIME, lotCourant->numLot, lotCourant->typePiece,
				codeOperateur);

		// Ajouter le carton à la file de convoyage :

		ret = msgQSend(fileConvoyage, (char *) &cartonCourant,
				sizeof(Carton), NO_WAIT, MSG_PRI_NORMAL);
		if (ret == ERROR && errno == S_objLib_OBJ_UNAVAILABLE) {
			erreur(FILE_CARTONS_PLEINE);
		}

		// Changer éventuellement de lot :

		semTake(semCompteurPalette, WAIT_FOREVER);
		_compteurPalette = compteurPalette;
		semGive(semCompteurPalette);

		if (nbCartonsLotCourant >= lotCourant->nbPalettesMax
				* nbCartonsParPalette) {
			if (lotCourant->lotSuivant == NULL)
				pauseConditionnement();
			else {
				lotCourant = lotCourant->lotSuivant;
				nbCartonsLotCourant = 0;
			}
		}
	}

}

void ITCapteurDim() {
	int _capteurDim;

	_capteurDim = capteurDim;
	if (_capteurDim == OK) {
		compteurCarton++;
		message(PIECE_DANS_CARTON, numLotCourant, typePieceLotCourant,
				compteurCarton);

		if (compteurCarton >= nbPiecesParCarton) {
			semGive(attentePiece);
		}
	} else {
		nbPiecesDefectueuses++;
	}
}

