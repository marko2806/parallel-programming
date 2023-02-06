#pragma once

#include <math.h>
#include <array>

#define CPU 1
#define HUMAN -1


#define NUM_OF_COLS 7
#define NUM_OF_ROWS 6

#define print(format, ...) do{	    \
	printf(format, __VA_ARGS__);	\
	fflush(stdout);					\
}while(0)

#define NIJE_ZAPOCETO_RACUNANJE 0
#define ZAPOCETNO_RACUNANJE 1
#define ZAVRSENO_RACUNANJE 2
#define NEMOGUCE_STANJE 3
#define POTREBAN_NOVI_ZADATAK 4

#define SUBTREE_DEPTH 3
#define SUBTREE_SIZE (49 + 7 + 1)
struct Potez {
	int state[6][7];
	int status;
	int made_by_player;
	int depth;
	int subtree_depth;
	double value;
	int index;
}typedef Potez;


struct PotezNode {
	int valid;
	struct Potez potez;
	struct PotezNode* children[NUM_OF_COLS];
} typedef PotezNode;

struct PotezSubtree {
	int size;
	struct Potez potez[SUBTREE_SIZE];
}typedef PotezSubtree;

void ispisiStanje(Potez* potez);

void odigraj_potez(Potez* tretnutni_potez, Potez* novi_potez, int col);

int unosPoteza();

int provjeri_pobjedu(Potez* potez);

int moguc_potez(int* stanje, int potez);
//double evaluiraj_potez(Potez* potez);