# 005Threads

Este proyecto es un analizador simple de logs que permite procesar un archivo `access.log` y obtener estadísticas como:

- IPs únicas
- URL más visitada
- Total de errores HTTP
- Comparativa entre ejecución single-thread y multi-thread

---

## Estructura del proyecto

- `main_linux.c` - Programa principal que carga el `access.log`, ejecuta la versión single-thread y multi-thread, y muestra resultados.
- `log_processor.c` - Lógica de parsing y conteo usando una tabla hash
- `log_processor.h` - Header con las estructuras y firmas de funciones.
- `access.log` - Ejemplo de archivo de log usado por defecto.
- `Makefile` - Reglas para compilar, ejecutar y limpiar.
- `run.sh` - Shell script

## Ejecución

```bash
./log_analyzer
```

Por defecto lee `access.log` en el mismo directorio.

También puede usar el script:

```bash
./run.sh
```

---

## Funcionamiento

1. El programa lee todas las líneas del archivo `access.log` en memoria.
2. Ejecuta dos versiones de procesamiento:
   - **Single-threaded**: procesa línea por línea en el hilo principal.
   - **Multi-threaded**: divide las líneas en `NUM_THREADS` partes y procesa cada una en un hilo distinto.
3. Para cada línea válida se extraen:
   - Dirección IP
   - URL solicitada
   - Código de estado HTTP
4. Se cuentan: IPs únicas, visitas por URL y errores HTTP.

