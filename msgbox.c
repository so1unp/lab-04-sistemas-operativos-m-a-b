#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>

#define USERNAME_MAXSIZE 20 // Máximo tamaño en caracteres del nombre del remitente.
#define TXT_SIZE 100        // Máximo tamaño del texto del mensaje.

mqd_t open_mailbox(char *queue, int mode);

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg
{
    char sender[USERNAME_MAXSIZE];
    char text[TXT_SIZE];
};

typedef struct msg msg_t;

/**
 * Imprime información acerca del uso del programa.
 */
void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
    fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");
    fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
    fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");
    fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
    fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
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

    // Variable para la cola de mensajes
    mqd_t queue;

    switch (option)
    {
    case 's':
        printf("Enviar %s a la cola %s\n", argv[3], argv[2]);

        // Crear y preparar el mensaje
        msg_t msg1;
        memset(&msg1, 0, sizeof(msg1));

        // Cargar el nombre de usuario en el mensaje
        getlogin_r(msg1.sender, USERNAME_MAXSIZE);

        // Cargar el mensaje
        strncpy(msg1.text, argv[3], TXT_SIZE - 1);

        // Abrimos la cola de mensajes
        queue = open_mailbox(argv[2], O_WRONLY);

        printf("username: %s\n", msg1.sender);
        printf("message: %s\n", msg1.text);

        // Enviar el mensaje
        mq_send(queue, (const char *)&msg1, sizeof(msg1), 0);

        // Cerrar la cola de mensajes
        mq_close(queue);
        break;

    case 'r':
        printf("Recibe el primer mensaje en %s\n", argv[2]);

        // Abrir la cola de mensajes
        queue = open_mailbox(argv[2], O_RDONLY);

        // Recibir el mensaje
        struct msg msg2;
        memset(&msg2, 0, sizeof(msg2));

        mq_receive(queue, (char *)&msg2, sizeof(msg2), NULL);

        printf("%s: %s\n", msg2.sender, msg2.text);
        mq_close(queue);
        break;
    case 'a':
        printf("Imprimer todos los mensajes en %s\n", argv[2]);
        break;
    case 'l':
        printf("Escucha indefinidamente por mensajes\n");
        break;
    case 'c':
        printf("Crea la cola de mensajes %s\n", argv[2]);
        // Crear la cola de mensajes
        open_mailbox(argv[2], O_CREAT);
        break;
    case 'd':
        printf("Borra la cola de mensajes %s\n", argv[2]);
        // Eliminar la cola de mensajes
        mq_unlink(argv[2]);
        break;
    case 'h':
        usage(argv);
        break;
    default:
        fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

/**
 * Abre o crea la cola de mensajes.
 * @param queue Nombre de la cola de mensajes.
 * @param mode Modo de apertura de la cola de mensajes.
 */
mqd_t open_mailbox(char *queue, int mode)
{
    return mq_open(queue, mode, 0664, NULL);
}
