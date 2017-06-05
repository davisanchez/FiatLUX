/* Nom: graphic.c
 * Description: module qui affiche un segment, un rectangle ou un cercle
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <math.h>
#include <GL/glu.h>
#include "graphic.h"
#include "utilitaire.h"

void graphic_draw_segment(double x1, double y1, double x2, double y2)
{ 
	glBegin (GL_LINES);

	glVertex2f (x1, y1);
	glVertex2f (x2, y2);

	glEnd ();
}

/*-----------------------------------------------------------------------*/
void graphic_draw_circle(float xc, float yc, float r)
{ 
	int i;
	const int SIDES = 50;

	glBegin (GL_LINE_LOOP);

	for (i=0; i < SIDES; i++)
    {
		float alpha = i * 2. * M_PI / SIDES;
		float x = xc + r * cos (alpha);
		float y = yc + r * sin (alpha);
		glVertex2f (x, y);    
	}

	glEnd ();
}

/*-----------------------------------------------------------------------*/
void graphic_draw_rect(POINT last_mouse, POINT mouse)
{
    glBegin (GL_LINE_LOOP);
 
    glVertex2f (mouse.posx, last_mouse.posy);
    glVertex2f (last_mouse.posx, last_mouse.posy);
    glVertex2f (last_mouse.posx, mouse.posy);
    glVertex2f (mouse.posx, mouse.posy);
 
    glEnd ();
}

/*-----------------------------------------------------------------------*/
void graphic_set_color_3f(float r, float g, float b)
{
	glColor3f(r, g, b);
}

/*-----------------------------------------------------------------------*/
void graphic_set_line_width(float width)
{
	glLineWidth(width);
}
/*-----------------------------------------------------------------------*/
