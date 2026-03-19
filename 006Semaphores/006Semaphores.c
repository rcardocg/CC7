#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
 
#define NUM_CARS    10
#define NUM_SPACES   3
 
// Contador de espacios disponibles (reemplaza al semáforo)
int available_spaces = NUM_SPACES;
 
// Mutex que protege available_spaces
pthread_mutex_t parking_mutex = PTHREAD_MUTEX_INITIALIZER;
 
// Variable de condición: los carros esperan aquí si no hay espacio
pthread_cond_t space_available = PTHREAD_COND_INITIALIZER;
 
// Mutex para imprimir sin mezclar mensajes
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
 
// Mutex para los contadores de estadísticas
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
 
// Estadísticas compartidas
int    total_cars_parked = 0;
double total_wait_time   = 0.0;
 
 
// Imprime un mensaje con timestamp de forma thread-safe
static void log_event(const char *msg)
{
    time_t     now = time(NULL);
    struct tm *t   = localtime(&now);
    char       buf[64];
    strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y", t);
 
    pthread_mutex_lock(&log_mutex);
    printf("[%s] %s\n", buf, msg);
    fflush(stdout);
    pthread_mutex_unlock(&log_mutex);
}
 
 
// Función que ejecuta cada thread (carro)
static void *car_thread(void *arg)
{
    int  id = *(int *)arg;
    char msg[128];
 
    // 1. Llegada
    snprintf(msg, sizeof(msg), "Car %d: Arrived at parking lot", id);
    log_event(msg);
 
    struct timespec arrive_ts, park_ts;
    clock_gettime(CLOCK_MONOTONIC, &arrive_ts);
 
    // 2. Esperar espacio libre
    // while y no if → evita spurious wakeups
    pthread_mutex_lock(&parking_mutex);
    while (available_spaces == 0)
        pthread_cond_wait(&space_available, &parking_mutex);
    available_spaces--;
    pthread_mutex_unlock(&parking_mutex);
 
    clock_gettime(CLOCK_MONOTONIC, &park_ts);
    double wait_sec = (park_ts.tv_sec  - arrive_ts.tv_sec) +
                      (park_ts.tv_nsec - arrive_ts.tv_nsec) / 1e9;
 
    // 3. Estacionado
    snprintf(msg, sizeof(msg),
             "Car %d: Parked successfully (waited %.2f seconds)", id, wait_sec);
    log_event(msg);
 
    pthread_mutex_lock(&stats_mutex);
    total_cars_parked++;
    total_wait_time += wait_sec;
    pthread_mutex_unlock(&stats_mutex);
 
    // Permanecer estacionado 1-5 segundos
    sleep((rand() % 5) + 1);
 
    // 4. Salida: liberar espacio y avisar a UN carro en espera
    pthread_mutex_lock(&parking_mutex);
    available_spaces++;
    pthread_cond_signal(&space_available);
    pthread_mutex_unlock(&parking_mutex);
 
    snprintf(msg, sizeof(msg), "Car %d: Leaving parking lot", id);
    log_event(msg);
 
    return NULL;
}
 
 
int main(void)
{
    pthread_t threads[NUM_CARS];
    int       ids[NUM_CARS];
 
    srand((unsigned)time(NULL));
 
    // Crear un thread por carro
    for (int i = 0; i < NUM_CARS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, car_thread, &ids[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        usleep(10000);
    }
 
    // Esperar a que todos terminen
    for (int i = 0; i < NUM_CARS; i++)
        pthread_join(threads[i], NULL);
 
    // Estadísticas finales
    printf("\nTotal cars parked: %d\n", total_cars_parked);
    if (total_cars_parked > 0)
        printf("Average wait time: %.2f seconds\n",
               total_wait_time / total_cars_parked);
 
    // Liberar recursos
    pthread_mutex_destroy(&parking_mutex);
    pthread_mutex_destroy(&log_mutex);
    pthread_mutex_destroy(&stats_mutex);
    pthread_cond_destroy(&space_available);
 
    return 0;
}
