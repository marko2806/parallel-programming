#include <stdio.h>
#include "mpi.h"
#include "Connect4.h"
#include "task.h"
#include <stdlib.h>
#include "linked_list.h"
#include <math.h>
#include <Windows.h>

#define KRAJ_ALGORITMA 1
#define PODSTABLO 2
#define ZAHTJEV 3
#define ZADATAK 5

#define MAX_DEPTH 8
#define SUBTREE_DEPTH 2

#define POBJEDA 1
#define PORAZ -1
#define NEUTRALNO 0
#define NEPOZNATO -2

#define print(format, ...) do{	    \
	printf(format, __VA_ARGS__);	\
	fflush(stdout);					\
}while(0)

PotezSubtree build_subtree(PotezNode *root) {
	PotezSubtree s;
	s.potez[0] = *root->potez;
	s.size = 1;
	for (int i = 1; i < SUBTREE_DEPTH; i++) {
		for (int j = 0; j < pow(NUM_OF_COLS, i); j++) {
			s.potez[(pow(NUM_OF_COLS, i) - 1) + j] = 
		}
	}
}

PotezNode* get_tree_node(PotezNode * root, int depth, int index) {
	PotezNode* node = root;
	for (int i = 0; i < depth - 1; i++) {
		int idx = index / pow(NUM_OF_COLS, i);
		node = node->children[idx];
	}
	node = node->children[index % NUM_OF_COLS];
	return node;
}

void evaluiraj_potez(Task * zad_potez);

PotezNode* update_tree(PotezNode * root, PotezSubtree subtree) {
	PotezNode* node = root;
	PotezNode* subtreeRoot;
	for (int i = 0; i < subtree.size; i++) {
		int depth = subtree.potez[i].depth;
		for (int i = 0; i < depth - 1; i++) {
			int idx = subtree.potez[i].index / (NUM_OF_COLS * depth);
			node = node->children[idx];
		}
		node = node->children[subtree.potez[i].index % 7];
		if (node == NULL) {
			node = malloc(sizeof(PotezNode));
		}
		*node->potez = subtree.potez[i];
		if (i == 0) {
			subtreeRoot = node;
		}
	}
	return subtreeRoot;
}

int main(const int* argc, char ***argv ) {
	MPI_Init(argc, argv);

	int rank, processes_count;
	
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	

	if (rank == 0) {
		//cekaj potez
		Task_list* zadaci = malloc(sizeof(Task_list));
		zadaci->first = NULL;
		zadaci->last = NULL;

		int col = unosPoteza();
		
		Potez* pocetni_potez = malloc(sizeof(Potez));
		pocetni_potez->made_by_player = HUMAN;
		pocetni_potez->status = NIJE_ZAPOCETO_RACUNANJE;
		pocetni_potez->depth = MAX_DEPTH;
		pocetni_potez->index = 0;



		for (int i = 0; i < NUM_OF_ROWS; i++) {
			for (int j = 0; j < NUM_OF_COLS; j++) {
				pocetni_potez->state[i][j] = 0;
			}
		}

		PotezNode pocetni_potez_node;
		pocetni_potez_node.potez = &pocetni_potez;
		pocetni_potez_node.valid = 1;

		for (int i = 0; i < NUM_OF_COLS; i++) {
			pocetni_potez_node.children[i] = malloc(sizeof(PotezNode));
			odigraj_potez(&pocetni_potez, pocetni_potez_node.children[i], i);
			pocetni_potez_node.children[i]->potez->status = NIJE_ZAPOCETO_RACUNANJE;
			Task *task = malloc(sizeof(Task));
			task->potez = *pocetni_potez_node.children[i];
			task->id = i;
			append(zadaci, task);
			
		}

		for (int i = 0; i < NUM_OF_COLS; i++) {
			print("%d\n", pocetni_potez_node.children[i]->potez->depth);
		}
		
		int kraj = 0;
		int ended_processes = 0;

		while (!kraj) {
			MPI_Status stat;
			int flag;
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stat);
			
			if (flag != 0) {
				if (stat.MPI_TAG == PODSTABLO) {
					PotezSubtree subtree;
					MPI_Status stat1;
					MPI_Recv(&subtree, 1, MPI_INT, stat.MPI_SOURCE, PODSTABLO, MPI_COMM_WORLD, &stat1);
					
					// azuriraj stablo u masteru

					PotezNode* newNode = update_tree(&pocetni_potez_node, subtree);

					//stvori zadatke iz roota i listova
					if (subtree.potez[0].status != ZAVRSENO_RACUNANJE) {
						Task* t = malloc(sizeof(Task));
						t->potez = *newNode;
						append(zadaci, t);
					}
					for (int i = 0; i < pow(NUM_OF_COLS, SUBTREE_DEPTH); i++ + ) {
						if (subtree.potez[subtree.size - i].status != ZAVRSENO_RACUNANJE) {
							Task* t = malloc(sizeof(Task));
							//dodaj listove podstabla u zadatke
							PotezNode* node = get_tree_node(&pocetni_potez_node, subtree.potez[subtree.size - i].depth, subtree.potez[subtree.size - i].index);
							t->potez = *node;
							append(zadaci, t);
						}
					}

				}
				else if(stat.MPI_TAG == ZADATAK) {
					
					Potez potez;
					MPI_Status stat1;
					MPI_Recv(&potez, sizeof(Potez), MPI_BYTE, stat.MPI_SOURCE, ZADATAK, MPI_COMM_WORLD, &stat1);
					// TODO dodaj potez u stablo

					//pronadi potez u podstablu
					Task* task = malloc(sizeof(Task));
					PotezNode *node = get_tree_node(&pocetni_potez_node, potez.depth, potez.index);
					task->potez = *node;
					if (potez.status == NIJE_ZAPOCETO_RACUNANJE) {
						insert(zadaci, task);
					}
					else {
						append(zadaci, task);
					}
					
				}
				else if (stat.MPI_TAG == ZAHTJEV) {
					int r;
					MPI_Status s;
					MPI_Recv(&r, 1, MPI_INT, stat.MPI_SOURCE, ZADATAK, MPI_COMM_WORLD, &s);
					if (has_elements(zadaci)) {
						Task* t = remove_first(zadaci);
						while (t != NULL && t->potez.potez->status == ZAVRSENO_RACUNANJE) {
							t = remove_first(zadaci);
						}
						
						PotezSubtree s;
						
						// TODO izgradi podstablo
						build_subtree(s.potez);
						
						MPI_Send(&t->potez, sizeof(PotezSubtree), MPI_CHAR, rank, ZAHTJEV, MPI_COMM_WORLD);
					}
					else {
						print("Javljam procesu %d da zavrsi s izvodenjem\n", rank);
						int kraj = 1;
						MPI_Send(&kraj, 1, MPI_INT, r, KRAJ_ALGORITMA, MPI_COMM_WORLD);
						ended_processes++;
						if (ended_processes == processes_count - 1) {
							break;
						}
					}
				}

			}
		}
			
		print("Master izasao iz petlje");
	}
	
	else if(rank != 0 || processes_count == 1) {

		// zatrazi jedno stanje
		// provjeri je li pobjednicko, gubitnicko ili neutralno
			// ako je neutralno stvori 7 novih stanja
				//	svih 7 stanja provjeri je li pobjednicko, gubitnicko ili neutralno
					// ako je stanje poraz i dolazi se potezom igraca -> parent je poraz
					// ako je stanje pobjeda i dolazi se potezom racunala -> parent je pobjeda
				// sva podstanja pobjeda ili poraz -> nadredeno je pobjeda ili poraz
				//  ako je neutralno stvori zadatak i poslji masteru

		int kraj = 0;
		int vec_poslao = 0;
		while (!kraj) {
			MPI_Status kraj_status;
			int flag;
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &kraj_status);
			if (flag != 0) {
				Task *task = malloc(sizeof(Task));
				MPI_Status status;
				MPI_Recv(task, sizeof(Task), MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (status.MPI_TAG == KRAJ_ALGORITMA) {
					print("Izasao iz petlje %d \n", rank);
					break;
				}
				else if(status.MPI_TAG == STANJE_ODGOVOR) {
					print("Krecem s evaluacijom poteza %d\n", task->id);
					print("Adresa poteza %d\n", task->potez->depth);
					evaluiraj_potez(task);
					//print("Potez: %f, process: %d\n",task.potez->value, rank);
					vec_poslao = 0;
				}
			}
			if (!vec_poslao) {
				print("Proces %d: Saljem zahtjev za potezom\n", rank);
				MPI_Send(&rank, 1, MPI_INT, 0, STANJE_ZAHTJEV, MPI_COMM_WORLD);
				vec_poslao = 1;
			}

		}
		print("Izasao iz petlje %d\n", rank);
	*/
	}

	MPI_Finalize();
}
/*

void evaluiraj_potez(Task* zad_potez) {
	// ne treba se evaluirati potez
	print("Krenula evaluacija\n");
	print("Evaluiram potez dubine %d\n", zad_potez->potez->depth);
	if (zad_potez->potez->status == ZAPOCETNO_RACUNANJE) {
		print("Zapoceto racunanje\n");
	}
	else if (zad_potez->potez->status == NIJE_ZAPOCETO_RACUNANJE) {
		print("Nije zapoceto racunanje\n");
	}
	else if (zad_potez->potez->status == ZAVRSENO_RACUNANJE){
		print("Zavrseno racunanje\n");
	}
	else {
		print("Nepoznato stanje %d\n", zad_potez->potez->status);
	}
	if (zad_potez->potez->status == ZAVRSENO_RACUNANJE) {
		return;
	}

	
	int status_stanja = provjeri_pobjedu(zad_potez->potez);
	// ako je pobjednicko stanje, zavrsavamo racunanje
	if (status_stanja) {
		zad_potez->potez->status = ZAVRSENO_RACUNANJE;
		zad_potez->potez->value = 1.0;
		print("Vracam 1\n");
		return;
	}
	// ako je gubitnicko stanje, zavrsavamo racunanje
	else if (status_stanja == -1) {
		zad_potez->potez->status = ZAVRSENO_RACUNANJE;
		zad_potez->potez->value = -1.0;
		print("Vracam -1\n");
		return;
	}
	// ako je potez na maksimalnoj dubini, zavrsavamo racunanje
	if (zad_potez->potez->depth == 0) {
		zad_potez->potez->status = ZAVRSENO_RACUNANJE;
		zad_potez->potez->value = 0.0;
		print("Vracam 0\n");
		return;
	}

	if (zad_potez->potez->status == ZAPOCETNO_RACUNANJE) {
		int moves_count = 0;
		int all_done = 1;
		double total = 0.0;
		int c_all_win = 1, c_all_lose = 1;
		// provjera jesu li sva djeca zavrsila s racunanjem
		for (int i = 0; i < NUM_OF_COLS; i++) {
			print("Provjerio %d\n", i);
			Potez *p = zad_potez->potez->children[i];
			// ako je djete zavrsilo s racunanjem
			if (p != NULL && p->status==ZAVRSENO_RACUNANJE) {
				print("Usao u if\n");
				moves_count++;
				if (p->value > -1) {
					c_all_lose = 0;
				}
				if (p->value != 1) {
					c_all_win = 0;
				}
				if (p->value == 1 && p->made_by_player == CPU) {
					zad_potez->potez->status = ZAVRSENO_RACUNANJE;
					zad_potez->potez->value = 1.0;
					for (int i = 0; i < NUM_OF_COLS; i++) {
						zad_potez->potez->children[i]->status = ZAVRSENO_RACUNANJE;
						zad_potez->potez->children[i]->value = 1.0;
					}
					return;
				}
				if (p->value == -1 && p->made_by_player == HUMAN) {
					zad_potez->potez->status = ZAVRSENO_RACUNANJE;
					zad_potez->potez->value = -1.0;
					for (int j = 0; j < NUM_OF_COLS; j++) {
						zad_potez->potez->children[j]->status = ZAVRSENO_RACUNANJE;
						zad_potez->potez->children[j]->value = -1.0;
					}
					print("Gotov s petljama\n");
					return;
				}
				total += p->value;
			}
			else if(p != NULL && p->status != NEMOGUCE_STANJE) {
				all_done = 0;
				print("Nije gotov\n");
				break;
			}
			else {
				print("Nešto je gadno krivo\n");
			}
		}
		if (all_done) {
			print("Svi su gotovi\n");
			if (c_all_win) {
				zad_potez->potez->status = ZAVRSENO_RACUNANJE;
				zad_potez->potez->value = 1.0;
				return;
			}
			if (c_all_lose) {
				zad_potez->potez->status = ZAVRSENO_RACUNANJE;
				zad_potez->potez->value = -1.0;
				return;
			}
			total = total / moves_count;
			zad_potez->potez->status = ZAVRSENO_RACUNANJE;
			zad_potez->potez->value = total;
			return;
		}
	}

	// nije zapoceto racunanje na potezu
	zad_potez->potez->status = ZAPOCETNO_RACUNANJE;
	print("Krecem s racunanjem\n");
	for (int i = 0; i < NUM_OF_COLS; i++) {
		if (moguc_potez(zad_potez->potez->state, i)) {
			
			Potez *novi_potez = malloc(sizeof(Potez));

			odigraj_potez(zad_potez->potez, novi_potez, i);

			novi_potez->status = NIJE_ZAPOCETO_RACUNANJE;
			zad_potez->potez->children[i] = novi_potez;
			Task task;
			//stvori zadatak
			task.potez = novi_potez;

			if (novi_potez->depth % 2 == 0) {

				// posalji novi zadatak
				MPI_Send(&task, sizeof(Task), MPI_BYTE, 0, ZADATAK, MPI_COMM_WORLD);
				
			}
			else {
				// rekurzivno evaluiraj
				evaluiraj_potez(&task);
				
			}
		}
		else {
			zad_potez->potez->children[i] = malloc(sizeof(Potez));
			zad_potez->potez->children[i]->status = NEMOGUCE_STANJE;
		}
	}
	// vrati trenutni zadatak
	MPI_Send(zad_potez, sizeof(Task), MPI_BYTE, 0, ZADATAK, MPI_COMM_WORLD);
	return;
}*/