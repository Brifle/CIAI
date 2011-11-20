
#ifndef CAPTEURS
#define CAPTEURS

#include <semLib.h>

#define OUVERT 1
#define FERME 0

volatile int capteurDim = OK;

volatile int capteurCarton = OK;

volatile int etatImp = OK;

volatile int capteurPalette = OK;

volatile int etatEmb = OK;

volatile int clapet = OUVERT;

#endif
