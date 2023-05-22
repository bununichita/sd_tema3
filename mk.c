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

int remove_n(trie_node_t *node, char *key, trie_t *trie)
{
    if (key[0] == '\0') {
        if (node->end_of_word) {
            node->end_of_word = 0;
            // if (!node->n_children) {
            //     free(node->children);
            //     free_value_cb(node->value);
            //     free(node);
            //     return 
            // }
            return (node->n_children == 0);
        }
        return 0;
    }
    trie_node_t *next = node->children[key[0] - 'a'];

    if (next && remove_n(next, key + 1, trie)) {
        // printf("test 194\n");
        free(next->children);
        free(next->value);
        free(next);
        node->children[key[0] - 'a'] = NULL;
        node->n_children--;
        trie->nNodes--;
        if (!node->n_children && !node->end_of_word) {
            // printf("test 199\n");
            return 1;

        }
    }
    // printf("test 204\n");
    return 0;
}

int trie_remove(trie_t* trie, char* key) {
    // TODO
    trie_node_t *node = trie->root;
    return remove_n(node, key, trie);
}

void autocorrect(trie_node_t *node, char *word, int k, int len, char *s)
{
	if (!node) {
        return;
    }
	
	if (len == strlen(word)) {
		if (!node->end_of_word) {
			return;
		}
		int dif = 0;
		for (int i = 0; i < strlen(word); i++) {
			if (s[i] != word[i]) {
				dif++;
			}
		}
		if (dif <= k) {
			printf("%s\n", s);
		}
		return;
	}
	
	

    for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (!len) {
			strcpy(s, "");
		}
		s[len] = i + 'a';
		s[len + 1] = '\0';
		autocorrect(node->children[i], word, k, len + 1, s);
		s[len] = '\0';
    }
}

trie_node_t *complete_node(trie_node_t *node, char *key)
{
	
	if (key[0] == '\0') {
        return node;
    }
    trie_node_t *next = node->children[key[0] - 'a'];
    if (!next) {
        return NULL;
    }
    return complete_node(next, key + 1);
}

int autocomplete_1(trie_node_t *node, char *word, char *s, int *found) {

	if (!node) {
		return 0;
	}
	
	if (node->end_of_word == 1) {
		if (*found) {
			if (strcmp(s, word) < 0) {
				strcpy(word, s);
			}
		} else {
			*found = 1;
			strcpy(word, s);
		}
		return 1;
	}
	
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		int len = strlen(s);
		s[len] = i + 'a';
		s[len + 1] = '\0';
		if (autocomplete_1(node->children[i], word, s, found)) {
			return 1;
		}		
		s[len] = '\0';
	}
}

int find_len(trie_node_t *node, int len)
{
	
	if (!node) {
		return -1;
	}

	if (!node->n_children) {
		return len;
	}
	int next_exists = 0;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		// int l = find_len(node->children[i], len + 1);
		// if (l >= 0) {
		// 	return l;
		// }
		if (node->children[i]) {
			next_exists = 1;
			if (node->children[i]->end_of_word) {
				return len + 1;
			}
		}
	}

	if (!next_exists) {
		return -1;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		int aux = find_len(node->children[i], len + 1);
		if (aux >= 0) {
			return aux;
		}
	}
}

void autocomplete_2(trie_node_t *node, char *s, int iter, int *found) {
	
	if (!node) {
		return;
	}

	if (!iter) {
		// printez cuvantul
		if (node->end_of_word) {
			printf("%s\n", s);
		}
	} else {
		for (int i = 0; i < ALPHABET_SIZE; i++) {
			int len = strlen(s);
			s[len] = i + 'a';
			s[len + 1] = '\0';
			autocomplete_2(node->children[i], s,  iter - 1, found);
			s[len] = '\0';
		}
	}
}

void find_freq(trie_node_t *node, int *max_freq) {
	if (!node) {
		return;
	}

	if (node->end_of_word) {
		if (*(int *)node->value > *max_freq) {
			int val = *(int *)node->value;
			*max_freq = val;
		}
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		find_freq(node->children[i], max_freq);
	}
}

void autocomplete_prep(trie_t *trie, char *word, int a_case)
{
	
	char s[MAX_SIZE_WORD], key[MAX_SIZE_WORD];
	strcpy(key, word);
	strcpy(s, word);
	int aux = 0;
	int *found;
	found = &aux;

	trie_node_t *node = complete_node(trie->root, key);

	strcpy(key, word);


	if (a_case == 1) {
		autocomplete_1(node, word, s, found);
		printf("%s\n", word);
	} else if (a_case == 2) {
		int len = find_len(node, 0);
		// printf("%d\n", len);
		autocomplete_2(node, word, len, found);
		// autocomplete_2(node, word, s, found);
	} else if (a_case == 3) {
		int *freq;
		*freq = 0;
		find_freq(node, freq);
		printf("freq = %d\n", *freq);
	}else if (a_case == 0) {

	}
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
		} else if (!strcmp(op, "REMOVE")) {
			char key[MAX_SIZE_WORD];
			scanf("%s", key);
			trie_remove(trie, key);
		} else if (!strcmp(op, "AUTOCORRECT")) {
			char word[MAX_SIZE_WORD], s[MAX_SIZE_WORD];
			int k;
			scanf("%s %d", word, &k);
			autocorrect(trie->root, word, k, 0, s);
		} else if (!strcmp(op, "AUTOCOMPLETE")) {
			char word[MAX_SIZE_WORD];
			int a_case;
			scanf("%s %d", word, &a_case);
			autocomplete_prep(trie, word, a_case);
		}
		scanf("%s", op);
	}
	return 0;
}
