[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/lasAkOCG)
# Laboratorio 4 - Sincronización y comunicación

## Ejercicio 1: semáforos
Completar el programa `semaphores.c` para crear, eliminar, modificar y consultar el estado de *semáforos con nombre* (esto es, semáforos que persisten por fuera de un proceso, con un nombre asociado). En este ejercicio *tienen que crear* el semáforo con la función `sem_open()`. Utilizar las siguientes funciones:

* [`sem_open()`](https://man7.org/linux/man-pages/man3/sem_open.3.html): abre o crea un semáforo.
* [`sem_post()`](https://man7.org/linux/man-pages/man3/sem_post.3.html): realiza una operación *up* sobre el semáforo.
* [`sem_wait()`](https://man7.org/linux/man-pages/man3/sem_post.3.html): realiza una operación *down* sobre el semáforo.
* [`sem_getvalue()`](https://man7.org/linux/man-pages/man3/sem_getvalue.3.html): obtiene el valor actual de un semáforo.
* [`sem_close()`](https://man7.org/linux/man-pages/man3/sem_close.3.html): cierra un semáforo.
* [`sem_unlink()`](https://man7.org/linux/man-pages/man3/sem_unlink.3.html): elimina un semáforo del sistema.

Tendrían que poder ejecutar la siguiente serie de comandos: primero, se crea un semáforo con un valor inicial de cero. Luego, se  realiza una operación _down_, luego un _up_, y finalmente se lo elimina.

```bash
$ ./semaphores -c semaforo 0
$ ./semaphores -d semaforo &
$ ./semaphores -u semaforo
$ ./semaphores -b semaforo
$
```

## Ejercicio 2: productor-consumidor
El programa `buf.c` implementa un ejemplo de productor-consumidor haciendo uso de un _buffer limitado_. El programa no utiliza mecanismos de sincronización para el acceso al buffer. Esto ocasiona condiciones de carrera. Modificar el programa para sincronizar el acceso al buffer, empleando semáforos y _mutexs_.

Utilizar las siguientes funciones para sincronización:
* Crear un mutex: [`pthread_mutex_init()`](http://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html)
* Inicializar un semáforo: [`sem_init()`](https://man7.org/linux/man-pages/man3/sem_init.3.html)
* Tomar un semáforo: [`sem_wait()`](https://man7.org/linux/man-pages/man3/sem_wait.3.html)
* Liberar un semáforo: [`sem_post()`](https://man7.org/linux/man-pages/man3/sem_post.3.html)
* Tomar un mutex: [`pthread_mutex_lock()`](https://www.man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
* Liberar un mutex: [`pthread_mutex_unlock()`](https://www.man7.org/linux/man-pages/man3/pthread_mutex_unlock.3p.html)
* Eliminar un semáforo: [`sem_destroy()`](https://man7.org/linux/man-pages/man3/sem_destroy.3.html)
* Eliminar un mutex: [`pthread_mutex_destroy()`](https://www.man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html)

## Ejercicio 3: cena de los filósofos
El programa `philo.c` implementa un ejemplo del problema de la _cena de los filósofos_: 

1. Describir la condición de carrera que puede ocurrir durante la ejecución del programa.

2. Modificar el programa para evitar la condición de carrera mediante el uso de semáforos y _mutexs_.

3. Agregar también una solución para evitar el _bloqueo mutuo_ o _abrazo mortal_. Explicarla.

## Ejercicio 4: paso de mensajes
Crear un programa que permita intercambiar mensajes entre los usuarios del sistema, utilizando la librería de mensajes POSIX.

El programa `msgbox.c` contiene un esqueleto del programa, que procesa los siguientes parámetros pero no hace más que imprimir un mensaje:

- `-s queue msg`: envía el mensaje `msg` a la cola `queue`.
- `-r queue`: imprime el primer mensaje en `queue`.
- `-a queue`: imprime todos los mensaje en `queue`.
- `-l queue`: el programa espera por mensajes en `queue`.
- `-c queue`: crea la cola de mensaje `queue`.
- `-d queue`: elimina la cola de mensajes `queue`.
- `-i queue`: imprime información de la cola de mensajes `queue` (máximo número de mensajes, tamaño de los mensajes, número actual de mensajes en la cola).

La estructura de datos que representa un mensaje es `struct msg`, que contiene el nombre del usuario que envía el mensaje (`sender`) y el mensaje (`text`). Notar que lo que se debe enviar como mensaje es la **estructura completa**.

```c
#define USERNAME_SIZE   15
#define TXT_SIZE        100

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg {
    char sender[USERNAME_SIZE];
    char text[TXT_SIZE];
};

typedef struct msg msg_t;
```

Para crear un buzón de mensajes, enviar y recibir mensajes, usar las siguientes funciones:

* [`mq_open()`](http://man7.org/linux/man-pages/man3/mq_open.3.html): crea una nueva cola de mensajes o abre una ya existente.
* [`mq_send()`](http://man7.org/linux/man-pages/man3/mq_send.3.html): envía un mensaje a la cola de mensajes.
* [`mq_receive()`](http://man7.org/linux/man-pages/man3/mq_receive.3.html): recibe un mensaje.
* [`mq_close()`](http://man7.org/linux/man-pages/man3/mq_close.3.html): cierra el descriptor de una cola de mensajes.
* [`mq_unlink()`](http://man7.org/linux/man-pages/man3/mq_unlink.3.html): elimina una cola de mensajes.
* [`getlogin_r`](https://www.man7.org/linux/man-pages/man3/getlogin.3.html): obtiene el nombre del usuario.

## Ejercicio 5: memoria compartida
Vamos a armar un _canvas_, un espacio de memoria compartida cuyo contenido puede ser modificado por distintos usuarios. Este espacio es un arreglo de 25x25 caracteres, sobre el cual se pueden escribir palabras en alguna posición. La estructura de datos que representa el _canvas_ es `canvas_t`:

```c
#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra

struct canvas {
    sem_t mutex;
    char canvas[HEIGHT*WIDTH];
};

typedef struct canvas canvas_t;
```

Para crear y acceder al _canvas_ necesitamos utilizar memoria compartida. Para lograrlo, usaremos el API de POSIX para crear y utilizar segmentos de memoria compartida. Mediante estos segmentos, diferentes procesos pueden intercambiar datos de una manera más rapida que mediante el uso de mensajes. El manual [`shm_overview`](http://man7.org/linux/man-pages/man7/shm_overview.7.html) tiene una introducción al API de memoria compartida de POSIX.

Completar el programa `canvas.c`, de manera que se puedan crear, eliminar, imprimir y modificar estos _canvas_. Las principales funciones que vamos a usar son:

* [`shm_open()`](http://man7.org/linux/man-pages/man3/shm_open.3.html): crea un nuevo objeto de memoria compartida, o abre uno ya existente.
* [`ftruncate()`](http://man7.org/linux/man-pages/man2/ftruncate.2.html): cambia ("trunca") el tamaño del segmento de memoria compartida.
* [`mmap()`](http://man7.org/linux/man-pages/man2/mmap.2.html): mapea el segmento de memoria compartida indicado dentro del espacio de direcciones del proceso.
* [`close()`](http://man7.org/linux/man-pages/man2/close.2.html): cierra el descriptor de un segmento de memoria compartida.
* [`shm_unlink()`](http://man7.org/linux/man-pages/man3/shm_unlink.3.html): elimina el segmento de memoria compartida indicado.

El canvas debe ser accedido sólo por un proceso a la vez, sea para lectura o escritura. Para el control de acceso, se debe utilizar un semáforo al que se accede mediante la variable `mutex` de la estructura. El proceso que crea el canvas debe inicializar este semáforo utilizando [`sem_init()`](https://man7.org/linux/man-pages/man3/sem_init.3.html)

---

¡Fin del Laboratorio 4!
