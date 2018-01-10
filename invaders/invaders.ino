// Un mini-Space Invader
// Développé en janvier 2018 par @elefevre pour la console kitco

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

#define LARGEUR_TITRE 64
#define HAUTEUR_TITRE 22
#define LARGEUR_JOUEUR 11
#define HAUTEUR_JOUEUR 5
#define LARGEUR_MECHANT 11
#define HAUTEUR_MECHANT 8
#define LARGEUR_TIR_MECHANT 3
#define HAUTEUR_TIR_MECHANT 5
#define ESPACEMENT_HORIZONTAL_MECHANT 13
#define ESPACEMENT_VERTICAL_MECHANT 8
#define SPRITE_INACTIF 0
#define NOMBRE_STYLE_MECHANT 3

// retourne la configuration d'un sprite actif
long xy(byte x, byte y, byte style) {
  // le dernier 1 indique que le sprite est actif (pas mort)
  return ((((((long) style << 7) + x) << 7) + y) << 1) + 1;
}

byte toX(long b) {
  return ((b >> 7) >> 1)&B01111111;
}

byte toY(long b) {
  return (b&B01111111) >> 1;
}

byte toStyle(long b) {
  return (((b >> 7) >> 7) >> 1)&B11;
}

bool estActif(long b) {
  return (b&B01) == 1;
}

byte modeMechant = 0;
bool tirEnCours = false;
byte xTir = 20;
byte yTir = 40;
byte xJoueur = 20;
byte yJoueur = 43;
char directionMechants = 1;
long mechants[NOMBRE_MECHANTS_HORIZONTAL][NOMBRE_MECHANTS_VERTICAL];
long tirsMechants[NOMBRE_TIRS_MECHANTS];
byte niveau = 1;
#define ANIMATION_MECHANTS 0
#define DEPLACEMENTS_MECHANTS 1
#define DEPLACEMENTS_TIRS_MECHANTS 2
#define DEPLACEMENTS_TIRS_JOUEUR 3
long unsigned int timings[] = {millis(), millis(), millis(), millis()};
long unsigned int tempos[] = {1000, 150, 200, 10};


bool timingExpire(byte index) {
  if (millis() - timings[index] > tempos[index]) {
    timings[index] = millis();
    return true;
  }
  return false;
}

void demarrer(byte nouveauNiveau) {
  niveau = nouveauNiveau;

  randomSeed(millis());

  timings[ANIMATION_MECHANTS] = millis();
  timings[DEPLACEMENTS_MECHANTS] = millis();
  timings[DEPLACEMENTS_TIRS_MECHANTS] = millis();
  timings[DEPLACEMENTS_TIRS_JOUEUR] = millis();
  tempos[ANIMATION_MECHANTS] = 1000;
  tempos[DEPLACEMENTS_MECHANTS] = 150 - 30 * niveau;
  tempos[DEPLACEMENTS_TIRS_MECHANTS] = 200 - 40 * niveau;
  tempos[DEPLACEMENTS_TIRS_JOUEUR] = 10;
  modeMechant = 0;
  tirEnCours = false;
  xTir = 20;
  yTir = 40;
  xJoueur = 20;
  yJoueur = 43;
  directionMechants = 1;
  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      mechants[i][j] = xy(ESPACEMENT_HORIZONTAL_MECHANT * i, ESPACEMENT_VERTICAL_MECHANT * j, j % NOMBRE_STYLE_MECHANT);
    }
  }
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    tirsMechants[i] = SPRITE_INACTIF;
  }
}


byte titre[] {
B00000000,B00000111,B11001111,B11000111,B11000011,B11100011,B11110000,B00000000,
B00000000,B00001111,B11101111,B11100111,B11000111,B11110111,B11110000,B00000000,
B00000000,B00001111,B11101111,B11110111,B11000111,B11110111,B11100000,B00000000,
B00000000,B00001111,B01110111,B01110111,B11100111,B01110111,B00000000,B00000000,
B00000000,B00000111,B01110111,B01110111,B11100111,B01110111,B00000000,B00000000,
B00000000,B00000111,B00000111,B01110110,B11100110,B00001111,B11000000,B00000000,
B00000000,B00000011,B11110111,B11110110,B11101110,B00001111,B11000000,B00000000,
B00000000,B00000001,B11110111,B11101110,B11101110,B11101110,B00000000,B00000000,
B00000000,B00000010,B00111011,B00001111,B11101110,B11101110,B00000000,B00000000,
B00000000,B00000011,B11111011,B10001110,B01111111,B11101111,B11000000,B00000000,
B00000000,B00000001,B11110011,B10001110,B01110111,B11011111,B10000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B11101110,B11100110,B00111001,B11100011,B11111001,B11111100,B11111100,B01111110,
B11110111,B01110111,B00111001,B11110011,B11111101,B11111101,B11111110,B11111110,
B01110111,B11110111,B00111101,B11110011,B10011101,B11000001,B11011101,B11101110,
B01111011,B10111011,B10111101,B11110011,B10011101,B11000001,B11011101,B11011100,
B00111011,B11111011,B10111101,B10111011,B10011101,B11111011,B11011101,B11100000,
B00011101,B11111001,B11011101,B10111011,B10111001,B11110011,B11110001,B11111000,
B00011101,B11111100,B11011101,B10111011,B10111011,B10000011,B10111000,B00111000,
B00001110,B11011100,B11111101,B11111011,B10111011,B10000011,B10110011,B01110000,
B00001110,B11101110,B01111101,B10011111,B11110011,B11110111,B01110011,B11100000
};

byte mechantSprites[][2][16] {
{{
B00100000,B10000000,
B00010001,B00000000,
B00111111,B10000000,
B01101110,B11000000,
B11111111,B11100000,
B10111111,B10100000,
B10100000,B10100000,
B00011011,B00000000,
},
{
B00100000,B10000000,
B10010001,B00100000,
B10111111,B10100000,
B11101110,B11100000,
B11111111,B11100000,
B00111111,B10000000,
B00100000,B10000000,
B01000000,B01000000
}},{{
B00001110,B00000000,
B01111111,B11000000,
B11111111,B11100000,
B11100100,B11100000,
B11111111,B11100000,
B00111011,B10000000,
B01100100,B11000000,
B00110001,B10000000
},{
B00001110,B00000000,
B01111111,B11000000,
B11111111,B11100000,
B11100100,B11100000,
B11111111,B11100000,
B00011011,B00000000,
B00110101,B10000000,
B11000000,B01100000
}},{{
B00011110,B00000000,
B00111111,B10000000,
B01111111,B11000000,
B11101110,B11100000,
B11111111,B11100000,
B01101110,B11000000,
B10000000,B00100000,
B01100000,B11000000,
},{
B00011111,B00000000,
B00111111,B10000000,
B01111111,B11000000,
B11101110,B11100000,
B11111111,B11100000,
B00010001,B00000000,
B00101110,B10000000,
B01010001,B01000000
}}
};

byte joueurSprite[] {
B00000100,B00000000,
B10001110,B00100000,
B11111111,B11100000,
B11111111,B11100000,
B10111111,B10100000
};

byte tirMechantSprite[] {
B11000000,
B01100000,
B11000000,
B01100000,
B11000000,
};

void ecrireChiffre(long i, byte x, byte y) {
  char buf [6];
  sprintf (buf, "%03i", i);
  ecrireEcran(buf, x, y, NOIR);
}

byte largeurTableau8Bits(byte largeur) {
  return ceil(largeur / 8.0);
}

void dessinerSprite(byte *pixelsSprite, byte largeurSprite, byte hauteurSprite, byte xSprite, byte ySprite) {
  for (byte x=0; x < largeurTableau8Bits(largeurSprite); x++) {
    for (byte y=0; y < hauteurSprite; y++) {
      for (byte i=0; i < 8; i++) {
        if (pixelsSprite[x + y * largeurTableau8Bits(largeurSprite)] & (B10000000 >> i)) setPixel(xSprite + x * 8 + i, ySprite + y);
      }
    }
  }
}

void dessinerMechant(long mechant) {
  if (estActif(mechant)) {
    dessinerSprite(mechantSprites[toStyle(mechant)][modeMechant], LARGEUR_MECHANT, HAUTEUR_MECHANT, toX(mechant), toY(mechant));
  }
}

void dessinerTirMechant(long tir) {
  if (estActif(tir)) {
    dessinerSprite(tirMechantSprite, LARGEUR_TIR_MECHANT, HAUTEUR_TIR_MECHANT, toX(tir), toY(tir));
  }
}

void dessinerJoueur() {
  dessinerSprite(joueurSprite, LARGEUR_JOUEUR, HAUTEUR_JOUEUR, xJoueur, yJoueur);
}

// est-ce que 2 segments sur une meme droite se touchent ?
bool segmentsSeTouchent(byte a, byte longueurA, byte b, byte longueurB) {
  bool auMoinsUnePartieDeAEstDansB = (a >= b &&  a <= b + longueurB || a + longueurA >= b &&  a + longueurA <= b + longueurB);
  bool auMoinsUnePartieDeBEstDansA = (b >= a &&  b <= a + longueurA || b + longueurB >= a &&  b + longueurB <= a + longueurA);

  return auMoinsUnePartieDeAEstDansB || auMoinsUnePartieDeBEstDansA;
}

// est-ce que 2 rectangles se touchent ?
bool seTouchent(byte x1, byte y1, byte largeur1, byte hauteur1, byte x2, byte y2, byte largeur2, byte hauteur2) {
  bool lesCoordonneesXDesRectanglesSeTouchent = segmentsSeTouchent(x1, largeur1, x2, largeur2);
  bool lesCoordonneesYDesRectanglesSeTouchent = segmentsSeTouchent(y1, hauteur1, y2, hauteur2);

  return lesCoordonneesXDesRectanglesSeTouchent && lesCoordonneesYDesRectanglesSeTouchent;
}

void finirJeu(const char * texte) {
  ecrireEcran(texte, 20, 20, NOIR);
  modeJeu = ECRAN_FIN_DE_JEU;
  while (touche()); // attend que l'utilisateur lache la touche
}

void gererCollisions() {
  bool tousLesMechantsSontMorts = true;
  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      if (estActif(mechants[i][j])) {
        tousLesMechantsSontMorts = false;
      }
    }
  }
  if (tousLesMechantsSontMorts) {
      niveau++;
      demarrer(niveau);
      return;
  }

  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      long mechant = mechants[i][j];
      if (!estActif(mechant)) continue;

      if (tirEnCours && seTouchent(xTir, yTir, 1, 1, toX(mechant), toY(mechant), LARGEUR_MECHANT, HAUTEUR_MECHANT )) {
        mechants[i][j] = SPRITE_INACTIF;
        tirEnCours = false;
      }

      if (seTouchent(toX(mechant), toY(mechant), LARGEUR_MECHANT, HAUTEUR_MECHANT, xJoueur, yJoueur, LARGEUR_JOUEUR, HAUTEUR_JOUEUR)) {
        finirJeu("PERDU :-(");
      }
    }
  }
  
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    long tir = tirsMechants[i];
    if (!estActif(tir)) continue;

    if (seTouchent(toX(tir), toY(tir), LARGEUR_TIR_MECHANT, HAUTEUR_TIR_MECHANT, xJoueur, yJoueur, LARGEUR_JOUEUR, HAUTEUR_JOUEUR)) {
      finirJeu("PERDU :-(");
    }
  }
}

void deplacerMechants() {
  if (timingExpire(DEPLACEMENTS_MECHANTS)) {
    byte incrementY = 0;  
    for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
      for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
        long mechant = mechants[i][j];
        if (!estActif(mechant)) continue;
  
        if (toX(mechant) + LARGEUR_MECHANT >= LARGEUR_ECRAN && directionMechants > 0) {
          directionMechants = -1;
          incrementY = 2;
        }
        if (toX(mechant) == 0 && directionMechants < 0) {
          directionMechants = 1;
          incrementY = 2;
        }
      }
    }
  
    for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
      for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
        long mechant = mechants[i][j];
        if (!estActif(mechant)) continue;
  
        mechants[i][j] = xy(toX(mechant) + directionMechants, toY(mechant) + incrementY, toStyle(mechant));
      }
    } 
  }
}

void deplacerTirs() {
  if (timingExpire(DEPLACEMENTS_TIRS_JOUEUR)) {
    if (tirEnCours) {
      yTir = yTir - 1;
    
      if (yTir == 0) {
        tirEnCours = false;
      }
    }
  }

  if (timingExpire(DEPLACEMENTS_TIRS_MECHANTS)) {
    for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
      if (estActif(tirsMechants[i])) {
        if (toY(tirsMechants[i]) + 1 >= HAUTEUR_ECRAN) {
          tirsMechants[i] = SPRITE_INACTIF;
        } else {
          tirsMechants[i] = xy(toX(tirsMechants[i]), toY(tirsMechants[i]) + 1, toStyle(tirsMechants[i]));
        }
      }
    }
  }

  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      long mechant = mechants[i][j];
      if (!estActif(mechant)) continue;

      byte unMechantTire=random(0, 100);
      if (unMechantTire < 1) { // 1% de chance qu'un mechant tire
        bool tirLance = false;
        for (int k = 0; k < NOMBRE_TIRS_MECHANTS ; k++) {
          if (!tirLance && !estActif(tirsMechants[k])) {
            tirsMechants[k] = xy(toX(mechant) + LARGEUR_MECHANT / 2, toY(mechant) + HAUTEUR_MECHANT, 0);
            tirLance = true;
          }
        }
      }
    }
  }
}

void dessinerMechants() {
  if (timingExpire(ANIMATION_MECHANTS)) {
    modeMechant = (modeMechant == 0) ? 1 : 0;
  }

  for (int i = 0; i < NOMBRE_MECHANTS_HORIZONTAL ; i++) {
    for (int j = 0; j < NOMBRE_MECHANTS_VERTICAL ; j++) {
      dessinerMechant(mechants[i][j]);
    }
  }
}

void dessinerTirs() {
  for (int i = 0; i < NOMBRE_TIRS_MECHANTS ; i++) {
    dessinerTirMechant(tirsMechants[i]);
  }

  if (tirEnCours) {
    setPixel(xTir, yTir);
  }
}

void dessiner() {
  ecrireLettre('0'+niveau%10, 75, 0, NOIR);

  dessinerMechants();
  dessinerTirs();
  dessinerJoueur();
}

void deplacerJoueur() {
  if (toucheGauche() && xJoueur > 0) {
    xJoueur = xJoueur - 1;
  }
  if (toucheDroite() && xJoueur + LARGEUR_JOUEUR < LARGEUR_ECRAN) {
    xJoueur = xJoueur + 1;
  }
  if (toucheA() && !tirEnCours) {
    tirEnCours = true;
    xTir = xJoueur + LARGEUR_JOUEUR / 2;
    yTir = yJoueur;
  }

}

byte xTitre = 10;
byte yTitre = 2;
void loop() {
  switch (modeJeu) {
    case ECRAN_ACCUEIL:
      if (timingExpire(ANIMATION_MECHANTS)) modeMechant = (modeMechant == 0) ? 1 : 0;
      effacerEcran(BLANC);
      dessinerSprite(titre, LARGEUR_TITRE, HAUTEUR_TITRE, xTitre, yTitre);
      dessinerMechant(xy(25, 30, 0));
      dessinerMechant(xy(45, 30, 0));
      rafraichirEcran();
      if (touche()) {
        // attend que l'utilisateur arrete de presser la touche
        // pour eviter qu'il tire automatiquement au debut du jeu
        while(touche());
        modeJeu = PARTIE;
        demarrer(1);
      }
      break;

    case PARTIE:
      effacerEcran(BLANC);

      deplacerMechants();
      deplacerTirs();
      deplacerJoueur();
      dessiner();
      gererCollisions();
    
      rafraichirEcran();
    
      delai(10);
      break;

    case ECRAN_FIN_DE_JEU:
      delai(1000);
      while (!touche()); // attend que l'utilisateur appuie sur une touche
      while (touche()); // attend que l'utilisateur lache la touche
      modeJeu = ECRAN_ACCUEIL;
      break;

  }
}
