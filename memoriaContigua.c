/*
	Hernández García Luis Angel
	Sistemas operativos Grupo 4
	Cuarto programa: Manejo de memoria contigua
	Fecha: 20 de mayo de 2018

	Descripción: Este programa realiza el manejo de memoria contigua.
	La memoria esta constituida por bloques del mismo tamaño. La memoria
	disponible para guardar los procesos es de 3000 unidades y empieza en
	la dirección 1500 y termina en 4499. La asignación de los procesos se
	realiza usando la tecnica del peor ajuste, en la que los procesos entran
	en el hueco con el tamaño más grande. Cada vez que entra un proceso a
	memoria el espacio disponible disminuye. Cuando un proceso no cabe en
	memoria el proceso es enviado a una cola donde espera a que le sea asignado
	un espacio. La cola de procesos se atiende en dos casos
		1. Cuando un proceso en memoria libera espacio se revisa la cola, proceso
		por proceso. Los procesos que quepan en el espacio disponible salen de la
		cola y toma su lugar, el resto espera en cola.
		2. Cuando se hace la compactación de huecos. Esta acción se realiza cuando
		se detecta que hay cinco procesos en la cola de espera. La compactación 
		en tomar todos los huecos disponibles y hacerlo uno solo. Esto implica
		mover a los procesos que ya se encuentran en memoria. Cuando se realiza la
		compactacion s erevisa la cola y los procesos que quepan en memoria
		se les asigna un lugar

	Conclusiones: Este proceso además de largo, me parecio interesante, ya que he
	aplicado temas vistos anteriormente, como lo es el manejo de colas y listas
	en C, apuntadores, etc. Conla realización de este programa pude entender mejor
	como se realiza el manejo de memoria contigua y cuales son las diferentes fases
	que debe hacer el administrador de memoria para asignar espacio a los procesos
	que hay en un programa.
*/

// Bibliotecas a usar
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/*------------------------ Modulo de memoria ------------------------*/
// Estructuras necesarias para crear la memoria
typedef struct bloque {
	int inicio;
	int fin;
	char ocupado[10];
	int tamanio;
} bloquePagina;

typedef struct memoria {
	bloquePagina dato;
	struct memoria *siguiente;
	struct memoria *anterior;
} Memoria;

// función encargada de crear un bloque para un proceso
// recibe el inicio, fin, tamaño y nombre del proceso
Memoria *crearBloque(int inicio, int fin, char ocupado[10], int tamanio) {
	bloquePagina dato;
	Memoria *p;

	p = (Memoria *) malloc (sizeof(Memoria));
	
	dato.inicio = inicio;
	dato.fin = fin;
	strcpy(dato.ocupado,ocupado);
	dato.tamanio = tamanio;

	p->dato = dato;
	p->siguiente = NULL;
	p->anterior = NULL;

	return p;
}

// Esta función busca el hueco de mayor tamaño para
// realizar la tecnica del peor ajuste
Memoria *buscaHueco(Memoria *mem) {
	int max = 0;
	Memoria *indice, *mayor = NULL;
	indice = mem;

	while(indice) { // Mientras no se llegue al fin
		if(strcmp(indice->dato.ocupado,"vacio") == 0) {
			if(indice->dato.tamanio > max) {
				max = indice->dato.tamanio;
				mayor = indice;				
			}
		}
		indice = indice->siguiente;
	}
	
	return mayor;
}

// Funcion que agrega un proceso a memoria
// recibe el nombre del proceso y tamaño además de la memoria donde se asignara
int atiendeProceso(char ocupado[10], int tamanio, Memoria **mem) {
	int inicio, fin;
	Memoria *nuevo;
	Memoria *p = buscaHueco(*mem); // Se busca el hueco de mayor tamaño
	if(p != NULL) { // Si la memoria no esta completamente lleno
		if(p->dato.tamanio > tamanio) {
			// Si el tamañpo del hueco es más grande que el del proceso, se realizan
			// las operaciones para tomar el espacio necesario
			inicio = p->dato.inicio;
			fin = p->dato.inicio + tamanio - 1;
			printf("\n***Al proceso %s se asignara a un hueco con las siguientes caracteristicas: ***\n", ocupado);
			printf("Inicio: %d, fin: %d, tamanio: %d\n", p->dato.inicio, p->dato.fin, p->dato.tamanio);
			p->dato.inicio += tamanio;
			p->dato.tamanio -= tamanio;
			// creación del bloque de memoria y asignación de datos
			nuevo = crearBloque(inicio, fin, ocupado, tamanio);
			if(p->anterior == NULL) { // Si el hueco esta al principio de la memoria
				nuevo->siguiente = p;
				p->anterior = nuevo;
				*mem = nuevo;
			} else { // Cuando el hueco esta en otro lado diferente al principio
				nuevo->anterior = p->anterior;
				nuevo->siguiente = p;
				p->anterior->siguiente = nuevo;
				p->anterior = nuevo;
			}
			return 1; // Se regresa 1 como indicador de asignación de memoria 
		} else if(p->dato.tamanio == tamanio) { // Cuando el hueco y el proceso son del mismo tamaño
			printf("\n***Al proceso %s se asignara a un hueco con las siguientes caracteristicas: ***\n", ocupado);
			printf("Inicio: %d, fin: %d, tamanio: %d\n", p->dato.inicio, p->dato.fin, p->dato.tamanio);
			strcpy(p->dato.ocupado, ocupado);
			return 1;
		} else
			return 0; // Se regresa 0 cuando no se puede agregar un proceso a memoria para saber si debe de ser almacenado en cola
	} else {
		return 0;
	}
}

// Esta función realiza casi la misma acción que el anterior
// para el caso de la compactación
int atiendeProceso2(char ocupado[10], int tamanio, Memoria **mem) {
	int inicio, fin;
	Memoria *nuevo;
	Memoria *p = buscaHueco(*mem);
	if(p != NULL) {
		if(p->dato.tamanio > tamanio) {
			inicio = p->dato.inicio;
			fin = p->dato.inicio + tamanio - 1;
			p->dato.inicio += tamanio;
			p->dato.tamanio -= tamanio; 
			nuevo = crearBloque(inicio, fin, ocupado, tamanio);
			if(p->anterior == NULL) {

				nuevo->siguiente = p;
				p->anterior = nuevo;
				*mem = nuevo;
			} else {
				nuevo->anterior = p->anterior;
				nuevo->siguiente = p;

				p->anterior->siguiente = nuevo;
				p->anterior = nuevo;
			}
			return 1;
		} else if(p->dato.tamanio == tamanio) {
			strcpy(p->dato.ocupado, ocupado);
			return 1;
		} else
			return 0;
	} else {
		return 0;
	}
}

// Esta función realiza la liberación de espacio de un proceso
Memoria *liberaEspacio(char proceso[5], Memoria **mem) {
	Memoria *indice;
	indice = *mem;
	while(indice) {
		// Se busca el proceso de forma lineal
		if(strcmp(indice->dato.ocupado,proceso) == 0)
			break;
		indice = indice->siguiente;
	}
	// Cuando se encuentra el proceso se cambia a hueco
	strcpy(indice->dato.ocupado,"vacio");
	return indice;
}

// Esta función realiza la impresión de los elementos de la memoria
// con un formato especifico
void imprimeMemoria(Memoria *mem) {
	Memoria *indice;
	indice = mem;

	printf("\nproceso/hueco    inicio     fin      tamanio\n");
	printf("\n--------------------------------------------------\n");
	while(indice) {
		printf("     %s          %d      %d       %d\n",
			indice->dato.ocupado, indice->dato.inicio, indice->dato.fin, indice->dato.tamanio);
		indice = indice->siguiente;
	}
}

// Esta función imprime solo los huecos que hay en la memoria
void imprimeHuecos(Memoria *mem) {
	Memoria *indice;
	indice = mem;

	printf("\nproceso/hueco    inicio     fin      tamanio\n");
	printf("\n--------------------------------------------------\n");
	while(indice) {
		if (strcmp(indice->dato.ocupado, "vacio") == 0) {
			printf("     %s          %d      %d       %d\n",
				indice->dato.ocupado, indice->dato.inicio, indice->dato.fin, indice->dato.tamanio);
		}
		indice = indice->siguiente;
	}
}

// Esta función crea la memoria vacia,
void iniciarMemoria(Memoria **mem) {
		*mem = (Memoria *) malloc (sizeof(Memoria));
		(*mem)->dato.inicio = 1500;
		(*mem)->dato.fin = 4499;
		strcpy((*mem)->dato.ocupado, "vacio");
		(*mem)->dato.tamanio = 3000;
		(*mem)->siguiente = NULL;
		(*mem)->anterior = NULL;
}


// esta función realiza la compactación de huecos de la memoria
void compactacion(Memoria **mem) {
	Memoria *nuevo, *aux;
	iniciarMemoria(&nuevo);
	while(*mem) {
		// los huecos se van reteniendo para que se ubiquen al final de la memoria
		// tomando en cuenta que la memoria crece de arriba hacia abajo
		if(strcmp((*mem)->dato.ocupado,"vacio")) {
			atiendeProceso2((*mem)->dato.ocupado, (*mem)->dato.tamanio, &nuevo);
		}
		aux = *mem;
		*mem = (*mem)->siguiente;
		free(aux);
	}
	*mem = (Memoria *) malloc (sizeof(Memoria));
	*mem = nuevo;
}

/*-----------------------------Cola de procesos-------------------------*/

// Estructuras necesarias para la creación de la cola
typedef struct proceso {
		char nombre[10];
		int tamanio;    
} Proceso;

typedef struct nodo {
		Proceso dato;
		struct nodo *siguiente;
} Nodo;

typedef struct queue{
		Nodo *head;
		Nodo *tail;
} Cola;

// inicia la cola con valores NULL
void crearCola(Cola *cola) {
		cola->head = cola->tail = NULL;
}

// Se crea un nodo para despues agregar a la cola
// Se recibe el tamaño y nombre del proceso
Nodo *crearNodo(char nombre[10], int tamanio) {
		Proceso proc;
		Nodo *p;
		p = (Nodo *) malloc (sizeof(Nodo));
		strcpy(proc.nombre, nombre);
		proc.tamanio = tamanio;

		p->dato = proc;
		p->siguiente = NULL;
		return p;
}

// esta función agrega un proceso a la cola
// usa la función anterior
void agregarProceso(Cola *cola, char nombre[10], int tamanio) {
		Nodo *nuevo;
		// Se crea el nodo con los datos correspondientes
		nuevo = crearNodo(nombre, tamanio);
		// Se agrega el proceso a la cola, tomando en cuenta si esta vacia o no
		if(cola->head ==  NULL)
				cola->head = nuevo;
		else
				cola->tail->siguiente = nuevo;
		cola->tail = nuevo;
}

// Función para obtener un proceso de la cola (FIFO)
Nodo *obtenerProceso(Cola *cola){
		Nodo *aux;
		if(cola->head == NULL)
				aux = NULL;
		else{
				aux = cola->head;
				cola->head = cola->head->siguiente;
		}
		return aux;
}

// Devuelve la cantida de procesos que hay en la cola
int cuentaProcesosCola(Cola cola) {
		int i = 0;
		Nodo *indice;
		indice = cola.head;
		while(indice) {
				i++;
				indice = indice->siguiente;
		}
		return i;
}

//-----------------------------Programa principal

int main(int argc, char *argv[]) {
	Memoria *mem;
	iniciarMemoria(&mem); // Creación de la memoria vacía

	Cola colaProcesos;
	crearCola(&colaProcesos); // Creación de la cola de procesos vacía
	Nodo *aux;

	char linea[30], *ptr;
	char *lineaSeparada[2];
	int tamanio, ind, i;

	if(argc == 1) {
			printf("Debes ingresar el nombre del archivo\n");
			return 1;
	}

	// Apertura del archivo para lectura
	FILE *archivo;
	archivo = fopen(argv[1],"r");

	if (archivo == NULL) {
		printf("Error al abrir el archivo\n");
		exit(1);
	} else {
		while (feof(archivo) == 0) { // Se itera por cada renglon del archivo
			fgets(linea,100,archivo); // Se obtiene la linea y se guarda en una cadena
			lineaSeparada[0] = strtok(linea, " \n\t"); // Seobtiene el nombre del proceso
			lineaSeparada[1] =strtok(NULL, " \n\t"); // Se obtiene el tamaño del proceso
			if(lineaSeparada[1] != NULL) {
				tamanio = atoi(lineaSeparada[1]);
				if(tamanio > 0) { // cuando se quiere agregar un proceso amemoria
					if(!atiendeProceso(lineaSeparada[0], tamanio, &mem)){
						// Si el proceso no se pudo atender se agrega a la cola
						agregarProceso(&colaProcesos, lineaSeparada[0], tamanio);
						printf("\n*** Se agregó un proceso a cola. Situacion de la memoria ***\n");
						imprimeMemoria(mem);
						ind = cuentaProcesosCola(colaProcesos);
						if(ind >= 5){
							// Cuando hay 5 procesos en cola se hace la compactación
							// y se atienden los procesos en espera
							compactacion(&mem);
							for(i = 0; i < ind; i++) {
								aux = obtenerProceso(&colaProcesos);
								if(!atiendeProceso(aux->dato.nombre, aux->dato.tamanio, &mem))
									// Si el proceso sigue sin poder atenderse se regresa a la cola
									agregarProceso(&colaProcesos, aux->dato.nombre, aux->dato.tamanio);
							}
							printf("\n*** Nueva situación después de compactación y asignación de memoria ***\n");
							imprimeMemoria(mem);
						}
					}
				} else if(tamanio == 0) { // Cuando un proceso quiere liberar espacio
					if(liberaEspacio(lineaSeparada[0], &mem)) {
						printf("\n*** Un proceso libero espacio ***\n");
						imprimeHuecos(mem);
						ind = cuentaProcesosCola(colaProcesos);
						for(i = 0; i < ind; i++) {
							// Cuando un proceso libera espacio se consulta la cola de procesos
							// para asignar espacios
							aux = obtenerProceso(&colaProcesos);
							if(!atiendeProceso(aux->dato.nombre, aux->dato.tamanio, &mem))
									agregarProceso(&colaProcesos, aux->dato.nombre, aux->dato.tamanio);
						}
					}
				}
			}
		}
	}
	fclose(archivo);

	return 0;   
}