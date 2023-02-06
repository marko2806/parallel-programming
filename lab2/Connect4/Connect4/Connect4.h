#pragma once
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


struct Potez {
	int state[6][7];
	int status;
	int made_by_player;
	int depth;
	double value;
	int index;
}typedef Potez;


struct PotezNode {
	int valid;
	struct Potez* potez;
	struct PotezNode* children[NUM_OF_COLS];
} typedef PotezNode;

struct PotezSubtree {
	int size;
	Potez potez[NUM_OF_COLS * NUM_OF_COLS];
}typedef PotezSubtree;

void ispisiStanje(Potez* potez);

void odigraj_potez(Potez* tretnutni_potez, Potez* novi_potez, int col);

int unosPoteza();

int provjeri_pobjedu(Potez* potez);

int moguc_potez(int* stanje, int potez);
//double evaluiraj_potez(Potez* potez);