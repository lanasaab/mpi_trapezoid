#include <mpi.h>
#include <stdio.h>
#include <math.h>

// function to evaluate the curve (y = f(x))
float fn(float x) {
    return x * x ; // example: y = x^2
}

// this function is to compute the area of a trapezoid
float trapezoid_area(float a, float b, float d) { 
    float area = 0;
    for (float x = a; x < b; x+=d) {
        area += fn(x) + fn(x+d);
    }
    
    return area * d / 2.0f;
}

int main(int argc, char** argv) {
    int rank, size;
    float a = 0.0f, b = 1.0f;  
    int nb;
    float start, end, local_area, total_area;

    //assume the number of intervals is given
    nb = 10000000;
    
    double sequential_start, sequential_end,sequential_time;
    double sequential_area = 0;
    float d = (b - a) / nb; // delta 

    //Parallel Implementation
    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes
    double parallel_start, parallel_end, parallel_time;


    if (rank == 0) {
        // Get number of intervals from the user
        //print the number of intervals
        //fflush(stdout);
        //scanf("%d", &n);
        //printf("Process 0 received n = %d\n", n); 

        //Sequential implemenetation 
    
        printf("The number of intervals is: %d \n", nb);

        sequential_start = MPI_Wtime();

        sequential_area = trapezoid_area(a, b, d);
        
        sequential_end = MPI_Wtime();

        sequential_time = sequential_end - sequential_start;

        printf("\n");
        printf("..Sequential Results.. \n");
        printf("The total area under the curve is: %f\n", sequential_area);
        printf("The time took to complete the operation is: %f  \n", sequential_time);

    }
   

    parallel_start = MPI_Wtime();
    // Broadcast the number of intervals to all the processes
    MPI_Bcast(&nb, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate interval size for each process
    float region = (b - a)/ size;
    
    // Calculate local bounds for each process
    start = a + rank * region;
    end = start + region;
    
    // Each process calculates the area of its subinterval
    local_area = trapezoid_area(start, end, d);
    
    // Reduce all local areas to the total area on root process
    MPI_Reduce(&local_area, &total_area, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    parallel_end = MPI_Wtime();

    parallel_time = parallel_end - parallel_start;
    
    if (rank == 0) {
        printf("\n");
        printf("..Parallel Results.. \n");
        printf("The total area under the curve is: %f\n", total_area);
        printf("The time took to complete the operation is: %f\n", parallel_time);
        printf("\n");

        float speedUp =  sequential_time/parallel_time;
        printf("The speedup factor is %f \n",speedUp);
        printf("The efficiency is %f",(float)(speedUp/size)*100);
    }
    
    MPI_Finalize(); // Finalize MPI
    return 0;
}
