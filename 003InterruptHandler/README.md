# 003InterruptHandler - BeagleBone Black Bare-Metal Timer Interrupt Example

El laboratorio se enfoca en enseñar como es que funcionan las interrupciones en la BeagleBone Black. Esto se logra realizando una interrupción cada dos segundos que imprime la palabra "Tick" en la terminal.
## ¿Qué es una interrupción?
De acuerdo con el ciclo de vida de los procesos, una **solicitud de I/O** es la que coloca al proceso actual en el estado **wait/block**. Una vez que la operación de I/O termina, se genera una **interrupción** que notifica al sistema operativo para mover el proceso de nuevo a **ready**, quedando a la espera de que el _scheduler_ lo asigne de nuevo a la CPU.
![[Pasted image 20260213143634.png]]

## Objetivos
- Implementar ejecución dirigida por interrupciones en lugar de depender de bucles de espera activa (_busy loops_).
- Configurar el periférico de temporización del ARM para ejecución periódica.
- Comprender el manejo de la Tabla de Vectores de Interrupción (IVT) y el servicio de interrupciones en ensamblador.
## 3. Arquitectura del Sistema
El proyecto se estructura en tres capas que deben interactuar coordinadamente:
1. **Nivel de OS (Lógica Principal):** El archivo `os.c` mantiene un bucle que genera números aleatorios continuamente, simulando la carga de trabajo normal de la CPU.
2. **Interfaz de Hardware (Timer):** Se configura el periférico `DMTIMER2` (Base `0x48040000`) para que cuente hacia atrás y genere una señal IRQ al llegar a cero.
3. **Manejo de Excepciones (Low-Level):** El archivo `root.s` gestiona la Tabla de Vectores. Cuando ocurre la IRQ, el flujo se desvía al vector en el offset `0x18`.
## 4. Flujo de la Interrupción (Hardware a Software)
El flujo de control sigue esta ruta crítica:
- **Dispositivo (Timer):** Alcanza el valor de desbordamiento y levanta la bandera en el registro `TISR`.
- **Controlador (INTCPS):** El Interrupt Controller gestiona la IRQ 68 (correspondiente al Timer2) y notifica a la CPU.
- **CPU (IRQ Exception):** La CPU detiene el proceso actual, cambia a modo IRQ y salta a la dirección configurada en la tabla de vectores.
- **Handler:** Se ejecutan las siguientes acciones en ensamblador y C:
    1. **Preservación:** Se guardan los registros del CPU (`r0-r12`, `lr`) para no corromper el proceso interrumpido.
    2. **Servicio:** Se imprime "Tick" vía UART.
    3. **Acknowledge:** Se limpia la bandera en el Timer y se notifica al controlador de interrupciones escribiendo en `INTC_CONTROL`.
    4. **Retorno:** Se restauran los registros y se regresa a la instrucción exacta donde estaba el programa principal usando `subs pc, lr, #4`.
## 5. Resultados y Validación
Se espera que el sistema opere de forma concurrente sin congelarse. La validación exitosa ocurre cuando los números aleatorios se imprimen de forma fluida y el mensaje "Tick" aparece puntualmente cada 2 segundos, demostrando que la interrupción es transparente para la lógica principal del programa.
