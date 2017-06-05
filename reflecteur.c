/* Nom: reflecteur.c
 * Description: lecture et test de l'entité reflecteur
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "reflecteur.h"
#include "error.h"
#include "graphic.h"
#include "utilitaire.h"
#include "constantes.h"

struct reflecteur
{
	VECTEUR refl;
	int id;
	REFLECTEUR* suivant;
};

static void vider_liste(REFLECTEUR *p);
static bool ajouter_reflecteur(VECTEUR refl, int* id);

static REFLECTEUR *tetedeliste = NULL; 
static REFLECTEUR* selec=NULL; 
static int nb_reflecteurs=0; 
static int nb_reflecteurs_max=0;

//---------------------------------------------------------------------
// lecture et test du nb reflecteurs

bool reflecteur_lecture_nb(char* ligne, int* nb_refl)
{
	*nb_refl = 0;
	
	if(sscanf(ligne,"%d",&nb_reflecteurs_max)!=1 || nb_reflecteurs_max<0)
    {
	   error_lect_nb_elements(ERR_REFLECTEUR);
	   return false;
    }
	*nb_refl = nb_reflecteurs_max;
    return true;
}

//---------------------------------------------------------------------
// lecture, test des reflecteurs

bool reflecteur_lecture(char *ligne, int *i)
{	
	int a = *i;
    REFLECTEUR* nouveau=NULL;
    if (!( nouveau = (REFLECTEUR*) malloc (sizeof(REFLECTEUR))))
    {
        printf("Pb d'allocation dans %s\n",__func__);
        return false;
    }
	if(sscanf(ligne,"%lf %lf %lf %lf",
              &(nouveau->refl.deb.posx),    			
              &(nouveau->refl.deb.posy),
              &(nouveau->refl.fin.posx),
              &(nouveau->refl.fin.posy)) != 4)
	{
		error_lecture_elements(ERR_REFLECTEUR, ERR_PAS_ASSEZ);
	    return false;
    }
   
//---------------------------------------------------------------------
// test distance entre deux points d'un réflecteur

	if(distance(nouveau->refl) <= EPSIL_CREATION)
	{
			error_lecture_point_trop_proche(ERR_REFLECTEUR,
														nb_reflecteurs);
			return false;
	}

    if(!(ajouter_reflecteur(nouveau->refl, &a)))
    {
		return false;
	}
	
    nb_reflecteurs++;
    *i = *(i)+1;
    return true;
}

/*--------------------------------------------------------------------*/
static bool ajouter_reflecteur(VECTEUR refl, int* id)
{
    REFLECTEUR* nouveau = NULL;
    if (!( nouveau = (REFLECTEUR*) malloc (sizeof(REFLECTEUR)))) 
    {
        printf("Pb d'allocation dans %s\n",__func__);
        return false;
    }
    
    nouveau->refl.deb.posx = refl.deb.posx;
    nouveau->refl.fin.posx = refl.fin.posx;    
    nouveau->refl.deb.posy = refl.deb.posy;
    nouveau->refl.fin.posy = refl.fin.posy;
    nouveau->id = *id;
    
    nouveau->suivant = tetedeliste; 
    tetedeliste = nouveau;
    return true;
    
}

/*--------------------------------------------------------------------*/
int reflecteur_info(void)
{
	return nb_reflecteurs;
}

/*--------------------------------------------------------------------*/
void reflecteur_save(FILE *fichier)
{
	REFLECTEUR* courant = tetedeliste;
	
	while(courant != NULL)
    {
        fprintf(fichier, "%g %g %g %g\n", courant->refl.deb.posx,
												courant->refl.deb.posy, 
												courant->refl.fin.posx,
												courant->refl.fin.posy);
        courant = courant->suivant;
    }
    fprintf(fichier, "FIN_LISTE\n");
	
}

/*--------------------------------------------------------------------*/
void reflecteur_dessine_tout(void)
{
	REFLECTEUR* courant = tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			reflecteur_dessine(courant);
			courant = courant->suivant;
		}
}

/*--------------------------------------------------------------------*/
void reflecteur_dessine(REFLECTEUR* p)
{
    graphic_set_line_width(EPAISS_TRAIT);									
	
	if(p==selec)
		 graphic_set_color_3f(1,1,0);
	else
		graphic_set_color_3f(1,0,0);
		
	graphic_draw_segment(p->refl.deb.posx, p->refl.deb.posy,
								p->refl.fin.posx, p->refl.fin.posy);
}

/*--------------------------------------------------------------------*/
void reflecteur_destruction(void)
{
	REFLECTEUR* elem = tetedeliste;
	
	while(elem)
	{
		vider_liste(elem);
		elem = elem->suivant;
	}
	
	nb_reflecteurs_max = 0;
	nb_reflecteurs = 0;
	
}

/*--------------------------------------------------------------------*/
static void vider_liste(REFLECTEUR *p)
{
	REFLECTEUR* courant = tetedeliste;
    REFLECTEUR* precedent = tetedeliste;
    
    while(courant !=p && courant)
    {
		precedent = courant;
		courant = courant->suivant;
	}
	
	if(courant != NULL)
    {
        if (courant != tetedeliste)
        {
            precedent -> suivant = courant -> suivant;
            free (courant);
        }
        else
        {
            tetedeliste = courant -> suivant;
            free(courant);
        }
        nb_reflecteurs--;
    }
}
	

/*--------------------------------------------------------------------*/
void reflecteur_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def)
{
	
	REFLECTEUR* courant=tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			float x = courant->refl.deb.posx;
			float y = courant->refl.deb.posy;
			
			if(*def)
			{
				*x_max = *x_min = x;
				*y_max = *y_min = y;
				*def = false;
			}
			
			*x_max = (x>*x_max) ? x : *x_max;
			*y_max = (y>*y_max) ? y : *y_max;
			*x_min = (x<*x_min) ? x : *x_min;
			*y_min = (y<*y_min) ? y : *y_min;
			
			x = courant->refl.fin.posx;
			y = courant->refl.fin.posy;
			
			if(*def)
			{
				*x_max = *x_min = x;
				*y_max = *y_min = y;
				*def = false;
			}
			
			*x_max = (x>*x_max) ? x : *x_max;
			*y_max = (y>*y_max) ? y : *y_max;
			*x_min = (x<*x_min) ? x : *x_min;
			*y_min = (y<*y_min) ? y : *y_min;
				
			courant = courant->suivant;
		}
}

/*--------------------------------------------------------------------*/
VECTEUR reflecteur_get_vecteur(int a)
{
    REFLECTEUR* temp = tetedeliste;
     
    int i=0;
     
    while(i!=a)
    {
        temp = temp->suivant;
        i++;
    }
     
    return temp->refl;
}
	
/*--------------------------------------------------------------------*/
REFLECTEUR* get_pointeur_refl(REFLECTEUR* courant)   
{
	REFLECTEUR* temp = courant;
	
	return temp->suivant;
}

/*--------------------------------------------------------------------*/
int get_id_refl(int a)
{
	int i=0;
	int id_r;
	for(i=1;i<reflecteur_info();i++)
			id_r = i;
		
	id_r=id_r-a+1;
		
	return id_r;
}

/*--------------------------------------------------------------------*/
void reflecteur_creation(POINT souris_p, POINT souris)
{
	VECTEUR c_refl;
	c_refl.deb.posx = souris_p.posx;
	c_refl.deb.posy = souris_p.posy;
	c_refl.fin.posx = souris.posx;
	c_refl.fin.posy = souris.posy;
	int id=nb_reflecteurs+1;
	
							
	ajouter_reflecteur(c_refl, &id);
	
	nb_reflecteurs++;
	nb_reflecteurs_max++;
}

/*--------------------------------------------------------------------*/
void reflecteur_selection(POINT souris)
{
	REFLECTEUR* temp = tetedeliste;
	float distance=INF;
	float dist_deb=0, dist_fin=0, dist_inf=0;
	
	while(temp)
	{	
		dist_deb = distance2(temp->refl.deb,souris);
		dist_fin = distance2(temp->refl.fin,souris);
	
		if(dist_deb<dist_fin)
			dist_inf = dist_deb;
		else
			dist_inf = dist_fin;
		
		if(dist_inf<distance)
		{
			selec = temp;
			distance = dist_inf;
		}
		
		temp = temp->suivant;
	}
	
	reflecteur_dessine(selec);
}

/*--------------------------------------------------------------------*/
//supprime le reflecteur selectionné
void reflecteur_suppression_selec(void)
{
	vider_liste(selec);
	
	selec = NULL;
}

/*--------------------------------------------------------------------*/
//supprime le reflecteur à la tete de liste
void enlever_tete_refl(void)
{
	REFLECTEUR* old = tetedeliste;
	tetedeliste=tetedeliste->suivant;
	
	free(old);
	old=NULL;
	
}

