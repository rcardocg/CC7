#include "log_processor_windows.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int hash_string(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (unsigned int)c; /* hash * 33 + c */
    }
    return hash % HASH_SIZE;
}

static Entry *ht_find_entry(HashTable *ht, const char *key) {
    unsigned int idx = hash_string(key);
    Entry *e = ht->buckets[idx];
    while (e) {
        if (strcmp(e->key, key) == 0) {
            return e;
        }
        e = e->next;
    }
    return NULL;
}

void ht_init(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
}

void ht_increment(HashTable *ht, const char *key) {
    if (!key) {
        return;
    }

    Entry *e = ht_find_entry(ht, key);
    if (e) {
        e->count++;
        return;
    }

    unsigned int idx = hash_string(key);
    e = (Entry *)malloc(sizeof(Entry));
    if (!e) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    e->key = strdup(key);
    if (!e->key) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    e->count = 1;
    e->next = ht->buckets[idx];
    ht->buckets[idx] = e;
}

static void ht_increment_by(HashTable *ht, const char *key, int count) {
    if (!key || count <= 0) {
        return;
    }

    Entry *e = ht_find_entry(ht, key);
    if (e) {
        e->count += count;
        return;
    }

    unsigned int idx = hash_string(key);
    e = (Entry *)malloc(sizeof(Entry));
    if (!e) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    e->key = strdup(key);
    if (!e->key) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    e->count = count;
    e->next = ht->buckets[idx];
    ht->buckets[idx] = e;
}

int ht_get_unique_count(HashTable *ht) {
    int total = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        Entry *e = ht->buckets[i];
        while (e) {
            total++;
            e = e->next;
        }
    }
    return total;
}

void ht_merge(HashTable *dest, HashTable *src) {
    if (!dest || !src) {
        return;
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        Entry *e = src->buckets[i];
        while (e) {
            ht_increment_by(dest, e->key, e->count);
            e = e->next;
        }
    }
}

void ht_get_max(HashTable *ht, char *max_key, int *max_count) {
    if (!max_key || !max_count) {
        return;
    }

    *max_count = 0;
    max_key[0] = '\0';

    for (int i = 0; i < HASH_SIZE; i++) {
        Entry *e = ht->buckets[i];
        while (e) {
            if (e->count > *max_count) {
                *max_count = e->count;
                strncpy(max_key, e->key, MAX_LINE - 1);
                max_key[MAX_LINE - 1] = '\0';
            }
            e = e->next;
        }
    }
}

void ht_free(HashTable *ht) {
    if (!ht) {
        return;
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        Entry *e = ht->buckets[i];
        while (e) {
            Entry *next = e->next;
            free(e->key);
            free(e);
            e = next;
        }
        ht->buckets[i] = NULL;
    }
}

int parse_log_line(const char *line, char *ip, char *url, int *status) {
    if (!line || !ip || !url || !status) {
        return 0;
    }

    /* Extract IP */
    if (sscanf(line, "%63s", ip) != 1) {
        return 0;
    }

    /* Find request string enclosed in quotes: "METHOD URL" */
    const char *start = strchr(line, '"');
    if (!start) {
        return 0;
    }
    const char *end = strchr(start + 1, '"');
    if (!end) {
        return 0;
    }

    char request[MAX_LINE];
    size_t len = (size_t)(end - start - 1);
    if (len >= sizeof(request)) {
        return 0;
    }
    memcpy(request, start + 1, len);
    request[len] = '\0';

    char method[32];
    if (sscanf(request, "%31s %255s", method, url) != 2) {
        return 0;
    }

    /* Parse status code after the closing quote */
    if (sscanf(end + 1, " %d", status) != 1) {
        return 0;
    }

    return 1;
}

void *process_chunk(void *arg) {
    if (!arg) {
        return NULL;
    }

    ThreadData *td = (ThreadData *)arg;
    ht_init(&td->ip_counts);
    ht_init(&td->url_counts);
    td->error_count = 0;

    for (int i = td->start; i < td->end; i++) {
        char ip[64];
        char url[256];
        int status;

        if (parse_log_line(td->lines[i], ip, url, &status)) {
            ht_increment(&td->ip_counts, ip);
            ht_increment(&td->url_counts, url);

            if (status >= 400 && status <= 599) {
                td->error_count++;
            }
        }
    }

    return NULL;
}