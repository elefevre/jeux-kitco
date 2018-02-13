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
} point;

byte longueurParcours = 0;
point parcours[] = {
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
  {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
};

typedef struct {
  point coord;
  float directionX;
  float directionY;
} pointEnMouvement;

typedef struct {
  pointEnMouvement point1;
  pointEnMouvement point2;
} barre;

barre barres[] = {
{{{0, 0}, 0, 0}, {{0, 0}, 0, 0}},
{{{0, 0}, 0, 0}, {{0, 0}, 0, 0}},
{{{0, 0}, 0, 0}, {{0, 0}, 0, 0}},
};

byte largeurTableau8Bits(byte largeur) {
  return ceil(largeur / 8.0);
}

#define NOMBRE_DE_BYTES_POUR_LARGEUR_ECRAN 11
byte pixels[ NOMBRE_DE_BYTES_POUR_LARGEUR_ECRAN * HAUTEUR_ECRAN ];

void assertEqual(long actual, long expected) {
  if (actual == expected) return;

  char buf [50];
  sprintf (buf, "actual value: %d, but expected: %d", actual, expected);
  Serial.println(buf);  
  delay(1000 * 60 * 60 * 24);// seemingly block everything
}

void lancerTests() {
  Serial.begin(9600);
  char buf [15];
  sprintf (buf, "running tests");
  Serial.println(buf);

  assertEqual(trouverIndexDansPixels(0, 0), 0);
  assertEqual(trouverIndexDansPixels(1, 0), 0);
  assertEqual(trouverIndexDansPixels(7, 0), 0);
  assertEqual(trouverIndexDansPixels(8, 0), 1);
  assertEqual(trouverIndexDansPixels(0, 1), 11);
  assertEqual(trouverIndexDansPixels(8, 2), 23);

  assertEqual(pointDansSegmentOrthogonal(0, 0, 0, 0, 0, 0), true);
  assertEqual(pointDansSegmentOrthogonal(0, 1, 0, 0, 0, 0), false);
  assertEqual(pointDansSegmentOrthogonal(1, 0, 0, 0, 0, 0), false);
  assertEqual(pointDansSegmentOrthogonal(1, 0, 0, 0, 2, 0), true);
  assertEqual(pointDansSegmentOrthogonal(0, 1, 0, 0, 0, 2), true);
  assertEqual(pointDansSegmentOrthogonal(0, 2, 0, 0, 0, 1), false);
  assertEqual(pointDansSegmentOrthogonal(2, 0, 0, 0, 1, 0), false);

  buf [15];
  sprintf (buf, "tests pass");
  Serial.println(buf);  
}

void setup() {
  initialiserKitco(0);
  lcdBegin();
  setContrast(50);

  lancerTests();

  demarrer();
  delay(1000);// Give reader a chance to see the output.
}

void demarrer() {
  remplirLigne(0, 0, LARGEUR_ECRAN - 1, 0);
  remplirLigne(0, HAUTEUR_ECRAN - 1, LARGEUR_ECRAN - 1, HAUTEUR_ECRAN - 1);
  remplirLigne(0, 0, 0, HAUTEUR_ECRAN - 1);
  remplirLigne(LARGEUR_ECRAN - 1, 0, LARGEUR_ECRAN - 1, HAUTEUR_ECRAN - 1);

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

void debugChiffre(char* txt, int i) {
  char buf [25];
  sprintf (buf, "%s: %d", txt, i);
  Serial.println(buf);
}

void debugJeu(int x, int y) {
  char buf [25];
  sprintf (buf, "x: %d, y: %d", x, y);
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
  if (x <= 0 || x >= LARGEUR_ECRAN) return COLLISION_VERTICALE;
  if (y <= 0 || y >= HAUTEUR_ECRAN) return COLLISION_HORIZONTALE;

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

pointEnMouvement gererCollisionSurUnPoint(pointEnMouvement p) {
  byte typeDeCollision = trouverCollision(p.coord.x, p.coord.y);

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

void remplirPixel(int x, int y) {
  byte bitAAjouter = B10000000 >> trouverBitshift(x);
  pixels[trouverIndexDansPixels(x, y)] = pixels[trouverIndexDansPixels(x, y)] | bitAAjouter;
}

void remplirLigne(int x0, int y0, int x1, int y1) {
  int dy = y1 - y0; // Difference between y0 and y1
  int dx = x1 - x0; // Difference between x0 and x1
  int stepx, stepy;

  if (dy < 0) {
    dy = -dy;
    stepy = -1;
  } else {
    stepy = 1;
  }

  if (dx < 0) {
    dx = -dx;
    stepx = -1;
  } else {
    stepx = 1;
  }

  dy <<= 1; // dy is now 2*dy
  dx <<= 1; // dx is now 2*dx
  remplirPixel(x0, y0);

  if (dx > dy) {
    int fraction = dy - (dx >> 1);
    while (x0 != x1) {
      if (fraction >= 0) {
        y0 += stepy;
        fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      remplirPixel(x0, y0);
    }
  }
  else {
    int fraction = dx - (dy >> 1);
    while (y0 != y1) {
      if (fraction >= 0) {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      remplirPixel(x0, y0);
    }
  }
}

void remplirRectangle(int x0, int y0, int x1, int y1) {
  int xDiff;

  if (x0 > x1) {
    xDiff = x0 - x1;
  } else {
    xDiff = x1 - x0;
  }

  while (xDiff > 0) {
    remplirLigne(x0, y0, x0, y1);

    if (x0 > x1) {
      x0--;
    } else {
      x0++;
    }

    xDiff--;
  }
}

bool pointDansSegmentOrthogonal(byte xPoint, byte yPoint, byte xSegment1, byte ySegment1, byte xSegment2, byte ySegment2) {
  if (yPoint == ySegment1 && yPoint == ySegment2) {
    // le point est sur la meme absysse
    return xPoint >= xSegment1 && xPoint <= xSegment2;
  }
  if (xPoint == xSegment1 && xPoint == xSegment2) {
    // le point est sur la meme ordonnee
    return yPoint >= ySegment1 && yPoint <= ySegment2;
  }

  return false;
}

bool pointSurLeParcours(byte xPoint, byte yPoint) {
  bool pointSurParcours = false;
  for (int i = 0; i < longueurParcours - 1; i++) {
    pointSurParcours = pointSurParcours || pointDansSegmentOrthogonal(xPoint, yPoint, parcours[i].x, parcours[i].y, parcours[i + 1].x, parcours[i + 1].y);
  }

  return pointSurParcours;
}

void choisirLaZoneLaPlusPetite() {
  if (longueurParcours <= 2) {
    return;
  }

  bool dansLaPartie1 = false;
  bool pixelPrecedentDansLaZoneCouverte = false;
  int pixelsDansLaPartie1 = 0;
  int pixelsDansLaPartie2 = 0;
  for (int x = 0; x < LARGEUR_ECRAN; x++) {
    for (int y = 0; y < HAUTEUR_ECRAN; y++) {
      bool pointSurParcours = pointSurLeParcours(x, y);
      bool pixelCourantDansLaZoneCouverte = trouverPresencePixel(x, y);

      if (pixelPrecedentDansLaZoneCouverte && !pixelCourantDansLaZoneCouverte) {
        dansLaPartie1 = !dansLaPartie1;
      }

      if (!pixelCourantDansLaZoneCouverte) {
        if (dansLaPartie1) {
          pixelsDansLaPartie1++;
        } else {
          pixelsDansLaPartie2++;
        }
      }

      pixelPrecedentDansLaZoneCouverte = pixelCourantDansLaZoneCouverte;
    }
  }

  char buf [25];
  sprintf (buf, "points dans la partie 1 %d, dans la partie 2 %d", pixelsDansLaPartie1, pixelsDansLaPartie2);
  Serial.println(buf);
}

void remplirEnCreantDesRectanglesAChaquePoint() {
  for (int i = 0; i < longueurParcours - 2; i++) {
    byte xMin = 255;
    byte xMax = 0;
    byte yMin = 255;
    byte yMax = 0;

    for (int j = 0; j < 3; j++) {
      if (xMin > parcours[i + j].x) xMin = parcours[i + j].x;
      if (xMax < parcours[i + j].x) xMax = parcours[i + j].x;
      if (yMin > parcours[i + j].y) yMin = parcours[i + j].y;
      if (yMax < parcours[i + j].y) yMax = parcours[i + j].y;
    }

    remplirRectangle(xMin, yMin, xMax, yMax);
  }
}

void remplirNouvelleZone() {
  if (longueurParcours <= 2) {
    return;
  }

  Serial.println("remplissons une zone");

  for (int i = 0; i < longueurParcours; i++) {
    char buf [25];
    sprintf (buf, "point %d, x: %d, y: %d", i, parcours[i].x, parcours[i].y);
    Serial.println(buf);
  }

  remplirEnCreantDesRectanglesAChaquePoint();
  choisirLaZoneLaPlusPetite();
}

void deplacer() {
  byte xJoueurPrecedent = xJoueur;
  byte yJoueurPrecedent = yJoueur;
  debugJeu(xJoueur, yJoueur);
  if (toucheGauche() && toucheA()) {
    xJoueur = xJoueur - 1;
  }
  if (toucheDroite() && toucheA()) {
    xJoueur = xJoueur + 1;
  }
  if (toucheHaut() && toucheA()) {
    yJoueur = yJoueur - 1;
  }
  if (toucheBas() && toucheA()) {
    yJoueur = yJoueur + 1;
  }

  if (trouverCollision(xJoueur, yJoueur) == PAS_DE_COLLISION) {
    if (longueurParcours == 0) {
      parcours[longueurParcours] = {xJoueurPrecedent, yJoueurPrecedent};
      longueurParcours++;
    } else if (parcours[longueurParcours - 1].x != xJoueur && parcours[longueurParcours - 1].y != yJoueur) {
      parcours[longueurParcours] = {xJoueurPrecedent, yJoueurPrecedent};
      longueurParcours++;
    }
  } else {
    if (longueurParcours > 0) {
      parcours[longueurParcours] = {xJoueur, yJoueur};
      longueurParcours++;

      remplirNouvelleZone();

      longueurParcours = 0;
    }
  }

  for (int i=1; i < NOMBRE_DE_BARRES ; i++) {
    barres[NOMBRE_DE_BARRES - i].point1.coord.x = barres[NOMBRE_DE_BARRES - i - 1].point1.coord.x;
    barres[NOMBRE_DE_BARRES - i].point1.coord.y = barres[NOMBRE_DE_BARRES - i - 1].point1.coord.y;
    barres[NOMBRE_DE_BARRES - i].point2.coord.x = barres[NOMBRE_DE_BARRES - i - 1].point2.coord.x;
    barres[NOMBRE_DE_BARRES - i].point2.coord.y = barres[NOMBRE_DE_BARRES - i - 1].point2.coord.y;
  }

  barres[0].point1.coord.x = barres[0].point1.coord.x + barres[0].point1.directionX;
  barres[0].point1.coord.y = barres[0].point1.coord.y + barres[0].point1.directionY;
  barres[0].point2.coord.x = barres[0].point2.coord.x + barres[0].point2.directionX;
  barres[0].point2.coord.y = barres[0].point2.coord.y + barres[0].point2.directionY;

  barres[0].point1 = gererCollisionSurUnPoint(barres[0].point1);
  barres[0].point2 = gererCollisionSurUnPoint(barres[0].point2);
}

void dessinerJoueur() {
  creerRectangle(xJoueur - 1, yJoueur - 1, xJoueur + 1, yJoueur, false, NOIR);
}

void dessiner() {
  for (int i = 0; i < NOMBRE_DE_BARRES ; i++) {
    ligneEcran(barres[i].point1.coord.x, barres[i].point1.coord.y, barres[i].point2.coord.x, barres[i].point2.coord.y, NOIR);
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

