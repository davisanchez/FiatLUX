# Definitions de macros

CC     = gcc
CFLAGS = -g
LIBS   = -lglut -lGL -lGLU -lm -lglui -L/usr/X11R6/lib -lX11 -lXext \
 -lXmu -lXi -lstdc++
CFILES = absorbeur.c photon.c projecteur.c reflecteur.c error.c main.cpp \
 modele.c utilitaire.c graphic.c
OFILES = $(CFILES:.c=.o)

# Definition de la premiere regle

rendu3.x: $(OFILES)
	$(CC) $(OFILES) -lm -o rendu3.x ${LIBS}

# Definitions de cibles particulieres

depend:
	@echo " *** MISE A JOUR DES DEPENDANCES ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CC) -MM $(CFLAGS) $(CFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -f *.o *.x *.c~ *.h~

#
# -- Regles de dependances generees automatiquement
#
# DO NOT DELETE THIS LINE
absorbeur.o: absorbeur.c absorbeur.h utilitaire.h constantes.h \
 tolerance.h error.h graphic.h
photon.o: photon.c photon.h utilitaire.h error.h graphic.h constantes.h \
 tolerance.h modele.h absorbeur.h reflecteur.h
projecteur.o: projecteur.c projecteur.h utilitaire.h photon.h error.h \
 graphic.h constantes.h tolerance.h
reflecteur.o: reflecteur.c reflecteur.h utilitaire.h error.h graphic.h \
 constantes.h tolerance.h
error.o: error.c error.h
main.o: main.cpp modele.h utilitaire.h photon.h error.h constantes.h \
 tolerance.h graphic.h absorbeur.h projecteur.h reflecteur.h
modele.o: modele.c modele.h utilitaire.h photon.h error.h absorbeur.h \
 constantes.h tolerance.h reflecteur.h projecteur.h
utilitaire.o: utilitaire.c utilitaire.h constantes.h tolerance.h
graphic.o: graphic.c graphic.h utilitaire.h
