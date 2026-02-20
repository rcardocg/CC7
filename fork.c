#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd[2];          // Variable para el pipe
    pid_t pid;          // Variable para almacenar el ID del proceso
    char buffer[100];   // Espacio para guardar mensajes leídos
    char message[] = "Hello from Parent";

    // --- PASO 1 Y 2: Creación y Espera ---
    pid = fork();

    if (pid == 0) {
        // Parte del hijo
        printf("Child Process: PID=%d, Parent PID=%d\n", getpid(), getppid());
        sleep(1); 
        exit(0); 
    } else {
        // Parte del padre
        printf("Parent Process: PID=%d\n", getpid());
        waitpid(pid, NULL, 0); // Paso 2: El padre espera al hijo
        printf("---------------------\n\n");
    }

    // --- PASO 3: Comunicación (Pipe) ---
    pipe(fd); // Crea la tubería
    pid = fork();

    if (pid == 0) {
        // Parte del hijo
        close(fd[1]); // Cierra escritura
        read(fd[0], buffer, sizeof(buffer)); // Lee del pipe
        printf("Paso 3 - Child Process: Received \"%s\"\n", buffer);
        close(fd[0]);
        exit(0);
    } else {  
        // Parte del padre
        close(fd[0]); // Cierra lectura
        printf("Paso 3 - Parent Process: Writing \"%s\"\n", message);
        write(fd[1], message, strlen(message) + 1); // Envía mensaje
        close(fd[1]);
        wait(NULL); 
        printf("\n");
    }

    // --- PASO 4: Múltiples Hijos (Loop) ---
    printf("Paso 4 - Parent Process: PID=%d\n", getpid());

    // Bucle para crear 3 hijos
    for (int i = 1; i <= 3; i++) {
        pid = fork();

        if (pid == 0) {
            // Parte de cada hijo
            printf("Child %d: PID=%d, Parent PID=%d\n", i, getpid(), getppid());
            exit(0); // El hijo termina para no repetir el bucle
        }
    }

    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}
