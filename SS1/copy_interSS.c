#include "client_req.c"

struct pathdata
{
    char path[1024];
    int flag;
};

char *path_concatenate(char *path)
{
    char puff[1024];
    strcpy(puff, path);
    char currentdir[1024];
    if (getcwd(currentdir, sizeof(currentdir)) != NULL)
    {
        // printf("Current working directory: %s\n", currentdir);
    }
    // char copy[1024];
    strcat(currentdir, path);
    strcpy(path, currentdir);
    printf("Current path: %s\n", path);

    return path;
}

// Function to check if a given path is a file or directory
void checkPath(char *path, int client_socket, char *file_path);

// Recursive function to process directories
void processDirectory(const char *dirPath, int client_socket, char *rel_path);

// int main(int argc, char *argv[])
// {
//     if (argc != 2)
//     {
//         printf("Usage: %s <path>\n", argv[0]);
//         return 1;
//     }
//     int kk;
//     const char *path = argv[1];
//     checkPath(path, kk);

//     return 0;
// }

void checkPath(char *path, int client_socket, char *rel_path)
{

    struct stat path_stat;

    if (stat(path, &path_stat) != 0)
    {
        perror("Error in stat");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(path_stat.st_mode))
    {
        struct pathdata Chunk;
        char bluff[1024];
        int len = strlen(rel_path);
        strcpy(bluff, path + len);
        strcpy(Chunk.path, bluff);
        Chunk.flag = 1;
        printf("jh:%s\n", Chunk.path);
        send(client_socket, &Chunk, sizeof(Chunk), 0);
        read_file(path, client_socket);
        char stop[1024];
        strcpy(stop,"STOP");
        send(client_socket, stop, sizeof(stop), 0);
        printf("%s is a regular file.\n", path);
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
        struct pathdata Chunk;
        char bluff[1024];
        int len = strlen(rel_path);
        strcpy(bluff, path + len);
        strcpy(Chunk.path, bluff);
        Chunk.flag = 2;
        send(client_socket, &Chunk, sizeof(Chunk), 0);
        printf("%s is a directory.\n", path);
        processDirectory(path, client_socket, rel_path);
    }
    // else
    // {
    //     printf("%s is neither a file nor a directory.\n", path);
    // }
}

void processDirectory(const char *dirPath, int client_socket, char *rel_path)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dirPath)) == NULL)
    {
        perror("Error in opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // Construct the full path of the entry within the directory
            char entryPath[1024];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", dirPath, entry->d_name);

            // Check if the entry is a file or directory
            checkPath(entryPath, client_socket, rel_path);
        }
    }

    closedir(dir);
}

// #endif
