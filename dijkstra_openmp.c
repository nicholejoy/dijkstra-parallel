#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <sys/timeb.h>

int **generateMatrix(int size)
{
    int matrixDim = size;
    int **testMatrix = malloc(matrixDim* sizeof(int*));
    for (int a = 0; a < matrixDim; a++)
    {
        testMatrix[a] = malloc(sizeof(int*) *matrixDim);
    }
    srand(200);

    for (int i = 0; i < matrixDim; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            testMatrix[i][j] = rand() % 99 + 1;
        }
    }
    for (int i = 0; i < matrixDim; i++)
    {
        for (int j = 0; j < matrixDim; j++)
        {
            if (j == i) testMatrix[i][j] = 0;
            if (j > i) testMatrix[i][j] = testMatrix[j][i];
        }
    }
    return testMatrix;

}

int main(int argc, char **argv)
{

    struct timeb start, end;
    int difference;
    int minDistNodeGlobal = INT_MAX, minDistIndexGlobal = NULL;
    int matrixDim = 1000;
    int **testMatrix = generateMatrix(matrixDim);
    int *minDist;
    int *nodeCheck;

    ftime(&start);

    minDist = (int*) malloc(matrixDim* sizeof(int*));
    nodeCheck = (int*) malloc(matrixDim* sizeof(int*));

    for (int i = 0; i < matrixDim; i++)
        minDist[i] = INT_MAX, nodeCheck[i] = 0;

    minDist[0] = 0;

    for (int j = 0; j < matrixDim; j++)
    {

        minDistNodeGlobal = INT_MAX;
        minDistIndexGlobal = NULL;

#pragma omp parallel
        {

            int minDistNode = INT_MAX, minDistIndex = NULL;
            int nproc;
            int procid;
            int part;
            int start;
            int end;
            nproc = omp_get_num_threads();
            procid = omp_get_thread_num();
            part = (matrixDim + nproc - 1) / nproc;
            start = procid * part;
            if ((procid + 1) *part < matrixDim)
            {
                end = (procid + 1) *part;
            }
            else end = matrixDim;

            for (int v = start; v < end; v++)
            {

                if (nodeCheck[v] == 0 && minDist[v] <= minDistNode)
                {
                    minDistNode = minDist[v], minDistIndex = v;
                }
            }

#pragma omp critical
            if (minDistNode < minDistNodeGlobal)
            {
                minDistNodeGlobal = minDistNode, minDistIndexGlobal = minDistIndex;
            }
#pragma omp barrier
#pragma omp master
            {
                nodeCheck[minDistIndexGlobal] = 1;
            }
#pragma omp barrier
            for (int i = start; i < end; i++)
            {
                if (nodeCheck[i] == 0 && testMatrix[minDistIndexGlobal][i] > 0 && minDist[minDistIndexGlobal] != INT_MAX &&
                    minDist[minDistIndexGlobal] + testMatrix[minDistIndexGlobal][i] < minDist[i])
                {
                    int a = minDist[minDistIndexGlobal];
                    int b = testMatrix[minDistIndexGlobal][i];
                    minDist[i] = a + b;
                }
            }
#pragma omp barrier
        }
    }

    ftime(&end);
    difference = (int)(1000.0 *(end.time - start.time) +
        (end.millitm - start.millitm));
    printf("\nTime taken: %u milliseconds\n", difference);

    for (int i = 0; i < matrixDim; i++)
    {
        free(testMatrix[i]);
    }
    free(testMatrix);
    free(minDist);
    free(nodeCheck);

}

