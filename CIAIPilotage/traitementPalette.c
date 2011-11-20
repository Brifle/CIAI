#include <vxworks.h>
#include <stdio.h>
#include <semLib.h>
#include <time.h>

#include "capteurs.h"
#include "conditionnement.h"
#include "communication.h"
#include "stockage.h"
#include "traitementCarton.h"
#include "traitementPalette.h"

extern int compteurPalette;

int filmPlastique(SEM_ID semEtatEmb) {
	int _etatEmb;
	
	semTake(semEtatEmb, WAIT_FOREVER);
	_etatEmb = etatEmb;
	semGive(semEtatEmb);
	
	return _etatEmb;
}

int traitementPalette(MSG_Q_ID fileConvoyage, SEM_ID semCapteurPalette,
		SEM_ID semEtatEmb, SEM_ID semCompteurPalette) {

	FOREVER {

		int _capteurPalette;
		Carton cartonCourant;
		int _compteurPalette;

		// Vérifier la présence d'une palette :

		semTake(semCapteurPalette, WAIT_FOREVER);
		_capteurPalette = capteurPalette;
		semGive(semCapteurPalette);

		if (_capteurPalette == OK) {
			semTake(semCompteurPalette, WAIT_FOREVER);
			compteurPalette = 0;
			semGive(semCompteurPalette);
		} else {
			erreur(ABSENCE_PALETTE);
		}
		
		// Remplir la palette :

		do {

			// Attendre l'arrivée d'un carton dans la file de convoyage :
			msgQReceive(fileConvoyage, (char *) &cartonCourant,
					sizeof(Carton), WAIT_FOREVER);

			semTake(semCompteurPalette, WAIT_FOREVER);
			_compteurPalette = ++compteurPalette;
			semGive(semCompteurPalette);
			
			message(CARTON_DANS_PALETTE, cartonCourant.numLot, cartonCourant.typePiece, _compteurPalette);

		} while (_compteurPalette < nbCartonsParPalette);
		
		// Appliquer le film plastique sur la palette :
		
		if(filmPlastique(semEtatEmb) != OK) {
			erreur(ERREUR_FILMAGE);
		}
		
		stockerPalette(cartonCourant.numLot, cartonCourant.typePiece);
		
		message(PALETTE_PLEINE, cartonCourant.numLot, cartonCourant.typePiece, NULL);
		log(PALETTE_PLEINE, cartonCourant.numLot, cartonCourant.typePiece, time(NULL));

	}

}
