#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <semaphore.h>

#define MAX_BUFFER_SIZE 1024

struct FileInfo
{
    size_t size;
    mode_t permissions;
};



void read_file(char *file_path, int client_socket)
{
    FILE *file = fopen(file_path, "r");
    // printf("lkjhgfd:%s\n", file_path);

    if (!file)
    {
        char error[1024];
        strcpy(error, "404");
        send(client_socket, error, sizeof(error), 0);
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t bytesRead;

    // Flush stdout to ensure any buffered data is printed before reading from the file
    fflush(stdout);

    while ((bytesRead = fread(buffer, 1, sizeof(buffer) - 1, file)) > 0)
    {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer

        printf("\nbuffers_read: %s\n\n\n", buffer);
        if (send(client_socket, buffer, sizeof(buffer), 0) == -1)
        {

            perror("Error sending data to client");
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    fclose(file);
    // printf("hi\n");
}

// int main() {
//     // Example usage
//     read_file("/path/to/your/file.txt");

//     return 0;
// }

void write_file(char *file_path, int client_socket)
{
    printf("path:%s\n", file_path);
    FILE *file = fopen(file_path, "w"); // Open the file in binary write mode
    if (!file)
    {
        char error[1024];
        strcpy(error, "404");
        send(client_socket, error, sizeof(error), 0);
        perror("jjError opening file");
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead;

    while (1)
    {
        char buffer[MAX_BUFFER_SIZE];
        bytesRead = recv(client_socket, buffer, sizeof(buffer), 0); //) > 0)                                        // {
        // buffer[bytesRead - 1] = '\0';
        printf("printing part:%s\n", buffer);
        // Process the received data
        if (strcmp(buffer, "STOP") == 0)
        {
            break;
        }
        else
            fprintf(file, "%s", buffer);
    }
    // }

    fclose(file);

    // Close sockets
    // close(client_socket);
    // close(server_socket);
}

void get_file_info(const char *file_path, int client_socket)
{

    // struct FileInfo file_info = query_file_info(file_path);
    struct FileInfo file_info;
    memset(&file_info, 0, sizeof(struct FileInfo));

    // Get file information
    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0)
    {
        file_info.size = (size_t)file_stat.st_size;
        file_info.permissions = file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    }
    else
    {
        file_info.permissions = 404;
        file_info.size = 404;
        // send(client_count, &file_info, sizeof(file_info), 0);
        perror("Error getting file information");
    }
    // Send file information to the client
    send(client_socket, &file_info, sizeof(struct FileInfo), 0);
    // }

    // close(client_socket);
    // close(server_socket);
}

// int main()
// {
//     char *server_ip = "127.0.0.1";
//     int port = 12345;
//     char *path = "/home/krishna/my_bitcoin.txt";
//     read_file(server_ip,port,path);

//     return 0;
// }