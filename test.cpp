#include <stdio.h>

int function1() {
    printf("function1 executed\n");
    return 1;  // Change this to 0 to see different behavior
}

int function2() {
    printf("function2 executed\n");
    return 1;
}

int main() {
    printf("Testing &&:\n");
    if (function1() && function2()) {
        printf("&& condition true\n");
    } else {
        printf("&& condition false\n");
    }

    printf("\nTesting ||:\n");
    if (function1() || function2()) {
        printf("|| condition true\n");
    } else {
        printf("|| condition false\n");
    }

    return 0;
}
