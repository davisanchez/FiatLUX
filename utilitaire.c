/* Nom: utilitaire.c
 * Description: module qui crée et fait des opératons sur des types concrets
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "utilitaire.h"
#include "constantes.h"

//---------------------------------------------------------------------
// distance d'un vecteur
double distance(VECTEUR vect)
{

	double dist = 0;

	dist = sqrt(pow((vect.fin.posx-vect.deb.posx),2) +
								pow((vect.fin.posy-vect.deb.posy),2));

	return dist;

}

//---------------------------------------------------------------------
//distance entre 2 points
double distance2(POINT pos1, POINT pos2)
{	
	double dist = 0;

	dist = sqrt(pow((pos1.posx-pos2.posx),2) +
										pow((pos1.posy-pos2.posy),2));
	
	return dist;
}

/*--------------------------------------------------------------------*/
//calcul le vecteur normal
POINT vecteur_normal(POINT u)
{
	POINT n;
    
    n.posx = -u.posy;
    n.posy = u.posx;

    return n;
}

/*--------------------------------------------------------------------*/
//calcul le vecteur unitaire
POINT vecteur_unitaire(POINT v)
{
	POINT u;
	u.posx = v.posx/norme_vecteur(v); //meme debut pour les 2 vecteurs
	u.posy = v.posy/norme_vecteur(v);

	return u;
}

/*--------------------------------------------------------------------*/
//calcul la norme
double norme_vecteur(POINT vect)
{
	double norme = 0;
	
	norme = sqrt(pow(vect.posx,2) + pow(vect.posy,2));
	return norme;	
}

/*--------------------------------------------------------------------*/
//calcul la norme
double norme_vect(VECTEUR vect)
{
   return sqrt(pow(vect.deb.posx-vect.fin.posx,2)+
									pow(vect.deb.posy-vect.fin.posy,2));
}

/*--------------------------------------------------------------------*/
//calcul le produit vectoriel
double produit_vectoriel(POINT u, POINT v)
{
	double pvect = 0;
	
	pvect = u.posx*v.posy-u.posy*v.posx; 
		   
	return pvect;
}

/*--------------------------------------------------------------------*/
// calcul le produit scalaire
double produit_scalaire(POINT u, POINT v)
{
	double pscal = 0;
	
	pscal = u.posx*v.posx + v.posy*u.posy;
		   
	return pscal;
}

