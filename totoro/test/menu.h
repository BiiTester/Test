#include "totoro/sqlite/sqlite3.h"
#include <stdio.h>
#include <string.h>

#define TITULO              -1
#define SALIR               0
#define ABRIRMENU           1
#define NUEVAEDITORIAL      2
#define EDITAREDITORIAL     3
#define BORRAREDITORIAL     4
#define CONSULTAEDITORIAL   5
// Añadiremos más códigos de operación a medida que implementemos nuevas opciones

struct stmenu { // Estructura para inicializar tabla de menús
    int idmenu;
    char letra[2];
    char texto[64];
    int hijo;
    int accion;
};

// Prototipos:
bool IniciarMenu(sqlite3 *);
void MostrarMenu(sqlite3 *, int);
int LeerMenu(sqlite3 *, int&);
