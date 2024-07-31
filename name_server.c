#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "lr.h"
#include "bk.c"
// #include "headers.h"

#define MAX_SS 10

typedef struct TrieNode *Trie;

LRUCache *lruCache;

pthread_mutex_t readlock;
// struct Node *head = NULL;

struct bwss
{
    char path[1024];
    int file;
};

struct TrieNode
{
    struct TrieNode *children[26];
    int ss_no;
    // int EndofWord;
};

Trie T;
Trie CreateNode()
{
    Trie T = (Trie)malloc(sizeof(struct TrieNode));
    for (int i = 0; i < 26; i++)
    {
        T->children[i] = NULL;
        T->ss_no = -1;
    }
    // T->EndofWord = 0;
    // T->children[letter - 97]->EndofWord = 0;
    return T;
}

void Insert_to_Trie(Trie T, char *str, int indx)
{
    printf("%d\n", indx);
    Trie temp = T;
    for (int i = 0; i < strlen(str); i++)
    {
        // printf("%d",i);
        if (str[i] != '/' && str[i] !='.')
        {
            int idx = str[i] - 97;
            if (temp->children[idx] == NULL)
                temp->children[idx] = CreateNode();
            temp = temp->children[idx];
        }
    }
    temp->ss_no = indx;
    printf("%d", temp->ss_no);
    printf("hi\n");
    // temp->EndofWord = 1;
}

int Search(Trie T, char *str)
{
    Trie temp = T;
    int indx;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != '/' && str[i] !='.')
        {
            int idx = str[i] - 97;
            if (temp->children[idx] != NULL)
            {
                temp = temp->children[idx];
            }
            else
            {
                return -1;
            }
        }
    }
    indx = temp->ss_no;
    // printf("search : %d",temp->ss_no);
    return indx;
}

int isLeafNode(struct TrieNode *node)
{
    for (int i = 0; i < 26; i++)
    {
        if (node->children[i])
        {
            return 0;
        }
    }
    return 1;
}

int deleteNode(struct TrieNode *root, const char *word)
{
    if (root == NULL || *word == '\0')
    {
        printf("temp");
        // printf("%s",word);
        return -1;
    }

    struct TrieNode *node = root;
    struct TrieNode *parent = NULL;

    // Traverse the Trie to find the node corresponding to the last character of the word
    while (*word)
    {
        if (*word != '/' && *word !='.')
        {
            int index = *word - 'a';
            if (!node->children[index])
            {

                printf("file_path does not exist in the Trie\n"); // Word does not exist in the Trie
                return -1;
            }
            parent = node;
            node = node->children[index];
        }
        word++;
    }

    // If the node is the end of the word, mark it as not the end and return if it has no children
    if (node->ss_no + 1)
    {
        node->ss_no = -1;
        if (isLeafNode(node))
        {
            free(node);
            parent->children[*word - 'a'] = NULL;
        }
        printf("delete successfull\n");
        return 0;
    }
    printf("file_path does not exist in the Trie\n");
    return -1; // Word does not exist in the Trie
}

// void start_namingserver();

typedef struct storage_server
{
    // ip address
    int sno;
    char ip[1024];
    in_port_t nm_port;
    in_port_t client_port;
    char arr[1024];
} ss;

// int sockets[MAX_SS];

struct sent_to_client
{
    char ip[1024];
    in_port_t client_port;
};

// socklen_t client_addr_len;
// int server_socket;

struct client_socket_info
{
    int sock;
    struct sockaddr_in client_addr;
};

struct nm_ss
{
    char ip[1024];
    int client_port;
    char path[1024];
};

ss storage_server[MAX_SS];

int num_ss = 0;

int status = 0;

int storage_ser[MAX_SS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int server_socket1;

void *server_initalization(void *args)
{
    // printf("hi");
    int *client = (int *)args;
    int client_socket = *client;
    ss buffer;
    ssize_t bytes_received = recv(client_socket, &buffer, sizeof(buffer), 0);
    if (bytes_received == -1)
    {
        perror("Receiving data failed");
        close(client_socket);
        // close(server_socket);
        exit(1);
    }
    buffer.arr[strlen(buffer.arr) - 1] = '\0';
    storage_server[num_ss].client_port = buffer.client_port;
    storage_server[num_ss].nm_port = buffer.nm_port;
    strcpy(storage_server[num_ss].arr, buffer.arr);
    strcpy(storage_server[num_ss].ip, buffer.ip);
    storage_server[num_ss].sno = buffer.sno;
    // printf("%d\n", buffer.nm_port);
    // printf("%d\n", buffer.sno);
    // printf("%d\n", buffer.client_port);
    // printf("%s\n", buffer.arr);
    // printf("%s\n", buffer.ip);
    char *copy = (char *)malloc(sizeof(char) * 1024);
    strcpy(copy, buffer.arr);
    char *ptr;
    char *token = (char *)malloc(sizeof(token) * 1024);
    token = __strtok_r(copy, " \n", &ptr);
    while (token != NULL)
    {
        // printf("%s\n",token);
        printf("%d %s\n", num_ss, token);
        Insert_to_Trie(T, token, num_ss);
        token = __strtok_r(NULL, " \n", &ptr);
    }
    pthread_mutex_lock(&mutex);
    storage_ser[num_ss] = num_ss;
    num_ss++;
    pthread_mutex_unlock(&mutex);
    // pthread_t threa[2];
    // pthread_create(threa[0],NULL,,NULL);
    // pthread_create(threa[1],NULL,,NULL);
    close(client_socket);
}

void *worker_thread(void *args)
{
    int *client = (int *)args;
    int client_socket = *client;
    while (1)
    {
        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        printf("%ld", bytes_received);
        if (bytes_received == -1)
        {
            perror("Receiving data failed");
            // close(client_socket);
            close(server_socket1);
            exit(1);
        }
        if (strcmp(buffer, "EXIT") == 0)
        {
            break;
        }
        printf("Received data from client: %s\n", buffer);
        char *copy = (char *)malloc(sizeof(char) * 1024);
        strcpy(copy, buffer);
        // copy[strlen(copy) - 1] = '\0';
        char *ptr;
        char *token = (char *)malloc(sizeof(char) * 1024);
        token = __strtok_r(buffer, " \n", &ptr);
        int flag = 0;
        // int x, y = 0;
        int k = 0;
        char *ip1 = (char *)malloc(sizeof(char) * 1024);
        // char *ip2 = (char *)malloc(sizeof(char) * 1024);
        int indx1 = -1, indx2 = -1;
        char path1[1024];
        char path2[1024];
        char path3[1024];
        in_port_t portt;
        Node *var;
        int lru = 0, lru1 = 0;
        int sgnl = 0;
        char temp[1024];
        int errorcode = 0;
        if (strcmp(token, "READ") == 0 || strcmp(token, "WRITE") == 0 || strcmp(token, "FILEINFO") == 0)
        {
            // printf("hi");
            token = __strtok_r(NULL, " \n", &ptr);
            if ((var = getNode(lruCache, token)) == NULL)
            {
                indx1 = Search(T, token);
                if ((indx1) >= 0)
                {
                    // portt = storage_server[indx1].client_port;
                    // strcpy(ip1, storage_server[indx1].ip);
                    strcpy(path1, token);
                    strcpy(path2, token);
                    flag = 1;
                }
                else
                {
                    errorcode = 504;
                }
            }
            else
            {
                lru = 1;
                indx1 = var->intValue;
                flag = 1;
            }
        }
        else if (strcmp(token, "COPY") == 0 || strcmp(token, "DELETE") == 0 || strcmp(token, "CREATE") == 0)
        {
            flag = 2;
            if (strcmp(token, "COPY") == 0)
            {
                k = 1;
            }
            else if (strcmp(token, "DELETE") == 0)
            {
                k = 2;
            }
            else if (strcmp(token, "CREATE") == 0)
            {
                k = 3;
            }
            // printf("hello");
            token = __strtok_r(NULL, " \n", &ptr);
            // printf("%s", token);
            if ((var = getNode(lruCache, token)) == NULL)
            {
                indx1 = Search(T, token);
                if (indx1 >= 0)
                {
                    strcpy(path3, token);
                    if (k == 1)
                    {
                        token = __strtok_r(NULL, " \n", &ptr);
                        Node *ecv;
                        if ((ecv = getNode(lruCache, token)) == NULL)
                        {
                            // printf("path3: %s\n", path3);
                            indx2 = Search(T, token);
                            if (indx2 >= 0)
                            {
                                strcpy(path2, token);
                                // printf("**%s**\n", path2);
                                if (indx1 == indx2)
                                {
                                    sgnl = 1;
                                }
                                else
                                {
                                    sgnl = 2;
                                }
                            }
                            else
                            {
                                errorcode = 405;
                            }
                        }
                        else
                        {
                            lru1 = 1;
                            indx1 = var->intValue;
                            // flag = 2;
                        }
                    }
                    if (sgnl != 2)
                    {
                        strcpy(path1, copy);
                    }
                }
                else
                {
                    errorcode = 404;
                }
                // flag = 2;
            }
            else
            {
                lru = 1;
                indx1 = var->intValue;
                // flag = 2;
            }
            // flag = 2;
            // printf("index1: %d\n", indx1);
            // printf("index2: %d\n", indx2);
        }
        int pack = -1;
        char response[1024];
        
        if (flag == 0)
        {

            // error file not found
            struct sent_to_client ip_add;
            // ip_add.ip = NULL;
            strcpy(response,"STP");
            ip_add.client_port = 505;
            const char *response = "process is not successful";
            ssize_t bytes_sent = send(client_socket, &ip_add, sizeof(ip_add), 0);
            if (bytes_sent == -1)
            {
                perror("Sending data failed");
                // close(client_socket);
                close(server_socket1);
                exit(1);
            }

            // error code invalid command
        }
        else if (flag == 1) // read and write and fileinfo   by clients
        {
            // printf("hi");

            struct sent_to_client ip_add;
            if (errorcode == -1)
            {
                strcpy(ip_add.ip, storage_server[indx1].ip);
                ip_add.client_port = storage_server[indx1].client_port;
                // strcpy(ip_add.path, path1);
                strcpy(response,"ACK");

                printf("client: %d ip: %s %d\n", client_socket, ip_add.ip, ip_add.client_port);
            }
            else
            {
                strcpy(response,"STP");
                ip_add.client_port = errorcode;
            }
            ssize_t bytes_sent = send(client_socket, &ip_add, sizeof(ip_add), 0);
            printf("%ld", bytes_sent);
            if (bytes_sent == -1)
            {
                perror("Sending data failed");
                // close(client_socket);
                close(server_socket1);
                exit(1);
            }
            // char *ack_pack = (char *)malloc(sizeof(ack_pack) * 1024);
            // bytes_received = recv(client_socket, ack_pack, sizeof(ack_pack), 0);
            // if (bytes_received == -1)
            // {
            //     perror("Receiving data failed");
            //     close(client_socket);
            //     close(server_socket);
            //     exit(1);
            // }
            // close(client_socket);
            // // check successful or not if(strcmp())
            // printf("Received data from client: %s\n", ack_pack);

            // check successful or not if(strcmp())
            // printf("Received data from client: %s\n", ack_pack);
        }
        else if (flag == 2) // create and delete and intra copy by nm itself
        {
            if (errorcode == 0)
            {
                // printf("indx1: %d", indx1);
                struct sockaddr_in server_addr, client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                // Create socket
                int server_sock = socket(AF_INET, SOCK_STREAM, 0);
                if (server_sock == -1)
                {
                    perror("Socket creation failed");
                    exit(1);
                }

                // Initialize server address
                memset(&server_addr, 0, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(storage_server[indx1].nm_port);
                server_addr.sin_addr.s_addr = inet_addr(storage_server[indx1].ip);
                printf("%s %d\n", storage_server[indx1].ip, storage_server[indx1].nm_port);
                if (connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
                {
                    perror("Connection failed");
                    close(client_socket);
                    exit(EXIT_FAILURE);
                }
                // printf("HI");

                if (sgnl != 2)
                {
                    ssize_t bytes_sent = send(server_sock, path1, sizeof(path1), 0);
                    if (bytes_sent == -1)
                    {
                        perror("Sending data failed");
                        close(client_socket);
                        close(server_socket1);
                        exit(1);
                    }
                    // printf("path3: %s\n", path3);
                    // printf("*%ld*", bytes_sent); // char *ack_pack = (char *)malloc(sizeof(ack_pack) * 1024);
                    bytes_received = recv(server_sock, &pack, sizeof(pack), 0);
                    if (bytes_received == -1)
                    {
                        perror("Receiving data failed");
                        close(client_socket);
                        close(server_socket1);
                        exit(1);
                    }
                    printf("pack: %d\n", pack);
                    printf("flag : %d\n", flag);
                }
                else if (sgnl == 2)
                {
                    struct sockaddr_in server_addr2, client_addr2;
                    socklen_t client_addr_len2 = sizeof(client_addr2);
                    // Create socket
                    int server_sock2 = socket(AF_INET, SOCK_STREAM, 0);
                    if (server_socket1 == -1)
                    {
                        perror("Socket creation failed");
                        exit(1);
                    }

                    // Initialize server address
                    memset(&server_addr2, 0, sizeof(server_addr2));
                    server_addr2.sin_family = AF_INET;
                    server_addr2.sin_port = htons(storage_server[indx2].nm_port);
                    server_addr2.sin_addr.s_addr = inet_addr(storage_server[indx2].ip);
                    printf("%s %d\n", storage_server[indx2].ip, storage_server[indx2].nm_port);
                    if (connect(server_sock2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) == -1)
                    {
                        perror("Connection failed");
                        close(client_socket);
                        exit(EXIT_FAILURE);
                    }
                    // printf("path1: %s\n", path1);
                    // printf("*%ld*", bytes_sent);
                    char buf[1024];
                    strcpy(buf, "BWSSREAD");
                    ssize_t bytes_sent = send(server_sock, buf, sizeof(buf), 0);
                    if (bytes_sent == -1)
                    {
                        perror("Sending data failed");
                        close(client_socket);
                        close(server_socket1);
                        exit(1);
                    }
                    bytes_sent = send(server_sock, path3, sizeof(path3), 0);
                    if (bytes_sent == -1)
                    {
                        perror("Sending data failed");
                        close(client_socket);
                        close(server_socket1);
                        exit(1);
                    }
                    printf("source: %s\n", path3);
                    if (bytes_sent == -1)
                    {
                        perror("Sending data failed");
                        close(client_socket);
                        close(server_socket1);
                        exit(1);
                    }
                    char bwswrite[1024];
                    strcpy(bwswrite, "BWSSWRITE");
                    bytes_sent = send(server_sock2, bwswrite, sizeof(bwswrite), 0);
                    if (bytes_sent == -1)
                    {
                        perror("Sending data failed");
                        close(client_socket);
                        close(server_socket1);
                        exit(1);
                    }
                    while (1)
                    {
                        struct bwss buffer;
                        bytes_received = recv(server_sock, &buffer, sizeof(buffer), 0);
                        if (bytes_received == -1)
                        {
                            perror("Receiving data failed");
                            close(client_socket);
                            close(server_socket1);
                            exit(1);
                        }
                        if (strcmp(buffer.path, "DONE") == 0)
                        {
                            char ackk[1024];
                            strcpy(ackk, "STOP");
                            bytes_received = send(server_sock2, ackk, sizeof(ackk), 0);
                            if (bytes_received == -1)
                            {
                                perror("Receiving data failed");
                                close(client_socket);
                                close(server_socket1);
                                exit(1);
                            }
                            pack = 0;
                            break;
                        }
                        printf("path2: %s\n", path2);
                        printf("buffer_path: %s\n", buffer.path);
                        // Insert_to_Trie(T)
                        char bluff[1024];
                        strcpy(bluff, path2);
                        strcat(bluff, buffer.path);
                        // strcpy(buffer.path,bluff);
                        printf("destination: %s\n", bluff);
                        Insert_to_Trie(T,bluff,indx2);
                        // struct bwss buffer2;
                        if (buffer.file == 2)
                        {
                            bytes_received = send(server_sock2, bluff, sizeof(bluff), 0);
                            if (bytes_received == -1)
                            {
                                perror("Receiving data failed");
                                close(client_socket);
                                close(server_socket1);
                                exit(1);
                            }
                            bytes_received = send(server_sock2, &buffer.file, sizeof(buffer.file), 0);
                            if (bytes_received == -1)
                            {
                                perror("Receiving data failed");
                                close(client_socket);
                                close(server_socket1);
                                exit(1);
                            }
                        }
                        else
                        {
                            bytes_sent = send(server_sock2, bluff, sizeof(bluff), 0);
                            if (bytes_sent == -1)
                            {
                                perror("sending data failed");
                                close(client_socket);
                                close(server_socket1);
                                exit(1);
                            }
                            bytes_sent = send(server_sock2, &buffer.file, sizeof(buffer.file), 0);
                            if (bytes_sent == -1)
                            {
                                perror("sending data failed");
                                close(client_socket);
                                close(server_socket1);
                                exit(1);
                            }
                            while (1)
                            {
                                char buffer2[1024];
                                bytes_received = recv(server_sock, buffer2, sizeof(buffer2), 0);
                                if (bytes_received == -1)
                                {
                                    perror("Receiving data failed");
                                    close(client_socket);
                                    close(server_socket1);
                                    exit(1);
                                }
                                if (strcmp(buffer2, "STOP") == 0)
                                {
                                    // printf("kk kart");
                                    char ack[1024];
                                    strcpy(ack, "STOP");
                                    bytes_sent = send(server_sock2, ack, sizeof(ack), 0);
                                    if (bytes_sent == -1)
                                    {
                                        perror("sending data failed");
                                        close(client_socket);
                                        close(server_socket1);
                                        exit(1);
                                    }
                                    break;
                                }
                                bytes_sent = send(server_sock2, buffer2, sizeof(buffer2), 0);
                                if (bytes_sent == -1)
                                {
                                    perror("sending data failed");
                                    close(client_socket);
                                    close(server_socket1);
                                    exit(1);
                                }
                            }
                        }
                    }
                    close(server_sock2);
                }
                close(server_sock);
            }
            // check successful or not if(strcmp())
            // printf("Received data from client: %s\n", ack_pack);
        }
        printf("flag : %d\n", flag);

        if (flag != 0)
        {
            if (lru != 1)
            {
                put(lruCache, path2, indx1);
            }
            if (flag == 2)
            {
                if (lru1 != 1)
                {
                    put(lruCache, path2, indx2);
                }
            }
        }
        printf("lru: %d\n", lru);
        // printf("flag2 : %d\n", flag);
        // Send response to the client
        if (flag == 2)
        {
            // printf("kkk");
            if (pack == 0)
            {
                // printf("jjkkkss");
                if (k == 3)
                {
                    Insert_to_Trie(T, path3, indx1);
                }
                else if (k == 2)
                {
                    // printf("kokokos");
                    deleteNode(T, path3);
                }

                strcpy(response , "ACK");
                ssize_t bytes_sent = send(client_socket, response, strlen(response), 0);
                if (bytes_sent == -1)
                {
                    perror("Sending data failed");
                    close(client_socket);
                    // close(client_data->client_socket);
                    close(server_socket1);
                    exit(1);
                }
                // errorcode = 200;
                // errorcode = 200;
                // recv
            }
            else
            {
                strcpy(response,"STP");
                ssize_t bytes_sent = send(client_socket, response, strlen(response), 0);
                if (bytes_sent == -1)
                {
                    perror("Sending data failed");
                    // close(client_data->client_socket);
                    close(server_socket1);
                    exit(1);
                }
                bytes_sent = send(client_socket, &errorcode, sizeof(errorcode), 0);
                if (bytes_sent == -1)
                {
                    perror("Sending data failed");
                    close(client_socket);
                    // close(client_data->client_socket);
                    close(server_socket1);
                    exit(1);
                }
            }

            printf("flag : %d\n", flag);
        }
        writeToFile("data.txt",copy,response,indx1,indx2);

        // insertNodeAtHead(&head,,copy,);
    }
    close(client_socket);
}

void *client_thread(void *args)
{
    // struct client_socket_info *client_data = (struct client_socket_info *)args;
    // Create socket
    server_socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket1 == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }
    struct sockaddr_in server_addr;
    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket
    int opt = 1;
    setsockopt(server_socket1, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(server_socket1, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        close(server_socket1);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket1, 5) == -1)
    {
        perror("Listening failed");
        close(server_socket1);
        exit(1);
    }

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket1, (struct sockaddr *)&client_addr, &client_addr_len);
        printf("client: %d\n", client_socket);
        if (client_socket == -1)
        {
            perror("accepting failed");
            exit(1);
        }
        else
        {
            pthread_t thread;
            pthread_create(&thread, NULL, worker_thread, &client_socket);
        }
        // int *client_socket = (int *)args;

        // close(client_socket);
    }
    close(server_socket1);
}

void *ss_thread(void *args)
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("kkBinding failed");
        close(server_socket);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Listening failed");
        close(server_socket);
        exit(1);
    }

    while (1)
    {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        // int *clinet_socket = (int *)args;
        if (client_socket != -1)
        {
            pthread_t thread;
            pthread_create(&thread, NULL, &server_initalization, &client_socket);
        }
    }
    close(server_socket);
}

int main()
{
    T = (Trie)malloc(sizeof(struct TrieNode));
    lruCache = createLRUCache(3);
    // int client_socket;
    // struct sockaddr_in server_addr, client_addr;
    // socklen_t client_addr_len = sizeof(client_addr);
    // // Create socket
    // int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // if (server_socket == -1)
    // {
    //     perror("Socket creation failed");
    //     exit(1);
    // }

    // // Initialize server address
    // memset(&server_addr, 0, sizeof(server_addr));
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(8080);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // // Bind the socket
    // int opt = 1;
    // setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    // {
    //     perror("jBinding failed");
    //     close(server_socket);
    //     exit(1);
    // }

    // // Listen for incoming connections
    // if (listen(server_socket, 5) == -1)
    // {
    //     perror("Listening failed");
    //     close(server_socket);
    //     exit(1);
    // }

    // for (int i = 0; i < 1; i++)
    // {
    //     client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

    //     if (client_socket == -1)
    //     {
    //         perror("Accepting client connection failed");
    //         close(server_socket);
    //         exit(1);
    //     }
    //     server_initalization(client_socket);
    //     // printf("%s nm: %d cp: %d", storage_server[i].arr, storage_server[i].nm_port, storage_server[i].client_port);
    //     // close(client_socket);
    //     // Insert_to_Trie( storage_server->itrie received_node;
    // }
    // close(server_socket);
    // num_ss = 2;
    pthread_mutex_init(&mutex, NULL);
    // while (1)
    // {
    //     // printf("hi");
    //     client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    //     if (client_socket == -1)
    //     {
    //         perror("Accepting client connection failed");
    //         close(server_socket);
    //         exit(1);
    //     }
    //     else
    //     {
    //         char buffer[1024];
    //         ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    //         if (bytes_received == -1)
    //         {
    //             perror("Receiving data failed");
    //             close(client_socket);
    //             close(server_socket);
    //             exit(1);
    //         }
    //         pthread_t th_data;
    //         if (strcmp(buffer, "ss") == 0)
    //         {
    //         }
    //         else if (strcmp(buffer, "client") == 0)
    //         {
    //         }
    //     }
    //     close(client_socket);
    // }

    pthread_t th_data[2];
    pthread_create(&th_data[0], NULL, client_thread, NULL);
    pthread_create(&th_data[1], NULL, ss_thread, NULL);

    pthread_join(th_data[0], NULL);
    pthread_join(th_data[1], NULL);

    // // Close sockets (not reached in this example)
    // close(client_socket);
    // close(server_socket);

    return 0;
}
