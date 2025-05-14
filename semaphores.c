#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

// Abrir/crear un semáforo
sem_t *sem_operation(char *name, int mode, int value);

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s [OPCION] [PARAMETRO]\n", argv[0]);
    fprintf(stderr, "Opciones:\n");
    fprintf(stderr, "\t-c semaforo valor\tcrea semáforo con el nombre y valor inicial indicado.\n");
    fprintf(stderr, "\t-u semaforo\t\trealiza un UP en el semáforo indicado.\n");
    fprintf(stderr, "\t-d semaforo\t\trealiza un DOWN en el semaforo indicado.\n");
    fprintf(stderr, "\t-b semaforo\t\telimina el semáforo indicado.\n");
    fprintf(stderr, "\t-i semaforo\t\tinformación del semáforo indicado.\n");
    fprintf(stderr, "\t-h\t\t\timprime este mensaje.\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-')
    {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];

    // Variable para el semáforo
    sem_t *sem;

    switch (option)
    {
    // Crear un semáforo
    case 'c':
        /* Crea el semáforo nombrado y luego lo cierra */
        sem_close(sem_operation(argv[2], O_CREAT, atoi(argv[3])));
        printf("Semáforo %s creado con valor inicial %s\n", argv[2], argv[3]);
        break;
    case 'u':
        /* Abro el semaforo */
        sem = sem_operation(argv[2], O_RDWR, 0);
        sem_post(sem);
        printf("UP en el semáforo %s\n", argv[2]);
        /* Cierro el semáforo */
        sem_close(sem);
        break;
    case 'd':
        /* Abro el semáforo */
        sem = sem_operation(argv[2], O_RDWR, 0);
        printf("DOWN en el semáforo %s\n", argv[2]);
        sem_wait(sem);
        /* Cierro el semáforo */
        sem_close(sem);
        break;
    case 'b':
        /* Elimina el semáforo */
        if (sem_unlink(argv[2]) == -1)
        {
            perror("Error al eliminar el semáforo");
            exit(EXIT_FAILURE);
        }
        printf("Semáforo %s eliminado\n", argv[2]);
        break;
    case 'i':
        /* Abro el semáforo */
        sem = sem_operation(argv[2], O_RDWR, 0);
        int value;
        if (sem_getvalue(sem, &value) == -1)
        {
            perror("Error al obtener el valor del semáforo");
            exit(EXIT_FAILURE);
        }
        printf("Valor del semáforo %s: %d\n", argv[2], value);
        /* Cierro el semáforo */
        sem_close(sem);
        break;
    case 'h':
        usage(argv);
        break;
    default:
        fprintf(stderr, "Opción desconocida: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

sem_t *sem_operation(char *name, int mode, int value)
{
    sem_t *sem;
    if ((sem = sem_open(name, mode, 0644, value)) == (sem_t *)-1)
    {
        perror("No se puede crear o abrir el semáforo");
        exit(1);
    }
    return sem;
}