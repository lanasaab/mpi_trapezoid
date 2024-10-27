#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

void pipelined_sieve(int limit, int rank, int Tprocesses) {
    int candidate;
    int end_marker = -1; 
    double start_time = MPI_Wtime();

    int *primes = malloc((limit / 2) * sizeof(int)); //space for primes
    int Pcounter = 0;

    if (rank == 0) {
        for (candidate = 2; candidate <= limit; candidate++) {
            MPI_Send(&candidate, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
        MPI_Send(&end_marker, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
        while (1) {
            MPI_Recv(&candidate, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (candidate == end_marker) {
                if (rank < Tprocesses - 1) {
                    MPI_Send(&end_marker, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
                break;
            }

            bool isPrime = true;
            for (int i = 0; i < Pcounter; i++) {
                if (candidate % primes[i] == 0) {
                    isPrime = false;
                    break;
                }
                if (primes[i] * primes[i] > candidate) {
                    break;
                }
            }

            // Store the prime number and send to the next process
            if (isPrime) {
                printf("Process %d identified prime: %d\n", rank, candidate);
                primes[Pcounter++] = candidate;
                if (rank < Tprocesses - 1) {
                    MPI_Send(&candidate, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // Process for collecting the primes (final process in the sequence)
    if (rank == Tprocesses - 1) {
        while (1) {
            MPI_Recv(&candidate, 1, MPI_INT, Tprocesses - 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (candidate == end_marker) break;
            
            // can print the primes 
        }
       
    }

    // Release allocated memory
    free(primes);

    // End of timing
    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Total execution time: %.6f seconds\n", end_time - start_time);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, Tprocesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &Tprocesses);

    if (argc != 2) {
        if (rank == 0) printf("Usage: %s <limit>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }
    int limit = atoi(argv[1]);

    pipelined_sieve(limit, rank, Tprocesses);

    MPI_Finalize();
    return 0;
}
