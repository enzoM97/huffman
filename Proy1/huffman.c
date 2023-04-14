#include "huffman.h"

#include <stdio.h>
#include <string.h>

#include "arbol.h"
#include "pq.h"
#include "bitstream.h"
#include "confirm.h"

/*====================================================
     Constantes
  ====================================================*/

#define NUM_CHARS 256


/*====================================================
     Campo de bits.. agrega funciones si quieres
     para facilitar el procesamiento de bits.
  ====================================================*/

typedef struct _campobits {
    unsigned int bits;
    int tamano;
} campobits;

/* Esto utiliza aritmetica de bits para agregar un
   bit a un campo.
   
   Supongamos que bits->bits inicialmene es (en binario):
   
      000110001000
      
   y le quiero agregar un 1 en vez del segundo 0 (desde izq).
   Entonces, creo una "mascara" de la siguiente forma:
   
      1 << 11   me da 0100000000000

   Y entonces si juntamos los dos (utilizando OR binario):      
      000110001000
    | 0100000000000
    ----------------
      010110001000

    Esta funcion utiliza bits->tamano para decidir donde colocar
    el siguiente bit.
    
    Nota: asume que bits->bits esta inicialmente inicializado a 0,
    entonces agregar un 0, no requiere mas que incrementar bits->tamano.
*/
      
static void bits_agregar(campobits* bits, int bit) {
    CONFIRM_RETURN(bits);
    CONFIRM_RETURN((unsigned int)bits->tamano < 8*sizeof(bits->bits));
    bits->tamano++;
    if (bit) {
        bits->bits = bits->bits | ( 0x1 << (bits->tamano-1));
    } 
}


/*====================================================
     Declaraciones de funciones 
  ====================================================*/

/* Puedes cambiar esto si quieres.. pero entiende bien lo que haces */
static int calcular_frecuencias(int* frecuencias, char* entrada);
static Arbol crear_huffman(int* frecuencias);
static int codificar(Arbol T, char* entrada, char* salida);
static void crear_tabla(campobits* tabla, Arbol T, campobits *bits);
static int es_hoja(Arbol arbol);

static Arbol leer_arbol(BitStream bs);
static void decodificar(BitStream in, BitStream out, Arbol arbol);

static void imprimirNodo(Arbol nodo);

/*====================================================
     Implementacion de funciones publicas
  ====================================================*/

/*
  Comprime archivo entrada y lo escriba a archivo salida.
  
  Retorna 0 si no hay errores.
*/
int comprimir(char* entrada, char* salida) {

	/* 256 es el numero de caracteres ASCII.
	   Asi podemos utilizar un unsigned char como indice.
	 */
	int frecuencias[NUM_CHARS];
	Arbol arbol = NULL;
	/* Primer recorrido - calcular frecuencias */
	CONFIRM_TRUE(0 == calcular_frecuencias(frecuencias, entrada), 1);

	arbol = crear_huffman(frecuencias);
	arbol_imprimir(arbol, imprimirNodo);
	
    /* Segundo recorrido - Codificar archivo */
    CONFIRM_TRUE(0 == codificar(arbol, entrada, salida), 1);
    
    arbol_destruir(arbol);
    
    return 0;
}


/*
  Descomprime archivo entrada y lo escriba a archivo salida.
  
  Retorna 0 si no hay errores.
*/
int descomprimir(char* entrada, char* salida) {

    BitStream in = 0;
    FILE* out = 0;
    Arbol arbol = NULL;
        
    /* Abrir archivo de entrada */
    in = OpenBitStream(entrada, "r");

    /* Leer Arbol de Huffman */
    arbol = leer_arbol(in);
	arbol_imprimir(arbol, imprimirNodo);
	
    /* Abrir archivo de salida */
    out = fopen(salida, "w");
    
    /* Decodificar archivo */
    decodificar(in, out, arbol);
    
    CloseBitStream(in);
    CloseBitStream(out);
    return 0;
}

/*====================================================
     Funciones privadas
  ====================================================*/

/* Devuelve 0 si no hay errores */
static int calcular_frecuencias(int* frecuencias, char* entrada) {

    /* Este metodo recorre el archivo contando la frecuencia
       que ocurre cada caracter y guardando el resultado en
       el arreglo frecuencias
    */

    /* TU IMPLEMENTACION AQUI */
    
    /* Nota: comienza inicializando todas las frecuencias a cero!! */
	int codigo;
	char caracter;
	FILE *archivo;
	int ascii_table[NUM_CHARS];
	
	/* inicializa las frecuencias a 0 agrega los valores a la tabla ASCII */
	for (int i = 0; i < NUM_CHARS; i++) {
		frecuencias[i] = 0;
		ascii_table[i] = i;
	}

	/* lee el archivo de entrada */
	archivo = fopen(entrada, "r");

	while ((caracter = fgetc(archivo)) != EOF) {
		codigo = caracter;
		/* comparar el codigo del caracter con su correspondiente codigo ascii */
		for (int j = 0; j < NUM_CHARS; j++) {
			if (codigo == j) {
				frecuencias[j]++;
			}
		}
	}
	
	fclose(archivo);
    return 0;
}

/* funcion que crea nodos de acuerdo a las frecuencias de caracteres
 y los carga en una cola de prioridad */
BOOLEAN crear_nodos(PQ* pq, int* frecuencias) {
	CONFIRM_TRUE(pq, FALSE);
	char caracter;
	int frecuencia;
	/* agregar cada nodo a cola... */
	for (int i = 0; i < NUM_CHARS; i++) {
		if (frecuencias[i] != 0) {
			caracter = i;
			frecuencia = frecuencias[i];
			Values* v = malloc(sizeof(struct _Values));
			CONFIRM_TRUE(v, FALSE);
			v->caracter = caracter;
			v->frecuencia = frecuencia;
			//se crea un nodo
			Arbol nodo = arbol_crear(v);
			CONFIRM_TRUE(nodo, FALSE);
			pq_add(pq, nodo, frecuencia);
		}
	}
	return TRUE;
}

/* Crea el arbol huffman en base a las frecuencias dadas */
static Arbol crear_huffman(int* frecuencias) {

	/* TU IMPLEMENTACION AQUI */
	int frec1, frec2, n_frec;

	Arbol retval1 = NULL;
	Arbol retval2 = NULL;
	Arbol n_arbol = NULL;
	Arbol arbol = NULL;

	Values* val1 = NULL;
	Values* val2 = NULL;
	Values* test = NULL;
	Values* val_izq = NULL;
	Values* val_der = NULL;
	
	PQ* pq_nodos = pq_create();

	crear_nodos(pq_nodos, frecuencias);

	/* mientras haya mas de un arbol en la cola,
	   sacar dos arboles y juntarlos (segun el algoritmo de huffman)
	   reinsertar el nuevo arbol combinado
	*/

	while (pq_nodos->size > 1) {
		//se reserva memoria para el nuevo nodo
		Values* n_val = malloc(sizeof(struct _Values));
		CONFIRM_NOTNULL(n_val, NULL);

		pq_remove(pq_nodos, &retval1);
		pq_remove(pq_nodos, &retval2);

		val1 = (Values*)arbol_valor(retval1);
		CONFIRM_NOTNULL(val1, NULL);
		val2 = (Values*)arbol_valor(retval2);
		CONFIRM_NOTNULL(val2, NULL);
		
		frec1 = val1->frecuencia;
		frec2 = val2->frecuencia;
		n_frec = frec1 + frec2;
		
		n_val->caracter = NULL;
		n_val->frecuencia = n_frec;

		//se crea el nuevo nodo con valor igual a la suma de las frecuencias
		//de sus hijos
		n_arbol = arbol_crear(n_val);
		CONFIRM_NOTNULL(n_arbol, NULL);

		//agregar los hijos al nodo creado
		arbol_agregarIzq(n_arbol, retval1);
		arbol_agregarDer(n_arbol, retval2);

		//se agrega el nuevo arbol a la pq 
		pq_add(pq_nodos, n_arbol, n_frec);
	}
	
	pq_remove(pq_nodos, &arbol);

    /* limpieza */
    pq_destroy(pq_nodos);

    return arbol;
}

static void generar_tabla(Arbol nodo, campobits* lista, int arr[], int top) {
	if (es_hoja(nodo)) {
		int i;
		Values* v = (Values*)arbol_valor(nodo);
		campobits *bits = (campobits*)malloc(sizeof(campobits));
		CONFIRM_RETURN(NULL != bits);
		bits->bits = 0;
		bits->tamano = 0;
		for (i = 0; i < top; i++) {
			bits_agregar(bits, arr[i]);
			printf("%i", arr[i]);
		}
		printf("/ %c / %i / %i \n", v->caracter, bits->bits, bits->tamano);
		lista[(int)v->caracter] = *bits;
		return;
	}
	else {
		arr[top] = 0;
		top++;
		generar_tabla(arbol_izq(nodo), lista, arr, top);
		top--;
		arr[top] = 1;
		top++;
		generar_tabla(arbol_der(nodo), lista, arr, top);
		top--;
	}
}

static void escribir_codigo(Arbol nodo, BitStream* bs) {
	Values* v = (Values*)arbol_valor(nodo);
	if (es_hoja(nodo)) {
		PutBit(bs, 1);
		PutByte(bs, v->caracter);
	}
	else {
		PutBit(bs, 0);
	}
}

static int codificar(Arbol T, char* entrada, char* salida) {

    FILE* in = NULL;
    BitStream out = NULL;
	int tmp[NUM_CHARS];
	char caracter;

    /* Dado el arbol crear una tabla que contiene la
       secuencia de bits para cada caracter.
       
       Los bits se guardan en campobits que es un struct
       que contiene un int (sin signo) y un tamano.
       La idea es que voy agregando bits al campo de bits
       mientras en un campo (bits), y el numero de bits
       que necesito en otro (tamano)
    */
    campobits tabla[NUM_CHARS];
	
    /* Inicializar tabla de campo de bits a cero */
    memset(tabla, 0, NUM_CHARS*sizeof(struct _campobits));

	generar_tabla(T, tabla, tmp, 0);

    /* Abrir archivos */
    /* TU IMPLEMENTACION VA AQUI .. */

	in = fopen(entrada, "r");
	out = OpenBitStream(salida, "w");

    /* Escribir el arbol al archivo de salida */
    /* TU IMPLEMENTACION VA AQUI .. 

        Nota: puedes utilizar arbol_preorden() para lograr esto
        facilmente.
        
        El truco es que al escribir el arbol, 
           - Si no es hoja: 
               escribe un bit 0 
           - Si es hoja:
                bit 1 seguido por el byte ASCII que representa el caracter 
        
        Para escribir bits utiliza PutBits() de bitstream.h
        Para escribir bytes utiliza PutByte() de bitstream.h
    */
	
	arbol_preorden(T, escribir_codigo, out);

    /* Escribir el texto codificado al archivo*/
    /* 
        TU IMPLEMENTACION VA AQUI .. 
        
        Lee todos los datos de nuevo del archivo de entrada
        y agregalos al archivo de salida utilizando la
        tabla de conversion.
        
        Recuerda que tienes que escribir bit por bit utilzando
        PutBit() de bitstream.h. Por ejemplo, dado una secuencia
        de bits podrias escribirlo al archivo asi:
        
            /- Esto escribe todos los bits en un campobits a un
               BitStream  -/
            for (i = 0; i < tabla[c].tamano; i++) {
               int bit =  0 != (tabla[c].bits & (0x1<<i));
               PutBit(out, bit);
            }
        
        Puedes colocarlo en una funcion si quieres
    */

	while ((caracter = fgetc(in)) != EOF) {
		for (int i = 0; i < tabla[caracter].tamano; i++) {
			int bit = 0 != (tabla[caracter].bits & (0x1 << i));
			PutBit(out, bit);
		}
	}

    /* No te olvides de limpiar */
    if (in)
        fclose(in);
    if (out)
        CloseBitStream(out);
       
    return 0;
}           

/* Esto se utiliza como parte de la descompresion (ver descomprimir())..
   
   Para leer algo que esta guardado en preorden, hay que
   pensarlo un poquito.
   
   Pero basicamente la idea es que vamos a leer el archivo
   en secuencia. Inicialmente, el archivo probablemente va 
   a empezar con el bit 0 representando la raiz del arbol. 
   Luego, tenemos que leer recursivamente (utiliza otra funcion
   para ayudarte si lo necesitas) un nodo izquierdo y uno derecho.
   Si uno (o ambos) son hojas entonces tenemos que leer tambien su
   codigo ASCII. Hacemos esto hasta que todos los nodos tienen sus 
   hijos. (Si esta bien escrito el arbol el algoritmo terminara
   porque no hay mas nodos sin hijos)
*/
static Arbol leer_arbol(BitStream bs) {
  
    /* TU IMPLEMENTACION AQUI */
	int bit = GetBit(bs);
	if (bit == 1) {
		char* caracter = GetByte(bs);
		Arbol nodo = arbol_crear(caracter);
		return nodo;
	}
	else {
		Arbol raiz = arbol_crear(NULL);
		arbol_agregarDer(raiz, leer_arbol(bs));
		arbol_agregarIzq(raiz, leer_arbol(bs));
		return raiz;
	}
}

/* Esto se utiliza como parte de la descompresion (ver descomprimir())..
   
   Ahora lee todos los bits que quedan en in, y escribelos como bytes
   en out. Utiliza los bits para navegar por el arbol de huffman, y
   cuando llegues a una hoja escribe el codigo ASCII al out con PutByte()
   y vuelve a comenzar a procesar bits desde la raiz.
   
   Sigue con este proceso hasta que no hay mas bits en in.
*/   
static void decodificar(BitStream in, FILE* out, Arbol arbol) {
  
    /* TU IMPLEMENTACION AQUI */
}

static int es_hoja(Arbol nodo) {
	if (NULL == arbol_izq(nodo) && NULL == arbol_der(nodo)) return 1;
	return 0;
}

/* Esto es para imprimir nodos..
   Tal vez tengas mas de uno de estas funciones debendiendo
   de como decidiste representar los valores del arbol durante
   la compresion y descompresion.
*/
//compresion
static void imprimirNodo(Arbol nodo) {
	// TU IMPLEMENTACION AQUI 
	CONFIRM_NOTNULL(nodo, NULL);
	Values* values = (Values*)arbol_valor(nodo);
	CONFIRM_NOTNULL(values, NULL);
	if (NULL == values->caracter) {
		printf("(%d", values->frecuencia);
		printf(" | ");
		printf("null)");
	}
	else if (10 == values->caracter){
		printf("(%d", values->frecuencia);
		printf(" | ");
		printf("'LF')", values->caracter);
	}
	else {
		printf("(%d", values->frecuencia);
		printf(" | ");
		printf("'%c')", values->caracter);
	}
}
//descompresion
/*static void imprimirNodo(Arbol nodo) {
	CONFIRM_NOTNULL(nodo, NULL);
	char valor = arbol_valor(nodo);
	if (NULL == valor) printf("(null)");
	printf("(%c)", valor);
}*/
