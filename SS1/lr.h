#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure for doubly linked list
typedef struct Node
{
    char *value1;
    // char* value2;
    int intValue;
    struct Node *prev;
    struct Node *next;
} Node;

// LRUCache structure
typedef struct LRUCache
{
    int capacity;
    int size;
    Node *head;
    Node *tail;
} LRUCache;

// Function to create a new node
Node *createNode(const char *value1, int intValue)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->value1 = strdup(value1); // Duplicate the input value1 string
    // newNode->value2 = strdup(value2); // Duplicate the input value2 string
    newNode->intValue = intValue;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize LRUCache
LRUCache *createLRUCache(int capacity)
{
    LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    return cache;
}

void removeNode(LRUCache *cache, const char *file_path)
{
    // Traverse the linked list to find the node with the given stringValue
    Node *current = cache->head;
    while (current != NULL)
    {
        if (strcmp(current->value1, file_path) == 0)
        {
            // Remove the node from the linked list
            if (current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            else
            {
                cache->head = current->next;
            }

            if (current->next != NULL)
            {
                current->next->prev = current->prev;
            }
            else
            {
                cache->tail = current->prev;
            }

            // Free the memory of the removed node, including the string
            free(current->value1);
            free(current);

            // Decrement the size of the cache
            cache->size--;

            return;
        }
        current = current->next;
    }

    // stringValue not found
}

// Function to move a node to the front of the linked list
void moveToHead(LRUCache *cache, Node *node)
{
    if (node == cache->head)
    {
        return; // Already at the front
    }

    // Remove the node from its current position
    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }

    // Update head and tail pointers
    if (node == cache->tail)
    {
        cache->tail = node->prev;
    }

    // Move the node to the front
    node->next = cache->head;
    node->prev = NULL;
    if (cache->head != NULL)
    {
        cache->head->prev = node;
    }
    cache->head = node;

    // Update tail pointer if needed
    if (cache->tail == NULL)
    {
        cache->tail = node;
    }
}

// Function to evict the least recently used node
void evictLRU(LRUCache *cache)
{
    if (cache->tail == NULL)
    {
        return; // Cache is empty
    }

    Node *lruNode = cache->tail;

    // Remove the LRU node from the linked list
    if (lruNode->prev != NULL)
    {
        lruNode->prev->next = NULL;
    }
    else
    {
        cache->head = NULL;
    }

    // Update the tail pointer
    cache->tail = lruNode->prev;

    // Free the memory of the evicted node, including the strings
    free(lruNode->value1);
    // free(lruNode->value2);
    free(lruNode);

    // Decrement the size of the cache
    cache->size--;
}

// Function to get a node pointer from the cache based on value1
Node *getNode(LRUCache *cache, const char *value1)
{
    // Traverse the linked list to find the node with the given value1
    Node *current = cache->head;
    while (current != NULL)
    {
        if (strcmp(current->value1, value1) == 0)
        {
            // Move the accessed node to the front
            moveToHead(cache, current);
            return current;
        }
        current = current->next;
    }

    // Value1 not found
    return NULL;
}

// Function to put key-values in the cache
void put(LRUCache *cache, const char *value1, int intValue)
{
    // Check if the node with value1 is already present in the cache
    Node *existingNode = getNode(cache, value1);
    if (existingNode != NULL)
    {
        // Update the values and move the node to the front
        // free(existingNode->value2);
        // existingNode->value2 = strdup(value2); // Duplicate the new value2 string
        existingNode->intValue = intValue;
        moveToHead(cache, existingNode);
        return;
    }

    // Create a new node for the key-values
    Node *newNode = createNode(value1, intValue);

    // Add the new node to the front of the linked list
    newNode->next = cache->head;
    if (cache->head != NULL)
    {
        cache->head->prev = newNode;
    }
    cache->head = newNode;

    // Update the tail pointer if needed
    if (cache->tail == NULL)
    {
        cache->tail = newNode;
    }

    // Increment the size of the cache
    cache->size++;

    // Evict the least recently used node if the cache is full
    if (cache->size > cache->capacity)
    {
        evictLRU(cache);
    }
}

// Function to free the memory allocated for the LRUCache
void freeLRUCache(LRUCache *cache)
{
    // Free the memory of all nodes in the linked list
    Node *current = cache->head;
    Node *next;
    while (current != NULL)
    {
        next = current->next;
        free(current->value1);
        // free(current->value2);
        free(current);
        current = next;
    }

    // Free the memory of the LRUCache structure
    free(cache);
}

// Example usage
// int main() {
//     // Create an LRUCache with a capacity of 3

//     // Add key-values to the cache with two strings and an integer
//     put(lruCache, "Value1A", "Value1B", 10);
//     put(lruCache, "Value2A", "Value2B", 20);
//     put(lruCache, "Value3A", "Value3B", 30);
//     char* s4=(char*)malloc(sizeof(char)*1024);
//     scanf("%s",s4);
//     // Get a node pointer, moving the corresponding node to the front
//     Node* node = getNode(lruCache, s4);
//     if (node != NULL) {
//         printf("Values for Value2A: %s, %s, %d\n", node->value1, node->value2, node->intValue); // Output: Values for Value2A: Value2A, Value2B, 20
//     } else {
//         printf("Value2A not found in the cache.\n");
//     }

//     // Free the memory allocated for the LRUCache
//     freeLRUCache(lruCache);

//     return 0;
// }
