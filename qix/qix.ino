// Un jeu de type Qix
// il faut remplir l'écran au maximum sans se faire toucher
// par les lignes qui bougent au milieu
// Développé en février 2018 par @elefevre pour la console kitco

#include "kitco.h"

#define NOMBRE_DE_BARRES 3

#define PAS_DE_COLLISION 0
#define COLLISION_VERTICALE 1
#define COLLISION_HORIZONTALE 2

byte xJoueur = 30;
byte yJoueur = 0;

typedef struct {
  byte x;
  byte y;
} pointVirage;

byte longueurParcours = 0;
pointVirage parcours[] = {
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
};

typedef struct {
  signed int x;
  signed int y;
  float directionX;
  float directionY;
} point;

typedef struct {
  point point1;
  point point2;
} barre;

barre barres[] = {
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
};

byte largeurTableau8Bits(byte largeur) {
  return ceil(largeur / 8.0);
}

#define NOMBRE_DE_BYTES_POUR_LARGEUR_ECRAN 11
byte pixels[ NOMBRE_DE_BYTES_POUR_LARGEUR_ECRAN * HAUTEUR_ECRAN ];


void setup() {
  initialiserKitco(0);
  lcdBegin();
  setContrast(50);

  demarrer();
  Serial.begin(9600);
  delay(1000);// Give reader a chance to see the output.
}

void demarrer() {
  for (int i = 0; i < HAUTEUR_ECRAN; i++) {
//    pixels[i * largeurTableau8Bits(LARGEUR_ECRAN)] = B11111111;
  }
  for (int i = 0; i < LARGEUR_ECRAN / 8; i++) {
    pixels[i + 45 * largeurTableau8Bits(LARGEUR_ECRAN)] = B11111111;
  }

  barres[0] = {12, 1, -1.0, -1.0, 40, 40, 1.0, 1.0};
}

void ecrireChiffre4(unsigned int i, byte x, byte y) {
  char buf [6];
  sprintf (buf, "%04i", i);
  ecrireEcran(buf, x, y, NOIR);
}

void ecrireChiffre2(unsigned int i, byte x, byte y) {
  char buf [6];
  sprintf (buf, "%02i", i);
  ecrireEcran(buf, x, y, NOIR);
}

void debugChiffre(char* txt, float i) {
  char buf [25];
  sprintf (buf, "%s: %f", txt, i);
  Serial.println(buf);
}

int trouverIndexDansPixels(byte x, byte y) {
  return x / 8 + y * largeurTableau8Bits(LARGEUR_ECRAN);
}

byte trouverBitshift(byte x) {
  return x % 8;
}

bool trouverPresencePixel(byte x, byte y) {
  if (x < 0 || x > LARGEUR_ECRAN) return true;
  if (y < 0 || y > HAUTEUR_ECRAN) return true;

  return pixels[trouverIndexDansPixels(x, y)] & (B10000000 >> trouverBitshift(x));
}

byte trouverCollision(signed int x, signed int y) {
  if (x < 0 || x > LARGEUR_ECRAN) return COLLISION_VERTICALE;
  if (y < 0 || y > HAUTEUR_ECRAN) return COLLISION_HORIZONTALE;

  if (trouverPresencePixel(x, y)) {
    if (trouverPresencePixel(x - 1, y) && trouverPresencePixel(x + 1, y)) {
      return COLLISION_HORIZONTALE;
    }
    if (trouverPresencePixel(x, y - 1) && trouverPresencePixel(x, y + 1)) {
      return COLLISION_VERTICALE;
    }
  }

  return PAS_DE_COLLISION;
}

point gererCollisionSurUnPoint(point p) {
  byte typeDeCollision = trouverCollision(p.x, p.y);

  if (typeDeCollision == COLLISION_VERTICALE) {
     p.directionX = -p.directionX;
  }
  if (typeDeCollision == COLLISION_HORIZONTALE) {
     p.directionY = -p.directionY;
  }

  return p;
}

bool estSurLaSurface(byte x, byte y) {
  bool cePixelEstOccupe = trouverPresencePixel(x, y);

  bool unPixelACoteEstLibre = !trouverPresencePixel(x - 1, y - 1)     || !trouverPresencePixel(x, y - 1)     || !trouverPresencePixel(x + 1, y - 1)
                           || !trouverPresencePixel(x - 1, y)                                                || !trouverPresencePixel(x + 1, y)
                           || !trouverPresencePixel(x - 1, y + 1)     || !trouverPresencePixel(x, y + 1)     || !trouverPresencePixel(x + 1, y + 1);
  return cePixelEstOccupe && unPixelACoteEstLibre;
}

void deplacer() {
  byte xJoueurPrecedent = xJoueur;
  byte yJoueurPrecedent = yJoueur;
  debugChiffre("xJoueur", xJoueur);
  if (toucheGauche()) {
    xJoueur = xJoueur - 1;
  }
  if (toucheDroite()) {
    xJoueur = xJoueur + 1;
  }
  if (toucheHaut()) {
    yJoueur = yJoueur - 1;
  }
  if (toucheBas()) {
    yJoueur = yJoueur + 1;
  }

  if (trouverCollision(xJoueur, yJoueur) == PAS_DE_COLLISION) {
    if (longueurParcours == 0) {
      parcours[longueurParcours] = {xJoueurPrecedent, yJoueurPrecedent};
      longueurParcours++;
    } else if (parcours[longueurParcours].x != xJoueur && parcours[longueurParcours].x != yJoueur) {
      parcours[longueurParcours] = {xJoueurPrecedent, yJoueurPrecedent};
      longueurParcours++;
    }
  } else {
    longueurParcours = 0;
  }

  for (int i=1; i < NOMBRE_DE_BARRES ; i++) {
    barres[NOMBRE_DE_BARRES - i].point1.x = barres[NOMBRE_DE_BARRES - i - 1].point1.x;
    barres[NOMBRE_DE_BARRES - i].point1.y = barres[NOMBRE_DE_BARRES - i - 1].point1.y;
    barres[NOMBRE_DE_BARRES - i].point2.x = barres[NOMBRE_DE_BARRES - i - 1].point2.x;
    barres[NOMBRE_DE_BARRES - i].point2.y = barres[NOMBRE_DE_BARRES - i - 1].point2.y;
  }

  barres[0].point1.x = barres[0].point1.x + barres[0].point1.directionX;
  barres[0].point1.y = barres[0].point1.y + barres[0].point1.directionY;
  barres[0].point2.x = barres[0].point2.x + barres[0].point2.directionX;
  barres[0].point2.y = barres[0].point2.y + barres[0].point2.directionY;

  barres[0].point1 = gererCollisionSurUnPoint(barres[0].point1);
  barres[0].point2 = gererCollisionSurUnPoint(barres[0].point2);
}

void dessinerJoueur() {
  ecrireChiffre2(xJoueur, 20,  0);
  ecrireChiffre2(yJoueur, 50,  0);

  creerRectangle(xJoueur - 1, yJoueur - 1, xJoueur + 1, yJoueur, false, NOIR);
}

void dessiner() {
  for (int i = 0; i < NOMBRE_DE_BARRES ; i++) {
    ligneEcran(barres[i].point1.x, barres[i].point1.y, barres[i].point2.x, barres[i].point2.y, NOIR);
  }

  for (int x = 0; x < largeurTableau8Bits(LARGEUR_ECRAN); x++) {
    for (int y = 0; y < HAUTEUR_ECRAN; y++) {
      for (int b = 0; b < 8; b++) {
        if (pixels[x + y * largeurTableau8Bits(LARGEUR_ECRAN)] & (B10000000 >> b)) setPixel(x * 8 + b, y, NOIR);
      }
    }
  }

  dessinerJoueur();

  if (longueurParcours > 0) {
    for (int i = 1; i < longueurParcours; i++) {
      ligneEcran(parcours[i - 1].x, parcours[i - 1].y, parcours[i].x, parcours[i].y, NOIR);
    }
    ligneEcran(parcours[longueurParcours - 1].x, parcours[longueurParcours - 1].y, xJoueur, yJoueur, NOIR);
  }
}


void loop() {
  effacerEcran(BLANC);
  deplacer();
  dessiner();
  rafraichirEcran();
  delai(100);
}

