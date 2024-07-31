#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *substringAfterLastSlash(const char *path) {
    const char *lastSlash = strrchr(path, '/');

    if (lastSlash != NULL) {
        // Calculate the length of the substring after the last slash
        size_t length = strlen(lastSlash + 1);

        // Allocate memory for the substring
        char *result = (char *)malloc((length + 1) * sizeof(char));

        if (result != NULL) {
            // Copy the substring after the last slash
            strcpy(result, lastSlash + 1);
            return result;
        } else {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }
    } else {
        // No slash found, return the entire path
        return strdup(path);
    }
}

// int main() {
//     const char *path = "/home/krishna/kk/sudhan/vamc";
    
//     // Example usage
//     char *result = substringAfterLastSlash(path);
//     printf("Substring after last slash: %s\n", result);

//     // Remember to free the allocated memory
//     free(result);

//     return 0;
// }
