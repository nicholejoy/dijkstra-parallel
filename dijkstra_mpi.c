#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <limits.h>
#include <mpi.h>

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
            *minDistNode = minDist[v], *minDistIndex = v;
    }
}

int main(int argc, char **argv)
{
    struct timeb startTime, endTime;
    int matrixDim = 1000;
    int difference;
    int **testMatrix = generateMatrix(matrixDim);
    int procid;
    int nproc;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &procid);

    if (procid == 0)
    {
        ftime(&startTime);
    }

    int part;
    int start;
    int end;
    int sendPair[2];
    int recvPair[2];

    int minDistNode;
    int minDistIndex;

    part = (matrixDim + nproc - 1) / nproc;
    start = procid * part;
    if ((procid + 1) *part < matrixDim)
    {
        end = (procid + 1) *part;
    }
    else end = matrixDim;
    MPI_Barrier(MPI_COMM_WORLD);

    minDist = (int*) malloc(matrixDim* sizeof(int*));
    nodeCheck = (int*) malloc(matrixDim* sizeof(int*));

    for (int i = 0; i < matrixDim; i++)
        minDist[i] = INT_MAX, nodeCheck[i] = 0;

    minDist[0] = 0;

    for (int j = 0; j < matrixDim; j++)
    {

        minDistNode = INT_MAX;

        findSmallest(start, end, &minDistNode, &minDistIndex);

        sendPair[0] = minDistNode;
        sendPair[1] = minDistIndex;
        if (procid != 0)
        {
            MPI_Send(&sendPair, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }

        if (procid == 0)
        {

            int k = 1;
            while (k < nproc)
            {
                MPI_Recv(&recvPair, 2, MPI_INT, MPI_ANY_TAG, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (recvPair[0] < sendPair[0])
                {
                    sendPair[0] = recvPair[0];
                    sendPair[1] = recvPair[1];
                }
                k = k + 1;
            }
        }
        MPI_Bcast(&sendPair, 2, MPI_INT, 0, MPI_COMM_WORLD);
        minDistIndex = sendPair[1];
        minDistNode = sendPair[0];
        nodeCheck[minDistIndex] = 1;	//problem line
        minDist[minDistIndex] = minDistNode;

        for (int i = 0; i < matrixDim; i++)
        {
            if (nodeCheck[i] == 0 && testMatrix[minDistIndex][i] > 0 && minDist[minDistIndex] != INT_MAX &&
                minDist[minDistIndex] + testMatrix[minDistIndex][i] < minDist[i])
            {
                minDist[i] = minDistNode + testMatrix[minDistIndex][i];
            }
        }
    }

    MPI_Finalize();
    if (procid == 0)
    {
        ftime(&endTime);
        difference = (int)(1000.0 *(endTime.time - startTime.time) +
            (endTime.millitm - startTime.millitm));
        printf("\nTime taken: %u milliseconds\n", difference);
    }

    for (int i = 0; i < matrixDim; i++)
    {
        free(testMatrix[i]);
    }
    free(testMatrix);
    free(minDist);
    free(nodeCheck);

}

