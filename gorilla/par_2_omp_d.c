// Dynamic Programming - C table : k+1 threads, O(n) in theory
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <omp.h>
#include "rdtsc.h"

int n, m, k;
char *text, *pattern;
int **C;
int NUMBER_OF_THREADS;


void set_C_table(int i, int j, int value)
{

    C[i + 1][j + k + 1] = value;
}

int get_C_table(int i, int j)
{
    return C[i + 1][j + k + 1];
}


//D = b => all elements where d+e = b
void setCDiagonalElem(int diag, int e, int value) {

    C[e + 1][diag - e + k + 1] = value;
}

int getCDiagonalElem(int diag, int e) {
    return C[e + 1][diag - e + k + 1];
}

void readTextandPattern(char *argv[], int *p_n, int *p_m)
{
  // Read text file
  FILE *f = fopen(argv[1], "r");
  if (f == NULL)
  {
    perror("Error opening file");
    return;
  }
  fseek(f, 0, SEEK_END);
  *p_n = ftell(f);
  fseek(f, 0, SEEK_SET);
  text = (char *)malloc((*p_n + 1) * sizeof(char));
  if (fgets(text, *p_n + 1, f) == NULL) {
    perror("Error reading file");
    return;
  }
  fclose(f);

  // Read pattern file
  f = fopen(argv[2], "r");
  if (f == NULL)
  {
    perror("Error opening file");
    return;
  }
  fseek(f, 0, SEEK_END);
  *p_m = ftell(f);
  fseek(f, 0, SEEK_SET);
  pattern = (char *)malloc((*p_m + 1) * sizeof(char));
  if (fgets(pattern, *p_m + 1, f) == NULL) {
    perror("Error reading file");
    return;
  }
  fclose(f);
}



// void printC() {

//     for (int j = 0; j < k + 2; j++) {
//         for (int i = 0; i < n - m + 2 * k + 3; i++) {
//             printf("|%d| ", C[j][i]);
//         }
//         printf("\n");
//     }
//     printf("\n\n\n");


// }

int main(int argc, char *argv[])
{     unsigned long long t1, t2;

    t1 = rdtsc();
    if (argc != 5) {
        printf("\n usage: ./exec text pattern k n_threads \n \n ");
        return 0;
    }
    readTextandPattern(argv, &n, &m);

    k = atoi(argv[3]);
    NUMBER_OF_THREADS = atoi(argv[4]);

    int not_initialized = -2;

    C = (int **)malloc((k + 2) * sizeof(int *));
    for (int i = 0; i < k + 2; i++)
        C[i] = (int *)malloc((n - m + 2 * k + 3) * sizeof(int));


    // printf("C allocated \n ");



     omp_set_num_threads(NUMBER_OF_THREADS);
    #pragma omp parallel
    { 

        int nthreads;
        nthreads = omp_get_num_threads();

        int ID = omp_get_thread_num();

        int not_initialized = -2;



        //**********************************************************
        //************************  MATRIX INITIALIZATION  *********************
        //**********************************************************




        for (int i = ID; i < k + 2; i = i + nthreads)
            for (int j = 0; j < n - m + 2 * k + 3; j++)
                C[i][j] = not_initialized;


        #pragma omp barrier

        int subportion  = (n - m + k + 2) / nthreads;
        int remainder = (n - m + k + 2) % nthreads;
        if (ID == 0) {
            for (int d = 0; d < subportion + remainder; d++)
                set_C_table(-1, d, d - 1);
        } else {
            for (int d = ID * subportion + remainder; d < ID * subportion + subportion + remainder; d++)
                set_C_table(-1, d, d - 1);
        }


        for (int d = -(k + 1) + ID; d <= -1; d = d + nthreads) {
            set_C_table(-d - 1, d, -1);
            set_C_table(-d - 2, d, INT_MIN);
        }


        //**********************************************************
        //************************  Computation: working in parallel  *********************
        //**********************************************************




        #pragma omp barrier


        for (int diag = ID; diag <=  n - m + k + 1; diag = diag + nthreads ) {
            for (int e = 0; e <= k ; e++) {
                int d = diag - e;
                while (getCDiagonalElem(diag - 2, e - 1) == not_initialized || getCDiagonalElem(diag - 1, e - 1) == not_initialized) {
                    // printf("wait C[%d, %d]\n", e-1, d+1);
                }
                int col = fmax(fmax( getCDiagonalElem(diag - 2, e - 1) + 1, getCDiagonalElem(diag - 1, e - 1) + 1), getCDiagonalElem(diag, e - 1));
                while (col < n && col - d < m && text[col] == pattern[col - d]) {
                    col++;
                }
                setCDiagonalElem(diag, e, fmin(fmin(col, m + d), n));
            }
        }

        #pragma omp barrier

   


        // #pragma omp barrier


        //**********************************************************
        //************************  Output: working in parallel  *********************
        //**********************************************************


        // #pragma omp barrier
        // //Matrix computed. Going through last line


        // if (ID == 0) {
        //     for (int d = -k  ; d <= n - m; d = d + 1)
        //         if (get_C_table(k, d) == d + m && d + m > 0)
        //             printf("%d  ", d + m);
        // }


        //**********************************************************
        //************************  Dealloc: working in parallel  *********************
        //**********************************************************

        #pragma omp barrier
        for (int i = ID; i < k + 2; i = i + nthreads)
            free(C[i]);

    }


    free(C);

  t2 = rdtsc();
  printf("%llu \n", t2 - t1);



}