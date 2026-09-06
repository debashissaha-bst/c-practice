#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUCKET_COUNT 8
#define LOAD_FACTOR_THRESHOLD 0.75

typedef struct HashNode {
    char *key;
    int value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode **buckets;
    size_t bucket_count;
    size_t entry_count;
} HashTable;


static unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++)) {
        hash = ((hash << 5) + hash) + (unsigned long)c; 
    }
    return hash;
}

static void ht_init(HashTable *ht, size_t bucket_count) {
    ht->bucket_count = bucket_count;
    ht->entry_count = 0;
    ht->buckets = calloc(bucket_count, sizeof(HashNode *));
    if (!ht->buckets) {
        fprintf(stderr, "Fatal: calloc failed in ht_init\n");
        exit(EXIT_FAILURE);
    }
}

static void ht_resize(HashTable *ht, size_t new_bucket_count);

static void ht_insert(HashTable *ht, const char *key, int value) {
    double load = (double)(ht->entry_count + 1) / (double)ht->bucket_count;
    if (load > LOAD_FACTOR_THRESHOLD) {
        ht_resize(ht, ht->bucket_count * 2);
    }

    unsigned long idx = hash_string(key) % ht->bucket_count;

    
    for (HashNode *cur = ht->buckets[idx]; cur != NULL; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) {
            cur->value = value;
            return;
        }
    }

    HashNode *node = malloc(sizeof(HashNode));
    node->key = strdup(key);
    node->value = value;
    node->next = ht->buckets[idx];
    ht->buckets[idx] = node;
    ht->entry_count++;
}

static int ht_get(const HashTable *ht, const char *key, int *out_value) {
    unsigned long idx = hash_string(key) % ht->bucket_count;
    for (HashNode *cur = ht->buckets[idx]; cur != NULL; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) {
            *out_value = cur->value;
            return 1;
        }
    }
    return 0;
}

static int ht_remove(HashTable *ht, const char *key) {
    unsigned long idx = hash_string(key) % ht->bucket_count;
    HashNode *cur = ht->buckets[idx];
    HashNode *prev = NULL;
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else ht->buckets[idx] = cur->next;
            free(cur->key);
            free(cur);
            ht->entry_count--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}


static void ht_resize(HashTable *ht, size_t new_bucket_count) {
    HashNode **new_buckets = calloc(new_bucket_count, sizeof(HashNode *));
    if (!new_buckets) return; 

    for (size_t i = 0; i < ht->bucket_count; i++) {
        HashNode *cur = ht->buckets[i];
        while (cur != NULL) {
            HashNode *next = cur->next; 
            unsigned long new_idx = hash_string(cur->key) % new_bucket_count;
            cur->next = new_buckets[new_idx];
            new_buckets[new_idx] = cur;
            cur = next;
        }
    }

    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->bucket_count = new_bucket_count;
    printf("[resize] bucket count now %zu (entries=%zu)\n",
           ht->bucket_count, ht->entry_count);
}

static void ht_print(const HashTable *ht) {
    printf("--- Hash Table (%zu buckets, %zu entries) ---\n",
           ht->bucket_count, ht->entry_count);
    for (size_t i = 0; i < ht->bucket_count; i++) {
        printf("  [%zu]: ", i);
        for (HashNode *cur = ht->buckets[i]; cur != NULL; cur = cur->next) {
            printf("(%s -> %d) ", cur->key, cur->value);
        }
        printf("\n");
    }
}

static void ht_free(HashTable *ht) {
    for (size_t i = 0; i < ht->bucket_count; i++) {
        HashNode *cur = ht->buckets[i];
        while (cur != NULL) {
            HashNode *tmp = cur;
            cur = cur->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(ht->buckets);
    ht->buckets = NULL;
    ht->bucket_count = 0;
    ht->entry_count = 0;
}

int main(void) {
    HashTable ht;
    ht_init(&ht, INITIAL_BUCKET_COUNT);

    const char *keys[] = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon", "mango"
    };
    int n = (int)(sizeof(keys) / sizeof(keys[0]));

    for (int i = 0; i < n; i++) {
        ht_insert(&ht, keys[i], i * 10);
    }

    ht_print(&ht);

    int value;
    if (ht_get(&ht, "cherry", &value)) {
        printf("\nLookup 'cherry' -> %d\n", value);
    }

    printf("\nUpdating 'banana' to 999...\n");
    ht_insert(&ht, "banana", 999);
    ht_get(&ht, "banana", &value);
    printf("Lookup 'banana' -> %d\n", value);

    printf("\nRemoving 'fig'...\n");
    ht_remove(&ht, "fig");
    if (!ht_get(&ht, "fig", &value)) {
        printf("'fig' no longer found (as expected).\n");
    }

    ht_free(&ht);
    return 0;
}
