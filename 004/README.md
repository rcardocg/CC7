Laboratorio hecho en parejas
Ricardo Caballeros
Yoselin Mejía

# Lab 004 - Process Creation and Management (Linux)

## Descripción General

Este laboratorio tiene como objetivo comprender cómo funciona la creación de procesos, la sincronización y la comunicación entre procesos (IPC) en sistemas Linux utilizando el lenguaje C.

El propósito principal fue analizar cómo un proceso padre puede:

- Crear procesos hijos
- Sincronizar su ejecución
- Comunicarse mediante Pipes
- Crear múltiples procesos hijos
- Compartir memoria entre procesos

## Tasks

### Task 1 & 2 – Creación y Sincronización de Procesos
- Se utilizó "fork()" para crear un proceso hijo.
- El proceso padre esperó al hijo utilizando "waitpid()".
- Se analizaron los identificadores de proceso (PID) y la relación padre-hijo.

### Task 3 – Comunicación mediante Pipe
- Se creó un canal de comunicación unidireccional con "pipe()".
- El padre escribió un mensaje y el hijo leyó el mensaje.


### Task 4 – Múltiples Procesos Hijos
- Se utilizó un ciclo for con "fork()" para crear tres procesos hijos.
- Cada hijo imprimió su información y finalizó.
- El padre esperó correctamente la terminación de todos los hijos.

### Task 5 – Memoria Compartida (Extra)
- Se creó un segmento de memoria compartida con "shmget()".
- Se conectó a la memoria usando "shmat()".
- El padre escribió un mensaje en memoria y el hijo leyó el mismo mensaje.
- Se liberaron correctamente los recursos con "shmdt()" y "shmctl()".

## Compilación y Ejecución

Compilar: "gcc -o fork fork.c && ./fork"