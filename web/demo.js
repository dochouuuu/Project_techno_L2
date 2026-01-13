// demo.js

Module.onRuntimeInitialized = () => {
    g = Module._new_default();
    drawGame(g);
};

const EMPTY = 0;      // empty shape
const ENDPOINT = 1;   // endpoint shape
const SEGMENT = 2;    // segment shape
const CORNER = 3;     // corner shape
const TEE = 4;        // tee shape
const CROSS = 5;      // cross shape
const NB_SHAPES = 6;  // nb of shapes

const NORTH = 0;      // north
const EAST = 1;       // east
const SOUTH = 2;      // south
const WEST = 3;       // west 
const NB_DIRS = 4;    // nb of directions


let _listimg = [
    [new Image(), new Image(), new Image(), new Image()],  // empty
    [new Image(), new Image(), new Image(), new Image()],  // endpoint
    [new Image(), new Image(), new Image(), new Image()],  // segment
    [new Image(), new Image(), new Image(), new Image()],  // corner
    [new Image(), new Image(), new Image(), new Image()],  // tee
    [new Image(), new Image(), new Image(), new Image()]   // cross
];

// Liste des chemins d'images pour chaque forme et orientation
const imagePaths = [
    ["res/empty.png", "res/empty.png", "res/empty.png", "res/empty.png"],  // empty
    ["res/endpoint0.png", "res/endpoint1.png", "res/endpoint2.png", "res/endpoint3.png"],  // endpoint
    ["res/segment0.png", "res/segment1.png", "res/segment0.png", "res/segment1.png"],  // segment
    ["res/corner0.png", "res/corner1.png", "res/corner2.png", "res/corner3.png"],  // corner
    ["res/tee0.png", "res/tee1.png", "res/tee2.png", "res/tee3.png"],  // tee
    ["res/cross.png", "res/cross.png", "res/cross.png", "res/cross.png"]  // cross
];

for (let i = 0; i < _listimg.length; i++) {
    for (let j = 0; j < _listimg[i].length; j++) {
        _listimg[i][j].src = imagePaths[i][j];
    }
}

function square2img(s, o) {
    return _listimg[s][o];
}

// Récupère l'image correspondante à la forme et à l'orientation
const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");


function adjustCanvasSize(game) {
    const nb_rows = Module._nb_rows(game);
    const nb_cols = Module._nb_cols(game);

    // Définir la taille du canvas en fonction de la taille de la fenêtre
    const max_width = window.innerWidth;  // Largeur de la fenêtre
    const max_height = window.innerHeight;  // Hauteur de la fenêtre

    // Calculer la taille de chaque cellule pour qu'elle occupe bien l'espace
    const cell_width = Math.floor(max_width / nb_cols);
    const cell_height = Math.floor(max_height / nb_rows);

    // Utiliser la plus petite des tailles calculées pour que les cellules soient carrées
    const cell_size = Math.min(cell_width, cell_height);

    // Appliquer un facteur de réduction pour réduire la taille globale du canvas
    const scale_factor = 0.8;  // Réduire la taille du canvas à 80% de sa taille calculée

    // Calculer la nouvelle taille du canvas avec le facteur de réduction
    canvas.width = nb_cols * cell_size * scale_factor;
    canvas.height = nb_rows * cell_size * scale_factor;
}

function drawGame(game) {
    // Ajuster la taille du canvas avant de dessiner
    adjustCanvasSize(game);

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    const nb_rows = Module._nb_rows(game);
    const nb_cols = Module._nb_cols(game);

    const cell_width = canvas.width / nb_cols;
    const cell_height = canvas.height / nb_rows;

    for (let row = 0; row < nb_rows; row++) {
        for (let col = 0; col < nb_cols; col++) {
            let s = Module._get_piece_shape(game, row, col);
            let o = Module._get_piece_orientation(game, row, col);
            let img = square2img(s, o);

            // Calculer la taille du carré
            let square_size = Math.min(cell_width, cell_height);

            // Calcul des coordonnées pour positionner l'image au centre de la cellule
            let x = col * cell_width + cell_width / 2;
            let y = row * cell_height + cell_height / 2;

            // Dessiner l'image en la centrant et en redimensionnant pour qu'elle soit carrée
            ctx.drawImage(img, x - square_size * 0.5, y - square_size * 0.5, square_size, square_size);
        }
    }
}

canvas.addEventListener("click", function (e) {
    if (!g) return;

    const rect = canvas.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    const nb_rows = Module._nb_rows(g);
    const nb_cols = Module._nb_cols(g);
    const cell_width = canvas.width / nb_cols;
    const cell_height = canvas.height / nb_rows;

    const col = Math.floor(x / cell_width);
    const row = Math.floor(y / cell_height);

    Module._play_move(g, row, col, 1);
    drawGame(g);

    if (Module._won(g)) {
        setTimeout(() => {
            alert("Congratulation! You win!");
        }, 100);
    }
});

// Mélanger le jeu
function restartButton() {
    if (g) {
        Module._restart(g);
        drawGame(g);
    }
}

// Anuler le mouvement
function undoButton() {
    if (g) {
        Module._undo(g);
        drawGame(g);
    }
}

// Refaire le mouvement
function redoButton() {
    if (g) {
        Module._redo(g);
        drawGame(g);
    }
}

// Résoudre le jeu
function solveButton() {
    if (!g) return;

    const solved = Module._solve(g);
    drawGame(g);

    if (Module._won(g)) {
        setTimeout(() => {
            alert("Congratulation! You win!");
        }, 100);
    } else {
        alert("Could not solve the game.");
    }
}

// Créer un jeu random
function randomButton() {
    const width = Number(document.getElementById('canvasWidth').value);
    const height = Number(document.getElementById('canvasHeight').value);
    const wrapping = document.getElementById('canvasWrapping').checked;

    if (g) {
        Module._delete(g); // Suppression de l'ancien jeu
    }

    g = Module._new_random_game(height, width, wrapping ? 1 : 0);

    if (g) {
        console.log('Jeu créé avec succès !');
        drawGame(g); // Affiche le jeu
    } else {
        console.error('Impossible de créer le jeu. Vérifiez les paramètres.');
    }
}

// Popup pour faire un jeu random
function openPopup() {
    document.getElementById("popup").style.display = "block";
    document.getElementById("overlay").style.display = "block";
}

function closePopup() {
    document.getElementById("popup").style.display = "none";
    document.getElementById("overlay").style.display = "none";
}

function applyRandom() {
    randomButton();
    closePopup();
}

document.getElementById("closePopupBtn").addEventListener("click", closePopup);

document.addEventListener("keydown", function (e) {
    const popupVisible = document.getElementById("popup").style.display === "block";
    if (popupVisible && e.key === "Enter") {
        applyRandom();
    }
});