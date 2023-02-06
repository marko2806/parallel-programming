#include <stdio.h>
#include "mpi.h"
#include "Connect4.h"
#include "task.h"
#include <stdlib.h>
#include <list>
#include <queue>
#include <math.h>
#include <Windows.h>
#include <stack>

#define KRAJ_ALGORITMA 1
#define PODSTABLO 2
#define ZAHTJEV 3
#define ZADATAK 5

#define MAX_DEPTH 5
#define SUBTREE_DEPTH 2


#define print(format, ...) do{	    \
	printf(format, __VA_ARGS__);	\
	fflush(stdout);					\
}while(0)

void evaluiraj_podstablo(PotezSubtree * subtree);

PotezSubtree build_subtree(PotezNode* root) {
	//print("Pozvao queue\n");
	PotezSubtree s;
	s.size = 1;
	std::queue<PotezNode>* q = new std::queue<PotezNode>();
	root->potez.subtree_depth = SUBTREE_DEPTH;
	q->push(*root);
	
	int ctr = 0;
	while (!q->empty()) {
		PotezNode node = q->front();
		
		
		q->pop();
		s.potez[ctr] = node.potez;

		if (node.potez.subtree_depth > 1) {
			for (int i = 0; i < NUM_OF_COLS; i++) {
				ctr++;
				if (node.children[i] != NULL && (MAX_DEPTH - node.potez.depth) <= SUBTREE_DEPTH) {
					q->push(*node.children[i]);
					s.size++;
				}
				else {
					//print("Value je null\n");
					break;
				}
			}
		}
	}
	delete q;
	return s;
}

PotezNode* get_tree_node(PotezNode* root, int depth, int index) {
	//print("Root je na %d\n", MAX_DEPTH);
	//print("Trazim cvor na dubini %d, index %d\n", depth, index);
	//print("Trebam %d razina preci\n", MAX_DEPTH - depth);
	PotezNode* node = root;
	int height = MAX_DEPTH - depth;
	for (int i = height; i >= 1; i--) {
		int idx = index / (int)pow(NUM_OF_COLS, i - 1);
		//print("Dijelim sa %d\n", (int)pow(NUM_OF_COLS, i - 1));
		idx = idx % NUM_OF_COLS;
		//print("Dosao do dubine %d, idem na %d\n", i, idx);
		node = node->children[idx];
	}
	//print("Izasao iz nodea\n");
	return node;
}

void create_tree_node(PotezNode* root, int depth, int index, Potez potez) {
	//print("Stvaram node depth %d, index %d\n", depth, index);
	PotezNode* parent = NULL;
	PotezNode* node = root;
	int child_idx;
	int height = MAX_DEPTH - depth;
	for (int i = height; i >= 1; i--) {
		int idx = index / (int)pow(NUM_OF_COLS, i - 1);
		idx = idx % NUM_OF_COLS;
		parent = node;
		node = node->children[idx];
		child_idx = idx;
	}
	node = new PotezNode();
	node->potez = potez;
	if (parent != NULL) {
		parent->children[child_idx] = node;
	}
	//print("Stvorio sam node\n");
}

void evaluiraj_potez(Potez *potez);

PotezNode* update_tree(PotezNode* root, PotezSubtree subtree) {
	//print("Gradim tree %d\n", subtree.size);
	PotezNode* node = root;
	PotezNode* subtreeRoot = new PotezNode();
	for (int i = 0; i < subtree.size; i++) {
		print("Dohvacam node %d, index %d, %d\n", subtree.potez[i].depth, subtree.potez[i].index, subtree.potez[i].status);
		node = get_tree_node(root, subtree.potez[i].depth, subtree.potez[i].index);
		if (node == NULL) {
			//print("Node je null, stvaram NODE\n");
			create_tree_node(root, subtree.potez[i].depth, subtree.potez[i].index, subtree.potez[i]);
			node = get_tree_node(root, subtree.potez[i].depth, subtree.potez[i].index);
		}
		else {
			node->potez = subtree.potez[i];
			print("Dohvatio node %d, index %d, %d\n", node->potez.depth, node->potez.index, node->potez.status);
		}
		
		
		
		if (i == 0) {
			subtreeRoot = node;
		}
	}
	//print("Updateao sam subtree\n");
	return subtreeRoot;
}

int main(const int* argc, char*** argv) {

	MPI_Init(argc, argv);

	int rank, processes_count;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);


	if (rank == 0) {
		// lista zadataka
		std::list<Task>* zadaci = new std::list<Task>();
		
		
		// stvaranje inicijalnog stabla
		Potez* pocetni_potez = (Potez*) malloc(sizeof(Potez));
		pocetni_potez->made_by_player = HUMAN;
		pocetni_potez->status = NIJE_ZAPOCETO_RACUNANJE;
		pocetni_potez->depth = MAX_DEPTH;
		pocetni_potez->index = 0;
		for (int i = 0; i < NUM_OF_ROWS; i++) {
			for (int j = 0; j < NUM_OF_COLS; j++) {
				pocetni_potez->state[i][j] = 0;
			}
		}

		pocetni_potez->state[5][0] = 1;
		pocetni_potez->state[5][1] = 1;
		pocetni_potez->state[5][2] = 1;

		pocetni_potez->state[4][0] = -1;
		pocetni_potez->state[4][1] = -1;
		pocetni_potez->state[4][2] = -1;
	

		ispisiStanje(pocetni_potez);

		// unos vrijednosti
		int col = unosPoteza();

		
		
		PotezNode pocetni_potez_node;
		pocetni_potez_node.potez = *pocetni_potez;
		pocetni_potez_node.valid = 1;

		// stvaranje djece stabla
		for (int i = 0; i < NUM_OF_COLS; i++) {

			pocetni_potez_node.children[i] = new PotezNode();
			odigraj_potez(pocetni_potez, &pocetni_potez_node.children[i]->potez, i);
			

			// stvaranje zadatka za svako dijete i dodavanje u listu
			Task* task = new Task();
			task->potez = *pocetni_potez_node.children[i];
			task->id = i;
			zadaci->push_back(*task);

		}
		
		for (int i = 0; i < NUM_OF_COLS; i++) {
			print("%f\n", pocetni_potez_node.children[i]->potez.value);
			print("%d\n", pocetni_potez_node.children[i]->potez.status);
		}

		int kraj = 0;
		int ended_processes = 0;
		//print("Dosao do while\n"); 
		while (!kraj) {
			MPI_Status stat;
			int flag;
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stat);

			if (flag != 0) {
				if (stat.MPI_TAG == PODSTABLO) {
					PotezSubtree subtree;
					MPI_Status stat1;
					//print("Primam podstablo\n");
					MPI_Recv(&subtree, sizeof(PotezSubtree), MPI_BYTE, stat.MPI_SOURCE, PODSTABLO, MPI_COMM_WORLD, &stat1);
					print("Potez %d, %d, %d, %d ima vrijednost %f\n", subtree.potez[0].depth, 
						subtree.potez[0].index, subtree.potez[0].status, subtree.size, subtree.potez[0].value);
					PotezNode* newNode = update_tree(&pocetni_potez_node, subtree);

					//stvori zadatke iz roota i listova
					for (int i = 0; i < (int)pow(NUM_OF_COLS, SUBTREE_DEPTH - 1); i++) {
						if (subtree.potez[subtree.size - i - 1].status == POTREBAN_NOVI_ZADATAK && 
								subtree.potez[subtree.size - i - 1].depth >= SUBTREE_DEPTH - 1) {
							Task* t = new Task();
							//dodaj listove podstabla u zadatke
							subtree.potez[subtree.size - i - 1].status = ZAPOCETNO_RACUNANJE;
							PotezNode* node = get_tree_node(&pocetni_potez_node, subtree.potez[subtree.size - i - 1].depth, subtree.potez[subtree.size - i - 1].index);
							t->potez = *node;
							zadaci->push_front(*t);
						}
					}

					if (subtree.potez[0].status != ZAVRSENO_RACUNANJE &&
						subtree.potez[0].depth >= SUBTREE_DEPTH - 1) {
						Task* t = new Task();
						t->potez = *newNode;
						zadaci->push_back(*t);
					}
				}
				else if (stat.MPI_TAG == ZADATAK) {
					//print("Dobio sam zadatak");
					Potez potez;
					MPI_Status stat1;
					MPI_Recv(&potez, sizeof(Potez), MPI_BYTE, stat.MPI_SOURCE, ZADATAK, MPI_COMM_WORLD, &stat1);
					

					//pronadi potez u podstablu
					Task* task = new Task();
					PotezNode* node = get_tree_node(&pocetni_potez_node, potez.depth, potez.index);
					task->potez = *node;
					if (potez.status == NIJE_ZAPOCETO_RACUNANJE) {
						zadaci->push_front(*task);
					}
					else {
						zadaci->push_back(*task);
					}

				}
				else if (stat.MPI_TAG == ZAHTJEV) {
					//print("Dobio sam zahtvjev \n");
					int r;
					MPI_Status s;
					MPI_Recv(&r, 1, MPI_INT, stat.MPI_SOURCE, ZAHTJEV, MPI_COMM_WORLD, &s);
					if (zadaci->size() != 0) {
						Task t = zadaci->front();
						zadaci->pop_front();
						while (t.potez.potez.status == ZAVRSENO_RACUNANJE && !zadaci->empty()) {
							t = zadaci->front();
							zadaci->pop_front();
						}
						print("Zadaci size: %d\n", t.potez.potez.status);
						//print("Building subtree\n");
						PotezSubtree s = build_subtree(&t.potez);
						print("Saljem %d, %d root\n", s.potez[0].depth, s.potez[0].index);
						MPI_Send(&s, sizeof(PotezSubtree), MPI_CHAR, r, ZAHTJEV, MPI_COMM_WORLD);
					}
					else {
						//print("Javljam procesu %d da zavrsi s izvodenjem\n", rank);
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

		print("Master izasao iz petlje\n");
		for (int i = 0; i < 7; i++) {
			print("%f\n", pocetni_potez_node.children[i]->potez.value);
			print("%d\n", pocetni_potez_node.children[i]->potez.status);
		}
	}

	else if (rank != 0 || processes_count == 1) {
		int kraj = 0;
		int vec_poslao = 0;
		while (!kraj) {
			MPI_Status status;
			int flag;
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
			if (flag != 0) {
				Task* task = new Task();
				
				if (status.MPI_TAG == KRAJ_ALGORITMA) {
					break;
				}
				else if (status.MPI_TAG == ZAHTJEV) {
					PotezSubtree s;
					MPI_Recv(&s, sizeof(PotezSubtree), MPI_BYTE, 0, ZAHTJEV, MPI_COMM_WORLD, &status);
					//print("Dobio sam potez\n");
					//ispisiStanje(&s.potez[0]);
					evaluiraj_podstablo(&s);
					
					MPI_Send(&s, sizeof(PotezSubtree), MPI_BYTE, 0, PODSTABLO, MPI_COMM_WORLD);
					
					vec_poslao = 0;
				}
			}
			if (!vec_poslao) {
				//print("Proces %d: Saljem zahtjev za potezom\n", rank);
				MPI_Send(&rank, 1, MPI_INT, 0, ZAHTJEV, MPI_COMM_WORLD);
				vec_poslao = 1;
			}

		}
		//print("Izasao iz petlje %d\n", rank);
		
	}

	MPI_Finalize();
}

void evaluiraj_podstablo(PotezSubtree* subtree) {
	// provjeri je li stablo generirano do kraja. Ako nije, generiraj
	//print("Stvaram podstablo\n");
	std::queue<Potez*>* q = new std::queue<Potez*>();
	Potez node = subtree->potez[0];

	int num_of_nodes = 0;
	int offset = 0;
	int previous_offset = 0;
	//stvaranje podstabla 

	if (subtree->potez[0].status == NIJE_ZAPOCETO_RACUNANJE || subtree->potez[0].status == POTREBAN_NOVI_ZADATAK) {
		for (int i = 0; i < SUBTREE_DEPTH - 1; i++) {
			num_of_nodes = (int)pow(NUM_OF_COLS, i);
			offset += num_of_nodes;
			for (int j = 0; j < num_of_nodes; j++) {
				Potez* parent = &subtree->potez[previous_offset + j];
				for (int k = 0; k < NUM_OF_COLS; k++) {
					Potez* child = &subtree->potez[offset + NUM_OF_COLS * j + k];
					if (moguc_potez((int*)parent->state, k)) {
						odigraj_potez(parent, child, k);
					}
					else {
						child->status = NEMOGUCE_STANJE;
						child->depth = parent->depth - 1;
						child->index = k;
						child->value = -2.0;
					}
				}
			}
			previous_offset += offset;
		}
	}

	for (int i = 0; i < subtree->size; i++) {
		print("Value %d: %d\n", i, subtree->potez[i].value);
	}

	subtree->size = offset + pow(NUM_OF_COLS, SUBTREE_DEPTH - 1);

	delete q;
	
	// evaluiraj sve èvorove podstabla
	int children_start = subtree->size;

	previous_offset = 0;

	for (int i = 0; i < SUBTREE_DEPTH; i++) {
		num_of_nodes = pow(NUM_OF_COLS, SUBTREE_DEPTH - i - 1);
		offset = (pow(NUM_OF_COLS, SUBTREE_DEPTH - i - 1) - 1) / (NUM_OF_COLS - 1);
		for (int j = offset; j < (offset + num_of_nodes); j++) {
			if (i == 0) {
				evaluiraj_potez(&subtree->potez[j]);
			}
			else {
				int parent_index = j;
				Potez* parent = &subtree->potez[parent_index];
				parent->status = ZAPOCETNO_RACUNANJE;
				int moves_count = 0;
				int all_done = 1;
				double total = 0.0;
				int c_all_win = 1, c_all_lose = 1;
				int first_child_index = children_start + j * NUM_OF_COLS;

				for (int k = 0; k < NUM_OF_COLS; k++) {
					int child_index = previous_offset + j * NUM_OF_COLS + k;
					Potez* child = &subtree->potez[child_index];

					// ako je djete zavrsilo s racunanjem
					if (child->status == ZAVRSENO_RACUNANJE) {
						moves_count++;
						if (child->value > -1) {
							c_all_lose = 0;
						}
						if (child->value != 1) {
							c_all_win = 0;
						}
						if (child->value == 1 && child->made_by_player == CPU) {
							parent->status = ZAVRSENO_RACUNANJE;
							parent->value = 1.0;
							break;
						}
						if (child->value == -1 && child->made_by_player == HUMAN) {
							parent->status = ZAVRSENO_RACUNANJE;
							parent->value = -1.0;
							break;
						}
						total += child->value;
					}
					else{
						all_done = 0;
					}
					/*else if (child->status != NEMOGUCE_STANJE) {
						all_done = 0;
						break;
					}*/

				}

				if (all_done) {
					if (c_all_win) {
						parent->status = ZAVRSENO_RACUNANJE;
						parent->value = 1.0;
						continue;
					}
					if (c_all_lose) {
						parent->status = ZAVRSENO_RACUNANJE;
						parent->value = -1.0;
						continue;
					}
					total = total / moves_count;
					parent->status = ZAVRSENO_RACUNANJE;
					parent->value = total;
					continue;
				}
			}
		}
		previous_offset = offset;
	}
}

void evaluiraj_potez(Potez* potez) {
	Potez *node = potez;
	// ne treba se evaluirati potez
	if (node->status == ZAVRSENO_RACUNANJE) {
		return;
	}
	
	int pobjeda = provjeri_pobjedu(node);
	// ako je pobjednicko stanje, zavrsavamo racunanje
	if (pobjeda == 1) {
		node->status = ZAVRSENO_RACUNANJE;
		node->value = 1.0;
		print("Pobjednicki potez");
		return;
	}
	// ako je gubitnicko stanje, zavrsavamo racunanje
	else if (pobjeda == -1) {
		node->status = ZAVRSENO_RACUNANJE;
		node->value = -1.0;
		return;
	}
	// ako je potez na maksimalnoj dubini, zavrsavamo racunanje
	if (node->depth == 0) {
		node->status = ZAVRSENO_RACUNANJE;
		node->value = 0.0;
		return;
	}

	node->status = POTREBAN_NOVI_ZADATAK;
	return;
}