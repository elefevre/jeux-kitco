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

#define ECRAN_ACCUEIL 0
#define PARTIE 1
#define ECRAN_FIN_DE_JEU 2
byte modeJeu = ECRAN_ACCUEIL;

#define NOMBRE_MECHANTS 12
#define NOMBRE_TIRS_MECHANTS 3
#define NOMBRE_MECHANTS_HORIZONTAL 4
#define NOMBRE_MECHANTS_VERTICAL 3

#define LARGEUR_JOUEUR 11
#define HAUTEUR_JOUEUR 5
#define LARGEUR_MECHANT 11
#define HAUTEUR_MECHANT 8
#define LARGEUR_TIR_MECHANT 3
#define HAUTEUR_TIR_MECHANT 2
#define ESPACEMENT_HORIZONTAL_MECHANT 13
#define ESPACEMENT_VERTICAL_MECHANT 8

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

bool modeMechant = false;
int temps = 0;
int tempo = 50;
bool tirEnCours = false;
byte xTir = 20;
byte yTir = 40;
byte xJoueur = 20;
byte yJoueur = 43;
char directionMechants = 1;
int mechants[NOMBRE_MECHANTS_HORIZONTAL][NOMBRE_MECHANTS_VERTICAL];
int tirsMechants[NOMBRE_TIRS_MECHANTS];

void demarrer() {
  randomSeed(millis());

  modeMechant = false;
  temps = 0;
  tempo = 50;
  tirEnCours = false;
  xTir = 20;
  yTir = 40;
  xJoueur = 20;
  yJoueur = 43;
  directionMechants = 1;
  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      mechants[i][j] = xy(ESPACEMENT_HORIZONTAL_MECHANT * i, ESPACEMENT_VERTICAL_MECHANT * j, true);
    }
  }
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    tirsMechants[i] = xy(0, 0, false);
  }
}


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

bool tirMechantSprite[HAUTEUR_TIR_MECHANT][LARGEUR_TIR_MECHANT] {
  {WW, WW, WW},
  {OO, WW, OO}
};

void ecrireChiffre(int i, byte x, byte y) {
  ecrireLettre('0'+i%10,x+15,y,NOIR);
  if (i>=10) ecrireLettre('0'+(i/10)%10,x+10,y,NOIR);
  if (i>=100) ecrireLettre('0'+(i/100)%10,x+5,y,NOIR);
  if (i>=1000) ecrireLettre('0'+(i/1000)%10,x,y,NOIR);
}

void dessinerMechant(int mechant, bool mode) {
  if (!estVivant(mechant)) {
    return;
  }
  for (byte x=0; x < LARGEUR_MECHANT; x++) {
    for (byte y=0; y < HAUTEUR_MECHANT; y++) {
      if (mode) {
        if (mechantSprite1[x + y*LARGEUR_MECHANT]) setPixel(toX(mechant) + x, toY(mechant) + y);
      } else {
        if (mechantSprite2[x + y*LARGEUR_MECHANT]) setPixel(toX(mechant) + x, toY(mechant) + y);
      }
    }
  }
}

void dessinerTirMechant(int tir) {
  if (!estVivant(tir)) {
    return;
  }

  for (byte x=0; x < LARGEUR_TIR_MECHANT; x++) {
    for (byte y=0; y < HAUTEUR_TIR_MECHANT; y++) {
        if (tirMechantSprite[x][y]) setPixel(toX(tir) + x, toY(tir) + y);
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
  modeJeu = ECRAN_FIN_DE_JEU;
  delai(500);
}

void gererCollisions() {
  bool tousLesMechantsSontMorts = true;
  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      int mechant = mechants[i][j];
      if (!estVivant(mechant)) {
        continue;
      }
      tousLesMechantsSontMorts = false;

      if (tirEnCours && toucheCible(xTir, yTir, toX(mechant), toY(mechant), LARGEUR_MECHANT, HAUTEUR_MECHANT)) {
        mechants[i][j] = xy(0, 0, false);
        tirEnCours = false;
      }

      if (toucheCible(xJoueur, yJoueur, toX(mechant), toY(mechant), LARGEUR_MECHANT, HAUTEUR_MECHANT)
          || toucheCible(xJoueur + LARGEUR_JOUEUR, yJoueur + HAUTEUR_JOUEUR, toX(mechant), toY(mechant), LARGEUR_MECHANT, HAUTEUR_MECHANT)) {
        finirJeu("PERDU :-(");
      }
    }
  }

  if (tousLesMechantsSontMorts) {
      finirJeu("GAGNE !");
  }
  
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    if (!estVivant(tirsMechants[i])) {
      continue;
    }
    if (toucheCible(xJoueur, yJoueur, toX(tirsMechants[i]), toY(tirsMechants[i]), LARGEUR_TIR_MECHANT, HAUTEUR_TIR_MECHANT)
        || toucheCible(xJoueur + LARGEUR_JOUEUR, yJoueur + HAUTEUR_JOUEUR, toX(tirsMechants[i]), toY(tirsMechants[i]), LARGEUR_TIR_MECHANT, HAUTEUR_TIR_MECHANT)) {
      finirJeu("PERDU :-(");
    }
  }
}

void deplacerMechants() {
  bool toucheLimiteDroite = false;
  bool toucheLimiteGauche = false;
  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      int mechant = mechants[i][j];
      if (!estVivant(mechant)) {
        continue;
      }

      if (toX(mechant) + LARGEUR_MECHANT == LARGEUR_ECRAN) {
        if (directionMechants == 1) toucheLimiteDroite = true;
      }
      if (toX(mechant) == 0) {
        if (directionMechants == -1) toucheLimiteGauche = true;
      }
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

  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      int mechant = mechants[i][j];
      if (!estVivant(mechant)) {
        continue;
      }

      mechants[i][j] = xy(toX(mechant) + directionMechants, toY(mechant) + incrementY, true);
    }
  }
}

void deplacerTir() {
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    if (estVivant(tirsMechants[i])) {
      if (toY(tirsMechants[i]) + 1 >= HAUTEUR_ECRAN) {
        tirsMechants[i] = xy(0, 0, false);
      } else {
        tirsMechants[i] = xy(toX(tirsMechants[i]), toY(tirsMechants[i]) + 1, true);
      }
    }
  }

  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      int mechant = mechants[i][j];
      if (!estVivant(mechant)) {
        continue;
      }

      byte unMechantTire=random(0,100);
      if (unMechantTire < 1) { // 1% de chance qu'un mechant tire
        bool tirLance = false;
        for (int k = 0; k < NOMBRE_TIRS_MECHANTS ; k++) {
          if (!tirLance && !estVivant(tirsMechants[k])) {
            tirsMechants[k] = xy(toX(mechant) + LARGEUR_MECHANT / 2, toY(mechant) + HAUTEUR_MECHANT, true);
            tirLance = true;
          }
        }
      }
    }
  }
  
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
  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      dessinerMechant(mechants[i][j], modeMechant);
    }
  }
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    dessinerTirMechant(tirsMechants[i]);
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


void loop() {
  switch (modeJeu) {
    case ECRAN_ACCUEIL:
      effacerEcran(BLANC);
      ecrireEcran("SPACE", 25, 10, NOIR);
      ecrireEcran("INVADERS", 15, 20, NOIR);
      rafraichirEcran();
      if (touche()) {
        demarrer();
        modeJeu = PARTIE;
      }
      break;

    case PARTIE:
      temps = temps + tempo;
    
      effacerEcran(BLANC);
      deplacerMechants();
      deplacerTir();
      dessiner();
    
      gererCollisions();
    
      rafraichirEcran();
      gererTouches();
    
      delai(tempo);
      break;

    case ECRAN_FIN_DE_JEU:
      delai(100);
      if (touche()) {
        modeJeu = ECRAN_ACCUEIL;
      }
      break;

  }
}
