#include "model.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

// Définition des chemins vers les ressources (polices, arrière-plan, textures
// des pièces)
#define FONT "res/georgiaz.ttf"
#define FONTSIZE 36
#define BACKGROUND "res/background.png"
#define BUTTON_COUTN 5
#define TRAIN_WIDTH 100

#define IMG_SEGMENT "res/segment.png"
#define IMG_CORNER "res/corner.png"
#define IMG_TEE "res/tee.png"
#define IMG_ENDPOINT "res/endpoint.png"
#define IMG_CROSS "res/cross.png"
#define IMG_EMPTY "res/empty.png"

#define IMG_TRAIN "res/train.png"
#define IMG_RANDOM "res/random.png"
#define IMG_DEFAULT "res/default.png"
#define IMG_SOLVE "res/solve.png"
#define IMG_UNDO "res/undo.png"
#define IMG_REDO "res/redo.png"
#define IMG_WIN "res/youwin.png"
#define IMG_LOAD "res/load.png"
#define IMG_SAVE "res/save.png"

// Définition de la structure principale du jeu
typedef struct game_s *game;
// Structure contenant les informations nécessaires à l'environnement SDL
struct Env_t {
  SDL_Texture *background;  // Texture de l'arrière-plan
  SDL_Texture *text;        // Texture pour le texte

  SDL_Texture *segment;   // Texture pour les segments
  SDL_Texture *corner;    // Texture pour les corners
  SDL_Texture *tee;       // Texture pour les tees
  SDL_Texture *endpoint;  // Texture pour les endpoints
  SDL_Texture *cross;     // Texture pour les cross
  SDL_Texture *empty;     // Texture pour les empty

  SDL_Texture *train;
  SDL_Texture *btn_random;   // Bouton pour game_random
  SDL_Texture *btn_default;  // Bouton pour game_default
  SDL_Texture *btn_solve;    // Bouton pour game_solve
  SDL_Texture *btn_undo;     // Bouton pour undo
  SDL_Texture *btn_redo;     // Bouton pour redo
  SDL_Texture *btn_load;     // Bouton pour game_load
  SDL_Texture *btn_save;     // Bouton pour game_save

  SDL_Texture *win_image;  // Texture pour you_win

  game g;  // Structure contenant le jeu
};

// Fonction pour afficher un message lorsque le jeu est résolu
bool display_you_win(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  // Récupérer la taille de la fenêtre
  int width, height;
  SDL_GetWindowSize(win, &width, &height);

  /// Définir la taille agrandie de l'image "YOU WIN!"
  int scale_factor = 4;  // Ajuste cette valeur pour modifier l'agrandissement
  int img_w = (width / 4) * scale_factor;
  int img_h = (height / 6) * scale_factor;

  // Positionner l'image bien au centre
  SDL_Rect img_rect = {(width - img_w) / 2, (height - img_h) / 2, img_w, img_h};

  // Afficher l'image
  SDL_RenderCopy(ren, env->win_image, NULL, &img_rect);
  SDL_RenderPresent(ren);

  // Attendre que l'utilisateur clique pour continuer
  SDL_Event event;
  bool waiting = true;
  while (waiting) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
        return true;  // Quitter complètement le jeu
      }
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        waiting = false;  // Un clic détecté alors continuer
      }
    }
  }

  // Effacer l'écran après le clic
  SDL_RenderClear(ren);
  render(win, ren, env);
  SDL_RenderPresent(ren);

  return false;  // Continuer le jeu
}

void button_game_default(Env *env) {
  game_delete(env->g);      // Supprime le jeu actuel
  env->g = game_default();  // Initialise une grille par défaut
  printf("Default game created\n");
}

void button_game_solve(Env *env) {
  printf("Solving game... \n");
  game_solve(env->g);
  if (game_solve(env->g)) {
    printf("Game solved !\n");
  } else {
    printf("No solution found\n");
  }
}

void button_game_undo(Env *env) { game_undo(env->g); }

void button_game_redo(Env *env) { game_redo(env->g); }

void button_game_random(Env *env) {
  game_delete(env->g);

  // Génération aléatoire des paramètres
  uint rows = rand() % 14 + 2;  // Nombre de lignes entre 3 et 14
  uint cols = rand() % 14 + 2;  // Nombre de colonnes entre 3 et 14
  bool wrapping = rand() % 2;   // Wrapping : true (1) ou false (0) aléatoire
  int nb_empty = 0;
  int nb_extra = 0;

  env->g = game_random(rows, cols, wrapping, nb_empty, nb_extra);
  game_shuffle_orientation(env->g);

  // Afficher les détails dans la console pour vérification
  printf("Random game created : <nb_rows>x<nb_cols> <wrapping>\n");
  printf("%dx%d", rows, cols);
  printf(" %s\n", wrapping ? "True" : "False");
}

void button_game_save(Env *env) {
  game_save(env->g, "savefile.txt");  // Sauvegarde sans utiliser de condition
  printf("Game saved to savefile.txt\n");
}

void button_game_load(Env *env) {
  game_delete(env->g);  // Supprime l'ancien jeu
  env->g = game_load("savefile.txt");

  if (env->g) {
    printf("Game loaded successfully!\n");
  } else {
    printf("Error: Could not load game.\n");
    env->g = game_default();  // Revenir au jeu par défaut en cas d'échec
  }
}

// Initialisation de l'environnement SDL, des textures et du jeu
Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  PRINT(
      "Appuyez sur ESC pour quitter. Initialisation de la grille de jeu...\n");

  // Chargement de la texture pour l'arrière-plan
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture : %s\n", BACKGROUND);

  // Initialisation de la structure de jeu avec une grille par défaut
  env->g = game_default();
  // env->g = game_default();  // Crée une grille de jeu par défaut

  // Chargement des textures pour les formes de pièces et des boutons
  env->segment = IMG_LoadTexture(ren, IMG_SEGMENT);
  if (!env->segment) ERROR("IMG_LoadTexture : %s\n", IMG_SEGMENT);

  env->corner = IMG_LoadTexture(ren, IMG_CORNER);
  if (!env->corner) ERROR("IMG_LoadTexture : %s\n", IMG_CORNER);

  env->tee = IMG_LoadTexture(ren, IMG_TEE);
  if (!env->tee) ERROR("IMG_LoadTexture : %s\n", IMG_TEE);

  env->endpoint = IMG_LoadTexture(ren, IMG_ENDPOINT);
  if (!env->endpoint) ERROR("IMG_LoadTexture : %s\n", IMG_ENDPOINT);

  env->cross = IMG_LoadTexture(ren, IMG_CROSS);
  if (!env->cross) ERROR("IMG_LoadTexture : %s\n", IMG_CROSS);

  env->empty = IMG_LoadTexture(ren, IMG_EMPTY);
  if (!env->empty) ERROR("IMG_LoadTexture : %s\n", IMG_EMPTY);

  env->train = IMG_LoadTexture(ren, IMG_TRAIN);
  if (!env->train) ERROR("IMG_LoadTexture : %s\n", IMG_TRAIN);

  env->btn_random = IMG_LoadTexture(ren, IMG_RANDOM);
  if (!env->btn_random) ERROR("IMG_LoadTexture : %s\n", IMG_RANDOM);

  env->btn_default = IMG_LoadTexture(ren, IMG_DEFAULT);
  if (!env->btn_default) ERROR("IMG_LoadTexture : %s\n", IMG_DEFAULT);

  env->btn_solve = IMG_LoadTexture(ren, IMG_SOLVE);
  if (!env->btn_solve) ERROR("IMG_LoadTexture : %s\n", IMG_SOLVE);

  env->btn_undo = IMG_LoadTexture(ren, IMG_UNDO);
  if (!env->btn_undo) ERROR("IMG_LoadTexture : %s\n", IMG_UNDO);

  env->btn_redo = IMG_LoadTexture(ren, IMG_REDO);
  if (!env->btn_redo) ERROR("IMG_LoadTexture : %s\n", IMG_REDO);

  env->win_image = IMG_LoadTexture(ren, IMG_WIN);
  if (!env->win_image) ERROR("IMG_LoadTexture : %s\n", IMG_WIN);

  env->btn_load = IMG_LoadTexture(ren, IMG_LOAD);
  if (!env->btn_load) ERROR("IMG_LoadTexture : %s\n", IMG_LOAD);

  env->btn_save = IMG_LoadTexture(ren, IMG_SAVE);
  if (!env->btn_save) ERROR("IMG_LoadTexture : %s\n", IMG_SAVE);

  game_print(env->g);
  return env;
}

// Fonction pour afficher les textures sur la fenêtre SDL
void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  // Récupération des dimensions de la fenêtre
  int width, height;
  SDL_GetWindowSize(win, &width, &height);

  int top_bar_height = height / 10;  // 10% hauteur pour barre
  int train_height = top_bar_height;
  int margin = width / 10;

  // Affichage de l'arrière-plan
  SDL_RenderCopy(ren, env->background, NULL, NULL);

  // Affichage du train
  SDL_Rect train_rect = {0, 0, TRAIN_WIDTH, train_height};
  SDL_RenderCopy(ren, env->train, NULL, &train_rect);

  // Affichage des boutons
  int btn_width = (width - TRAIN_WIDTH) / BUTTON_COUTN;
  int btn_height = top_bar_height;

  SDL_Texture *btn_textures[BUTTON_COUTN];
  btn_textures[0] = env->btn_random;
  btn_textures[1] = env->btn_default;
  btn_textures[2] = env->btn_solve;
  btn_textures[3] = env->btn_undo;
  btn_textures[4] = env->btn_redo;

  for (int i = 0; i < BUTTON_COUTN; i++) {
    SDL_Rect btn_rect = {TRAIN_WIDTH + i * btn_width, 0, btn_width, btn_height};
    if (btn_textures[i]) {
      SDL_RenderCopy(ren, btn_textures[i], NULL, &btn_rect);
    }
  }

  // Dimensions des boutons LOAD et SAVE
  int btn_w = width / 10;
  int btn_h = height / 4;

  // Position verticale des boutons
  int btn_x_load = width / 150;                   // Placé sur le côté gauche
  int btn_x_save = width - (width / 50) - btn_w;  // Placé sur le côté droit
  int btn_y = (height - btn_h) / 2;               // Centré verticalement

  // Définition des rectangles
  SDL_Rect btn_load_rect = {btn_x_load, btn_y, btn_w,
                            btn_h};  // Bouton LOAD à gauche
  SDL_Rect btn_save_rect = {btn_x_save, btn_y, btn_w,
                            btn_h};  // Bouton SAVE à droite

  SDL_RenderCopyEx(ren, env->btn_load, NULL, &btn_load_rect, 0, NULL,
                   SDL_FLIP_NONE);
  SDL_RenderCopyEx(ren, env->btn_save, NULL, &btn_save_rect, 0, NULL,
                   SDL_FLIP_NONE);

  // Calcul les dimensions grilles
  int grid_width = width - 2 * margin;
  int grid_height = height - top_bar_height - 2 * margin;

  uint rows = game_nb_rows(env->g);
  uint cols = game_nb_cols(env->g);
  int cell_size = fmin(grid_width / cols, grid_height / rows);

  int offset_x = (width - (cell_size * cols)) / 2;
  int offset_y =
      top_bar_height + (height - top_bar_height - (cell_size * rows)) / 2;

  // Affichage les pièces de la grille
  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      SDL_Rect cell = {offset_x + j * cell_size, offset_y + i * cell_size,
                       cell_size, cell_size};

      shape s = game_get_piece_shape(env->g, i, j);
      direction d = game_get_piece_orientation(env->g, i, j);

      SDL_Texture *texture = NULL;
      switch (s) {
        case SEGMENT:
          texture = env->segment;
          break;
        case CORNER:
          texture = env->corner;
          break;
        case TEE:
          texture = env->tee;
          break;
        case ENDPOINT:
          texture = env->endpoint;
          break;
        case CROSS:
          texture = env->cross;
          break;
        case EMPTY:
          texture = env->empty;
          break;
        default:
          break;
      }

      if (texture) {
        // Déterminer angle de rotation
        double angle = 0.0;
        if (d == EAST)
          angle = 90.0;
        else if (d == SOUTH)
          angle = 180.0;
        else if (d == WEST)
          angle = 270.0;

        SDL_RenderCopyEx(ren, texture, NULL, &cell, angle, NULL, SDL_FLIP_NONE);
      }
    }
  }
}

// Fonction pour gérer les événements (clavier, souris, etc.)
bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;  // Quitter l'application
  }

  if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_ESCAPE:
        return true;  // Quitter avec Échap
      default:
        break;
    }
  }

  if (e->type == SDL_MOUSEBUTTONDOWN) {
    int x = e->button.x;
    int y = e->button.y;

    // Récupérer la taille de la fenêtre
    int width, height;
    SDL_GetWindowSize(win, &width, &height);

    // Dimensions des boutons supérieurs
    int btn_width = (width - TRAIN_WIDTH) / BUTTON_COUTN;
    int btn_height = height / 10;

    // Vérifier si le clic est sur la barre supérieure
    if (y >= 0 && y <= btn_height) {
      if (x >= TRAIN_WIDTH) {  // Ignorer la zone du train
        int btn_index = (x - TRAIN_WIDTH) / btn_width;

        switch (btn_index) {
          case 0:
            button_game_random(env);  // Nouvelle partie aléatoire
            break;
          case 1:
            button_game_default(env);  // Réinitialiser la partie
            break;
          case 2:
            button_game_solve(env);  // Résoudre le jeu
            SDL_RenderClear(ren);
            render(win, ren, env);
            SDL_RenderPresent(ren);
            display_you_win(win, ren, env);
            break;
          case 3:
            button_game_undo(env);  // Annuler le dernier coup
            break;
          case 4:
            button_game_redo(env);  // Refaire le dernier coup
            break;
          default:
            break;
        }
        // Rafraîchir l'affichage après l'action du bouton
        SDL_RenderClear(ren);
        render(win, ren, env);
        SDL_RenderPresent(ren);
        return false;
      }
    }

    // Vérifier si le clic est sur les boutons LOAD / SAVE
    int btn_w = width / 12;
    int btn_h = height / 6;
    int btn_y = (height - btn_h) / 2;

    // Position des boutons
    int btn_x_load = width / 150;
    int btn_x_save = width - (width / 50) - btn_w;

    SDL_Rect btn_load_rect = {btn_x_load, btn_y, btn_w, btn_h};
    SDL_Rect btn_save_rect = {btn_x_save, btn_y, btn_w, btn_h};

    // Vérifier si le clic est sur le bouton LOAD
    if (x >= btn_load_rect.x && x <= btn_load_rect.x + btn_w &&
        y >= btn_load_rect.y && y <= btn_load_rect.y + btn_h) {
      button_game_load(env);
    }

    // Vérifier si le clic est sur le bouton SAVE
    if (x >= btn_save_rect.x && x <= btn_save_rect.x + btn_w &&
        y >= btn_save_rect.y && y <= btn_save_rect.y + btn_h) {
      button_game_save(env);
    }

    // Vérifier si le clic est dans la grille
    uint rows = game_nb_rows(env->g);
    uint cols = game_nb_cols(env->g);
    int cell_w = fmin((width - 2 * (width / 20)) / cols,
                      (height - (height / 10) - 2 * (width / 20)) / rows);
    int grid_x = (width - (cols * cell_w)) / 2;
    int grid_y = (height / 10) + (height - (height / 10) - (rows * cell_w)) / 2;

    if (x >= grid_x && x < (grid_x + cols * cell_w) && y >= grid_y &&
        y < (grid_y + rows * cell_w)) {
      int col = (x - grid_x) / cell_w;
      int row = (y - grid_y) / cell_w;

      // Jouer le coup sur la grille

      // Gérer le clic gauche ou droit
      if (e->button.button == SDL_BUTTON_LEFT) {
        game_play_move(env->g, row, col, 1);  // Rotation horaire
      } else if (e->button.button == SDL_BUTTON_RIGHT) {
        game_play_move(env->g, row, col, -1);  // Rotation anti-horaire
      }

      // Rafraîchir l'affichage après le mouvement
      SDL_RenderClear(ren);
      render(win, ren, env);
      SDL_RenderPresent(ren);

      // Vérifier si le jeu est terminé
      if (game_won(env->g)) {
        return display_you_win(win, ren, env);
      }
    }
  }
  return false;  // Continuer l'application
}

// Libération de la mémoire et des ressources SDL
void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->text);

  SDL_DestroyTexture(env->segment);
  SDL_DestroyTexture(env->corner);
  SDL_DestroyTexture(env->tee);
  SDL_DestroyTexture(env->endpoint);
  SDL_DestroyTexture(env->cross);
  SDL_DestroyTexture(env->empty);

  SDL_DestroyTexture(env->train);
  SDL_DestroyTexture(env->btn_random);
  SDL_DestroyTexture(env->btn_default);
  SDL_DestroyTexture(env->btn_solve);
  SDL_DestroyTexture(env->btn_undo);
  SDL_DestroyTexture(env->btn_redo);
  SDL_DestroyTexture(env->btn_load);
  SDL_DestroyTexture(env->btn_save);

  SDL_DestroyTexture(env->win_image);

  game_delete(env->g);  // Supprime le jeu
  free(env);            // Libère la mémoire de l'environnement
}