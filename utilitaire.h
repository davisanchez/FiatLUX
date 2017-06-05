/* Nom: utlitaire.h
 * Description: module qui crée et fait des opérations sur des types concrets
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */

#include <stdbool.h>
 
#ifndef UTILITAIRE_H
#define UTILITAIRE_H

typedef struct point
{
	double posx;
	double posy;
	
}POINT;

typedef struct vecteur
{
	POINT deb;
	POINT fin;
	int dernier_reflechi;
	
}VECTEUR;

double distance(VECTEUR vect);
double distance2(POINT pos1, POINT pos2);
POINT vecteur_normal(POINT u);
double norme_vecteur (POINT vect);
double norme_vect(VECTEUR vect);
double produit_vectoriel(POINT u, POINT v);
double produit_scalaire(POINT u, POINT v);
POINT vecteur_unitaire(POINT u);

#endif
