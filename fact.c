/*
	Hernandez Garcia Luis Angel
	Sistemas Operativos Grupo 4
	Tarea opcional: Factorial de un numero con hilos
	Fecha: 18 de marzo de 2018

	Descripcion: En este programa se realiza el calculo del factorial de
	un numero (maximo hasta 5), utilizando la biblioteca "pthread". El
	numero sera leido por el hilo principal, el factorial lo calculara el
	hilo secundario y el resultado sera mostrado por el principal.

	Conclusion: Con este programa se puede observar de manera practica lo
	visto en clase sobre el manejo de hilos para la ejecucion de procesos,
	ya que este tema es importante para la programacion en la actualidad.
*/

// Bibliotecas necesarias
#include "pthread.h"
#include "stdio.h"
#include "string.h"

// prototipo de funcion
void *factorial(void *);

// FUNCION PRINCIPAL
int main() {
	pthread_t hilo;
	int num, *salida;
	char opt[10];

	do {
		printf("\nEscribe un numero entero para calcular su factorial (maximo 5): ");
		scanf("%d", &num);

		int num2 = num; // Respaldo del numero

		if (num > 5)
			printf("\nNumero invalido intente con otro.\n");
		else {
			if(pthread_create(&hilo, NULL, factorial, &num)) // Creacion del hilo secundario
				printf("\nError al crear el hilo");
			else if(pthread_join(hilo, (void **)&salida)) // Union de hilos y recibimiento del dato
				printf("\nError al recibir el dato");
			else
				printf("\nEl factorial de %d es %d\n", num2, *salida); // impresion de resultado
		}

		printf("\nQuiere continuar con otro numero? (S/N): ");
		scanf("%s", opt);
		if(!strcmp(opt, "n") || !strcmp(opt, "N"))
			return 0; // Fin de ciclo

	} while(1);
	return 0;
}


// Funcion que calcula el factorial
void *factorial(void *num) {
	int i, fact = 1;

	// calculo
	for (i = 0; i < *(int *)num; ++i)
		fact *= (i + 1);

	*(int *)num = fact;
	pthread_exit(num); // Fin del hilo y envio de resultado
}