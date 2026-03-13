#define _POSIX_C_SOURCE 200809L
#include "log_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 4
#define INITIAL_CAPACITY 1024

static double get_time_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static char **read_all_lines(const char *filename, int *line_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int capacity = INITIAL_CAPACITY;
    char **lines = (char **)malloc(sizeof(char *) * capacity);
    if (!lines) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    *line_count = 0;
    char buffer[MAX_LINE];

    while (fgets(buffer, sizeof(buffer), file)) {
        if (*line_count >= capacity) {
            capacity *= 2;
            char **temp = (char **)realloc(lines, sizeof(char *) * capacity);
            if (!temp) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            lines = temp;
        }

        lines[*line_count] = strdup(buffer);
        if (!lines[*line_count]) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

        (*line_count)++;
    }

    fclose(file);
    return lines;
}

static void free_lines(char **lines, int count) {
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}

static void run_single_threaded(char **lines, int line_count) {
    HashTable ip_counts, url_counts;
    ht_init(&ip_counts);
    ht_init(&url_counts);

    int error_count = 0;
    double start = get_time_seconds();

    for (int i = 0; i < line_count; i++) {
        char ip[64];
        char url[256];
        int status;

        if (parse_log_line(lines[i], ip, url, &status)) {
            ht_increment(&ip_counts, ip);
            ht_increment(&url_counts, url);

            if (status >= 400 && status <= 599) {
                error_count++;
            }
        }
    }

    char max_url[MAX_LINE];
    int max_count;
    ht_get_max(&url_counts, max_url, &max_count);

    double end = get_time_seconds();

    printf("=== Single-threaded ===\n");
    printf("Total Unique IPs: %d\n", ht_get_unique_count(&ip_counts));
    printf("Most Visited URL: %s (%d times)\n", max_url, max_count);
    printf("HTTP Errors: %d\n", error_count);
    printf("Execution Time: %.6f seconds\n\n", end - start);

    ht_free(&ip_counts);
    ht_free(&url_counts);
}

static void run_multi_threaded(char **lines, int line_count) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk_size = line_count / NUM_THREADS;
    int start_index = 0;
    double start = get_time_seconds();

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].lines = lines;
        thread_data[i].start = start_index;
        thread_data[i].end = (i == NUM_THREADS - 1) ? line_count : start_index + chunk_size;
        start_index = thread_data[i].end;

        if (pthread_create(&threads[i], NULL, process_chunk, &thread_data[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    HashTable merged_ips, merged_urls;
    ht_init(&merged_ips);
    ht_init(&merged_urls);

    int total_errors = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        ht_merge(&merged_ips, &thread_data[i].ip_counts);
        ht_merge(&merged_urls, &thread_data[i].url_counts);
        total_errors += thread_data[i].error_count;

        ht_free(&thread_data[i].ip_counts);
        ht_free(&thread_data[i].url_counts);
    }

    char max_url[MAX_LINE];
    int max_count;
    ht_get_max(&merged_urls, max_url, &max_count);

    double end = get_time_seconds();

    printf("=== Multi-threaded ===\n");
    printf("Total Unique IPs: %d\n", ht_get_unique_count(&merged_ips));
    printf("Most Visited URL: %s (%d times)\n", max_url, max_count);
    printf("HTTP Errors: %d\n", total_errors);
    printf("Execution Time: %.6f seconds\n\n", end - start);

    ht_free(&merged_ips);
    ht_free(&merged_urls);
}

int main(void) {
    const char *filename = "access.log";
    int line_count = 0;

    char **lines = read_all_lines(filename, &line_count);

    printf("Log lines loaded: %d\n\n", line_count);

    run_single_threaded(lines, line_count);
    run_multi_threaded(lines, line_count);

    free_lines(lines, line_count);
    return 0;
}