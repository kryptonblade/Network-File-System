#include <stdio.h>

// Function to write data to a file in the specified format
void writeToFile(const char *filename,char* request,char* ack,int id1,int id2) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        return;
    }

    fprintf(file, "%d;%s;%s;%d\n", id1, request, ack,id2);
    // fprintf(file, "%d;%s;%s\n", 2, "Request 2", "Ack 2");
    // fprintf(file, "%d;%s;%s\n", 3, "Request 3", "Ack 3");

    fclose(file);
}

// int main() {
//     // Write data to a file in the specified format
//     writeToFile("data.txt",copy,ack,indx1,indx2);

//     printf("Data has been written to the file.\n");

//     return 0;
// }