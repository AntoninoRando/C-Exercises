#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ------------------------------- Group 1 -------------------------------------
// --------------------------- Antonino Rando ----------------------------------

// 1. TYPE SAFETY --------------------------------------------------------------

/** @brief The `isLittleEndian` function is used to determine if the user system
 * is little endian or big endian.
 * @return `1` if the system is little endian, `0` otherwise.
 */
int isLittleEndian()
{
    unsigned int x = 1;
    // The value of the first byte of x is (char*)&x. If the system is little
    // endian, it will be `1`, otherwise it will be `0`.
    return ((char *)&x) ? 1 : 0;
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

/** @brief A variation of the `merge` function for the merge sort algorithm, in
 * which the list to sort is passed as a list of pointers to the original list
 * elements. Duplicate elements will be stored as -1 in the original list, in
 * this way they can be removed.
 *
 * The original order will not be sorted, only @arrPtr@ will be sorted. In this
 * way, we can easily remove duplicates from the original list while preserving
 * elements original order.
 * @param arrPtr The list of pointers to the original list elements.
 * @param dups An array of flags to mark duplicated elements.
 * @param arr The original array, used to retrieve duplicated elements indexes.
 * @param l starting index of the left subarray to merge.
 * @param m last index of the left subarray to merge.
 * @param r last index of the right subarray to merge.
 */
void merge(int **arrPtr, int *dups, int *arr, int l, int m, int r)
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
            int elementIndex = R[j] - arr;
            dups[elementIndex] = 1;
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

void mergeSort(int **arrPtr, int *dups, int *arr, int arrBegin, int arrEnd)
{
    if (arrBegin >= arrEnd)
        return;

    // Same as (l+r)/2, but avoids overflow for large l and r
    int med = arrBegin + (arrEnd - arrBegin) / 2;
    mergeSort(arrPtr, dups, arr, med + 1, arrEnd);
    mergeSort(arrPtr, dups, arr, arrBegin, med);
    merge(arrPtr, dups, arr, arrBegin, med, arrEnd);
}

/**
 * @brief Removes duplicated elements from an array of integers. The elements
 * will preserve their original order and collapsed at the beginning of the
 * array.
 *
 * @param arr The array from which to remove duplicates.
 * @param len The length of the array.
 * @return int The new length of the array.
 */
int removeDups(int *arr, int len)
{
    int **arrPtr = (int **)malloc(len * sizeof(int *));
    int *dups = (int *)malloc(len * sizeof(int));
    int removed = 0;

    memset(dups, 0, len * sizeof(int));
    for (int i = 0; i < len; i++)
        arrPtr[i] = &arr[i];

    mergeSort(arrPtr, dups, arr, 0, len - 1);

    for (int i = 0; i < len; i++)
    {
        if (dups[i] == 1)
        {
            removed++;
            continue;
        }
        arr[i - removed] = arr[i];
    }

    free(arrPtr);
    free(dups);

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

/**
 * @brief Computes the binomial coefficient C(n, k) and returns a binary tree
 * representing the recursive computation.
 *
 * @param n
 * @param k
 * @return cBinTree*
 */
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

cBinTree *_cbinDP(int n, int k, cBinTree ***memo)
{
    if (memo[n][k] != 0)
        return memo[n][k];

    cBinTree *result = (cBinTree *)malloc(sizeof(cBinTree));
    memo[n][k] = result;
    result->n = n;
    result->k = k;

    if (n == k || k == 0)
    {
        result->binomial = 1;
        result->left = NULL;
        result->right = NULL;
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
    cBinTree ***memo = (cBinTree ***)malloc((n + 1) * sizeof(cBinTree **));

    for (int i = 0; i <= n; i++)
    {
        memo[i] = (cBinTree **)malloc((k + 1) * sizeof(cBinTree *));
        memset(memo[i], 0, (k + 1) * sizeof(cBinTree *));
    }

    cBinTree *result = _cbinDP(n, k, memo);
    for (int i = 0; i <= n; i++)
        free(memo[i]);
    free(memo);

    return result;
}

// 4. EULER SIEVE --------------------------------------------------------------

typedef struct
{
    int next;
    int prev;
} pair;

void removeMultiples(pair *result, int n, int next, int prime)
{
    int removeIndex = next * prime - 1;
    if (removeIndex >= n)
        return;

    pair toRemove = result[removeIndex];

    next += result[next - 1].next;

    // "Remove" the following prime number BEFORE adjusting the result,
    // otherwise we would lose the information about the next number to remove.
    removeMultiples(result, n, next, prime);

    // "Remove" the non-prime number
    result[removeIndex].prev = 0;
    result[removeIndex].next = 0;

    // Adjust the next and prev values
    if (removeIndex - toRemove.prev < n)
        result[removeIndex - toRemove.prev].next += toRemove.next;
    if (removeIndex + toRemove.next < n)
        result[removeIndex + toRemove.next].prev += toRemove.prev;
}

/**
 * @brief Computes the first n primes using the Euler sieve algorithm.
 *
 * @param n The target number of primes to compute.
 * @return pair* A list of pairs from which is possible to retrieve first n
 * primes sequence.
 */
pair *eulerSieve(int n)
{
    if (n < 2)
        return NULL;

    pair *result = (pair *)malloc(n * sizeof(pair));

    // Initialize the array (we could also initialize it with 1s, but this
    // avoids useless operations in the following loop).
    int nextPrev[] = {2, 0};
    for (int i = 0; i < n; i++)
    {
        result[i].next = nextPrev[i % 2];
        result[i].prev = nextPrev[i % 2];
    }
    result[0].next = 1;
    result[0].prev = 0;
    result[1].next = 1; // 2 is prime
    result[1].prev = 0;

    int prime = 3;
    while (prime * prime <= n)
    {
        removeMultiples(result, n, prime, prime);
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
    printf("\n");
}

void printPrimes(pair *pairs, int n)
{
    int i = 1;
    while (i < n)
    {
        printf("%d ", i + 1);
        i += pairs[i].next;
    }
    printf("\n");
}

int main(int arc, char **argv)
{
    int left;

    // My system is little endian, thus I'm checking for 1
    printf("1. TEST LITTLE ENDIAN\n");
    assert(isLittleEndian() == 1 && "FAIL");
    printf("OK\n");

    printf("2.1 TEST REMOVE DUPS, unique ordered\n");
    int expected[] = {1, 2, 3, 4, 5};
    int uniquesOrdered[] = {1, 2, 3, 4, 5};
    left = removeDups(uniquesOrdered, 5);
    assert(memcmp(uniquesOrdered, expected, sizeof(expected)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("2.2 TEST REMOVE DUPS, unique unordered\n");
    int expUniques[] = {4, 5, 3, 1, 2};
    int uniques[] = {4, 5, 3, 1, 2};
    left = removeDups(uniques, 5);
    assert(memcmp(uniques, expUniques, sizeof(expUniques)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("2.3 TEST REMOVE DUPS, unordered\n");
    int expDups[] = {4, 5, 3, 1, 2};
    int dups[] = {4, 4, 5, 3, 5, 1, 4, 2};
    left = removeDups(dups, 8);
    assert(memcmp(dups, expDups, sizeof(expDups)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("2.4 TEST REMOVE DUPS, all equals\n");
    int expEqs[] = {4};
    int eqs[] = {4, 4, 4, 4, 4, 4, 4, 4};
    left = removeDups(eqs, 8);
    assert(memcmp(eqs, expEqs, sizeof(expEqs)) == 0 && "FAIL");
    assert(left == 1 && "FAIL");
    printf("OK\n");

    printf("2.5 TEST REMOVE DUPS, unordered 2\n");
    int expDups2[] = {4, 8, 9, 1, 0};
    int dups2[] = {4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 9, 1, 1, 1, 1, 0, 0, 4, 4};
    left = removeDups(dups2, 21);
    assert(memcmp(dups2, expDups2, sizeof(expDups2)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("2.6 TEST REMOVE DUPS, unordered 3\n");
    int expDups3[] = {0, -1, 2, 78, 1, 4, 8, 9};
    int dups3[] = {0, -1, 2, 78, 1, -1, 0, 2, 4, 8, 9, -1, 2, 0};
    left = removeDups(dups3, 14);
    assert(memcmp(dups3, expDups3, sizeof(expDups3)) == 0 && "FAIL");
    assert(left == 8 && "FAIL");
    printf("OK\n");

    printf("3.1 TEST BINOMIAL, (5 3)\n");
    printCBinTree(cbin(5, 3));

    printf("3.2 TEST BINOMIAL, (5 3) DP\n");
    printCBinTree(cbinDP(5, 3));

    printf("4.1 TEST EULER SIEVE, 24\n");
    printPairs(eulerSieve(24), 24);

    printf("4.2 TEST EULER SIEVE, 120\n");
    printPrimes(eulerSieve(120), 120);

    printf("4.3 TEST EULER SIEVE, 5000\n");
    printPrimes(eulerSieve(5000), 5000);
}