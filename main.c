#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/macro.h"
#include "usual.c"

// Structure pour une case du plateau de jeu
typedef struct {
    char symbol[10];
    int flipped; //variable booléenne qui vaut 1 si la case est retournée, 0 sinon
    int emptied; //variable booléenne qui vaut 1 si la case est vidée (monstre battu/objet pris), 0 sinon
} Square;

typedef struct {
    int number;
    char symbol[10];
    int weapon;

    int start_x; //case de départ attribuée abscisse
    int start_y; //case de départ attribuée ordonnée
    int position_x; //position durant le tour abscicsse
    int position_y; //position durant le tour abscicsse

    int ancientWeapon_found;
    int treasure_found;

    char name[];
    //color mais j'ai la flemme pour l'instant
} Player;

// Cherche l'indice du symbole de la case dans un tableau spécifié, renvoie l'indice de la première occurrence du symbole dans le tableau, sinon renvoie -1
int SymbolIdInArray(Square square, char array[][10], int size){
    for(int i = 0; i<size; i++){
        if(strcmp(square.symbol, array[i]) == 0)
            return i; // retourne le premier indice correspondant si trouvé dans le tableau
    }
    return -1; //si pas dans le tableau
}

//placement aléatoire de symboles
void random_placement(Square **board, int gridSize, char tab[][10], int tabSize, char monsters[4][10]){
    int randomRow, randomCol;
    for (int i = 0; i < tabSize; i++) {
        do{
            randomRow = rand() % gridSize+1;
            randomCol = rand() % gridSize+1;
        }while(SymbolIdInArray(board[randomRow][randomCol], monsters, 4) == -1); //vérification que la case choisie aléatoirement ne soit pas déjà une arme/trésor
        strcpy(board[randomRow][randomCol].symbol, tab[i]);
    }
}

//Initialisation de la map aléatoire de symboles
void initialize_map(Square **board, int boardSize, int gridSize, char monsters[4][10], char weapons[4][10], char treasures[5][10]) {

    //remplissage du labyrinthe par des monstres aléatoires et initialisation de l'état de base du plateau (cartes retournées et remplies)
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            board[i][j].emptied = 0;
            board[i][j].flipped = 1;
            if((i == 0 && j == boardSize-3) || (i == 2 && j == 0) || (i == boardSize-1 && j == 2) || (i == boardSize-3 && j == boardSize-1)){
                strcpy(board[i][j].symbol, START);
            }
            else if((i%(boardSize-1) == 0) || (j%(boardSize-1) == 0)){
                strcpy(board[i][j].symbol, " ");
            }
            else{
                strcpy(board[i][j].symbol, monsters[rand() % 4]);
                board[i][j].flipped = 0;
            }
        }
    }

    //placement aléatoire des trésors
    random_placement(board, gridSize, treasures, 5, monsters);
    //placement aléatoire des armes antiques
    random_placement(board, gridSize, weapons, 4, monsters);
}

//Création et initialisation d'un tableau 2D de structures Square <=> création d'un plateau de jeu
Square **create_board(int boardSize, int gridSize, char monsters[4][10], char weapons[4][10], char treasures[5][10]) {
    Square **board = malloc(boardSize * sizeof(Square *));
    if (board == NULL) { //gérer si l'allocation a échoué
        exit(1);
    }

    //pour chaque ligne du plateau
    for (int i = 0; i < boardSize; i++) {
        board[i] = malloc(boardSize * sizeof(Square)); //allouer de l'espace pour chacune des lignes du plateau (pour chacun des tableaux de char*)
        if (board[i] == NULL) { //gérer si l'allocation a échoué
            exit(1);
        }
    }

    initialize_map(board, boardSize, gridSize, monsters, weapons, treasures);

    return board;
}

//libération de la mémoire allouée pour le plateau
void free_board(Square **board, int size) {
    for (int i = 0; i < size; i++) {
        free(board[i]); //libération de la mémoire des lignes
    }
    free(board); //libération du plateau
}

//intialisation d'un joueur
void init_player(Player* player, int num, char* symbol, int start_x, int start_y) {
    player->number = num;
    strcpy(player->symbol, symbol);

    printf("Entrer un prénom pour le joueur %d : ", player->number);
    scanf("%s", player->name);
    flush_input_buffer(); //au cas où le joueur entre un nom avec des espaces

    player->start_x = start_x;
    player->start_y = start_y;
    player->position_x = player->start_x;
    player->position_y = player->start_y;

    player->ancientWeapon_found = 0;
    player->treasure_found = 0;
}

void print_board(Square **board, int boardSize, Player* activePlayer) { //version 2 avec position joueur actif ajouté
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if(activePlayer->position_x == i && activePlayer->position_y == j){
                printf("%s ", activePlayer->symbol);
            }
            else if (board[i][j].flipped == 1){
                if(board[i][j].emptied == 1){
                    printf("%s ", EMPTY);
                }
                else{
                    printf("%s ", board[i][j].symbol);
                }
            }

            else {
                printf("%s ", HIDDEN);
            }
        }
        printf("\n");
    }
    printf("\n");
}

////////////        A SUPPRIMER, UNIQUEMENT POUR TESTER        ////////////
void print_board_admin(Square **board, int boardSize, Player* activePlayer) { //print board mais on voit toutes les cases
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if(activePlayer->position_x == i && activePlayer->position_y == j){
                printf("%s ", activePlayer->symbol);
            }
            else if(board[i][j].emptied == 1){
                printf("%s ", EMPTY);
            }
            else {
                printf("%s ", board[i][j].symbol);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void weapon_choice(Player *player) {
    printf("Choisissez une arme pour vous préparer à un potentiel combat...\n");
    printf("1 : Torche\n2 : Bouclier réfléchissant\n3 : Hâche\n4 : Arc\n");
    int i = 0; // i est le nombre d'erreurs que le joueur commet. C'est-à-dire quand ils donnent autre chose que demandé
    player->weapon = getint()-1;
    flush_input_buffer();
    while (player->weapon < 0 || player->weapon > 3) {
        i++; // on incrémente le nombre d'erreurs
        if (i >= 3) { //trop d'erreurs alors le jeu prend des mesures
            printf("Ok si vous insistez...pas d'armes...\n");
            player->weapon = 0;
            waiting();
            break;
        }
        cursor_move('A', 1); //déplace le curseur d'une case vers le haut pour remplacer la dernière ligne
        printf("Bien essayé mais vous n'avez pas d'autres armes.\n");
        player->weapon = getint();
        flush_input_buffer();
    }
}

//Choisir des coordonnées x et y ne sortant pas du tableau (allant de 0 à 6). Sera utilisée pour la fonction de mouvement, pour la téléportation, pour le totem de transmutation.
void choose_coordinates(Square **board, int* x, int* y, int boardSize, int gridSize) {
    printf("\nChoisir une case en entrant les numéros de la ligne,colonne : ");
    scanf("%d,%d", x, y);
    flush_input_buffer();
    if (*x < boardSize - 1 - gridSize || *x > gridSize || *y < boardSize - 1 - gridSize || *y > gridSize || board[*x][*y].flipped == 1) {
        printf("Coordonnées invalides.");
        choose_coordinates(board, x, y, boardSize, gridSize);
    }
}

// Vérifie que l'arme choisie est correcte par rapport à l'ennemi affronté. Renvoie 1 si oui, 0 sinon
int fight(Player player, int monster) {
    if (player.weapon == monster) {
        return 1;
    }
    return 0;
}

//Sous fonction pour gérer les découvertes d'armes antiques. Renvoie 1 si l'arme antique correspond à celle du joueur, 0 sinon.
int event_ancient_weapon(Player* player, int ancientWeapon){
    printf("Vous avez découvert une arme antique !\n");
    if (player->number == ancientWeapon+1) {
        printf("C'est la vôtre !\n");
        player->ancientWeapon_found = 1;
        return 1;
    }
    else{
        printf("Il semblerait que ce ne soit pas la vôtre... Vous continuez votre chemin.\n");
        return 0;
    }
}

//Fonction qui gère les rencontres de monstres/d'objets lors du déplacement. Renvoie 1 si le joueur peut continuer à se déplacer librement, 0 si le tour doit se finir
int event_manager(int* x, int* y, Square **board, int boardSize, int gridSize, Player* activePlayer, char monsters[4][10], char weapons[4][10], char treasures[5][10]) {
    board[*x][*y].flipped = 1;
    print_board(board, boardSize, activePlayer);

    if(SymbolIdInArray(board[*x][*y], monsters, 4) != -1){
        if(fight(*activePlayer, SymbolIdInArray(board[*x][*y], monsters, 4))){
            printf("Combat gagné !\n");
            board[*x][*y].emptied = 1;
        }
        else{
            printf("Combat perdu !\n");
            return 0;
        }
    }

    else if(SymbolIdInArray(board[*x][*y], weapons, 4) != -1){
        event_ancient_weapon(activePlayer, SymbolIdInArray(board[*x][*y], weapons, 4));
    }
    else if(SymbolIdInArray(board[*x][*y], treasures, 5) != -1){
        switch(SymbolIdInArray(board[*x][*y], treasures, 5)){
            case 0:
                printf("Vous avez découvert un trésor !\n");
                activePlayer->treasure_found = 1;
                break;

            case 1:
                printf("Vous avez découvert un portail magique de téléportation. Vous pouvez choisir n'importe quelle case encore cachée où vous téléporter. Mais avant cela...\n");
                weapon_choice(activePlayer);
                choose_coordinates(board, x, y, boardSize, gridSize);
                return event_manager(x, y, board, boardSize, gridSize, activePlayer, monsters, weapons, treasures);

            case 2:
                printf("Vous avez découvert un totem de transmutation. Vous pouvez choisir n'importe quelle case encore cachée pour l'échanger avec le totem. Cela mettra fin à votre tour.\n");

                int l,c;
                choose_coordinates(board, &l, &c, boardSize, gridSize);
                while((l == 1 && c == 3) || (l == 2 && c == 1) || (l == 5 && c == 2) || (l == 4 && c == 5)){ //interdiction de prendre les cases devant celles de départ
                    printf("Coordonnées invalides.");
                    choose_coordinates(board, &l, &c, boardSize, gridSize);
                }

                Square temp = board[*x][*y];
                board[*x][*y] = board[l][c];
                board[l][c] = temp;

                printf("Poof!\n");
                print_board(board, boardSize, activePlayer);
                return 0;
        }

    }
    return 1;
}

//Déplacement d'un joueur et évènements
void move(Square **board, int boardSize, int gridSize, Player* activePlayer, char monsters[4][10], char weapons[4][10], char treasures[5][10]) {
    int x, y;
    choose_coordinates(board, &x, &y, boardSize, gridSize);
    while(x < activePlayer->position_x - 1 //conditions pour n'avancer que d'une case adjacente à la position actuelle
            || x > activePlayer->position_x + 1
            || y < activePlayer->position_y - 1
            || y > activePlayer->position_y + 1
            || (x != activePlayer->position_x && y != activePlayer->position_y) //ne pas aller en diagonale
            || (x == activePlayer->position_x && y == activePlayer->position_y))//ne pas faire du sur place
    {
        printf("Coordonnées invalides.");
        choose_coordinates(board, &x, &y, boardSize, gridSize);
    }

    if(event_manager(&x, &y, board, boardSize, gridSize, activePlayer, monsters, weapons, treasures)){
        activePlayer->position_x = x;
        activePlayer->position_y = y;
    }
    else{
        for(int i = 1; i <= gridSize; i++){ //reset du plateau à la mort du joueur
            for (int j = 1; j <= gridSize; j++){
                board[i][j].flipped = 0;
                board[i][j].emptied = 0;
            }
        }
        activePlayer->position_x = activePlayer->start_x;
        activePlayer->position_y = activePlayer->start_y;
        printf("Vous revenez à votre position de départ...\n");
    }
    print_board(board, boardSize, activePlayer);
}

int main() {
    srand(time(NULL));

    char weapons[4][10] = {STICK, SWORD, DAGGER, SPELLBOOK}; //Symboles des armes

    char treasures[5][10] = {CHEST, PORTAL, TOTEM, TOTEM, CHEST}; //Symboles des objets spéciaux (coffres, totems, portails)

    char monsters[4][10] = {ZOMBIE, BASILISK, TROLL, HARPY}; //Symboles des monstres

    char adventurers[4][10] = {RANGER, THIEF, MAGICIAN, WARRIOR}; //Symboles des monstres

    int start_x[4] = {0, 2, 4, 6};
    int start_y[4] = {4, 0, 6, 2};
    ////////////        CREATION ET INITIALISATION DU PLATEAU DE CASES ////////////
    Square **board = create_board(BOARD_SIZE, GRID_SIZE, monsters, weapons, treasures);

    ////////////        CREATION PERSONNAGE        ////////////

    Player players[4];
    for(int i=0; i<4; i++){
        init_player(&players[i], i+1, adventurers[i], start_x[i], start_y[i]);
        printf("Bienvenue %s !\n", players[i].name);
    }

    ////////////        A SUPPRIMER, UNIQUEMENT POUR TESTER        ////////////
    print_board_admin(board, BOARD_SIZE, &players[0]); //print board mais on voit toutes les cases
    printf("\n\n");

    print_board(board, BOARD_SIZE, &players[0]);

    ////////////        WIP TEST GAMEPLAY        ////////////
    for (int i = 0; i < 40; i++) {
        weapon_choice(&players[0]);
        move(board, BOARD_SIZE, GRID_SIZE, &players[0], monsters, weapons, treasures);
    }

    ////////////        LIBERATION DE LA MEMOIRE        ////////////
    free_board(board, BOARD_SIZE);

    return 0;
}
