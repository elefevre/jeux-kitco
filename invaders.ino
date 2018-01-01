// Programme clignotant LED verte
// Fait clignoter la LED verte

// Nécessaire pour l'environnement Kitco
#include "kitco.h"



// La partie Setup concerne ce qui va être exécuté au démarrage de Kitco
void setup() {
  initialiserKitco(0);
  lcdBegin();
  setContrast(50);
}

#define NOMBRE_MECHANTS 12

#define LARGEUR_JOUEUR 11
#define HAUTEUR_JOUEUR 5
#define LARGEUR_MECHANT 11
#define HAUTEUR_MECHANT 8
#define ESPACEMEMENT_HORIZONTAL_MECHANT 13
#define ESPACEMEMENT_VERTICAL_MECHANT 8

bool modeMechant = false;
bool jeuTermine = false;
int temps = 0;
int tempo = 50;
bool tirEnCours = false;
byte xTir = 20;
byte yTir = 40;
byte xJoueur = 20;
byte yJoueur = 43;
char directionMechants = 1;

int xy(byte x, byte y, bool vivant) {
  return (((x << 7) + y) << 1) + (vivant ? 1 : 0);
}

byte toX(int b) {
  return (b >> 7) >> 1;
}

byte toY(int b) {
  return (b&B01111111) >> 1;
}

bool estVivant(int b) {
  return (b&B01) == 1;
}

int mechants[] = {xy(0, 0, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT, 0, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT*2, 0, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT*3, 0, true),
                  xy(0, ESPACEMEMENT_VERTICAL_MECHANT, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT, ESPACEMEMENT_VERTICAL_MECHANT, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT*2, ESPACEMEMENT_VERTICAL_MECHANT, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT*3, ESPACEMEMENT_VERTICAL_MECHANT, true),
                  xy(0, ESPACEMEMENT_VERTICAL_MECHANT*2, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT, ESPACEMEMENT_VERTICAL_MECHANT*2, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT*2, ESPACEMEMENT_VERTICAL_MECHANT*2, true), xy(ESPACEMEMENT_HORIZONTAL_MECHANT*3, ESPACEMEMENT_VERTICAL_MECHANT*2, true),};


#define WW true
#define OO false

bool mechantSprite1[] {
  OO, OO, WW, OO, OO, OO, OO, OO, WW, OO, OO,
  OO, OO, OO, WW, OO, OO, OO, WW, OO, OO, OO,
  OO, OO, WW, WW, WW, WW, WW, WW, WW, OO, OO,
  OO, WW, WW, OO, WW, WW, WW, OO, WW, WW, OO,
  WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW,
  WW, OO, WW, WW, WW, WW, WW, WW, WW, OO, WW,
  WW, OO, WW, OO, OO, OO, OO, OO, WW, OO, WW,
  OO, OO, OO, WW, WW, OO, WW, WW, OO, OO, OO
};

bool mechantSprite2[] {
  OO, OO, WW, OO, OO, OO, OO, OO, WW, OO, OO,
  WW, OO, OO, WW, OO, OO, OO, WW, OO, OO, WW,
  WW, OO, WW, WW, WW, WW, WW, WW, WW, OO, WW,
  WW, WW, WW, OO, WW, WW, WW, OO, WW, WW, WW,
  WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW,
  OO, OO, WW, WW, WW, WW, WW, WW, WW, OO, OO,
  OO, OO, WW, OO, OO, OO, OO, OO, WW, OO, OO,
  OO, WW, OO, OO, OO, OO, OO, OO, OO, WW, OO
};

bool joueurSprite[] {
  OO, OO, OO, OO, OO, WW, OO, OO, OO, OO, OO,
  OO, OO, OO, OO, WW, WW, WW, OO, OO, OO, OO,
  WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW,
  WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW,
  WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW
};

void ecrireChiffre(int i, byte x, byte y) {
  ecrireLettre('0'+i%10,x+15,y,NOIR);
  if (i>=10) ecrireLettre('0'+(i/10)%10,x+10,y,NOIR);
  if (i>=100) ecrireLettre('0'+(i/100)%10,x+5,y,NOIR);
  if (i>=1000) ecrireLettre('0'+(i/1000)%10,x,y,NOIR);
}

void dessinerMechant(byte n, bool mode) {
  if (!estVivant(mechants[n])) {
    return;
  }
  for (byte x=0; x < LARGEUR_MECHANT; x++) {
    for (byte y=0; y < HAUTEUR_MECHANT; y++) {
      if (mode) {
        if (mechantSprite1[x + y*LARGEUR_MECHANT]) setPixel(toX(mechants[n]) + x, toY(mechants[n]) + y);
      } else {
        if (mechantSprite2[x + y*LARGEUR_MECHANT]) setPixel(toX(mechants[n]) + x, toY(mechants[n]) + y);
      }
    }
  }
}

void dessinerJoueur() {
  for (byte x=0; x < LARGEUR_JOUEUR; x++) {
    for (byte y=0; y < HAUTEUR_JOUEUR; y++) {
        if (joueurSprite[x + y*LARGEUR_JOUEUR]) setPixel(xJoueur + x, yJoueur + y);
    }
  }
}

bool toucheCible(byte x, byte y, byte cibleX, byte cibleY, byte cibleLargeur, byte cibleHauteur) {
  return ( x >= cibleX && x <= cibleX + cibleLargeur && y >= cibleY && y <= cibleY + cibleHauteur);
}

void finirJeu(const char * texte) {
  ecrireEcran(texte, 20, 20, NOIR);
  jeuTermine = true;
}

void gererCollisions() {
  bool mechantsMorts = true;
  for (int i = 0; i < NOMBRE_MECHANTS; i++) {
    if (!estVivant(mechants[i])) {
      continue;
    }

    mechantsMorts = false;

    if (tirEnCours && toucheCible(xTir, yTir, toX(mechants[i]), toY(mechants[i]), LARGEUR_MECHANT, HAUTEUR_MECHANT)) {
      mechants[i] = xy(0, 0, false);
      tirEnCours = false;
    }
    if (toucheCible(xJoueur, yJoueur, toX(mechants[i]), toY(mechants[i]), LARGEUR_MECHANT, HAUTEUR_MECHANT)
        || toucheCible(xJoueur + LARGEUR_JOUEUR, yJoueur + HAUTEUR_JOUEUR, toX(mechants[i]), toY(mechants[i]), LARGEUR_MECHANT, HAUTEUR_MECHANT)) {
      finirJeu("PERDU :-(");
    }
  }

  if (mechantsMorts) {
      finirJeu("GAGNE !");
  }
}

void deplacerMechants() {
  bool toucheLimiteDroite = false;
  bool toucheLimiteGauche = false;
  for (int i = 0; i < NOMBRE_MECHANTS; i++) {
    if (!estVivant(mechants[i])) {
      continue;
    }

    if (toX(mechants[i]) + LARGEUR_MECHANT == LARGEUR_ECRAN) {
      if (directionMechants == 1) toucheLimiteDroite = true;
    }
    if (toX(mechants[i]) == 0) {
      if (directionMechants == -1) toucheLimiteGauche = true;
    }
  }

  if (toucheLimiteDroite) directionMechants = -1;
  if (toucheLimiteGauche) directionMechants = 1;
  byte incrementY = 0;
  if (toucheLimiteDroite || toucheLimiteGauche) {
    incrementY = 2;
    if (tempo > 5) {
      tempo = tempo - 5;
    } else {
      tempo = 0;
    }
  }

  for (int i = 0; i < NOMBRE_MECHANTS; i++) {
    if (!estVivant(mechants[i])) {
      continue;
    }

    mechants[i] = xy(toX(mechants[i]) + directionMechants, toY(mechants[i]) + incrementY, true);
  }
}

void deplacerTir() {
  if (!tirEnCours) return;

  yTir = yTir - 1;

  if (yTir == 0) {
    tirEnCours = false;
  }
}

void dessiner() {
  if (temps % 1000 == 0) {
    modeMechant = !modeMechant;
  }
  for (int i = 0; i < NOMBRE_MECHANTS; i++) {
    dessinerMechant(i, modeMechant);
  }

  dessinerJoueur();

  if (tirEnCours) {
    setPixel(xTir, yTir);
  }
}

void gererTouches() {
  if (toucheGauche()) {
    xJoueur = xJoueur - 1;
  }
  if (toucheDroite()) {
    xJoueur = xJoueur + 1;
  }
  if (toucheA() && !tirEnCours) {
    tirEnCours = true;
    xTir = xJoueur + LARGEUR_JOUEUR / 2;
    yTir = yJoueur;
  }

  if (xJoueur + LARGEUR_JOUEUR > LARGEUR_ECRAN) xJoueur = LARGEUR_ECRAN - LARGEUR_JOUEUR;
  if (xJoueur < 1) xJoueur = 0;
}


// Loop est la boucle principale, va se lancer en boucle après Setup
void loop() {
  if (jeuTermine) return;

  temps = temps + tempo;

  effacerEcran(BLANC);
//  ecrireChiffre(toX(mechants[0]), 7, 3);
//  ecrireChiffre(toX(mechants[1]), 27, 3);
//  ecrireChiffre(directionMechants, 47, 3);
  deplacerMechants();
  deplacerTir();
  dessiner();

  gererCollisions();

  rafraichirEcran();
  gererTouches();

  delai(tempo);
}
