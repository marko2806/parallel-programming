#include <stdio.h>
#include "Connect4.h"
#include <stdlib.h>

void ispisiStanje(Potez *potez) {
    for (int i = 0; i < NUM_OF_ROWS; i++) {
        for (int j = 0; j < NUM_OF_COLS; j++) {
            printf("%3d ", potez->state[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void odigraj_potez(Potez* tretnutni_potez, Potez* novi_potez, int col) {
    
    novi_potez->made_by_player = tretnutni_potez->made_by_player ^ 1;
    //novi_potez->parent = tretnutni_potez;
    novi_potez->depth = (tretnutni_potez->depth) - 1;
    novi_potez->index = (tretnutni_potez->index) * NUM_OF_COLS + col;
    memcpy(novi_potez->state, tretnutni_potez->state, sizeof(int) * NUM_OF_ROWS * NUM_OF_COLS);
    int red = 0;
    int trenutno = novi_potez->state[red][col];
    while (trenutno == 0 && red < NUM_OF_ROWS) {
        red++;
        trenutno = novi_potez->state[red][col];
    }
    if (red > 0) {
        novi_potez->state[red - 1][col] = novi_potez->made_by_player ^ -1;
    }
}

int unosPoteza() {
    int a;
    scanf_s("%d", &a);
    return a;
}


int provjeri_pobjedu(Potez* potez) {
    for (int i = 0; i < NUM_OF_ROWS; i++) {
        for (int j = 0; j < NUM_OF_COLS; j++) {
            int seq_cpu = 0, seq_human = 0;
            // provjera vodoravno 
            for (int k = j; k < 4 && (k + j) < NUM_OF_COLS; k++) {
                if (potez->state[i][k + j] == CPU) {
                    seq_cpu++;
                }
                else if (potez->state[i][k + j] == HUMAN) {
                    seq_human++;
                }
            }
            if (seq_cpu == 4) {
                return 1;
            }
            if (seq_human == 4) {
                return 1;
            }
            // provjera okomito
            seq_cpu = 0;
            seq_human = 0;
            for (int k = i; k < 4 && (k + i) < NUM_OF_ROWS; k++) {
                if (potez->state[k + i][j] == CPU) {
                    seq_cpu++;
                }
                else if (potez->state[k + i][j] == HUMAN) {
                    seq_human++;
                }
            }
            if (seq_cpu == 4) {
                return 1;
            }
            if (seq_human == 4) {
                return 1;
            }
            // provjera dijagonale prema dolje
            seq_cpu = 0;
            seq_human = 0;
            for (int k = 0; k < 4 && (k + i) < NUM_OF_ROWS && (k + j) < NUM_OF_COLS; k++) {
                if (potez->state[k + i][k + j] == CPU) {
                    seq_cpu++;
                }
                else if (potez->state[k + i][k + j] == HUMAN) {
                    seq_human++;
                }
            }
            if (seq_cpu == 4) {
                return 1;
            }
            if (seq_human == 4) {
                return 1;
            }
            // provjera dijagonale prema gore
            seq_cpu = 0;
            seq_human = 0;
            for (int k = 0; k < 4 && (i - k) >= 0 && (k + j) < NUM_OF_COLS; k++) {
                if (potez->state[i-k][k+j] == CPU) {
                    seq_cpu++;
                }
                else if (potez->state[i-k][k+j] == HUMAN) {
                    seq_human++;
                }
            }
            if (seq_cpu == 4) {
                return 1;
            }
            if (seq_human == 4) {
                return 1;
            }

        }
    }
    return 0;
}


int moguc_potez(int* stanje, int potez) {
    if (*(stanje + potez) == 0) {
        return 1;
    }
    return 0;
}
/*
double evaluiraj_potez(Potez* potez) {
    int status_stanja = provjeri_pobjedu(potez->state, potez->made_by_player);
    if (status_stanja) {
        return 1.0;
    }
    else if (status_stanja == -1) {
        return -1.0;
    }

    if (potez->depth == 0) {
        return 0.0;
    }
    
    
    int c_all_win = 1, c_all_lose = 1;
    int moves_count = 0;
    double total = 0.0;
    for (int i = 0; i < NUM_OF_COLS; i++) {
        if (moguc_potez(potez->state, i) && potez->depth > 0) {
            moves_count++;
            Potez novi_potez;
            odigraj_potez(potez->state, &novi_potez, i);
            double res = evaluiraj_potez(&novi_potez);
            
            if (res > -1) {
                c_all_lose = 0;
            }
            if (res != 1) {
                c_all_win = 0;
            }
            if (res == 1 && novi_potez.made_by_player == CPU) {
                return 1;
            }
            if (res == -1 && novi_potez.made_by_player == HUMAN) {
                return -1;
            }
            total += res;
        }
    }
    if (c_all_win) {
        return 1;
    }
    if (c_all_lose) {
        return -1;
    }
    total = total / moves_count;
    
    return total;
}*/