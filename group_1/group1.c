#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ------------------------------- Group 1 -------------------------------------
// --------------------------- Antonino Rando ----------------------------------

// 1. TYPE SAFETY --------------------------------------------------------------

/// @brief The `isLittleEndian` function is used to determine if the user system
/// is little endian or big endian.
/// @return `1` if the system is little endian, `0` otherwise.
int isLittleEndian()
{
    unsigned int x = 1;
    char *c = (char *)&x;
    return (*c) ? 1 : 0;
}

// 2. MUTABLE ARRAYS -----------------------------------------------------------

/* Since the function returns an `int` and not a vector, we must pass the input
vector by reference, otherwise it would be impossible to retrieve the new vector
from this function. */

/* An array can be passed as a pointer to the first element of the array. The
void type indicates that the array element can be of any type. */

/* The idea behing the following algorithm is to apply a sorting algorithm on an
array of pointers (to the original array's elements). In this way, we are both
able to:
    - obtains the elements values in order to sort them;
    - change duplicated elements in the original lists using the pointer.
After we changed every duplicated element in the original list with a `NULL`
pointer, we can easily create a new list with no duplicates in which every
element preserves its original order. */

/// @brief A variation of the `merge` function for a the merge sort algorithm,
/// in which: duplicates are "removed"; the list to merge is a list of
/// pointers of elements of an original list. The duplicated elements are stored
/// as a `NULL` pointer in the original list.
void merge(int **arrPtr, int l, int m, int r)
{
    int n1 = m - l + 1;
    int n2 = r - m;

    int **L = (int **)malloc(n1 * sizeof(int *));
    int **R = (int **)malloc(n2 * sizeof(int *));

    memcpy(L, arrPtr + l, n1 * sizeof(int *));
    memcpy(R, arrPtr + m + 1, n2 * sizeof(int *));

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
    {
        if (*L[i] < *R[j])
            arrPtr[k++] = L[i++];
        else if (*L[i] > *R[j])
            arrPtr[k++] = R[j++];
        else
        {
            *R[j] = -1; // We set to `NULL` the rightmost element to preserver
                        // original order.
            arrPtr[k++] = L[i++];
        }
    }

    while (i < n1)
        arrPtr[k++] = L[i++];
    while (j < n2)
        arrPtr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int **arrPtr, int arrBegin, int arrEnd)
{
    if (arrBegin >= arrEnd)
        return;

    // Same as (l+r)/2, but avoids overflow for large l and r
    int med = arrBegin + (arrEnd - arrBegin) / 2;
    mergeSort(arrPtr, med + 1, arrEnd);
    mergeSort(arrPtr, arrBegin, med);
    merge(arrPtr, arrBegin, med, arrEnd);
}

int removeDups(int *arr, int len)
{
    int **arrPtr = (int **)malloc(len * sizeof(int *));
    int removed = 0;

    for (int i = 0; i < len; i++)
        arrPtr[i] = &arr[i];

    mergeSort(arrPtr, 0, len - 1);

    int *arrClone = (int *)malloc(len * sizeof(int));

    int k = 0;
    for (int i = 0; i < len; i++)
    {
        if (arr[i] == -1)
        {
            removed++;
            continue;
        }
        arrClone[k++] = arr[i];
    }

    memcpy(arr, arrClone, (len - removed) * sizeof(int));

    return len - removed;
}

// 3. BINOMIAL COEFFICIENT -----------------------------------------------------

typedef struct cBinTree
{
    int n;
    int k;
    int binomial;
    struct cBinTree *left;
    struct cBinTree *right;
} cBinTree;

cBinTree *cbin(int n, int k)
{
    cBinTree *result = (cBinTree *)malloc(sizeof(cBinTree));
    result->n = n;
    result->k = k;

    if (n == k || k == 0)
    {
        result->binomial = 1;
        result->left = NULL;
        result->right = NULL;
        return result;
    }

    result->left = cbin(n - 1, k - 1);
    result->right = cbin(n - 1, k);
    result->binomial = result->left->binomial + result->right->binomial;
    return result;
}

cBinTree *_cbinDP(int n, int k, cBinTree *memo[n + 1][k + 1])
{
    if (memo[n][k] != NULL)
        return memo[n][k];

    cBinTree *result = (cBinTree *)malloc(sizeof(cBinTree));
    result->n = n;
    result->k = k;

    if (n == k || k == 0)
    {
        result->binomial = 1;
        result->left = NULL;
        result->right = NULL;
        memo[n][k] = result;
        return result;
    }

    result->left = _cbinDP(n - 1, k - 1, memo);
    result->right = _cbinDP(n - 1, k, memo);
    result->binomial = result->left->binomial + result->right->binomial;
    memo[n][k] = result;
    return result;
}

cBinTree *cbinDP(int n, int k)
{
    cBinTree *memo[n + 1][k + 1];

    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= k; j++)
            memo[i][j] = NULL;

    return _cbinDP(n, k, memo);
}

// 4. EULER SIEVE --------------------------------------------------------------

typedef struct
{
    int next;
    int prev;
} pair;

pair *eulerSieve(int n)
{
    if (n < 2)
        return NULL;

    pair *result = (pair *)malloc(n * sizeof(pair));
    for (int i = 0; i < n; i++)
    {
        result[i].next = 1;
        result[i].prev = 1;
    }

    int prime = 2;
    // int step = 1;
    while (prime * prime <= n)
    {
        int i = prime - 1;
        pair current = result[i];
        i += current.next * prime;
        while (i < n)
        {
            // printf("step: %d\n", step++);
            // printPairs(result, n);
            // printf("\n--------------------------------\n");

            pair toRemove = result[i];
            result[i - toRemove.prev].next += toRemove.next;
            pair prev = result[i - toRemove.prev];
            result[i - toRemove.prev + prev.next].prev = prev.next;

            toRemove.prev = 0;
            toRemove.next = 0;
            result[i] = toRemove;
            i += prime;
        }
        prime += result[prime - 1].next;
    }

    return result;
}

// TESTS -----------------------------------------------------------------------

void printArray(int A[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

void _printCBinTree(cBinTree *root, int level)
{
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("--");
    printf("(%d %d) = %d\n", root->n, root->k, root->binomial);
    _printCBinTree(root->right, level + 1);
    _printCBinTree(root->left, level + 1);
}

void printCBinTree(cBinTree *root)
{
    _printCBinTree(root, 0);
}

void printPairs(pair *pairs, int n)
{
    printf("pos ");
    for (int i = 1; i <= n; i++)
        printf(i < 10 ? "%d  " : "%d ", i);
    printf("\nnxt ");
    for (int i = 0; i < n; i++)
        printf("%d  ", pairs[i].next);
    printf("\nprv ");
    for (int i = 0; i < n; i++)
        printf("%d  ", pairs[i].prev);
}

int main(int arc, char **argv)
{
    printPairs(eulerSieve(24), 24);
    // int left;

    // // My system is little endian, thus I'm checking for 1
    // printf("1. TEST LITTLE ENDIAN\n");
    // assert(isLittleEndian() == 1 && "FAIL");
    // printf("OK\n");

    // printf("2. TESTS REMOVE DUPS, unique ordered\n");
    // int expected[] = {1, 2, 3, 4, 5};
    // int uniquesOrdered[] = {1, 2, 3, 4, 5};
    // left = removeDups(uniquesOrdered, 5);
    // assert(memcmp(uniquesOrdered, expected, sizeof(expected)) == 0 && "FAIL");
    // assert(left == 5 && "FAIL");
    // printf("OK\n");

    // printf("3. TESTS REMOVE DUPS, unique unordered\n");
    // int expUniques[] = {4, 5, 3, 1, 2};
    // int uniques[] = {4, 5, 3, 1, 2};
    // left = removeDups(uniques, 5);
    // assert(memcmp(uniques, expUniques, sizeof(expUniques)) == 0 && "FAIL");
    // assert(left == 5 && "FAIL");
    // printf("OK\n");

    // printf("4. TESTS REMOVE DUPS, unordered\n");
    // int expDups[] = {4, 5, 3, 1, 2};
    // int dups[] = {4, 4, 5, 3, 5, 1, 4, 2};
    // left = removeDups(dups, 8);
    // assert(memcmp(dups, expDups, sizeof(expDups)) == 0 && "FAIL");
    // assert(left == 5 && "FAIL");
    // printf("OK\n");

    // printf("5. TESTS REMOVE DUPS, all equals\n");
    // int expEqs[] = {4};
    // int eqs[] = {4, 4, 4, 4, 4, 4, 4, 4};
    // left = removeDups(eqs, 8);
    // assert(memcmp(eqs, expEqs, sizeof(expEqs)) == 0 && "FAIL");
    // assert(left == 1 && "FAIL");
    // printf("OK\n");

    // printf("6. TESTS REMOVE DUPS, unordered 2\n");
    // int expDups2[] = {4, 8, 9, 1, 0};
    // int dups2[] = {4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 9, 1, 1, 1, 1, 0, 0, 4, 4};
    // left = removeDups(dups2, 21);
    // assert(memcmp(dups2, expDups2, sizeof(expDups2)) == 0 && "FAIL");
    // assert(left == 5 && "FAIL");
    // printf("OK\n");

    // printf("7. TESTS BINOMIAL, (5 3)\n");
    // printCBinTree(cbin(5, 3));
    // printCBinTree(cbinDP(5, 3));
}