#include <stdio.h>

int searchRotated(int A[], int n, int x) {
    int left = 0, right = n - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        if (A[mid] == x)
            return mid;
        if (A[left] <= A[mid]) {
            if (x >= A[left] && x < A[mid])
                right = mid - 1;
            else
                left = mid + 1;
        }
        else {
            if (x > A[mid] && x <= A[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }

    return -1; // not found
}

int main() {
    int n;
    printf("Enter number of elements: ");
    scanf("%d", &n);

    int A[n];
    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }

    int x;
    printf("Enter element to search: ");
    scanf("%d", &x);

    int result = searchRotated(A, n, x);

    printf("Index of %d: %d\n", x, result);

    return 0;
}
