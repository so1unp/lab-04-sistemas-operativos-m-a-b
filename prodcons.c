// prodcons.c 
// Ejemplo de productor-consumidor sobre un buffer compartido
//
// Usa 3 semáforos:
//    mutex: Para proteger exclusión mutua de acceso al buffer compartido
//    llenos: Para contar los items que se han llenado en el buffer
//    vacios: Para contar las ranuras vacías del buffer
// 
// El productor produce caracteres del abecedario, en orden
// El consumidor los consume y muestra en pantalla
//
// Recibe tres parámetros
//  argv[1] tiempo en segundos que el productor tarda en producir
//  argv[2] tiempo en segundos que el consumidor tarda en consumir
//  argv[3] Máximo de items a producir
//
// Autor: CB - UNPSJB 
// 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <pthread.h> 
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 5  // tamaño del buffer
char buffer[BUFFER_SIZE]; // buffer de lectura y escritura 

//declara las funciones para ejecutar productor y consumidor
void* productor(void*);
void* consumidor(void*);

//declara los semáforos
sem_t *mutex;
sem_t *vacios;
sem_t *llenos;

int timeprod, timecons, maxprod;

int main(int argc, char** argv) 
{
    pthread_t prod_t;  // hilo escritor
    pthread_t cons_t;  // hilo lector
    int i;
    int retprod, retcons;

   // Controla parametros de entrada
   if (argc<4)
   {   perror("uso: prodcons <tiempoProd> <tiempoCons> <maxProd> \n"); exit(1); }

   timeprod = atoi(argv[1]);
   timecons = atoi(argv[2]);
   maxprod = atoi(argv[3]);

   printf("Inicio....%i,%i,%i\n",timeprod,timecons,maxprod);

   /* Crea los semaforos */
   if((mutex=sem_open("Smutex", O_CREAT, 0644, 1))==(sem_t *)-1)
   {   perror("No se puede crear mutex"); exit(1); }
   printf("Mutex creado....\n");

   if((vacios=sem_open("Svacios", O_CREAT, 0644, 1))==(sem_t *)-1)
   {   perror("No se puede crear vacios"); exit(1); }
    //inicializa vacios = BUFFER_SIZE;
    for(i = 0; i < BUFFER_SIZE-1; i++) {
     sem_post(vacios);        
    }
   printf("Svacios creado....\n");

   if((llenos=sem_open("Sllenos", O_CREAT, 0644, 1))==(sem_t *)-1)
   {   perror("No se puede crear llenos"); exit(1); }
    //inicializa llenos = 0;
    sem_wait(llenos);
   printf("Sllenos creado....\n");

   /* Crea y dispara los threads de productor y consumidor */
   pthread_create(&prod_t, NULL, productor, NULL);  // crea el hilo escritor
   pthread_create(&cons_t, NULL, consumidor, NULL);  // crea el hilo lector

   printf("Threads creados....\n");

   // esperamos que cada hilo finalice
   pthread_join(cons_t, (void*) &retprod);
//   pthread_join(cons_t, (void*) &retcons);  
    

   printf("Destruye semaforos....\n");
   //destruimos semaforos
   sem_close(mutex);
   sem_unlink("Smutex");
   sem_close(llenos);
   sem_unlink("Sllenos");
   sem_close(vacios);
   sem_unlink("Svacios");

   printf("FIN PRODCONS\n");
   pthread_exit(0);

}

/* **** PRODUCTOR ****/
void* productor(void* p) 
{
    int gen = 0;  // item a producir
    char item = '@';

    printf("Entra el productor\n");
    while (gen < maxprod) {
      
      //produce un item
      item = (char) ((int) item)+1;
      sleep(timeprod);

      //lo pone en el buffer
      sem_wait(vacios);
      sem_wait(mutex);
      printf("Produce %c y lo coloca en %i\n",item,gen%BUFFER_SIZE);
      buffer[gen%BUFFER_SIZE] = item;
      sem_post(mutex);
      sem_post(llenos);
      gen++;
      
    }
    printf("fin del productor \n");
    return 0;
}

/* **** CONSUMIDOR ****/
void* consumidor(void* p) 
{
    int con = 0;  // item consumidos
    char item;

    printf("Entra el consumidor\n");
    while (con < maxprod) {
     
      //retira el item
      sem_wait(llenos);
      sem_wait(mutex);
      item = buffer[con%BUFFER_SIZE];
      sem_post(mutex);
      sem_post(vacios);

      //consume el item
      printf("Consume %c\n",item);
      sleep(timecons);
      con++;

    }
    printf("fin del consumidor \n");
    return 0;
}
