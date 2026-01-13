# net-a46

## Description

Ce projet est un jeu de puzzle en solo, inspiré du jeu "Net" de la collection Simon Tatham's Portable Puzzle Collection. Le but du jeu est de connecter les pièces d'une grille pour former un réseau cohérent, où chaque pièce doit être bien orientée pour établir une connexion avec les pièces adjacentes. Le joueur peut tourner les pièces pour résoudre le puzzle.

Ce projet est divisé en plusieurs fichiers :
- `game.h`, `game_aux.h` et `game_ext.h` définissent l'interface du jeu, avec des fonctions pour manipuler les pièces, vérifier l'état de la grille, et contrôler les orientations.
- `game_struct.h` définit la structure du jeu.
- `queue.h` et `queue.c`permettent de gérer les piles undo et redo.
- `game.c`, `game_aux.c` et `game_ext.c` implémentent les fonctions déclarées dans les fichiers d'en-tête.
- `game_text.c` permet de jouer au jeu en mode texte dans le terminal.
- Plusieurs fichiers de test vérifient le bon fonctionnement de chaque partie du jeu.

Le projet utilise **CMake** pour la gestion de la compilation aisni que **Make** et **CTest** pour automatiser les tests et la construction.


## Auteurs

- Eliane CHAAL
- Tran Minh Chau DO 
- Léa DRION


## Commandes

Pour jouer il faut entrer une des commandes suivantes dans le terminal :

- **Tourner la pièce (i,j) dans le sens horaire** : `c i j`
- **Tourner la pièce (i,j) dans le sens antihoraire** : `a i j`
- **Mélanger le jeu** : `r`
- **Annuler le dernier mouvement** : `z`
- **Refaire le dernier mouvement** : `y`
- **Afficher l'aide** : `h`
- **Quitter le jeu** : `q`


## Paramètres personnalisables

Les paramètres suivants peuvent être personnalisés :

- **Largeur et Hauteur**  
  Taille de la grille en nombre de cases.

- **Bordures connectées (wrapping)**  
  Si activé, les connexions peuvent se faire entre les bords gauche/droit et haut/bas de la grille.