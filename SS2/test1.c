#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* get_path_before_last_slash(char *file_path) {
    // Find the last occurrence of '/'
    const char *last_slash = strrchr(file_path, '/');

    if (last_slash != NULL) {
        // Calculate the length of the substring before the last '/'
        size_t length_before_last_slash = last_slash - file_path;

        // Allocate memory for the substring
        char *path_before_last_slash = malloc(length_before_last_slash + 1);

        // Copy the substring before the last '/' to the new buffer
        strncpy(path_before_last_slash, file_path, length_before_last_slash);
        
        // Null-terminate the string
        path_before_last_slash[length_before_last_slash] = '\0';

        return path_before_last_slash;
    } else {
        // No '/' found, handle error or return NULL
        printf("Error: No '/' found in the file path.\n");
        return NULL;
    }
}

// int main() {
//     const char *file_path = "/folder1/folder2/file.txt";

//     // Get the path before the last '/'
//     char *result = get_path_before_last_slash(file_path);

//     if (result != NULL) {
//         // Print the result
//         printf("Result: %s\n", result);

//         // Don't forget to free the allocated memory
//         free(result);
//     }

//     return 0;
// }
