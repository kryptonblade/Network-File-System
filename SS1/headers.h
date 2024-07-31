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

// typedef struct Storage_Server SS;

// int strg_count;// int count = 0; // index of which storage server is being connected

typedef struct TrieNode *Trie;

struct TrieNode
{
    struct TrieNode *children[26];
    int ss_no;
    // int EndofWord;
};

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

void Insert_to_Trie(Trie T, char *str,int len)
{
    printf("&&%d&&",len);
    Trie temp = T;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != '/')
        {
            int idx = str[i] - 97;
            if (temp->children[idx] == NULL)
                temp->children[idx] = CreateNode();
            temp = temp->children[idx];
        }
    }
    temp->ss_no = len;
    // temp->EndofWord = 1;
}

int Search(Trie T, char *str)
{
    Trie temp = T;
    int indx;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != '/')
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
    return indx;
}

// void start_namingserver();
