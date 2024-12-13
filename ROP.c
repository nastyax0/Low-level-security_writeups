#include <stdio.h>
#include <string.h>

void secret_function() {
    printf("You have successfully called the secret function!\n");
}

void vulnerable_function(char *input) {
    char buffer[64];
    strcpy(buffer, input);  // This is unsafe, it doesn't check input size!
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input>\n", argv[0]);
        return 1;
    }

    vulnerable_function(argv[1]);
    printf("Returned from vulnerable function!\n");
    return 0;
}
