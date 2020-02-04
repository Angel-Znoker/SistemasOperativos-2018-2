/*
	Hernandez Garcia Luis Angel
	Sistemas Operativos Grupo 4
	Primer programa: Interprete de comandos (Shell)
	Fecha: 5 de marzo de 2018

	Descripción: Este programa realiza llamadas al sistema para ejecutar comandos
	de Linux. Para realizalo se usa el comando execvp(argv[0], argv). El programa
	se ejecuta con un while hasta que el usuario ingrese la palabra 'salir'. Cuando
	una cadena diferente a 'salir' es leida, esta se separa por los espacios que
	contiene dicha cadena y se pasa como argumento a execvp para su validacion y
	ejecucion como comando.

	Conclusiones: Al realizar el programa tuve algunos problemas al usar apuntadores
	para manejar los strings los cuales fueron solucionados buscando documentacion
	de sobre el tema. En el caso de la funcion execvp(), una vez que se pudo separar
	la entrada por espacios no se encontraron problemas al pasar los argumentos a la
	funcion. 
*/

// Bibliotecas a utilizar
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "string.h"

// Prototipo de la funcion que separa el comando ingresado
void  separarComando(char *, char **);

// FUNCION PRINCIPAL
int main() {
	char comando[30];
	char *comandoSeparado[15];

	printf("Oprima ENTER para continuar ...");

	while(1) {
		getchar(); // Este getchar se usa para limpiar el buffer
		printf("\nmiShell_# ");
		// Para leer la cadena con espacios se uso la funcion printf()
		// con la expresion regular ^\n para que lea hasta que
		// encuentre un salto de linea
		scanf("%[^\n]",comando);

		separarComando(comando, comandoSeparado);

		if (!strcmp(comandoSeparado[0], "salir"))
			// Si el usuario ingresa 'salir' se rompe el ciclo y termina el programa
			return 0;

		if(fork()) // Creacion del proceso hijo
			wait(NULL); // El padre espera a que el hijo termine
		else {
			execvp(comandoSeparado[0], comandoSeparado); // Llamada a los comandos
			printf("ERROR: revise el comando ingresado\n");
			exit(0); // Cuando el comando no se encuentra se termina la ejecución del proceso hijo
		}
	}

	return 0;
}

// Esta funcion se encarga de separar el comando ingresado por espacios
void  separarComando(char *comando, char **comandoSeparado) {
	while (*comando != '\0') { // Mientras no se llegue a fin de linea
		while (*comando == ' ' || *comando == '\t' )
			*comando++ = '\0'; // Se reemplazan los espacios por el caracter nulo

		*comandoSeparado++ = comando; // Se guarda la posicion de la palabra encontrada
		while (*comando != '\0' && *comando != ' ' && *comando != '\t' && *comando != '\n') 
			comando++; // Se recorre la cadena hasta el siguiente espacio
	}
	*comandoSeparado = '\0'; // Fin de la lista
}