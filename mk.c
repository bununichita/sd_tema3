#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_SIZE_OP 20

#define MAX_SIZE_WORD 50

#define ALPHABET_SIZE 26

#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

#define DATA_SIZE 32

#define FILENAME_SIZE 20

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
    /* Value associated with key (set if end_of_word = 1) */
    void* value;

    /* 1 if current node marks the end of a word, 0 otherwise */
    int end_of_word;

    trie_node_t** children;
    int n_children;
};

typedef struct trie_t trie_t;
struct trie_t {
    trie_node_t* root;
    
    /* Number of keys */
    int size;

    /* Generic Data Structure */
    int data_size;

    /* Trie-Specific, alphabet properties */
    int alphabet_size;
    char* alphabet;

    /* Callback to free value associated with key, should be called when freeing */
    void (*free_value_cb)(void*);

    /* Optional - number of nodes, useful to test correctness */
    int nNodes;
};

trie_node_t* trie_create_node(trie_t * trie) {
    // TODO
    trie_node_t *node;
    node = malloc(sizeof(trie_node_t));
    node->value = calloc(1, trie->data_size);
    node->end_of_word = 0;
    node->n_children = 0;
    node->children = calloc(ALPHABET_SIZE, sizeof(trie_node_t *));

    return node;
}

trie_t* trie_create(int data_size, int alphabet_size, char* alphabet,
void (*free_value_cb)(void*)) {
    // TODO
    trie_t *trie;
    trie = malloc(sizeof(trie_t));
    trie->data_size = data_size;
    trie->alphabet_size = alphabet_size;
    trie->alphabet = alphabet;
    trie->free_value_cb = free_value_cb;
    trie->nNodes = 1;
    trie->size = 0;
    trie_node_t *root = trie_create_node(trie);
    *(int *)root->value = -1;
    trie->root = root;
    return trie;
}

void *search(trie_node_t *node, char *key, trie_t *trie)
{
    if (key[0] == '\0') {
        // printf("test 131\n");
        if (!node->end_of_word && node != trie->root)
            return NULL;
        return node->value;
    }
    // printf("test 134\n");
    // printf("%d\n", *(int *)node->value);
    trie_node_t *next = node->children[key[0] - 'a'];
    // printf("%p\n", next);
    if (!next) {
        // printf("test 139\n");
        return NULL;
    }
    // printf("test 142\n");
    return search(next, key + 1, trie);
}

void* trie_search(trie_t* trie, char* key) {
    // TODO
    trie_node_t *node = trie->root;
    return search(node, key, trie);
}

void insert(trie_node_t *node, char *key, trie_t *trie, int add)
{
    if (key[0] == '\0') {
        // memcpy(node->value, value, trie->data_size);
		if (add) {
			*(int *)node->value++;
		} else {
			*(int *)node->value = 1;
        	node->end_of_word = 1;
		}
        return;
    }
    // printf("%d\n", *(int *)node->value);
    trie_node_t *next = node->children[key[0] - 'a'];
    // printf("%p\n", next);
    if (!next) {
        node->children[key[0] - 'a'] = trie_create_node(trie);
        next = node->children[key[0] - 'a'];
        // printf("%p\n", next);
        node->n_children++;
        trie->nNodes++;
    }

    insert(next, key + 1, trie, add);
}

void trie_insert(trie_t* trie, char* key) {
    // TODO
    trie_node_t *node = trie->root;
	int *value;
	value = trie_search(trie, key);
	if (value) {
		insert(node, key, trie, 1);
	} else {
		insert(node, key, trie, 0);
	}
    trie->size++;
}

void load(trie_t *trie, char *file)
{
	FILE *in = fopen(file, "rt");
	char key[MAX_SIZE_WORD];
	while (fscanf(in, "%s", key) != EOF) {
		trie_insert(trie, key);
	}

	fclose(in);
}

int main(void)
{
	trie_t *trie;
	trie = trie_create(DATA_SIZE, ALPHABET_SIZE, ALPHABET, free);
	
	char op[MAX_SIZE_OP];
	scanf("%s", op);
	while(strcmp(op, "EXIT"))
	{
		if (!strcmp(op, "INSERT")) {
			char key[MAX_SIZE_WORD];
			scanf("%s", key);
			trie_insert(trie, key);
		} else if (!strcmp(op, "LOAD")) {
			char file[FILENAME_SIZE];
			scanf("%s", file);
			load(trie, file);
		}
		printf("ckasjdnc\n");
		scanf("%s", op);
	}
	return 0;
}
