#include "pq.h"
#include "confirm.h"

#include <stdlib.h>


/* Crea la cola de prioridad e inicializa sus atributos
retorna un puntero a la cola de prioridad 
retorna NULL si hubo error*/
PQ* pq_create() {

	PQ* pq = malloc(sizeof(struct Heap));
	/* AGREGUE SU CODIGO AQUI */
	CONFIRM_NOTNULL(pq, NULL);
	pq->cap = 10;
	pq->size = 0;
	pq->arr = (PrioValue*)malloc(sizeof(struct _PrioValue) * 10);
	if (NULL == pq->arr) {
		free(pq);
		return NULL;
	}
	return pq;
}

/*
Redimensiona el arreglo de la cola de prioridad
multiplicando su capacidad * 2
*/
int _redim(PQ* pq) {
	PrioValue* arr = NULL;
	int i = 0;
	if (NULL == pq) return 0;
	arr = (malloc(sizeof(struct _PrioValue) * pq->cap * 2));
	if (arr == NULL) return 0;
	for (i = 0; i < pq->cap; i++) {
		arr[i].prio = pq->arr[i].prio;
		arr[i].value = pq->arr[i].value;
	}
	free(pq->arr);
	pq->arr = arr;
	pq->cap = pq->cap * 2;
	return 1;
}

/*
Agrega un valor a la cola con la prioridad dada

retorna TRUE si tuvo exito, FALSE si no
*/
BOOLEAN pq_add(PQ* pq, void* valor, int prioridad) {
   
	/* AGREGUE SU CODIGO AQUI */
	CONFIRM_TRUE(pq, FALSE);
	PrioValue* hijo = NULL;
	PrioValue* padre = NULL;
	int i_actual, i_padre;
	/* si el tama�o es igual a la capacidad-1, est� lleno */
	if (pq->size == pq->cap - 1 && !_redim(pq)) return FALSE;
	/* se agrega al final del arreglo */
	pq->size++;
	pq->arr[pq->size].prio = prioridad;
	pq->arr[pq->size].value = valor;
	i_actual = pq->size;
	i_padre = i_actual / 2;
	/* si tiene menor prioridad flota hacia arriba */
	while (i_padre >= 1 && pq->arr[i_padre].prio > pq->arr[i_actual].prio) {
		padre = &pq->arr[i_padre];
		hijo = &pq->arr[i_actual];

		if (hijo->prio < padre->prio) {
			int prio = padre->prio;
			void* value = padre->value;
			pq->arr[i_padre].prio = hijo->prio;
			pq->arr[i_padre].value = hijo->value;
			pq->arr[i_actual].prio = prio;
			pq->arr[i_actual].value = value;
		}
		i_actual = i_padre;
		i_padre = i_actual / 2;
	}
	return TRUE;
}

/* 
  Saca el valor de menor prioridad (cima del monticulo) y lo guarda en la posicion retVal (paso por referencia)
  retorna FALSE si tiene un error
  retorna TRUE si tuvo EXITO
*/
BOOLEAN pq_remove(PQ* pq, void** retVal) {

	/* AGREGUE SU CODIGO AQUI */
	CONFIRM_TRUE(pq, FALSE);
	/* Variables */
	PrioValue* actual = NULL;
	PrioValue* hizq = NULL;
	PrioValue* hder = NULL;
	PrioValue* tmp = NULL;
	int tam;
	int i = 1;

	*retVal = pq->arr[i].value;
	pq->arr[i] = pq->arr[pq->size];
	pq->size--;

	/* empieza desde la raiz */
	while ((2 * i) <= pq->size) {
		actual = &pq->arr[i];
		hizq = &pq->arr[2 * i];
		hder = &pq->arr[(2 * i) + 1];

		if (hizq->prio < hder->prio) {
			int prio = actual->prio;
			void* value = actual->value;
			pq->arr[i].prio = hizq->prio;
			pq->arr[i].value = hizq->value;
			pq->arr[2 * i].prio = prio;
			pq->arr[2 * i].value = value;
			i = 2 * i;
		}
		else {
			int prio = actual->prio;
			void* value = actual->value;
			pq->arr[i].prio = hder->prio;
			pq->arr[i].value = hder->value;
			pq->arr[(2 * i) + 1].prio = prio;
			pq->arr[(2 * i) + 1].value = value;
			i = (2 * i) + 1;
		}
	}
	return TRUE;
}

/* retorna el tama�o de la cola de prioridad, 
   retorna 0 si hubo error 
 */
int pq_size(PQ* pq) {

   	/* AGREGUE SU CODIGO AQUI */
	if (NULL == pq) return 0;
	return pq->size;
}

/* Destruye la cola de prioridad, 
retorna TRUE si tuvo exito
retorna FALSE si tuvo error*/
BOOLEAN pq_destroy(PQ* pq) {
   
   /* AGREGUE SU CODIGO AQUI */
	CONFIRM_TRUE(pq, FALSE);
	free(pq->arr);
	pq->arr = NULL;
	free(pq);
	pq = NULL;
	return TRUE;
}
/*
Imprime los elementos y sus prioridades arreglo
*/
void pq_display(PQ* pq) {
	int i;

	printf("Values: ");
	for (i = 1; i <= pq->size; i++) {
		printf("|%c|", pq->arr[i].value);
	}
	printf("\n");
	printf("Prios: ");
	for (i = 1; i <= pq->size; i++) {
		printf("|%d|", pq->arr[i].prio);
	}
	printf("\n");
}
