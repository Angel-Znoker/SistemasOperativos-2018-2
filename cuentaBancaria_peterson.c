/*
	Hernández García Luis Angel
	Sistemas Operativos Grupo 4
	Segundo programa: Algoritmo de Peterson con hilos
	Fecha: 04/04/2018

	Descripción: Este programa implementa el algoritmo de Peterson para simular las operaciones
	de deposito y retiro a una cuenta bancaria. Para ello se poya del uso de hilos. Se usan
	primitivas de entrada y de salida para entrar a la region critica en este caso el saldo
	disponible de la cuenta.

	Conclusiones: Con este ejercicio pude comprender de una forma más ilustrativa el uso del
	algoritmo de Peterson y de las primitivas de entrada y salida. De esta forma comprendi mejor
	su implementacion, haciendo uso de un ejemplo sencillo.
*/

// Bibliotecas usadas
#include "pthread.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "unistd.h"

int proc_fav,
	proc1wantIn,
	proc2wantIn;

float saldo = 1000.0; // se da un saldo inicial

// Depsoito: preguntar, sumar y mostrar el saldo
void actividades_de_deposito(float *deposito) {
	printf("\nCual es la cantidad a depositar: ");
	scanf("%f", deposito);
	saldo += *deposito;
	printf("\nSaldo actual: %f\n\n", saldo);
	fflush(stdin);
}

// Retirar: preguntar, restar y mostrar saldo
int actividades_de_retiro(float *retiro){
	printf("\nCual es la cantidad a retirar: ");
	scanf("%f", retiro);
	if(*retiro < saldo) // Si hay saldo suficiente
		saldo -= *retiro;
	else
		printf("\nSaldo insuficiente para retiro");
	printf("\nSaldo actual: %f\n\n", saldo);
	fflush(stdin);
}

// Primer proceso concurrente
void *proc_deposito() {
	srand(time(NULL));
	int random = rand() % 3 + 1;
	float deposito;
	do {
		proc1wantIn = 1; // Primitiva de entrada
		proc_fav = 2; // Primitiva de entrada
		while(proc2wantIn && proc_fav == 2);
		actividades_de_deposito(&deposito);
		proc1wantIn = 0; // Primitiva de salida
	} while(deposito != 0.0);
	sleep(random); // otras tareas
	pthread_exit(NULL);
}

// Segundo proceso concurrente
void *proc_retiro() {
	srand(time(NULL));
	int random = rand() % 3 + 1;
	float retiro;
	do {
		proc2wantIn = 1; // primitiva de entrada
		proc_fav = 1; // Primitiva de entrada
		while(proc1wantIn && proc_fav == 1);
		actividades_de_retiro(&retiro);
		proc2wantIn = 0; // primitiva de salida
	} while(retiro != 0.0);
	sleep(random); // otras tareas
	pthread_exit(NULL);
}

// FUNCION PRINCIPAL
int main() {

	// Creacion de hilos
	pthread_t hilo_dep, hilo_ret;
	int error;

	// Llamada al primer proceso
	error = pthread_create(&hilo_dep, NULL, proc_deposito, NULL);
	if(error) printf("\nError al ingresar a la cuenta.\n");

	// Llamada al segundo proceso
	error = pthread_create(&hilo_ret, NULL, proc_retiro, NULL);
	if(error) printf("\nError al ingresar a la cuenta.\n");

	// Cierre del primer proceso
	error = pthread_join(hilo_dep, NULL);
	if(error) printf("\nError al cerrar la cuenta.\n");

	// Cierre del segundo proceso
	error = pthread_join(hilo_ret, NULL);
	if(error) printf("\nError al cerrar la cuenta.\n");

	return 0;
}