#include "headers.h"
// #include "client_req.c"
#include "operations.c"
#include "copy_intraSS.c"
#include "copy_interSS.c"
#include "test1.c"
#include "test.c"
#include <pthread.h>

#define MAX_LENGTH 1024
#define MAX_CLIENTS 100

struct Storage_Server
{
    int Sno;
    char ip[1024];
    in_port_t port_NS;
    in_port_t port_SS;
    char arr[1024];
};

typedef struct Storage_Server SS;

struct socket
{
    int server_socket;
    int client_socket;
};

char *NM_ip = "127.0.0.1";
int client_count = 0;

sem_t readCountMutex;
sem_t writeMutex;
int readCount = 0;

// struct socket sockets[MAX_CLIENTS];
SS storage_server;

// pthread_t naming_thread;
// pthread_t client_thread[MAX_CLIENTS];
// int strg_count = 0;

void *ns(void *args)
{
    int *nserver = (int *)args;
    int client_socket = *nserver;
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1)
    {
        perror("Receiving data failed");
        close(client_socket);
        // close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);
    char *operation = strtok(buffer, " \n");
    int success;
    if (strcmp(operation, "CREATE") == 0)
    {
        char *path = strtok(NULL, " \n");
        printf("%s\n", path);
        char *name = strtok(NULL, " \n");
        char NAME[1024];
        strcpy(NAME, name);
        printf("NAME      %s\n", name);
        char *file_path = path_concatenate(path);
        // printf("name %s file path %s\n path %s\n",NAME,file_path);
        success = create_file(file_path, NAME, 0);
        // printf("%d\n",success);
        send(client_socket, &success, sizeof(success), 0);
    }
    else if (strcmp(operation, "DELETE") == 0)
    {
        char *path = strtok(NULL, " \n");
        char *file_path = path_concatenate(path);
        success = delete_file(file_path);
        send(client_socket, &success, sizeof(success), 0);
    }
    else if (strcmp(operation, "COPY") == 0)
    {
        char *path = strtok(NULL, " \n");
        char *name = strtok(NULL, " \n");
        char NAME[1024];
        strcpy(NAME, name);
        printf("NAME      %s\n", name);
        char *token = strtok(NULL, " \n");
        char TOKEN[1024];
        strcpy(TOKEN, token);
        char *file_path = path_concatenate(path);
        char *destination = path_concatenate(NAME);
        printf("destination: %s\n", destination);
        printf("token: %s\n", TOKEN);
        printf("file_path: %s\n", file_path);
        if (strcmp(TOKEN, "FILE") == 0)
        {
            // printf("jj");
            success = copy_file(file_path, destination);
            // delete_file(path,name,0);
            // success = 0;
            send(client_socket, &success, sizeof(success), 0);
        }
        else if (strcmp(TOKEN, "DIR") == 0)
        {
            // printf("hh");
            // char *file_path = path_concatenate(path);
            // char *destination = path_concatenate(NAME);
            success = copy_directory(file_path, destination);

            // delete_file(path,name,0);
            // success = 0;
            send(client_socket, &success, sizeof(success), 0);
        }
    }
    else if (strcmp(operation, "BWSSREAD") == 0)
    {
        char path[1024];
        ssize_t bytes = recv(client_socket, path, sizeof(path), 0);
        if (bytes == -1)
        {
            perror("Receiving data failed");
            close(client_socket);
            // close(server_socket);
            exit(EXIT_FAILURE);
        }
        printf("bytes_received: %ld", bytes);
        // printf("koushik");
        printf("gegec :%s\n", path);
        // char *path = strtok(NULL, " \n");
        char *file_path = path_concatenate(path);
        char *rel_path = get_path_before_last_slash(file_path);
        printf("rel_path:%s\n", rel_path);

        checkPath(file_path, client_socket, rel_path);
        printf("1bwss read done\n");
        struct pathdata C;
        strcpy(C.path, "DONE");
        send(client_socket, &C, sizeof(C), 0);
        printf("2bwss read done\n");
    }
    else if (strcmp(operation, "BWSSWRITE") == 0)
    {
        while (1)
        {
            char path[1024];
            int flag;
            int bytes = recv(client_socket, path, sizeof(path), 0);
            if (bytes == -1)
            {
                perror("Receiving data failed");
                close(client_socket);
                // close(server_socket);
                exit(EXIT_FAILURE);
            }
            if (strcmp(path, "STOP") == 0)
            {
                break;
            }
            printf("pathhh: %s\n", path);
            bytes = recv(client_socket, &flag, sizeof(flag), 0);
            if (bytes == -1)
            {
                perror("Receiving data failed");
                close(client_socket);
                // close(server_socket);
                exit(EXIT_FAILURE);
            }
            // char *path = strtok(NULL, " \n");
            char *rel_path = path_concatenate(path);
            printf("rel_path: %s\n", rel_path);
            if (flag == 1)
            {
                write_file(rel_path, client_socket);
            }
            else if (flag == 2)
            {

                if (mkdir(rel_path, 0777) == 0)
                {
                    printf("Directory '%s' created successfully.\n", rel_path);
                }
                // else
                // {
                //     perror("Error creating directory");
                //     exit(EXIT_FAILURE);
                //     send(client_socket, "COMPLETED", sizeof("COMPLETED"), 0);
                // }
            }
        }
    }
    close(client_socket);
}

void *Naming_server()
{
    char server_ip[1024];
    strcpy(server_ip, storage_server.ip);
    int server_port = storage_server.port_NS;

    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(storage_server.ip);
    server_addr.sin_port = htons(storage_server.port_NS);

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Listening failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("TCP server is listening on %s:%d\n", server_ip, server_port);

    // Accept incoming connections
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_socket == -1)
        {
            perror("Accepting connection failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            pthread_t thread;
            pthread_create(&thread, NULL, ns, &client_socket);
        }
        // Receive data from the client
    }
    // connected_count++;

    // send(client_socket, &storage_server, sizeof(storage_server), 0);

    // close(client_socket);
}

void *Client(void *arg)
{
    // struct socket S = *(struct socket *)arg;
    // int client_socket = S.client_socket;
    // int server_socket = S.server_socket;
    int *client = (int *)arg;
    int client_socket = *client;
    // Receive data from the client
    char buffer[1024];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1)
    {
        perror("Receiving data failed");
        close(client_socket);
        // close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);

    // char *request = strtok(buffer, " \n");
    buffer[bytes_received] = '\0';
    char *path = strtok(buffer, " \n");
    if (strcmp(path, "READ") == 0)
    {
        path = strtok(NULL, " \n");
        char *full_path = path_concatenate(path);
        sem_wait(&readCountMutex);
        readCount++;
        if (readCount == 1)
        {
            // If this is the first reader, acquire the write mutex to block writers
            sem_wait(&writeMutex);
        }
        sem_post(&readCountMutex);
        read_file(full_path, client_socket);
        sem_wait(&readCountMutex);
        readCount--;
        if (readCount == 0)
        {
            // If this is the last reader, release the write mutex to allow writers
            sem_post(&writeMutex);
        }
        sem_post(&readCountMutex);
        char stop[1024];
        strcpy(stop, "STOP");
        ssize_t bytes_sent = send(client_socket, stop, strlen(stop), 0);
        printf("%ld", bytes_sent);
        if (bytes_sent == -1)
        {
            perror("send failed");
        }
        else
        {
            printf("READ SUCCESSFUL!!\n");
        }
    }
    else if (strcmp(path, "WRITE") == 0)
    {
        path = strtok(NULL, " \n");
        char *full_path = path_concatenate(path);
        char ok[1024];
        strcpy(ok, "OK");
        send(client_socket, ok, sizeof(ok), 0);
        sem_wait(&writeMutex);
        write_file(full_path, client_socket);
        sem_post(&writeMutex);
        send(client_socket, ok, sizeof(ok), 0);
        printf("WRITE SUCCESSFUL!!\n\n");
    }
    else if (strcmp(path, "FILEINFO") == 0)
    {
        path = strtok(NULL, " \n");
        char *full_path = path_concatenate(path);
        get_file_info(full_path, client_socket);
        // char ACK[1024];
        // strcpy(ACK, "ACK");
        // send(client_socket, ACK, sizeof(ACK), 0);
    }

    // Close sockets
    close(client_socket);
    // close(server_socket);
}

void *worker_thread(void *args)
{
    struct sockaddr_in server_addr;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the server address
    // struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(storage_server.ip);
    server_addr.sin_port = htons(storage_server.port_SS);

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Listening failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("TCP server is listening on %s:%d\n", storage_server.ip, storage_server.port_SS);
    while (1)
    {
        // sockets[client_count].server_socket = server_socket;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1)
        {
            perror("accepting connection failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        else
        {
            pthread_t thread;
            printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            pthread_create(&thread, NULL, Client, &client_socket);

            client_count++;
            printf("client_count:%d\n", client_count);
        }
    }
}

int main()
{

    int initial_n;
    int idx;
    if (sem_init(&readCountMutex, 0, 1) != 0 || sem_init(&writeMutex, 0, 1) != 0)
    {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    // printf("Initial number of storage severs:");
    scanf("%d %d", &idx, &initial_n);
    storage_server.Sno = idx;
    // printf("Enter the ip address:");
    scanf("%s", storage_server.ip);
    // printf("Enter Port to connect with Naming server:");
    scanf("%hd", &storage_server.port_NS);
    // printf("Enter Port to connect with Client:");
    scanf("%hd", &storage_server.port_SS);
    char path[MAX_LENGTH];
    printf("Enter %d file paths\n", initial_n);
    // accessible_paths = (char **)malloc(sizeof(char *) * initial_n);
    while (getchar() != '\n')
        ;
    char packet[1024];
    fgets(packet, sizeof(packet), stdin);
    strcpy(storage_server.arr, packet);

    // char *server_ip = NM_ip;
    int server_port = storage_server.port_NS;

    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address to connect
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // connected_count++;
    // send(client_socket, "ss", sizeof("ss"), 0);
    send(client_socket, &storage_server, sizeof(storage_server), 0);

    close(client_socket);

    pthread_t naming_thread;
    pthread_t client_thread;
    pthread_create(&naming_thread, NULL, Naming_server, NULL);
    pthread_create(&client_thread, NULL, worker_thread, NULL);

    pthread_join(naming_thread, NULL);
    pthread_join(client_thread, NULL);
    // for (int i = 0; i < client_count; i++)
    // {
    //     pthread_create(&client_thread[i], NULL, Client, &client_socket);

    //     pthread_join(client_thread[i], NULL);
    // }
    // close(server_socket);
    // sleep(1);
    //     }
    // }

    return 0;
}