#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <stdlib.h>

#define FORK_REQUEST 0
#define FORK_RESPONSE 1

static inline void indent(int n)
{
	for (int i = 0; i < n; i++) {
		putchar('\t');
		putchar('\t');
	}
}

int main(int argc, char** argv) {

	
	// Initialize the MPI environment
	MPI_Init(argc, argv);
	srand(clock());

	// Find out rank, size
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	

	int have_left_fork = 0;
	int have_right_fork = 0;
	int is_left_dirty = 1;
	int is_right_dirty = 1;

	int left_neighbor_index = (world_rank - 1 + world_size) % world_size;
	int right_neighbor_index = (world_rank + 1) % world_size;

	int asked_for_left = 0;
	int asked_for_right = 0;

	if (world_rank == 0) {
		have_left_fork = 1;
		have_right_fork = 1;
	}
	else if (world_rank != (world_size - 1)) {
		have_right_fork = 1;
	}
	while (1) {
		// Proces razmišljanja
		int thinking_time = (rand() * rand()) % 5000;
		indent(world_rank);
		fprintf(stdout, "mislim\n");
		fflush(stdout);
		while (thinking_time > 0) {
			// Provjera svakih 100ms je li došla neka poruka
			if (thinking_time >= 100) {
				int flag;
				MPI_Status probeStatus;
				MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &probeStatus);
				// Ako je došla poruka
				if (flag != 0) {
					int philosopher_rank;
					MPI_Status message_status;
					MPI_Recv(&philosopher_rank, 1, MPI_INT, probeStatus.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &message_status);
					// ako je trazena vilica kod mene, šaljem ju
					if (message_status.MPI_TAG == FORK_REQUEST) {
						// Èistim prikladnu vilicu
						// Više nemam tu vilicu
						if (philosopher_rank == left_neighbor_index && have_left_fork) {
							is_left_dirty = 0;
							have_left_fork = 0;
						}
						else if (philosopher_rank == right_neighbor_index && have_right_fork) {
							is_right_dirty = 0;
							have_right_fork = 0;
						}
						// šaljem vilicu filozofu koji ju je zatražio
						MPI_Send(&world_rank, 1, MPI_INT, philosopher_rank, FORK_RESPONSE, MPI_COMM_WORLD);
					}
				}

				Sleep(10);
				thinking_time -= 10;
			}
			else {
				Sleep(thinking_time);
				thinking_time = 0;
			}
		}
		// Gotov proces razmišljanja
		// 
		// Poèetak procesa skupljanja vilica
		int rightForkRequested = 0;
		int leftForkRequested = 0;
		while (!have_left_fork || !have_right_fork) {
			int missingFork = 0;
			// ako nemam neku od vilica, šaljem zahtjev za njima
			if (!have_left_fork && !asked_for_left) {
				indent(world_rank);
				fprintf(stdout, "trazim vilicu (%d)\n", left_neighbor_index);
				fflush(stdout);
				MPI_Send(&world_rank, 1, MPI_INT, left_neighbor_index, FORK_REQUEST, MPI_COMM_WORLD);
				asked_for_left = 1;
			}
			if (!have_right_fork && !asked_for_right) {
				indent(world_rank);
				fprintf(stdout, "trazim vilicu (%d)\n", right_neighbor_index);
				fflush(stdout);
				MPI_Send(&world_rank, 1, MPI_INT, right_neighbor_index, FORK_REQUEST, MPI_COMM_WORLD);
				missingFork = 1;
				asked_for_right = 1;
			}
			
			do {
				int philosopher_rank;
				MPI_Status message_status;
				MPI_Recv(&philosopher_rank, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &message_status);

				// Ako je primljena poruka odgovor na zahtjev za vilicom
				if (message_status.MPI_TAG == FORK_RESPONSE) {
					// Ako je lijevi susjed polao vilicu, azuriram lijevu vilicu
					if (philosopher_rank == left_neighbor_index && !have_left_fork) {
						have_left_fork = 1;
						is_left_dirty = 0;
						asked_for_left = 0;
					}
					// Ako je desni susjed polao vilicu i imam prljav, azuriram desnu vilicu
					else if (philosopher_rank == right_neighbor_index && !have_right_fork) {
						have_right_fork = 1;
						is_right_dirty = 0;
						asked_for_right = 0;
					}
				}
				// Ako je primljena poruka zahtjev za vilicom
				else if (message_status.MPI_TAG == FORK_REQUEST) {
					// Ako je lijevi filozof tražio vilicu, imam ju i prljava je -> èistim vilicu i šaljem lijevom filozofu
					if (philosopher_rank == left_neighbor_index && have_left_fork && is_left_dirty) {
						is_left_dirty = 0;
						MPI_Send(&world_rank, 1, MPI_INT, philosopher_rank, FORK_RESPONSE, MPI_COMM_WORLD);
						have_left_fork = 0;
					}
					// Ako je desni filozof tražio vilicu, imam ju i prljava je -> èistim vilicu i šaljem desnom filozofu
					else if (philosopher_rank == right_neighbor_index && have_right_fork && is_right_dirty) {
						is_right_dirty = 0;
						MPI_Send(&world_rank, 1, MPI_INT, philosopher_rank, FORK_RESPONSE, MPI_COMM_WORLD);
						have_right_fork = 0;
					}
					// Ako je susjedni filozof poslao zahtjev, a vilica koju je tražio nije prljava ili ju nemam trenutno, bilježim zahtjev
					else {
						if (message_status.MPI_SOURCE == left_neighbor_index && have_left_fork) {
							leftForkRequested = 1;
						}
						else if (message_status.MPI_SOURCE == right_neighbor_index && have_right_fork) {
							rightForkRequested = 1;
						}
					}
				}
			} while ((missingFork == 0 && !have_left_fork) || (missingFork == 1 && !have_right_fork));
		}
		// Kraj procesa skupljanja vilica
		// 
		// Poèetak procesa jela
		indent(world_rank);
		fprintf(stdout, "jedem\n");
		fflush(stdout);
		Sleep(rand() % 5000);
		// nakon jela vilice postaju prljave
		is_left_dirty = 1;
		is_right_dirty = 1;

		// obrada zabilježenih zahtjeva
		if (rightForkRequested == 1) {
			is_right_dirty = 0;
			MPI_Send(&world_rank, 1, MPI_INT, right_neighbor_index, FORK_RESPONSE, MPI_COMM_WORLD);
			have_right_fork = 0;
			rightForkRequested = 0;
		}
		if (leftForkRequested == 1) {
			is_left_dirty = 0;
			MPI_Send(&world_rank, 1, MPI_INT, left_neighbor_index, FORK_RESPONSE, MPI_COMM_WORLD);
			have_left_fork = 0;
			leftForkRequested = 0;
		}
	}

	MPI_Finalize();
	return 0;
}