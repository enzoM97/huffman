
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "huffman.h"
#include "arbol.h"

void forma_de_uso() {
    printf("\nCodificador de Huffman:\n\n");
    printf("\tProy1.exe [comprimir|descomprimir] archivoent archivosal\n");
}

/*
	Main temporal para pruebas
*/
#define TAM 256
/*int main() {
	int frecuencias[TAM];
	
	Arbol arbol = NULL;

	if (1 == comprimir("test1.txt", frecuencias)) {
		printf("error en comprimir\n");
	}

	arbol = crear_huffman(frecuencias);

	free(arbol);
	system("pause");
	return 1;
}*/

/* Este es un main() con argumentos.
    argc - numero de argumentos (incluyendo el ejecutable)
    argv - vector de argumentos
*/
int main(int argc, char* argv[]) {
    
    int errores = 0;

    // Revisar que estan bien los parametros 
    if (argc != 4) {
        forma_de_uso();
        return 1;
    }

    if (0 == strcmp("comprimir", argv[1])) {
        errores = comprimir(argv[2], argv[3]);
    } else {
        errores = descomprimir(argv[2], argv[3]);
    }


    
    //char** str;
    //printf("\nPresione Enter para continuar ... %c",str);
    //scanf("%s",&str);
	system("PAUSE");
    return errores;
}
