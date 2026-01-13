⁸# net-a46

## Description

Ce projet est un jeu de puzzle en solo, inspiré du jeu "Net" de la collection Simon Tatham's Portable Puzzle Collection. Le but du jeu est de connecter les pièces d'une grille pour former un réseau cohérent, où chaque pièce doit être bien orientée pour établir une connexion avec les pièces adjacentes. Le joueur peut tourner les pièces pour résoudre le puzzle.

Ce projet est divisé en plusieurs parties :
- `game.h` et `game_aux.h` définissent l'interface du jeu, avec des fonctions pour manipuler les pièces, vérifier l'état de la grille, et contrôler les orientations.
- `game.c` et `game_aux.c` implémentent les fonctions déclarées dans les fichiers d'en-tête.
- Plusieurs fichiers de test vérifient le bon fonctionnement de chaque partie du jeu.

Le projet utilise **CMake** pour la gestion de la compilation et **Make** pour automatiser les tests et la construction.

## Auteurs
- Eliane CHAAL
- Tran Minh Chau DO 
- Léa DRION