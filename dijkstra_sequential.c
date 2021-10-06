#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <limits.h>

int *minDist;
int *nodeCheck;

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

void findSmallest(int start, int end, int *minDistNode, int *minDistIndex)
{
    for (int v = start; v < end; v++)
    {
        if (nodeCheck[v] == 0 && minDist[v] <= *minDistNode)
            *
            minDistNode = minDist[v], *minDistIndex = v;
    }
}

int main(int argc, char **argv)
{
    struct timeb start, end;
    int difference;
   	
    int matrixDim = 1000;
    int **testMatrix = generateMatrix(matrixDim);

    ftime(&start);

    minDist = (int*) malloc(matrixDim* sizeof(int*));
    nodeCheck = (int*) malloc(matrixDim* sizeof(int*));

    for (int i = 0; i < matrixDim; i++)
        minDist[i] = INT_MAX, nodeCheck[i] = 0;

    minDist[0] = 0;

    for (int j = 0; j < matrixDim; j++)
    {

        int minDistNode = INT_MAX;
        int minDistIndex;
        int start = 0;
        int end = matrixDim;
        findSmallest(start, end, &minDistNode, &minDistIndex);

        nodeCheck[minDistIndex] = 1;

        for (int i = 0; i < matrixDim; i++)
        {
            if (nodeCheck[i] == 0 && testMatrix[minDistIndex][i] > 0 && minDist[minDistIndex] != INT_MAX &&
                minDist[minDistIndex] + testMatrix[minDistIndex][i] < minDist[i])
            {
                int a = minDist[minDistIndex];
                int b = testMatrix[minDistIndex][i];
                minDist[i] = a + b;
            }
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

