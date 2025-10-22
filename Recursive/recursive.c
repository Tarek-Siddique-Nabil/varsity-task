#include <stdio.h>


void reverseSubarray(int A[], int L, int R) {
    if (L >= R) return;

    int temp = A[L];
    A[L] = A[R];
    A[R] = temp;
    reverseSubarray(A, L + 1, R - 1);
}


int main() {
    int n;
    printf("Enter array length: ");
    scanf("%d", &n);

    int A[n];
    printf("Enter array of numbers: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }  
    int L, R;
    printf("Enter the indices L and R: ");
    scanf("%d %d", &L, &R);

    reverseSubarray(A, L, R);
    for (int i = 0; i < n; i++) {
        printf("%d ", A[i]);
    }

    return 0;
}