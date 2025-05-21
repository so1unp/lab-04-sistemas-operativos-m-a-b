# Laboratorio 4 - Respuestas Amarillo Bonansea Medina
## Ejercicio 3:

### 1. Describir la condición de carrera que puede ocurrir durante la ejecución del programa.
La condición de carrera en el problema de la cena de los filósofos ocurre cuando múltiples filósofos intentan acceder simultáneamente a los mismos recursos compartidos (los tenedores) sin ningún mecanismo de sincronización. Específicamente:

Cada filósofo necesita dos tenedores para comer (el tenedor a su izquierda y el tenedor a su derecha).
Si todos los filósofos toman primero su tenedor izquierdo al mismo tiempo, ninguno podrá tomar el segundo tenedor (derecho) porque ya estará ocupado por otro filósofo.
Esta situación provoca una condición de carrera donde el resultado depende del orden exacto en que se ejecutan las operaciones, lo que puede llevar a un deadlock (todos los filósofos sosteniendo un tenedor y esperando eternamente por el segundo).

### 2. Modificar el programa para evitar la condición de carrera mediante el uso de semáforos y _mutexs_.

En el programa `philo.c`, se utilizaron mutex para evitar la condición de carrera en el acceso a los tenedores:

1. Se creó un array de mutex donde cada elemento representa un tenedor:
   ```c
   pthread_mutex_t *mutex[N];
   ```

2. Se inicializaron los mutex para cada tenedor en la función `initialize_sem()`:
   ```c
   void initialize_sem()
   {
       for (int i = 0; i < N; i++)
       {
           mutex[i] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
           if (mutex[i] == NULL)
           {
               fprintf(stderr, "Error al asignar memoria para mutex[%d]\n", i);
               exit(EXIT_FAILURE);
           }
           pthread_mutex_init(mutex[i], NULL);
       }
   }
   ```

3. Cuando un filósofo quiere tomar un tenedor, debe obtener el mutex correspondiente:
   ```c
   pthread_mutex_lock(mutex[f[i]]);
   ```

4. Cuando un filósofo termina de comer, libera los mutex de los tenedores:
   ```c
   pthread_mutex_unlock(mutex[f[0]]);
   pthread_mutex_unlock(mutex[f[1]]);
   ```


### 3. Agregar también una solución para evitar el _bloqueo mutuo_ o _abrazo mortal_. Explicarla.

Para evitar el deadlock, el programa implementa una estrategia asimétrica en la adquisición de tenedores:

```c
// los tenedores a tomar
if (id % 2 == 0)
{
    // Filósofo par toma tenedor derecho primero
    f[0] = (id + 1) % N; // Tenedor derecho
    f[1] = id;           // Tenedor izquierdo
}
else
{
    // Filósofo impar toma tenedor izquierdo primero
    f[0] = id;           // Tenedor izquierdo
    f[1] = (id + 1) % N; // Tenedor derecho
}
```

La solución funciona de la siguiente manera:

- Los filósofos con ID par (0, 2, 4) intentan primero tomar el tenedor derecho y luego el izquierdo.
- Los filósofos con ID impar (1, 3) intentan primero tomar el tenedor izquierdo y luego el derecho.

Esta asimetría evita la situación de bloqueo mutuo porque rompe el ciclo donde todos los filósofos toman un tenedor simultáneamente. Al menos un filósofo podrá tomar ambos tenedores (generalmente el que tiene un ID diferente al resto) lo que eventualmente permitirá que los demás filósofos también puedan comer.