/*
 * 凤大骅 2000012965
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    int a0, a1, a2, a3, a4, a5, a6, a7; //临时变量;
    int i = 0, j = 0, k = 0;            //循环变量;
    if (M == 32 && N == 32)             //分成8*8的小块分别转置;
    {
        for (i = 0; i < M; i += 8)
        {
            for (j = 0; j < N; ++j)
            {
                a0 = A[j][i];
                a1 = A[j][i + 1];
                a2 = A[j][i + 2];
                a3 = A[j][i + 3];
                a4 = A[j][i + 4];
                a5 = A[j][i + 5];
                a6 = A[j][i + 6];
                a7 = A[j][i + 7];
                B[i][j] = a0;
                B[i + 1][j] = a1;
                B[i + 2][j] = a2;
                B[i + 3][j] = a3;
                B[i + 4][j] = a4;
                B[i + 5][j] = a5;
                B[i + 6][j] = a6;
                B[i + 7][j] = a7;
            }
        }
    }
    else if (M == 64 && N == 64) //分成8*8的小块分别处理;
    {
        for (i = 0; i < M; i += 8)
        {
            for (j = 0; j < N; j += 8)
            {
                for (k = j; k < j + 4; ++k) //前四行前四列直接转置;
                {
                    a0 = A[k][i];
                    a1 = A[k][i + 1];
                    a2 = A[k][i + 2];
                    a3 = A[k][i + 3];
                    B[i][k] = a0;
                    B[i + 1][k] = a1;
                    B[i + 2][k] = a2;
                    B[i + 3][k] = a3;
                }
                for (k = j; k < j + 4; ++k) //前四行后四列先存到前四行前四列对应的位置右边的4*4方格中;
                {
                    a0 = A[k][i + 4];
                    a1 = A[k][i + 5];
                    a2 = A[k][i + 6];
                    a3 = A[k][i + 7];
                    B[i][k + 4] = a0;
                    B[i + 1][k + 4] = a1;
                    B[i + 2][k + 4] = a2;
                    B[i + 3][k + 4] = a3;
                }
                for (k = i; k < i + 4; ++k) //前四行后四列存到该存的位置，转置后四行前四列;
                {
                    a0 = B[k][j + 4];
                    a1 = B[k][j + 5];
                    a2 = B[k][j + 6];
                    a3 = B[k][j + 7];
                    a4 = A[j + 4][k];
                    a5 = A[j + 5][k];
                    a6 = A[j + 6][k];
                    a7 = A[j + 7][k];
                    B[k][j + 4] = a4;
                    B[k][j + 5] = a5;
                    B[k][j + 6] = a6;
                    B[k][j + 7] = a7;
                    B[k + 4][j] = a0;
                    B[k + 4][j + 1] = a1;
                    B[k + 4][j + 2] = a2;
                    B[k + 4][j + 3] = a3;
                }
                for (k = i + 4; k < i + 8; ++k) //转置后四行后四列;
                {
                    a0 = A[j + 4][k];
                    a1 = A[j + 5][k];
                    a2 = A[j + 6][k];
                    a3 = A[j + 7][k];
                    B[k][j + 4] = a0;
                    B[k][j + 5] = a1;
                    B[k][j + 6] = a2;
                    B[k][j + 7] = a3;
                }
            }
        }
    }
    else if (M == 60 && N == 68) //直接每次处理8个，一行中不够8个就处理4个;
    {
        for (j = 0; j < N + 4; j += 8)
        {
            for (i = 0; i < M + 4; i += 8)
            {
                for (k = i; k < i + 8 && k < M; ++k)
                {
                    a0 = A[j][k];
                    a1 = A[j + 1][k];
                    a2 = A[j + 2][k];
                    a3 = A[j + 3][k];
                    if (j + 4 < N)
                    {
                        a4 = A[j + 4][k];
                        a5 = A[j + 5][k];
                        a6 = A[j + 6][k];
                        a7 = A[j + 7][k];
                    }
                    B[k][j] = a0;
                    B[k][j + 1] = a1;
                    B[k][j + 2] = a2;
                    B[k][j + 3] = a3;
                    if (j + 4 < N)
                    {
                        B[k][j + 4] = a4;
                        B[k][j + 5] = a5;
                        B[k][j + 6] = a6;
                        B[k][j + 7] = a7;
                    }
                }
            }
        }
    }
    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
  * trans - A simple baseline transpose function, not optimized for the cache.
  */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
