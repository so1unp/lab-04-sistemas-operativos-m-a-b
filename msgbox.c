#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>

#define USERNAME_MAXSIZE 30 // Máximo tamaño en caracteres del nombre del remitente.
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
    fprintf(stderr, "\t-i queue: imprime información de la cola de mensajes queue.\n");
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
    // ---- Enviar un mensaje ----
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

        break;

    // ---- Recibir un mensaje ----
    case 'r':
        printf("Recibe el primer mensaje en %s\n", argv[2]);

        // Abrir la cola de mensajes
        queue = open_mailbox(argv[2], O_RDONLY);

        // Recibir el mensaje
        struct msg msg2;
        memset(&msg2, 0, sizeof(msg2));

        mq_receive(queue, (char *)&msg2, sizeof(msg2), NULL);

        printf("%s: %s\n", msg2.sender, msg2.text);

        break;

    // ---- Imprimir todos los mensajes ----
    case 'a':
        printf("Imprimir todos los mensajes en %s\n", argv[2]);

        // Abrir la cola de mensajes
        queue = open_mailbox(argv[2], O_RDONLY);

        // Obtener atributos de la cola para saber cuántos mensajes hay
        struct mq_attr attr;
        mq_getattr(queue, &attr);

        printf("Mensajes en la cola (%ld):\n", attr.mq_curmsgs);

        // Leer todos los mensajes disponibles
        for (int i = 0; i < attr.mq_curmsgs; i++)
        {
            struct msg msg_temp;
            memset(&msg_temp, 0, sizeof(msg_temp));

            if (mq_receive(queue, (char *)&msg_temp, sizeof(msg_temp), NULL) != -1)
            {
                printf("%s: %s\n", msg_temp.sender, msg_temp.text);
            }
        }
        break;

    // ---- Escuchar mensajes ----
    case 'l':
        printf("Escucha indefinidamente por mensajes en %s\n", argv[2]);

        // Abrir la cola de mensajes
        queue = open_mailbox(argv[2], O_RDONLY);

        printf("Esperando mensajes... (Ctrl+C para salir)\n");

        // Loop infinito para escuchar mensajes
        while (1)
        {
            struct msg msg_listen;
            memset(&msg_listen, 0, sizeof(msg_listen));

            // Recibir mensaje (bloquea hasta que llegue uno)
            if (mq_receive(queue, (char *)&msg_listen, sizeof(msg_listen), NULL) != -1)
            {
                printf("Nuevo mensaje de %s: %s\n", msg_listen.sender, msg_listen.text);
            }
        }
        break;

    // ---- Crear cola ----
    case 'c':
        printf("Crea la cola de mensajes %s\n", argv[2]);
        // Crear la cola de mensajes
        open_mailbox(argv[2], O_CREAT);
        break;

    // ---- Eliminar cola ----
    case 'd':
        printf("Borra la cola de mensajes %s\n", argv[2]);
        // Eliminar la cola de mensajes
        mq_unlink(argv[2]);
        break;

    // ---- Imprimir información de la cola ----
    case 'i':
        printf("Imprimir información de la cola de mensajes %s\n", argv[2]);

        // Abrir la cola de mensajes
        queue = open_mailbox(argv[2], O_RDONLY);

        // Obtener atributos de la cola
        struct mq_attr attr_info;
        if (mq_getattr(queue, &attr_info) == 0)
        {
            printf("Atributos de la cola %s:\n", argv[2]);
            printf("  mq_flags: %ld\n", attr_info.mq_flags);
            printf("  mq_maxmsg: %ld\n", attr_info.mq_maxmsg);
            printf("  mq_msgsize: %ld\n", attr_info.mq_msgsize);
            printf("  mq_curmsgs: %ld\n", attr_info.mq_curmsgs);
        }
        else
        {
            perror("Error al obtener atributos de la cola");
        }
        break;

    // ---- Mostrar uso del programa ----
    case 'h':
        usage(argv);
        break;

    default:
        fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Cerrar la cola de mensajes (solo si fue abierta)
    if (option != 'd' && option != 'h')
    {
        mq_close(queue);
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
    // Para crear una cola se necesitan más flags
    if (mode & O_CREAT)
    {
        struct mq_attr attr;
        attr.mq_flags = 0;               // sin flags especiales
        attr.mq_maxmsg = 10;             // máximo 10 mensajes
        attr.mq_msgsize = sizeof(msg_t); // tamaño de cada mensaje
        attr.mq_curmsgs = 0;             // número actual de mensajes (inicialmente 0)

        return mq_open(queue, O_CREAT | O_RDWR, 0664, &attr);
    }
    return mq_open(queue, mode, 0664, NULL);
}
