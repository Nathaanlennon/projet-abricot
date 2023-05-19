#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "/include/header.h"
#include "/include/macro.h"


// efface tout le terminal visible
void clear_all(){
    printf("\033[0;0H\033[J");
}

//efface une partie du terminal
void clear_part(int line, int column){
    printf("\033[%d;%dH\033[J", line, column); // code ANSI pour déplacer le curseur puis efface
}

//déplace le curseur dans la direction et la valeur indiquée
// A: up, B: down, C: forward, D: backward
void cursor_move(char direction, int num){
    printf("\033[%d%c", num, direction);
}
// Va vider le "buffer" pour éviter les fuites de donnée quand on fait des getchar notamment
void flush_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// retourne un simple int d'un seul caractère, utile pour les cas de choix pour par exemple de 1 à 5, moins
// d'utilisations de ressources qu'un scanf
int getint(){
    return getchar() - '0'; // ascii du chiffre transformé en le chiffre lui meme en lui enlevant le code de 0
}

// permet d'écrire un "commentaire" d'une couleur differente et de remettre à la bonne couleur pour la suite
// exemple : appuyez sur entrée pour continuer
void commentary(char tab[]){ // tab est la chaine de caractère désiré en commentaire
    printf("%s%s%s\n", "\033[90m", tab, C_WHT); // le premier %s met la couleur en gris et le 3e la remet en blanc
}

// permet de faire un "entrer pour continuer", "presser une touche pour continuer"
void waiting(){
    commentary("(Press 'enter' to continue..)");
    getchar();
}

//pour créer un fichier de rapport de crash s'il y en a un
void write_crash_report(const char* error_message) {
    time_t current_time = time(NULL); //la fonction time renvoie nombre de secondes écoulées depuis le 01/01/1970 00:00:00
    struct tm* time = localtime(&current_time); //conversion du timestamp précédent en date lisible selon le fuseau horaire local
    char filename[50];
    strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S_crash_report.txt", time); //formatage du nom de fichier en année-mois-jour_heure-min-sec

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erreur lors de la création du rapport de crash.\n");
    }

    fprintf(file, "Date et heure du crash : %s", asctime(time));
    fprintf(file, "Message d'erreur : %s\n", error_message);

    fclose(file);
    printf("Le rapport de crash a été créé : %s\n", filename);
}

// Cherche l'indice du symbole de la case dans un tableau spécifié, renvoie l'indice de la première occurrence du symbole dans le tableau, sinon renvoie -1
int SymbolIdInArray(Square square, const char array[][10], int size){
    for(int i = 0; i<size; i++){
        if(strcmp(square.symbol, array[i]) == 0)
            return i; //retourne le premier indice correspondant si trouvé dans le tableau
    }
    return -1; //si pas dans le tableau
}

//afficher le plateau avec le joueur actif
void print_board(Square **board, int boardSize, Player* player) {

    if(board == NULL || player == NULL){
        write_crash_report("pointer in parameters is NULL");
        exit(1);
    }

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if(player->position_x == i && player->position_y == j){ //afichage du joueur selon sa position
                printf("%s ", player->symbol);
            }
            else if (board[i][j].flipped == 1){ //afichages des cartes retournées
                if(board[i][j].emptied == 1){ //monstres vaincus (cases vides)
                    printf("%s ", EMPTY);
                }
                else{
                    printf("%s ", board[i][j].symbol); //trésors et armes découverts
                }
            }

            else {
                printf("%s ", HIDDEN); //cartes retournées
            }
        }
        printf("\n");
    }
    printf("\n");
}

////////////        A SUPPRIMER, UNIQUEMENT POUR TESTER        ////////////
//print board mais on voit toutes les cases
void print_board_admin(Square **board, int boardSize, Player* player) {

    if(board == NULL || player == NULL){
        write_crash_report("pointer in parameters is NULL");
        exit(1);
    }

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if(player->position_x == i && player->position_y == j){
                printf("%s ", player->symbol);
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