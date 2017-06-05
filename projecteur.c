/* Nom: projecteur.c
 * Description: lecture et test de l'entité projecteur
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "projecteur.h"
#include "photon.h"
#include "error.h"
#include "graphic.h"
#include "utilitaire.h"
#include "constantes.h"

struct projecteur
{
	POINT pos;
	double alpha;
	PROJECTEUR* suivant;
	
};

static void vider_liste(PROJECTEUR *p);
static bool ajouter_projecteur(POINT pos, double alpha);

static PROJECTEUR *tetedeliste = NULL;
static PROJECTEUR* selec=NULL;
static int nb_projecteurs=0; 
static int nb_projecteurs_max=0;

//---------------------------------------------------------------------
// lecture et test du nb projecteurs

bool projecteur_lecture_nb(char* ligne, int* nb_proj)
{
	*nb_proj = 0;
	if(sscanf(ligne, "%d", &nb_projecteurs_max) != 1 ||
												nb_projecteurs_max<0)
    {
	   error_lect_nb_elements(ERR_PROJECTEUR);
	   return false;
    }
	*nb_proj = nb_projecteurs_max;
    return true;
}

//---------------------------------------------------------------------
// lecture, test des projecteurs

bool projecteur_lecture(char* ligne)
{
    PROJECTEUR* nouveau=NULL;
     //scane les fichiers et les mets dans la liste chainee
    if (!( nouveau = (PROJECTEUR*) malloc (sizeof(PROJECTEUR)))) 
    {
        printf("Pb d'allocation dans %s\n",__func__);
        return false;
    }
	if(sscanf(ligne,"%lf %lf %lf",
              &nouveau->pos.posx, 
              &nouveau->pos.posy,
              &nouveau->alpha) != 3)
														
	{	
		error_lecture_elements(ERR_PROJECTEUR, ERR_PAS_ASSEZ);
	    return false;
    } 
    if(!(ajouter_projecteur(nouveau->pos, nouveau->alpha)))
    {
		return false;
	}
	
    nb_projecteurs++;
    return true;	
}

/*--------------------------------------------------------------------*/
static bool ajouter_projecteur(POINT pos, double alpha) 
{
    PROJECTEUR* nouveau = NULL;
    if (!( nouveau = (PROJECTEUR*) malloc (sizeof(PROJECTEUR))))  
    {
        printf("Pb d'allocation dans %s\n",__func__);
        return false;
    }
    
    nouveau->pos.posx = pos.posx; //valeurs scannees en haut
    nouveau->pos.posy = pos.posy;
    nouveau->alpha = alpha;
    
    nouveau->suivant = tetedeliste;
    tetedeliste = nouveau;
    return true;
}

/*--------------------------------------------------------------------*/
int projecteur_info()
{
	return nb_projecteurs;
}

/*--------------------------------------------------------------------*/
void projecteur_save(FILE *fichier)
{
	PROJECTEUR* courant = tetedeliste;
	
	while(courant != NULL)
    {
        fprintf(fichier, "%g %g %g\n", 
				courant->pos.posx, courant->pos.posy, courant->alpha);
        courant = courant->suivant;
    }
    fprintf(fichier, "FIN_LISTE\n");
	
}

/*--------------------------------------------------------------------*/
void projecteur_dessine(PROJECTEUR* p)
{        
    graphic_set_line_width(2*EPAISS_TRAIT);
    
    POINT fin_proj;
	fin_proj.posx = p->pos.posx + 
								(NBPH-1)*EPSIL_PROJ*cos(p->alpha+M_PI/2);
	fin_proj.posy = p->pos.posy + 
								(NBPH-1)*EPSIL_PROJ*sin(p->alpha+M_PI/2);									
	
	if(p==selec)
		 graphic_set_color_3f(1,1,0);
	else
		graphic_set_color_3f(0,1,0);
		
	graphic_draw_segment(p->pos.posx, p->pos.posy,
								fin_proj.posx, fin_proj.posy);
    
}

void projecteur_dessine_tout(void)
{
	PROJECTEUR* courant = tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			projecteur_dessine(courant);
			courant = courant->suivant;
		}
}

/*--------------------------------------------------------------------*/
void projecteur_destruction(void)
{
	PROJECTEUR* elem = tetedeliste;
	
	while(elem)
	{
		vider_liste(elem);
		elem = elem->suivant;
	}
	
	nb_projecteurs_max = 0;
	nb_projecteurs = 0;
	
}

/*--------------------------------------------------------------------*/
static void vider_liste(PROJECTEUR *p)
{
	PROJECTEUR* courant = tetedeliste;
    PROJECTEUR* precedent = tetedeliste;
    
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
        nb_projecteurs--;
    }
}

/*--------------------------------------------------------------------*/
void projecteur_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def)
{	
	PROJECTEUR* courant=tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			float x = courant->pos.posx;
			float y = courant->pos.posy;
			
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
VECTEUR get_proj(int i)
{
	VECTEUR transformation_proj;
	PROJECTEUR* courant = tetedeliste;
	int a = 0;
	for(a=0; a != i; a++)
		courant=courant->suivant; 
		
	double alpha = courant->alpha;		
	transformation_proj.deb.posx = courant->pos.posx; 
	transformation_proj.deb.posy = courant->pos.posy;
	transformation_proj.fin.posx = courant->pos.posx + 
								(NBPH-1)*EPSIL_PROJ*cos(alpha+M_PI/2);
	transformation_proj.fin.posy = courant->pos.posy + 
								(NBPH-1)*EPSIL_PROJ*sin(alpha+M_PI/2);
	
	return transformation_proj; //renvoie le vecteur ième projecteur
	
}

/*--------------------------------------------------------------------*/
PROJECTEUR* get_pointeur_proj(PROJECTEUR* courant)   
{
	PROJECTEUR* temp = courant;
	
	return temp->suivant;
}				

/*--------------------------------------------------------------------*/	
PROJECTEUR* get_tete_proj(void)
{
	return tetedeliste;
}

/*--------------------------------------------------------------------*/
//créer les nouveaux photons
void projecteur_sim(void)
{
	PROJECTEUR *courant = tetedeliste;
	int i=0;
	POINT nouveau;
	
	if(tetedeliste)
	{
		while(courant!=NULL)
		{
			for(i=0;i<NBPH;i++)
			{
				//donne les coordonnées initiales des photons (avant proj)
				nouveau.posx = courant->pos.posx + 
							i*EPSIL_PROJ*cos((courant->alpha)+M_PI/2);
				nouveau.posy = courant->pos.posy + 
							i*EPSIL_PROJ*sin((courant->alpha)+M_PI/2);
		
				photon_transfert(nouveau, courant->alpha);
			}
			courant = courant->suivant;
		}
	}
}

/*--------------------------------------------------------------------*/
void projecteur_creation(POINT souris_p, POINT souris)
{
	double alpha = (atan2 ((souris.posy - souris_p.posy),
		(souris.posx - souris_p.posx)));
	ajouter_projecteur(souris_p, alpha);  //premier point:pos du proj
	nb_projecteurs++;
	nb_projecteurs_max++;
}

/*--------------------------------------------------------------------*/
//selectionne le projecteur aux point le plus proche
void projecteur_selection(POINT souris)
{
	PROJECTEUR* temp = tetedeliste;
	float distance=INF;
	float dist_deb=0, dist_fin=0, dist_inf=0;
	POINT fin_proj;
	
	while(temp)
	{	
		fin_proj.posx = temp->pos.posx + 
							(NBPH-1)*EPSIL_PROJ*cos(temp->alpha+M_PI/2);
		fin_proj.posy = temp->pos.posy + 
							(NBPH-1)*EPSIL_PROJ*sin(temp->alpha+M_PI/2);
		
		dist_deb = distance2(temp->pos,souris);
		dist_fin = distance2(fin_proj,souris);
	
		if(dist_deb<dist_fin)
			dist_inf = dist_deb;
		else
			dist_inf = dist_fin;;
		
		if(dist_inf<distance)
		{
			selec = temp;
			distance = dist_inf;
		}
		
		temp = temp->suivant;
	}
	
	projecteur_dessine(selec);
}

/*--------------------------------------------------------------------*/
void projecteur_suppression_selec(void)
{
	vider_liste(selec);
	
	selec = NULL;
}

/*--------------------------------------------------------------------*/
void enlever_tete_proj(void)
{
	PROJECTEUR* old = tetedeliste;
	tetedeliste=tetedeliste->suivant;
	
	free(old);
	old=NULL;
	
}
