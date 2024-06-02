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
/// in which: duplicates are removed (in the mean that they are not sorted but
/// instead put at the end of the list); the list to merge is a list of
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

void printArray(int A[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
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

int main(int arc, char **argv)
{
    int left;

    // My system is little endian, thus I'm checking for 1
    printf("1. TEST LITTLE ENDIAN\n");
    assert(isLittleEndian() == 1 && "FAIL");
    printf("OK\n");

    printf("2. TESTS REMOVE DUPS, unique ordered\n");
    int expected[] = {1, 2, 3, 4, 5};
    int uniquesOrdered[] = {1, 2, 3, 4, 5};
    left = removeDups(uniquesOrdered, 5);
    assert(memcmp(uniquesOrdered, expected, sizeof(expected)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("3. TESTS REMOVE DUPS, unique unordered\n");
    int expUniques[] = {4, 5, 3, 1, 2};
    int uniques[] = {4, 5, 3, 1, 2};
    left = removeDups(uniques, 5);
    assert(memcmp(uniques, expUniques, sizeof(expUniques)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("4. TESTS REMOVE DUPS, unordered\n");
    int expDups[] = {4, 5, 3, 1, 2};
    int dups[] = {4, 4, 5, 3, 5, 1, 4, 2};
    left = removeDups(dups, 8);
    assert(memcmp(dups, expDups, sizeof(expDups)) == 0 && "FAIL");
    assert(left == 5 && "FAIL");
    printf("OK\n");

    printf("5. TESTS REMOVE DUPS, all equals\n");
    int expEqs[] = {4};
    int eqs[] = {4, 4, 4, 4, 4, 4, 4, 4};
    left = removeDups(eqs, 8);
    assert(memcmp(eqs, expEqs, sizeof(expEqs)) == 0 && "FAIL");
    assert(left == 1 && "FAIL");
    printf("OK\n");
}