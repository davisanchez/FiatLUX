/* Nom: modele.c
 * Description: module qui gère les opérations de lecture, écriture etc.
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "modele.h"
#include "error.h"
#include "absorbeur.h"
#include "reflecteur.h"
#include "projecteur.h"
#include "photon.h"
#include "utilitaire.h"
#include "constantes.h"


static int decodage_ligne(char *ligne);
static POINT pos_mouse;
static bool decodage_fin_liste(char *ligne, ERREUR_ORIG origine);

enum Etat_lecture {
	NB_PHOTON,	PHOTONS,	FIN_PHOTON, 
	NB_ABSORBEUR, 	ABSORBEURS,  	FIN_ABSORBEUR,
	NB_REFLECTEUR, 	REFLECTEURS,  	FIN_REFLECTEUR,
	NB_PROJECTEUR,  PROJECTEURS,	FIN_PROJECTEUR, FIN_PARTICULE
};

typedef enum Lecture {
	LECTURE_SUCCES, LECTURE_ERREUR, LECTURE_CONTINUE
	}LECTURE;
	
typedef enum Verification {
	VERIF_SUCCES, VERIF_ERREUR}VERIFICATION;

/*--------------------------------------------------------------------*/
void modele_lecture(int mode, char* nom_fichier)
{
	
	switch (mode)
	{
		case MODE_ERROR:	if(modele_lecture_fichier(nom_fichier) !=
														LECTURE_SUCCES)
								exit(EXIT_FAILURE);
							else
								error_success();
								exit(EXIT_SUCCESS);
							break;
							
		case MODE_VERIF:	if(modele_lecture_fichier(nom_fichier)
													!= LECTURE_SUCCES)
								exit(EXIT_FAILURE);
			
							if(modele_verification_rendu2() == true)
							{
								error_success();
								exit(EXIT_SUCCESS);
							}
							else
								exit(EXIT_FAILURE);	
							break;
							
		case MODE_GRAPHIC:	modele_load(nom_fichier);
							break;
							
		case MODE_FINAL:	modele_load(nom_fichier);
							mode = MODE_DEFAULT;
							break;
		default :
							error_msg((char*)"mode incorrect");
							exit(EXIT_FAILURE);
	}
}

/*--------------------------------------------------------------------*/
int modele_lecture_fichier(char* nom_fichier)
{
	char ligne[MAX_LINE];
	FILE *fichier = NULL;
	
		
	if((fichier = fopen(nom_fichier, "r")) != NULL)
	{
		decodage_ligne(NULL);
		while(fgets(ligne,MAX_LINE,fichier))
		{
			if((ligne[0]=='#')||(ligne[0]=='\n')||(ligne[0]=='\r'))
				continue;
			int result = decodage_ligne(ligne);
			
			if (result == LECTURE_CONTINUE) continue;
			else if(result == LECTURE_ERREUR) return LECTURE_ERREUR;
			
			fclose(fichier);
			return LECTURE_SUCCES;
		}
			
			error_fichier_incomplet();
			
			fclose(fichier);
			return LECTURE_ERREUR;
			
	}
	else
	{
		error_fichier_inexistant();
		return LECTURE_ERREUR;
	}
}
	
/*--------------------------------------------------------------------*/
void modele_info(int* nb_phot, int* nb_proj, int* nb_abso, int* nb_refl)
{	
	*nb_phot = photon_info();
	*nb_proj = projecteur_info();
	*nb_abso = absorbeur_info();
	*nb_refl = reflecteur_info();
}

/*--------------------------------------------------------------------*/
void modele_save(char* nom_fichier)
{
	FILE *fichier = NULL;
	if((fichier = fopen(nom_fichier, "w+")) == NULL)
		error_msg((char*)"impossible de créer le fichier de sauvegarde");
	else
	{
		int nb_proj = projecteur_info();
		fprintf(fichier, "\n%d\n", nb_proj);
		projecteur_save(fichier);

		int nb_refl = reflecteur_info();
		fprintf(fichier, "\n%d\n", nb_refl);
		reflecteur_save(fichier);
		
		int nb_abso = absorbeur_info();
		fprintf(fichier, "\n%d\n", nb_abso);
		absorbeur_save(fichier);
		
		int nb_phot = photon_info();
		fprintf(fichier, "\n%d\n", nb_phot);
		photon_save(fichier);
	}
	
	fclose(fichier);
			
}

/*--------------------------------------------------------------------*/
bool modele_load(char *nom_fichier)
{ 
	modele_destruction();
	if((modele_lecture_fichier(nom_fichier)!=LECTURE_SUCCES) ||
										!(modele_verification_rendu2()))
	{
		modele_destruction();
		return false;
	}
	return true;
	
}

/*--------------------------------------------------------------------*/
void modele_dessine(void)
{
	if(projecteur_info() != 0)
		projecteur_dessine_tout();
		
	if(reflecteur_info() != 0)
		reflecteur_dessine_tout();
		
	if(absorbeur_info() != 0)
		absorbeur_dessine_tout();
		
	if(photon_info() != 0)
		photon_dessine();
}

/*--------------------------------------------------------------------*/
void modele_destruction(void)
{
	projecteur_destruction();
	reflecteur_destruction();
	absorbeur_destruction();
	photon_destruction();
}

/*--------------------------------------------------------------------*/
//effectue une mise à jour
void sim_update(void)
{
	projecteur_sim();
	photon_sim();
}

/*--------------------------------------------------------------------*/
void modele_def_limites(float *x_min, float *y_min, float *x_max,
														float *y_max)
{
	bool def = true;
	projecteur_def_limites(x_min,y_min,x_max,y_max, &def);
	reflecteur_def_limites(x_min,y_min,x_max,y_max, &def);
	absorbeur_def_limites(x_min,y_min,x_max,y_max, &def);
	photon_def_limites(x_min,y_min,x_max,y_max, &def);
	
}

/*--------------------------------------------------------------------*/
void modele_set_mouse(POINT souris)
{
	pos_mouse.posx=souris.posx;
	pos_mouse.posy=souris.posy;
}

/*--------------------------------------------------------------------*/
static int decodage_ligne(char* ligne)
{
	static int etat = NB_PROJECTEUR;
	static int nb_photons, nb_projecteurs, nb_reflecteurs, nb_absorbeurs;
	int i = 0;
	
	if(ligne==NULL)
	{
		etat=NB_PROJECTEUR;
		return LECTURE_SUCCES;
	}
	switch (etat)
	{
		case NB_PROJECTEUR:
			if(projecteur_lecture_nb(ligne,&nb_projecteurs))
				etat = (nb_projecteurs == 0)? FIN_PROJECTEUR :
															PROJECTEURS;
			else return LECTURE_ERREUR;
			break;
		case PROJECTEURS:
			if(projecteur_lecture(ligne))
				{ if(--nb_projecteurs == 0) etat = FIN_PROJECTEUR;}
			else return LECTURE_ERREUR;
			break;
		case FIN_PROJECTEUR:
			if(decodage_fin_liste(ligne, ERR_PROJECTEUR))
				etat = NB_REFLECTEUR;
			else return LECTURE_ERREUR;
			break;
		case NB_REFLECTEUR:
			if(reflecteur_lecture_nb(ligne,&nb_reflecteurs))
				etat = (nb_reflecteurs == 0)? FIN_REFLECTEUR :
															REFLECTEURS;
			else return LECTURE_ERREUR;
			break;
		case REFLECTEURS:
			if(reflecteur_lecture(ligne, &i))
				{ if(--nb_reflecteurs == 0) etat = FIN_REFLECTEUR;}
			else return LECTURE_ERREUR;
			break;
		case FIN_REFLECTEUR:
			if(decodage_fin_liste(ligne, ERR_REFLECTEUR))
				etat = NB_ABSORBEUR;
			else return LECTURE_ERREUR;	
			break;
		case NB_ABSORBEUR:
			if(absorbeur_lecture_nb(ligne,&nb_absorbeurs))
				etat = (nb_absorbeurs == 0)? FIN_ABSORBEUR : ABSORBEURS;
			else return LECTURE_ERREUR;
			break;
		case ABSORBEURS:
			if(absorbeur_lecture(ligne))
				{ if(--nb_absorbeurs == 0) etat = FIN_ABSORBEUR;}
			else return LECTURE_ERREUR;
			break;
		case FIN_ABSORBEUR:
			if(decodage_fin_liste(ligne, ERR_ABSORBEUR))
				etat = NB_PHOTON;
			else return LECTURE_ERREUR;
			break;
		case NB_PHOTON:
			if(photon_lecture_nb(ligne,&nb_photons))
				etat = (nb_photons == 0)? FIN_PHOTON : PHOTONS;
			else return LECTURE_ERREUR;
			break;
		case PHOTONS:
			if(photon_lecture(ligne))
				{ if(--nb_photons == 0) etat = FIN_PHOTON;}
			else return LECTURE_ERREUR;
			break;
		case FIN_PHOTON:
			if(decodage_fin_liste(ligne, ERR_PHOTON))
				return LECTURE_SUCCES;
			else return LECTURE_ERREUR;
			break;
	}
	return(LECTURE_CONTINUE);
}

/*--------------------------------------------------------------------*/
static bool decodage_fin_liste(char *ligne, ERREUR_ORIG origine)
{
	char fin_liste[MAX_LINE];
	if(sscanf(ligne, "%s", fin_liste) != 1 || 
									strcmp(fin_liste, "FIN_LISTE")!= 0)
	{
		error_lecture_elements(origine, ERR_TROP);
		return false;
	}
	else return true;
}

/*--------------------------------------------------------------------*/
bool modele_verification_rendu2(void)
{
	int i=0,j=0,k=0;
	VECTEUR temp1, temp2;
	POINT pi;
	
	for(i=reflecteur_info()-1;i>=0;i--)
	{	
		temp1 = reflecteur_get_vecteur(i);
		
		for(j=absorbeur_info()-1;j>=0;j--)
			for(k=0;k<get_nbp(j)-1;k++)
			{
				temp2 = get_abs(j,k);
				
				if(modele_veri(temp1,temp2, &pi))
				{
					error_lecture_intersection(ERR_ABSORBEUR,
								absorbeur_info()-1-j,ERR_REFLECTEUR,
											reflecteur_info()-1-i);
					return false;
				}
			}
		for(j=i-1;j>=0;j--)
		{
			temp2 = reflecteur_get_vecteur(j);
			if(modele_veri(temp1,temp2, &pi))
			{
				error_lecture_intersection(ERR_REFLECTEUR,
								reflecteur_info()-1-j,ERR_REFLECTEUR,
											reflecteur_info()-1-i);
				return false;
			}
		}
		for(j=projecteur_info()-1;j>=0;j--)
		{
			temp2 = get_proj(j);
			if(modele_veri(temp1,temp2, &pi))
			{
				error_lecture_intersection(ERR_PROJECTEUR,
								projecteur_info()-1-j,ERR_REFLECTEUR,
											reflecteur_info()-1-i);
				return false;
			}
		}
	}
	for(i=absorbeur_info()-1;i>=0;i--)
		for(k=0;k<get_nbp(i)-1;k++)
		{
			temp1 = get_abs(i,k);
			
			for(j=projecteur_info()-1;j>=0;j--)
			{
				temp2 = get_proj(j);
				
				if(modele_veri(temp1,temp2, &pi))
				{
					error_lecture_intersection(ERR_ABSORBEUR,
								absorbeur_info()-1-i,ERR_PROJECTEUR,
											projecteur_info()-1-j);
					return false;
				}
			}
		}
	return true;
}


/*--------------------------------------------------------------------*/
bool modele_veri(VECTEUR s1, VECTEUR s2, POINT* pi)
{
	POINT v1, v2, n1, n2, u1, u2, vd1d2, vd1f2, vd2f1, ud1d2;
	POINT O;
	double nv1, nvd1d2;
	double psdeb, psu1v2, ps1d1d2, ps1d1f2, ps2d1d2, ps2d2f1;
	nv1 = norme_vecteur(v1);
	O.posx =0;
	O.posy =0;
	v1.posx = s1.fin.posx - s1.deb.posx;
	v1.posy = s1.fin.posy - s1.deb.posy;
	v2.posx = s2.fin.posx - s2.deb.posx;
	v2.posy = s2.fin.posy - s2.deb.posy;
	u1 = vecteur_unitaire(v1);
	u2 = vecteur_unitaire(v2);
	vd1d2.posx = s2.deb.posx - s1.deb.posx;
	vd1d2.posy = s2.deb.posy - s1.deb.posy;
	nvd1d2 = distance2(vd1d2,O);
	ud1d2.posx = vd1d2.posx/nvd1d2;
	ud1d2.posy = vd1d2.posy/nvd1d2;
	if(fabs(produit_vectoriel(u1,u2))<=EPSIL_PARAL)
		if(fabs(produit_vectoriel(u1,ud1d2))<=EPSIL_PARAL)
		{
			psdeb = produit_scalaire(u1,vd1d2);
			psu1v2 = produit_scalaire(u1,v2);
			if(psdeb>0)
			{
				if(psdeb+psu1v2<=nv1+EPSIL_CONTACT || 
												psdeb<=nv1+EPSIL_CONTACT)
					return true;
			}
			else
			{
				if(psdeb>=-EPSIL_CONTACT || psdeb+psu1v2>=-EPSIL_CONTACT)
					return true;
			}
		}	
	n1.posx = 1;
	n1.posy = -n1.posx*v1.posx/v1.posy;
	n2.posx = 1;
	n2.posy = -n2.posx*v2.posx/v2.posy;
	vd1f2.posx = s2.fin.posx - s1.deb.posx;
	vd1f2.posy = s2.fin.posy - s1.deb.posy;
	vd2f1.posx = s1.fin.posx - s2.deb.posx;
	vd2f1.posy = s1.fin.posy - s2.deb.posy;
	ps1d1d2 = produit_scalaire(n1,vd1d2);
	ps1d1f2 = produit_scalaire(n1,vd1f2);
	ps2d1d2 = -produit_scalaire(n2,vd1d2);
	ps2d2f1 = produit_scalaire(n2,vd2f1);
	if((fabs(ps1d1d2)<EPSIL_CONTACT || fabs(ps1d1f2)<EPSIL_CONTACT) ||
													ps1d1d2*ps1d1f2<0)
	{
		if((fabs(ps2d1d2)<EPSIL_CONTACT || 
					fabs(ps2d2f1)<EPSIL_CONTACT) || ps2d1d2*ps2d2f1<0)
		{

			*pi = point_intersection(s1, u1, u2, vd1d2);
			return true;
		}
	}
	return false;
}

/*--------------------------------------------------------------------*/
//calcul le point d'intersection
POINT point_intersection(VECTEUR s1, POINT u1, POINT u2, POINT vd1d2)
{
	POINT p;
	
	p.posx = s1.deb.posx + (produit_vectoriel(u2, vd1d2)/
							produit_vectoriel(u2, u1)) * u1.posx;
	p.posy = s1.deb.posy + (produit_vectoriel(u2, vd1d2)/
							produit_vectoriel(u2, u1)) * u1.posy;						
	return p;
}

/*--------------------------------------------------------------------*/
POINT calc_inter(VECTEUR s1, VECTEUR s2)
{
    POINT temp;
    POINT v1, v2, u1, u2, vd1d2;
     
    double nv1, nv2;
     
    v1.posx = s1.fin.posx - s1.deb.posx;
    v1.posy = s1.fin.posy - s1.deb.posy;
    v2.posx = s2.fin.posx - s2.deb.posx;
    v2.posy = s2.fin.posy - s2.deb.posy;
     
    nv1 = norme_vect(s1);
    nv2 = norme_vect(s2);
     
    u1.posx = v1.posx/nv1;
    u1.posy = v1.posy/nv1;
    u2.posx = v2.posx/nv2;
    u2.posy = v2.posy/nv2;
     
    vd1d2.posx = s2.deb.posx - s1.deb.posx;
    vd1d2.posy = s2.deb.posy - s1.deb.posy;
     
    temp.posx = s1.deb.posx + 
            u1.posx*(produit_vectoriel(u2,vd1d2)/
											produit_vectoriel(u2,u1));
    temp.posy = s1.deb.posy + 
            u1.posy*(produit_vectoriel(u2,vd1d2)/
											produit_vectoriel(u2,u1));
     
    return temp;
}

/*--------------------------------------------------------------------*/
//délegue la selection aux entitées
void modele_selection(int a)
{
	switch(a)
	{
		case 0:
			projecteur_selection(pos_mouse);
			break;
			
		case 1:
			reflecteur_selection(pos_mouse);
			break;
			
		case 2:
			absorbeur_selection(pos_mouse);
			break;
	}
	
}

/*--------------------------------------------------------------------*/
void modele_efface_selection(int a)
{
	switch(a)
	{
		case 0:
			projecteur_suppression_selec();
			break;
			
		case 1:
			reflecteur_suppression_selec();
			break;
			
		case 2:
			absorbeur_suppression_selec();
			break;
	}
}

/*--------------------------------------------------------------------*/
void modele_beyond(float x_min, float x_max, float y_min, float y_max)
{
	photon_beyond(x_min,x_max,y_min,y_max);
}
