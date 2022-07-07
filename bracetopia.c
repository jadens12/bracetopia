#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

/*

    The Brace-topia simulation
    @author: Jaden Seaton

*/

int DIM = 15;
int ePercent = 60;
int vPercent = 20;
int cycles = -1;
int sleepDelay = 900000;
int thresholdPercent = 50;

int usleep(int sleepDelay);

// A struct to store vacant coordinates
struct vacantSpots{
    int row;
    int col;
};

// Displays the standard usage message
void displayUsage(){
    fprintf(stderr, "usage: \n");
    fprintf(stderr, "bracetopia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac    ] [-e %%end] \n");
}

// Displays the help messages
void displayHelp(){
    displayUsage();
    fprintf(stderr, "Option      Default   Example   Description\n");
    fprintf(stderr, "'-h'        NA        -h        print this usage message.\n"    );
    fprintf(stderr, "'-t N'      900000    -t 5000   microseconds cycle delay.\n"    );
    fprintf(stderr, "'-c N'      NA        -c4       count cycle maximum value.\n    ");
    fprintf(stderr, "'-d dim'    15        -d 7      width and height dimension.\    n");
    fprintf(stderr, "'-s %%str'   50        -s 30     strength of preference.\n")    ;
    fprintf(stderr, "'-v %%vac'   20        -v30      percent vacancies.\n");
    fprintf(stderr, "'-e %%endl'  60        -e75      percent Endline braces. Oth    ers want Newline.\n");
}

/*
    Swaps two points on the grid
    @param: char *a: point on the grid
            char *b: point on the grid
*/
void swap( char *a, char* b){

    char temp = *a;
    *a = *b;
    *b = temp;
}

/*
    Makes the grid of 'n', 'e', and '.' based on percentages
    @param: int row: dimension of the grid
            char grid[][DIM]: the grid
*/
void makeGrid(int row, char grid[][DIM]){

    int num = DIM * DIM;
    int numV = (int) num * (vPercent / 100.0);
    num -= numV;
    int numE = (int) num * (ePercent / 100.0);
    num -= numE;
    int numN = num;

    for (int i = 0; i < row; i++){
        for (int j = 0; j < row; j++){
            if (numE != 0){
                grid[i][j] = 'e';
                numE--;
            }
            else if (numN != 0){
                grid[i][j] = 'n';
                numN--;
            }
            else{
                grid[i][j] = '.';
                numV--;
            }
        }
    }

    for (int i = 0; i < row; i++){
        for (int j = 0; j < row; j++){
            long int k = rand() % DIM;
            long int l = rand() % DIM;
            swap(&grid[i][j], &grid[k][l]);
        }
    }
}

/*
    Calculates the Happiness of the current cell in the grid
    @param: int x: the row
            int y: the col
            char grid[][DIM]: the grid
    @return: the cell's happiness
*/
double getHappiness(int x, int y, char grid[][DIM]){
    bool toRight = false;
    bool toDown = false;
    bool toLeft = false;
    bool toUp = false;
    int countE = 0;
    int countN = 0;
    int total = 0;
    double happiness;

    if (x < DIM - 1){
        toRight = true;
        if (grid[x + 1][y] == 'e'){
            countE++;
        }
        else if (grid[x + 1][y] == 'n'){
            countN++;
        }
    }
    if (y < DIM - 1){
        toDown = true;
        if (grid[x][y+1] == 'e'){
            countE++;
        }
        else if (grid[x][y+1] == 'n'){
            countN++;
        }
    }
    if (x > 0){
        toLeft = true;
        if (grid[x-1][y] == 'e'){
            countE++;
        }
        else if (grid[x-1][y] == 'n'){
            countN++;
        }
    }
    if (y > 0){
        toUp = true;
        if (grid[x][y-1] == 'e'){
            countE++;
        }
        else if (grid[x][y-1] == 'n'){
            countN++;
        }
    }
    if (toLeft && toUp){
        if (grid[x-1][y-1] == 'e'){
            countE++;
        }
        else if (grid[x-1][y-1] == 'n'){
            countN++;
        }
    }
    if (toRight && toUp){
        if (grid[x+1][y-1] == 'e'){
            countE++;
        }
        else if (grid[x+1][y-1] == 'n'){
            countN++;
        }
    }
    if (toLeft && toDown){
        if (grid[x-1][y+1] == 'e'){
            countE++;
        }
        else if (grid[x-1][y+1] == 'n'){
            countN++;
        }
    }
    if (toRight && toDown){
        if (grid[x+1][y+1] == 'e'){
            countE++;
        }
        else if (grid[x+1][y+1] == 'n'){
            countN++;
        }
    }
    total = countE + countN;
    if (grid[x][y] == 'e'){
        happiness = countE / (double) total;
    }
    else if (grid[x][y] == 'n'){
        happiness = countN / (double) total;
    }
    return happiness;
}

/*
    Swaps the unhappy agents with the vacant spots on the grid
    @param: char grid[][DIM]: the grid of agents
            char Hgrid[][DIM]: the grid of happiness
            int numV: the number of vacant spots on the grid
    @return: the amount of moves made
*/
int makeMove(char grid [][DIM], char Hgrid[][DIM], int numV){
    int movesMade = 0;
    struct vacantSpots spots[numV];
    int num = 0;

    for (int i = 0; i < DIM; i++){
        for (int j = 0; j < DIM; j++){
            if (grid[i][j] == '.'){
                spots[num].row = i;
                spots[num].col = j;
                num++;
            }
        }
    }

    for (int i = 0; i < DIM; i++){
        for (int j = 0; j < DIM; j++){
            if (Hgrid[i][j] == 'n'){
                if(num <= 0){
                    break;
                }
                else{
                    swap(&grid[i][j], &grid[spots[num-1].row][spots[num-1].col]);
                    num--;
                    movesMade++;
                }
            }
        }
    }

    return movesMade;
}

/*
    The finite mode for the simulation
*/
void finiteMode(){

    char grid[DIM][DIM];
    char Hgrid[DIM][DIM];

    makeGrid(DIM, grid);
    double num1;
    double num2 = (double) thresholdPercent / 100.0;
    int currCycle = -1;
    int movesMade = 0;
    int numV;
    while (cycles >= 0){
        currCycle++;
        double happiness = 0;
        double avgHappiness = 0;
        numV = 0;
        for (int i = 0; i < DIM; i++){
            for (int j = 0; j < DIM; j++){
                printf("%c", grid[i][j]);
                if (grid[i][j] == 'e' || grid[i][j] == 'n'){
                    num1 = getHappiness(i, j, grid);
                    if (num1 < num2){
                        Hgrid[i][j] = 'n';
                    }
                    else if (num1  >= num2){
                        Hgrid[i][j] = 'y';
                    }
                    happiness += num1;
                }
                else{
                    Hgrid[i][j] = '.';
                    numV++;
                }
            }
            printf("\n");
        }
        printf("cycle: %d\n", currCycle);
        printf("moves this cycle: %d\n", movesMade);
        movesMade = 0;
        avgHappiness = happiness / (DIM * DIM - numV);
        printf("teams' \"happiness\": %.4f\n", avgHappiness);
        printf("dim: %d, %%strength of preference:  %d%%, %%vacancy:  %d%%, %%end    :  %d%%\n", DIM, thresholdPercent, vPercent, ePercent);
        cycles--;
        movesMade = makeMove(grid, Hgrid, numV);
    }
}

/*
    The infinite mode for the simulation
*/
void infiniteMode(){

    char grid[DIM][DIM];
    char Hgrid[DIM][DIM];
    makeGrid(DIM, grid);
    double num1;
    double num2 = (double) thresholdPercent / 100.0;
    int currCycle = -1;
    int movesMade = 0;
    int numV;
    initscr();

    while(1) {
        move(0, 0);
        currCycle++;
        double happiness = 0;
        double avgHappiness = 0;
        numV = 0;
        for (int i = 0; i < DIM; i++){
            for (int j = 0; j < DIM; j++){
                printw("%c", grid[i][j]);
                if (grid[i][j] == 'e' || grid[i][j] == 'n'){
                    num1 = getHappiness(i, j, grid);
                    if (num1 < num2){
                        Hgrid[i][j] = 'n';
                    }
                    else if (num1  >= num2){
                        Hgrid[i][j] = 'y';
                    }
                    happiness += num1;
                }
                else{
                    Hgrid[i][j] = '.';
                        numV++;
                }
            }
            printw("\n");
        }
        printw("cycle: %d\n", currCycle);
        printw("moves this cycle: %d\n", movesMade);
        movesMade = 0;
        avgHappiness = happiness / (DIM * DIM - numV);
        printw("teams' \"happiness\": %.4f\n", avgHappiness);
        printw("dim: %d, %%strength of preference:  %d%%, %%vacancy:  %d%%, %%end    :  %d%%\n", DIM, thresholdPercent, vPercent, ePercent);
        printw("Use Control-C to quit.\n");
        refresh();
        usleep(sleepDelay);
        movesMade = makeMove(grid, Hgrid, numV);
    }
    endwin();
}
 
/*
    Gets the command line arguments and calls the appropriate functions for simul    ation
*/
int main(int argc, char* argv[]){

    int opt;
    int num;
    while ((opt = getopt(argc, argv, "d:s:v:e:c:t:h::")) != -1){
        switch(opt){
            case 'h':
                displayHelp();
                return (EXIT_SUCCESS);
            case 'd':
                num = (int)strtol(optarg, NULL, 10);
                if (num <=0){
                    fprintf(stderr, "dimension (%d) must be a non-negative intege    r.\n)", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                else if (num <5 || num > 39){
                    fprintf(stderr, "dimension (%d) must be a value in [5...39]\n    ", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                DIM = num;
                break;
            case 's':
                num = (int)strtol(optarg, NULL, 10);
                if (num <0){
                    fprintf(stderr, "preference strength (%d) must be a non-negat    ive integer.\n", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                else if (num == 0|| num >= 100){
                    fprintf(stderr, "preference strength (%d) must be a value in     [1...99]\n", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                thresholdPercent = num;
                break;
            case 'v':
                num = (int)strtol(optarg,NULL,10);
                if (num <0){
                    fprintf(stderr, "Vacancy (%d) must be a non-negative integer.    \n", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                else if (num == 0 || num >=100){
                    fprintf(stderr, "vacancy (%d) must be a value in [1...99]\n",     num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                vPercent = num;
                break;
            case 'e':
                num = (int)strtol(optarg, NULL, 10);
                if (num <0){
                    fprintf(stderr, "Endline proportion (%d) must be a non-negati    ve integer.\n", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                else if (num == 0 || num >=100){
                    fprintf(stderr, "Endline proportion (%d) must be a value in [    1...99]\n", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                ePercent = num;
                break;
            case 'c':
                num = (int)strtol(optarg, NULL, 10);
                if (num < 0){
                    fprintf(stderr, "Cycles (%d) must be a non-negative integer.\    n", num);
                    displayUsage();
                    return (1 + EXIT_FAILURE);
                }
                cycles = num;
                break;
            case 't':
                num = (int)strtol(optarg, NULL, 10);
                if (num <= 0){
                    break;
                }
                sleepDelay = num;
                break;
            default:
                displayUsage();
                return ( EXIT_FAILURE );
        }
    }

    srand(time(NULL));
    if (cycles == -1){
        infiniteMode();
    }
    else{
        finiteMode();
    }
    return (EXIT_SUCCESS);
}