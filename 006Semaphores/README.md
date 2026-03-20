# 006Semaphores

El programa simula un sistema de parqueo con múltiples carros utilizando threads y sincronización con mutex y variables de condición.

## Componentes principales
- `006Semaphores.c`:
  - `NUM_CARS = 10` Número de Carros.
  - `NUM_SPACES = 3`Número de Parqueos.
  - `available_spaces` controla espacios libres.
  - `parking_mutex` protege el acceso a `available_spaces` y slots.
  - `space_available` permite que los carros esperen hasta que haya un lugar.
  - `log_mutex` sincroniza salidas `printf` para evitar mezcla de texto.
  - `stats_mutex` protege `total_cars_parked` y `total_wait_time`.
  - `finish_mutex` protege `finished_cars` (condición de fin).

- `parkingGUI.c` / `parkingGUI.h`:
  - Funciones de GUI (`gui_init`, `gui_set_slot_car`, `gui_set_status`, `gui_process_frame`, `gui_set_finished`, `gui_close`).
  - La interfaz gráfica muestra el estado de los espacios: verde = libre, rojo = ocupado.

## Lógica del thread del carro
1. Llega (log de evento).
2. Intenta obtener `parking_mutex`.
3. Si `available_spaces == 0`, espera en `pthread_cond_wait(&space_available, &parking_mutex)`.
4. Reduce `available_spaces`, asigna un slot (arreglo `slot_car[]`), marca GUI.
5. Calcula tiempo de espera y actualiza estadísticas.
6. Duerme entre 1 y 5 segundos simulando estacionado.
7. Libera slot y aumenta `available_spaces`, notifica `pthread_cond_signal(&space_available)`.
8. Avisa salida, actualiza estado terminado.

## Estadísticas finales
Después de que se completan todos los carros, informa:
- total cars parked
- average wait time
- Simulation Finished

## Instrucciones
1. Entrar a la carpeta:
   `cd /006Semaphores`
2. Compilar:
   `make`
3. Ejecutar:
   `make run`
4. Limpiar:
   `make clean`
 