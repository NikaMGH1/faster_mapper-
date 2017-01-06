// Dynamic Programming - D table : O(mn)
#include <stdio.h>
#include "rdtsc.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int n, m, k;
char *text;
char *pattern;
int **D;

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

int main(int argc, char *argv[])
{
    if (argc != 4)
        printf("usage: ./exec text pattern k");

    readTextandPattern(argv, &n, &m);
    k = atoi(argv[3]);

    unsigned long long t1, t2;
    t1 = rdtsc();

    D = (int **)malloc((m + 1) * sizeof(int *));
    for (int i = 0; i < m + 1; i++)
        D[i] = (int *)malloc((n + 1) * sizeof(int));

    for (int i = 0; i <= n; i++)
        D[0][i] = 0;
    for (int i = 1; i <= m; i++)
        D[i][0] = i;

    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (pattern[i - 1] == text[j - 1])
                D[i][j] = fmin(fmin(D[i - 1][j] + 1, D[i][j - 1] + 1), D[i - 1][j - 1]);
            else
                D[i][j] = fmin(fmin(D[i - 1][j] + 1, D[i][j - 1] + 1), D[i - 1][j - 1] + 1);
        }
    }
    for (int i = 1; i <= n; i++) {
        if (D[m][i] <= k) {
            // printf("%d ", i);
        }
    }

    for (int i = 0; i < m+1; i++)
        free(D[i]);
    free(D);
    free(text);
    free(pattern);

    t2 = rdtsc();
    printf("%llu\n", t2 - t1);
}