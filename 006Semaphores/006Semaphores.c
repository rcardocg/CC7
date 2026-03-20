#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include <stdbool.h>
#include "parkingGUI.h"

#define NUM_CARS    10
#define NUM_SPACES   3

// Contador de espacios disponibles
int available_spaces = NUM_SPACES;

// Mutex que protege available_spaces
pthread_mutex_t parking_mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable de condición
pthread_cond_t space_available = PTHREAD_COND_INITIALIZER;

// Mutex para imprimir sin mezclar mensajes
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Mutex para estadísticas
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

//Mutex para carros finalizados
pthread_mutex_t finish_mutex = PTHREAD_MUTEX_INITIALIZER;

// Estadísticas compartidas
int    total_cars_parked = 0;
double total_wait_time   = 0.0;

// Cuántos carros ya terminaron
int finished_cars = 0;
int slot_car[NUM_SPACES] = {-1, -1, -1}; // Espacio libre = -1, (ID)


// Imprime mensaje con timestamp
static void log_event(const char *msg)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[64];

    strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y", t);

    pthread_mutex_lock(&log_mutex);
    printf("[%s] %s\n", buf, msg);
    fflush(stdout);
    pthread_mutex_unlock(&log_mutex);
}

// Asigna un espacio a un carro
static int assign_slot(int car_id)
{
    for (int i = 0; i < NUM_SPACES; i++) {
        if (slot_car[i] == -1) {
            slot_car[i] = car_id;
            return i;
        }
    }
    return -1;
}

//Libera el espacio 
static void release_slot(int car_id)
{
    for (int i = 0; i < NUM_SPACES; i++) {
        if (slot_car[i] == car_id) {
            slot_car[i] = -1;
            gui_set_slot_car(i, -1);
            return;
        }
    }
}

// Función de cada carro
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
    pthread_mutex_lock(&parking_mutex);
    while (available_spaces == 0)
        pthread_cond_wait(&space_available, &parking_mutex);

    available_spaces--;

    // Asignar espacio y actualizar GUI
    int slot = assign_slot(id);
    if (slot != -1) {
        gui_set_slot_car(slot, id);
    }

    gui_set_status(available_spaces, NUM_SPACES, NUM_CARS - finished_cars);   // actualizar GUI
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

    sleep((rand() % 5) + 1);

    // 4. Salida
    pthread_mutex_lock(&parking_mutex);
    available_spaces++;

    // Liberar slot
    release_slot(id);

    pthread_cond_signal(&space_available);
    gui_set_status(available_spaces, NUM_SPACES, NUM_CARS - finished_cars);   // actualizar GUI
    pthread_mutex_unlock(&parking_mutex);

    snprintf(msg, sizeof(msg), "Car %d: Leaving parking lot", id);
    log_event(msg);

    pthread_mutex_lock(&finish_mutex);
    finished_cars++;
    int active = NUM_CARS - finished_cars;
    pthread_mutex_unlock(&finish_mutex);

    gui_set_status(available_spaces, NUM_SPACES, active);   // actualizar GUI

    return NULL;
}


int main(void)
{
    pthread_t threads[NUM_CARS];
    int ids[NUM_CARS];

    srand((unsigned)time(NULL));

    if (!gui_init()) {
        fprintf(stderr, "Error: no se pudo inicializar la GUI\n");
        return EXIT_FAILURE;
    }

    gui_set_status(NUM_SPACES, NUM_SPACES, NUM_CARS);

    // Crear un thread por carro
    for (int i = 0; i < NUM_CARS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, car_thread, &ids[i]) != 0) {
            perror("pthread_create");
            gui_close();
            exit(EXIT_FAILURE);
        }
        usleep(10000);
    }

    // Loop principal de GUI
    bool running = true;
    while (running) {
        running = gui_process_frame();

        pthread_mutex_lock(&finish_mutex);
        bool all_done = (finished_cars == NUM_CARS);
        pthread_mutex_unlock(&finish_mutex);

        if (all_done) {
            // Espera a que el usuario cierre la ventana
        }
    }

    // Esperar threads
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
    pthread_mutex_destroy(&finish_mutex);
    pthread_cond_destroy(&space_available);

    gui_close();
    return 0;
}