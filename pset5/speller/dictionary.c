// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    // char word[LENGTH + 1];
    bool isValid;
    struct node *next[27];
}
node;

// Number of buckets in hash table
const unsigned int N = 1;

// Hash table
node *table[N];

// Number of words loaded in dictionary
int dic_size = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    node *n = table[0];
    int i = 0;

    while (n != NULL)
    {
        // If reaches the end of the word, check if it's a valid one
        if (word[i] == '\0')
        {
            return n->isValid;
        }

        // Get current letter position in trie array
        int index = hash(strstr(word, &word[i++]));
        n = n->next[index];
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // If first char is ' return the last index possible, which is 26. Otherwise,
    // hashes the first letter to be between 0 and 26 (exclusive)
    return word[0] == '\'' ? 26 : tolower(word[0]) % 26;
}


// Allocates memory for a new node and initializes its properties
node *new_node()
{
    node *n = malloc(sizeof(node));

    n->isValid = false;
    for (int i = 0; i < 27; i++)
    {
        n->next[i] = NULL;
    }

    return n;
}

// Inserts given word to hash table
void insert_word(node *parent, char *word)
{
    node *n = parent;

    // When at the end of the word, mark current node as valid and
    // increase dize of dictionary
    if (*word == '\0')
    {
        n->isValid = true;
        dic_size++;
        return;
    }

    int index = hash(word);
    if (n->next[index] == NULL)
    {
        // If the next node to be visited is null, allocate memory
        n->next[index] = new_node();
    }

    insert_word(n->next[index], (word + 1));
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *dic = fopen(dictionary, "r");
    if (dic == NULL)
    {
        return false;
    }

    // Allocate memory for root node of trie
    table[0] = new_node();

    char word[LENGTH + 1];
    while (fscanf(dic, "%s", word) != EOF)
    {
        insert_word(table[0], word);
    }

    fclose(dic);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return dic_size;
}

// Free memory allocated for all nodes that required it
bool free_nodes(node *n)
{
    for (int i = 0; i < 27; i++)
    {
        if (n->next[i] != NULL)
        {
            free_nodes(n->next[i]);
        }
    }
    free(n);
    return true;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    return free_nodes(table[0]);
}
