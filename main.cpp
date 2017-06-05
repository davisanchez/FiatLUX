/* Nom: main.cpp
 * Description: module principal
 * Date: 21.05.2016
 * version : 3.0
 * auteur : Frank Dupont, David Sanchez Del Rio
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <GL/glui.h>
#include <GL/glut.h>
#include <math.h>


extern "C"
{
	#include "modele.h"	
	#include "error.h"
	#include "constantes.h"
	#include "utilitaire.h"
	#include "graphic.h"
	#include "absorbeur.h"
	#include "projecteur.h"
	#include "reflecteur.h"
}

#define NOM_FENETRE		"Fiat Lux!"
#define SELEC 0
#define CREA 1
#define PROJO 0
#define REFLEC 1
#define ABSOR 2

namespace
{
	int main_window;
	int prog_mode=MODE_DEFAULT;
	int w=400,h=600;
	POINT mouse;
	POINT last_mouse;
	float f_x_min, f_x_max, f_y_min, f_y_max;
	float x_min, x_max, y_min, y_max;
	float temp_x_min, temp_x_max, temp_y_min, temp_y_max;
	
	bool bouton, appuye=false;
	
	
	char text1[200] = "test.txt";
	char text2[200] = "save.txt";
	
	GLfloat aspect_ratio;
	
	/*pointeurs pour creer les objets de GLUI*/
	GLUI_EditText *edittext1;
	GLUI_EditText *edittext2;
	GLUI_EditText *edittext_nb_phot;
	GLUI_EditText *edittext_nb_proj;
	GLUI_EditText *edittext_nb_abso;
	GLUI_EditText *edittext_nb_refl;
	
	GLUI_Button *load;
	GLUI_Button *save;
	GLUI_Button *start;
	GLUI_Button *step;
	GLUI_Button *quit;

	GLUI_RadioGroup *action_group;
	GLUI_RadioGroup *entity_group;
	
	enum Id {EDITTEXT1_ID, EDITTEXT2_ID, LOAD_ID, SAVE_ID, START_ID,
					STEP_ID, EDITTEXT_NB_PHOT_ID,EDITTEXT_NB_PROJ_ID,
							EDITTEXT_NB_ABSO_ID, EDITTEXT_NB_REFL_ID,
									ACTION_GROUP_ID, ENTITY_GROUP_ID};
	
	enum State {NOT_READY, STOP, START, STEP};
	
	enum Select_1 {SELECTION, CREATION};
	
	enum Select_2 {ABSO, PROJ, REFL};
	
	int state = NOT_READY;
	int select_1=SELECTION;
	int select_2=PROJ;
	
}

// défini le mode
int defini_mode(char* nom_mode);

//  lecture du clavier au programme
static void clavier_cb(unsigned char key, int x, int y);

// lecture de l'état de la souris au programme
static void mouse_cb(int button, int state, int x, int y);

// lecture du déplacement de la souris
static void mouse_move(int x, int y);

// Fonction update
static void update(void);

// Callback en contexte Idle
static void idle(void);

// Affichage de l'interface GLUI
static void interface_utilisateur(void);

// Affichage de l'interface GLUT
static void display_cb(void);

// Callback appelée lors d'un changement de fenètre
static void reshape(int w1, int h1);

// Fonction callback de GLUI
static void control_cb(int id);

// Affiche la fenêtre GLUT
static void affichage_fenetre();

// Défini les bords
static void def_limites(float* x_min, float* y_min, float* x_max, 
														float* y_max);


/*--------------------------------------------------------------------*/
int main(int argc, char** argv)
{
	if(argc != 3 && argc!=1)
	{
			char err_msg[100] = "";
			sprintf(err_msg,
			"usage : \"./%s mode_test nom_fichier\" ""ou \"./%s\"",
													argv[0],argv[0]);
			error_msg(err_msg);
	}
	
	prog_mode = (argc==3) ? defini_mode(argv[1]) : MODE_DEFAULT;
	
	if(prog_mode!=MODE_DEFAULT)
	{
		strcpy(text1, argv[2]);
		modele_lecture(prog_mode,argv[2]);
		def_limites(&x_min,&y_min,&x_max,&y_max);
		f_x_min = x_min;
		f_x_max = x_max;
		f_y_min = y_min;
		f_y_max = y_max;
		state=STOP;
	}
	glutInit(&argc, argv);
	affichage_fenetre();
	
	interface_utilisateur();
	srand(0);
	
	if(prog_mode==MODE_GRAPHIC || prog_mode==MODE_FINAL)
	{
		edittext1->set_text(argv[2]);
		glutPostRedisplay();
	}
	
	glutMainLoop();
	return EXIT_SUCCESS;	

}

/*--------------------------------------------------------------------*/
int defini_mode(char* nom_mode)
{
	if(strcmp(nom_mode,(char*) "Error") == 0)
		return MODE_ERROR;
	else if(strcmp(nom_mode,(char*) "Verification") == 0)
		return MODE_VERIF;
	else if(strcmp(nom_mode,(char*) "Graphic") == 0)
		return MODE_GRAPHIC;
	else if(strcmp(nom_mode,(char*) "Final") == 0)
		return MODE_FINAL;
	else
		error_msg((char*)"mode de test inutilisé");
	
	return MODE_DEFAULT;
}

/*--------------------------------------------------------------------*/
static void def_limites(float* x_min, float* y_min, float* x_max,
														float* y_max)
{
	modele_def_limites(x_min,y_min,x_max,y_max);
	
	*x_min-=DMAX;
	*y_min-=DMAX;
	*x_max+=DMAX;
	*y_max+=DMAX;
}
	
/*--------------------------------------------------------------------*/
static void control_cb( int id)
{
	
	switch (id)
	{
	case (LOAD_ID):	if(state==START) break;
					state=NOT_READY;
					if(modele_load((char*)edittext1->get_text()))
					{
						x_min = x_max = y_min = y_max = 0;
						def_limites(&x_min, &y_min, &x_max, &y_max);
						f_x_min = x_min;
						f_x_max = x_max;
						f_y_min = y_min;
						f_y_max = y_max;
						state=STOP;
					}
					update();
					glutPostRedisplay();
					break;
	case (SAVE_ID):	modele_save((char*)edittext2->get_text());
					break;
	case (START_ID):	switch(state)
						{
							case START: state = STOP;
								start->set_name("Start !");
								step->enable();
								action_group->enable();
								entity_group->enable();
								break;
							case STOP: state = START;
								start->set_name("Stop !");
								step->disable();
								action_group->disable();
								entity_group->disable();
								break;
						}
						glutPostRedisplay();
						break;
	case (STEP_ID): start->set_name("Start !");
					state = STEP;
					glutPostRedisplay();
					break;
	case (ACTION_GROUP_ID): if(state==STOP)
	
							printf("action_group: %f\n",
										action_group->get_float_val() );
							break;
	case (ENTITY_GROUP_ID): if(state==STOP)
								printf("OK pour select\n");
							break;
	default:	error_msg((char*)"Unknown command");
				break;
	}
}

/*--------------------------------------------------------------------*/
static void reshape_cb(int w1, int h1)
{ 
	w = w1;
	h = h1;
	glViewport(0, 0, w1, h1); 
	aspect_ratio = (GLfloat) w1 / (GLfloat) h1 ; 

	glutPostRedisplay();
} 

/*--------------------------------------------------------------------*/
static void display_cb(void)
{
	glutSetWindow(main_window);
	glClear(GL_COLOR_BUFFER_BIT); 
	glLoadIdentity();
	glLineWidth(EPAISS_TRAIT);
	
	float aspect_ratio_init = (x_max - x_min)/(y_max - y_min);
	float display_ratio = (float)aspect_ratio / aspect_ratio_init;
	
	if (display_ratio <= 1) // window is narrower than sim space
	{
 		float delta = ((y_max-y_min)/display_ratio)/2;
		float mid   = (y_max + y_min)/2;
        glOrtho(x_min, x_max, mid-delta, mid+delta, -1.0, 1.0);
        temp_x_max=x_max;
        temp_x_min=x_min;
        temp_y_max=mid+delta;
        temp_y_min=mid-delta;
	}
    else                    // window is wider that sim space
    {
		float delta = display_ratio*(x_max-x_min)/2;
		float mid   = (x_max + x_min)/2;
		glOrtho(mid-delta, mid+delta, y_min, y_max, -1.0, 1.0);
		temp_x_max=mid+delta;
        temp_x_min=mid-delta;
        temp_y_max=y_max;
        temp_y_min=y_min;
    }
    
	if(state!=NOT_READY) modele_dessine();
	//dessine le rectangle de zoom
	if(appuye && bouton)
    {
		graphic_set_color_3f(0.,0.,0.);
		graphic_draw_rect(last_mouse,mouse);
		glutSwapBuffers();
	}

	glutSwapBuffers();
}

/*--------------------------------------------------------------------*/
static void update(void)
{
	 int nb_phot = 0, nb_proj = 0, nb_abso = 0, nb_refl = 0;
	
	//appel fonction dans modèle pour uptade
	modele_info(&nb_phot, &nb_proj, &nb_abso, &nb_refl);
	
	edittext_nb_phot->set_int_val(nb_phot);
	edittext_nb_proj->set_int_val(nb_proj);
	edittext_nb_abso->set_int_val(nb_abso);
	edittext_nb_refl->set_int_val(nb_refl);
}

/*--------------------------------------------------------------------*/
static void idle(void)
{
	switch (state)
	{
		case (STEP):
			sim_update();
			state = STOP;
			if(prog_mode == MODE_GRAPHIC) printf("one step\n");	
			break;		
		case (START) :
			sim_update();
			break;
	}
	update();
	glutSetWindow(main_window);
	glutPostRedisplay();
}
			
/*--------------------------------------------------------------------*/
static void clavier_cb(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'r' :	x_min = f_x_min;
					x_max = f_x_max;
					y_min = f_y_min;
					y_max = f_y_max;
					break;
					
		case 'k' :	printf("sim_delete_beyond\n");
					modele_beyond(x_min,x_max,y_min,y_max);
					break;
					
		case 'd' :	if(state==STOP && !bouton && 
									action_group->get_int_val()==SELEC)
					{
						if(prog_mode==MODE_GRAPHIC)
							printf("sim_elem_delete\n");
						modele_efface_selection(entity_group->
														get_int_val());
					}
					break;
					
		case 'q' :	exit(EXIT_SUCCESS);
					break;
					
		case 'Q' :	exit(EXIT_SUCCESS);
					break;
	}	
}

/*--------------------------------------------------------------------*/
static void mouse_cb(int button, int state, int x, int y)
{
	static int compteur=0;
	switch(button)
	{
		case GLUT_RIGHT_BUTTON :
			bouton = false;
			switch(state)
			{
				case GLUT_UP :
					printf("sim_elem_select\n");
					break;
					
				case GLUT_DOWN :
					if(prog_mode==MODE_GRAPHIC)
						printf("sim_elem_select\n");
						
					mouse_move(x,y);
					if(!appuye && action_group->get_int_val() == CREA)
					{
						last_mouse.posx = mouse.posx;
						last_mouse.posy = mouse.posy;
						appuye = true;
					}
					else if(appuye && action_group->
												get_int_val() == CREA)
					{
						switch(entity_group->get_int_val())
						{
							case PROJO:
								projecteur_creation(last_mouse, mouse);
								if(!modele_verification_rendu2())
									enlever_tete_proj();
								break;
								
							case REFLEC:
								reflecteur_creation(last_mouse, mouse);
								if(!modele_verification_rendu2())
									enlever_tete_refl();
								break;
								
							case ABSOR:
								absorbeur_creation(last_mouse, mouse, 
															&compteur);
								if(!modele_verification_rendu2())
									enlever_tete_abs();
								break;
						}
						appuye=false;
						if(entity_group->get_int_val() == ABSOR)
							appuye=true;
						if(entity_group->get_int_val() == ABSOR &&
														compteur == 0)
							appuye = false;
					}
					else
					{	
						modele_selection(entity_group->get_int_val());
						compteur=0;
						appuye=false;
					}
						
						break;
			}
			break;
		case GLUT_LEFT_BUTTON :
			bouton = true;
			switch (state)
			{
				case GLUT_DOWN :
					mouse_move(x,y);
					appuye = true;
					last_mouse.posx=mouse.posx;
					last_mouse.posy=mouse.posy;
					break;
					
				case GLUT_UP :
					mouse_move(x,y);
					appuye = false;
					if(distance2(mouse,last_mouse)>EPSIL_CREATION)
					{
						if(mouse.posx>last_mouse.posx)
						{
							x_max = mouse.posx;
							x_min = last_mouse.posx;
						}
						else
						{
							x_min = mouse.posx;
							x_max = last_mouse.posx;
						}
						if(mouse.posy>last_mouse.posy)
						{
							y_max = mouse.posy;
							y_min = last_mouse.posy;
						}
						else
						{
							y_min = mouse.posy;
							y_max = last_mouse.posy;
						}
						display_cb();
					}
					break;
			}
			break;
	}
	glutPostRedisplay();
}

/*--------------------------------------------------------------------*/
static void mouse_move(int x, int y)
{
	mouse.posx = (((float) x/w))*(temp_x_max-temp_x_min)+temp_x_min;
	mouse.posy = temp_y_max-(((float) y/h))*(temp_y_max-temp_y_min);
	
	modele_set_mouse(mouse);
}
		
/*--------------------------------------------------------------------*/
static void affichage_fenetre()
{	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(50, 20);
	glutInitWindowSize(w, h);
	
	main_window = glutCreateWindow("Fiat Lux !");
	glClearColor(1., 1., 1., 0.);
	
	glutReshapeFunc(reshape_cb);
	glutDisplayFunc(display_cb);
	glutMouseFunc(mouse_cb);
	glutMotionFunc(mouse_move);
	glutKeyboardFunc(clavier_cb);
	glutIdleFunc(idle);
}
	
/*--------------------------------------------------------------------*/
static void interface_utilisateur()
{
	/****************************************/
	/* widgets GLUI */
	/****************************************/
	glutInitWindowPosition(1000, 20);
	glutInitWindowSize(300, 150);
	GLUI *glui = GLUI_Master.create_glui((char*)NOM_FENETRE);
	
	//File
	GLUI_Panel *panel_File = glui->add_panel((char*)"File",
												GLUI_PANEL_EMBOSSED);
	edittext1 = glui->add_edittext_to_panel(panel_File,
						(char*)"FileName:", GLUI_EDITTEXT_TEXT, NULL);
	edittext1->set_text(text1);
	load = glui->add_button_to_panel(panel_File, (char*)"Load",
												LOAD_ID, control_cb);
	edittext2 = glui->add_edittext_to_panel(panel_File,
						(char*)"Filename:",GLUI_EDITTEXT_TEXT, NULL);
	edittext2->set_text(text2);
	save = glui->add_button_to_panel(panel_File, (char*)"Save", SAVE_ID,
															control_cb);
	
	//Simulation
	GLUI_Panel *panel_Simulation = glui->add_panel((char*)"Simulation",
												GLUI_PANEL_EMBOSSED);
	start = glui->add_button_to_panel(panel_Simulation, (char*)"Start!",
												START_ID, control_cb);
	step = glui->add_button_to_panel(panel_Simulation, (char*)"Step",
												STEP_ID, control_cb);
	
	// Quit
	glui->add_button((char*) "Exit", 0,(GLUI_Update_CB)exit );
	
	glui->add_column(true);
	
	//Information
	GLUI_Panel *panel_Information = glui->add_panel((char*)"Information",
													GLUI_PANEL_EMBOSSED);
	edittext_nb_phot = glui->add_edittext_to_panel(panel_Information,
						(char*)"Nb Photons:", GLUI_EDITTEXT_INT, NULL);
	edittext_nb_proj = glui->add_edittext_to_panel(panel_Information,
					(char*)"Nb Projecteurs:", GLUI_EDITTEXT_INT, NULL);
	edittext_nb_abso = glui->add_edittext_to_panel(panel_Information,
					(char*)"Nb Absorbeurs:",  GLUI_EDITTEXT_INT, NULL);
	edittext_nb_refl = glui->add_edittext_to_panel(panel_Information,
					(char*)"Nb Reflecteurs:", GLUI_EDITTEXT_INT, NULL);
	
	//Mouse panel
	GLUI_Panel *panel_Mouse = glui->add_panel((char*)"Mouse panel",
												GLUI_PANEL_EMBOSSED);
	GLUI_Panel* action = glui->add_panel_to_panel(panel_Mouse,
											(char*)"Action selection");
	
	action_group = glui->add_radiogroup_to_panel(action, NULL,
										ACTION_GROUP_ID, control_cb);
	glui->add_radiobutton_to_group(action_group,(char*) "Selection" );
	glui->add_radiobutton_to_group(action_group,(char*) "Creation" );
	GLUI_Panel* select_panel = glui->add_panel_to_panel(panel_Mouse,
											(char*)"Entity selection");
	entity_group = glui->add_radiogroup_to_panel(select_panel, NULL,
										ENTITY_GROUP_ID, control_cb);
	glui->add_radiobutton_to_group(entity_group,(char*) "Projecteur" );
	glui->add_radiobutton_to_group(entity_group,(char*) "Reflecteur" );
	glui->add_radiobutton_to_group(entity_group,(char*) "Absorbeur" );
	
	glui->set_main_gfx_window(main_window);
	
}
	
