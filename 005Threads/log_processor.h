#ifndef LOG_PROCESSOR_H
#define LOG_PROCESSOR_H

#include <pthread.h>

#define MAX_LINE 512
#define HASH_SIZE 211

typedef struct Entry {
    char *key;
    int count;
    struct Entry *next;
} Entry;

typedef struct {
    Entry *buckets[HASH_SIZE];
} HashTable;

typedef struct {
    char **lines;
    int start;
    int end;
    HashTable ip_counts;
    HashTable url_counts;
    int error_count;
} ThreadData;

void ht_init(HashTable *ht);
void ht_increment(HashTable *ht, const char *key);
int ht_get_unique_count(HashTable *ht);
void ht_merge(HashTable *dest, HashTable *src);
void ht_get_max(HashTable *ht, char *max_key, int *max_count);
void ht_free(HashTable *ht);

int parse_log_line(const char *line, char *ip, char *url, int *status);
void *process_chunk(void *arg);

#endif