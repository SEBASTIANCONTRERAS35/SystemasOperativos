/**
  * AUTORES:    Bruno Gabriel Carrada Alarcon
  *             Calzontzi Hernandez Yaretzi
  *             Contreras Colmenero Emilio Sebastian        
  * FECHA DE ELABORACIÃ“N: 26/06/2024
  * PROPÃ“SITO: SimulaciÃ³n del manejo de saldo en una cuenta bancaria 
    usando hilos POSIX en C.
  **/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SALDO_INICIAL 100000
#define SALDO_MINIMO 0
#define SALDO_MAXIMO 1000000
#define NUM_OPERACIONES 10000
#define NUM_MONTOS 100

// Variables globales 
int saldo = SALDO_INICIAL;
int peticion = 0;
int respuesta = 0;
int operaciones_exitosas_servicio = 0;
int operaciones_fallidas_servicio = 0;
int operaciones_exitosas_cliente = 0;
int operaciones_fallidas_cliente = 0;

// Mutex para controlar el acceso a las variables compartidas
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Funciones de los hilos con sus respectivos nombres 
void* servicio(void* arg);
void* cliente(void* arg);

// Montos de prueba 
int montos[NUM_MONTOS] = 
{1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -5000, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500, 
1000, -500, 2000, -1500, 3000, -2500, 4000, -3500, 5000, -4500};

int main() {
    pthread_t hilo_servicio, hilo_cliente;
    
    // Creamos los dos hilos
    pthread_create(&hilo_servicio, NULL, servicio, NULL);
    pthread_create(&hilo_cliente, NULL, cliente, NULL);

    // Esperar a que el cliente termine
    pthread_join(hilo_cliente, NULL);
    
    // Mnadamos los resultados a pantalla
    printf("Operaciones exitosas del cliente: %d\n", operaciones_exitosas_cliente);
    printf("Operaciones fallidas del cliente: %d\n", operaciones_fallidas_cliente);
    printf("Operaciones exitosas del servicio: %d\n", operaciones_exitosas_servicio);
    printf("Operaciones fallidas del servicio: %d\n", operaciones_fallidas_servicio);
    
    // Terminar el servicio
    pthread_cancel(hilo_servicio);
    
    return 0;
}

void* servicio(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        if (peticion != 0) {
            int nuevo_saldo = saldo + peticion;
            if (nuevo_saldo >= SALDO_MINIMO && nuevo_saldo <= SALDO_MAXIMO) {
                saldo = nuevo_saldo;
                respuesta = 1;  // OperaciÃ³n exitosa
                operaciones_exitosas_servicio++;
            } else {
                respuesta = -1;  // OperaciÃ³n fallida
                operaciones_fallidas_servicio++;
            }
            peticion = 0;  // Reseteamos la peticiÃ³n para esperar una nueva
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* cliente(void* arg) {
    for (int i = 0; i < NUM_OPERACIONES; i++) {
        pthread_mutex_lock(&mutex);
        peticion = montos[i % NUM_MONTOS];
        respuesta = 0;  // Valor especial para esperar la respuesta
        pthread_mutex_unlock(&mutex);

        // Esperar respuesta del servicio
        while (respuesta == 0);

        if (respuesta == 1) {
            operaciones_exitosas_cliente++;
        } else {
            operaciones_fallidas_cliente++;
        }
    }
    return NULL;
}
