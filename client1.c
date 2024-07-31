#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 1024
#define PORT 8081

#define SA struct sockaddr
int A;

struct ip_port
{
    char ip[1024];
    in_port_t port;
};

struct file_info
{
    size_t size;
    mode_t permissions;
};

void func(int sockfd);
void func1(int sockfd, char *buff);
void func2(int sockfd, char *buff);
void func3(int sockfd, char *buff);
void connect_ss(in_port_t port, char *ip, char *buff);

void func(int sockfd)
{
    int n;
    for (;;)
    {
        printf("Enter the request : ");
        n = 0;
        A = 0;
        char buff[MAX];
        // memset(buff, 0, sizeof(buff));
        fgets(buff, sizeof(buff), stdin);
        buff[strlen(buff) - 1] = '\0';
        char *delim = " \n";
        char *dummy = (char *)malloc(sizeof(char) * MAX);
        char *dummy1 = (char *)malloc(sizeof(char) * MAX);
        strcpy(dummy, buff);
        strcpy(dummy1, buff);
        char *token = strtok(dummy, delim);
        printf("hi");
        if (strcmp(token, "READ") == 0)
        {
            A = 1;
        }
        else if (strcmp(token, "WRITE") == 0)
        {
            A = 2;
        }
        else if (strcmp(token, "FILEINFO") == 0)
        {
            A = 3;
        }
        printf("%s\n", buff);
        send(sockfd, buff, sizeof(buff), 0);
        memset(buff, 0, sizeof(buff));
        if (A != 0)
        {
            struct ip_port I;
            recv(sockfd, &I, sizeof(I), 0);
            printf("received");
            connect_ss(I.port, I.ip, dummy1);
        }
        else
        {
            recv(sockfd, buff, sizeof(buff), 0);
            if ((strcmp(buff, "ACK")) == 0)
            {
                printf("%s successful!\n", dummy1);
            }
            else
            {
                printf("error!!\n");
                return;
            }
        }
        if ((strcmp(buff, "exit")) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
}

void func1(int sockfd, char *buff)
{
    char buf[MAX];
    strcpy(buf, buff);
    // buf[strlen(buff)] = '\0';
    printf("%s\n", buf);
    ssize_t bytes_sent = send(sockfd, buf, sizeof(buf), 0);
    if (bytes_sent == -1)
    {
        perror("sending data failed");
        exit(1);
    }
    printf("data sent successfully to ss");

    // memset(buf, 0, sizeof(buf));
    for (;;)
    {
        char buffer[MAX];
        // memset(buffer, 0, sizeof(buffer));
        // memset(buffer, 0, sizeof(buffer));
        size_t bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        // send(sockfd,"HELLO",sizeof("HELLO"),0);
        buffer[bytes_received] = '\0';
        // printf("%ld\n",bytes_received);
        printf("%s", buffer);
        if (bytes_received == -1)
        {
            perror("receive failed");
            exit(-1);
        }
        if ((strcmp(buffer, "STOP")) == 0)
        {
            printf("%s", buffer);
            break;
        }
    }
    printf("\n");
}

void func2(int sockfd, char *buff)
{
    char buf[MAX];
    strcpy(buf, buff);
    send(sockfd, &buf, sizeof(buf), 0);
    memset(buf, 0, sizeof(buf));
    recv(sockfd, buf, sizeof(buf), 0);
    if ((strcmp(buf, "OK")) != 0)
    {
        printf("error!!\n");
        return;
    }
    printf("Write here:\n");
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    send(sockfd, buffer, sizeof(buffer), 0);
    memset(buf, 0, sizeof(buf));
    recv(sockfd, buf, sizeof(buf), 0);
    if ((strcmp(buf, "OK")) == 0)
    {
        printf("write successful!!\n");
    }
    else
    {
        printf("error!!\n");
    }
}

void func3(int sockfd, char *buff)
{
    char buf[MAX];
    strcpy(buf, buff);
    send(sockfd, buf, sizeof(buf), 0);
    memset(buf, 0, sizeof(buf));
    struct file_info fi;
    recv(sockfd, &fi, sizeof(fi), 0);
    printf("file size: %ld\n", fi.size);
    printf("file permissions: %d\n", fi.permissions);
}

void connect_ss(in_port_t port, char *ip, char *buff)
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    printf("port: %d ip: %s\n",port,ip);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
    {
        printf("connected to the server..\n");
    }
    // function for chat
    if (A == 1)
    {
        printf("hu\n");
        func1(sockfd, buff);
    }
    else if (A == 2)
    {
        func2(sockfd, buff);
    }
    else if (A == 3)
    {
        func3(sockfd, buff);
    }
    // close the socket
    close(sockfd);
    printf("ss socket closed\n");
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    A = 0;
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("1Socket successfully created..\n");
    memset(&servaddr, 0, sizeof(servaddr));

    // int port;
    // char ip[1024];
    // scanf("%d", &port);
    // fgets(ip,sizeof(ip),stdin);
    // ip[strlen(ip) - 1] = '\0';
    // scanf("%s", ip);
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("1connection with the server failed...\n");
        exit(0);
    }
    else
        printf("1connected to the server..\n");
    // send(sockfd, "clinet", sizeof("client"), 0);

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
    return 0;
}
