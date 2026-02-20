#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    int fd[2];
    pid_t pid;
    char buffer[100];
    char message[] = "Hello from Parent";

    // --- TASK 1 & 2: Creating & Synchronizing ---
    printf("TASK 1 & 2: Creating & Synchronizing\n");
    pid = fork();
    
    if (pid < 0) die("fork");

    if (pid == 0) {
        printf("Child Process: PID=%d, Parent PID=%d\n", getpid(), getppid());
        fflush(stdout);
        sleep(1);
        _exit(0);
    } else {
        printf("Parent Process: PID=%d\n", getpid());
        fflush(stdout);

        if (waitpid(pid, NULL, 0) < 0) die("waitpid");
        printf("---------------------\n\n");
    }

    // --- TASK 3: Communication (Pipe) ---
    printf("TASK 3: Communication (Pipe)\n");
    if (pipe(fd) < 0) die("pipe");

    pid = fork();
    if (pid < 0) die("fork");

    if (pid == 0) {
        close(fd[1]);
        ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n < 0) die("read");
        buffer[n] = '\0';

        printf("Child Process: Received \"%s\"\n", buffer);
        fflush(stdout);

        close(fd[0]);
        _exit(0);
    } else {
        close(fd[0]);

        printf("Parent Process: Writing \"%s\"\n", message);
        fflush(stdout);

        size_t to_write = strlen(message) + 1;
        ssize_t w = write(fd[1], message, to_write);
        if (w < 0) die("write");
        if ((size_t)w != to_write) {
            fprintf(stderr, "write: partial write (%zd/%zu)\n", w, to_write);
            exit(EXIT_FAILURE);
        }

        close(fd[1]);
        if (waitpid(pid, NULL, 0) < 0) die("waitpid");
        printf("---------------------\n\n");
    }

    // --- TASK 4: Multiple Child Processes ---
    printf("TASK 4: MMultiple Child Processes\n");
    printf("Parent Process: PID=%d\n", getpid());
    fflush(stdout);

    for (int i = 1; i <= 3; i++) {
        pid = fork();
        if (pid < 0) die("fork");

        if (pid == 0) {
            printf("Child %d: PID=%d, Parent PID=%d\n", i, getpid(), getppid());
            fflush(stdout);
            _exit(0);
        }
    }

    for (int i = 0; i < 3; i++) {
        if (wait(NULL) < 0) die("wait");


    }

    // --- TASK 5: Shared Memory (Advanced Task) ---
    printf("---------------------\n\n");
    printf("TASK 5: Shared Memory (Advanced Task)\n");
    const char shm_msg[] = "Shared Memory Example";

    int shmid = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0600);
    if (shmid < 0) die("shmget");

    pid = fork();
    if (pid < 0) die("fork");

    if (pid == 0) {
        char *shm = (char *)shmat(shmid, NULL, 0);
        if (shm == (char *)-1) die("shmat (child)");

        printf("Child Process: Read \"%s\"\n", shm);
        fflush(stdout);

        if (shmdt(shm) < 0) die("shmdt (child)");
        _exit(0);
    } else {
        char *shm = (char *)shmat(shmid, NULL, 0);
        if (shm == (char *)-1) die("shmat (parent)");

        snprintf(shm, 128, "%s", shm_msg);

        printf("Parent Process: Writing \"%s\"\n", shm_msg);
        fflush(stdout);

        if (waitpid(pid, NULL, 0) < 0) die("waitpid");

        if (shmdt(shm) < 0) die("shmdt (parent)");
        if (shmctl(shmid, IPC_RMID, NULL) < 0) die("shmctl(IPC_RMID)");
    }

    return 0;
}