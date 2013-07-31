#include "totoro/sqlite/sqlite3.h"
#include <iostream>


#include "totoro/test/menu.h"

using namespace std;

const int nTablas = 9;

bool VerificarTablas(sqlite3 *);



int main()
{
    int rc;
    sqlite3 *db;
    int nivel=1;
    bool salir=false;

    // Abrir base de datos
    rc = sqlite3_open("biblioteca.db", &db);
    if(SQLITE_OK != rc) {
        cout << "Error: No se puede abrir la base de datos" << endl;
		return 1;
    }

    if(!VerificarTablas(db)) return -1;
    if(!IniciarMenu(db)) return -1;

    do {
        MostrarMenu(db, nivel);
        switch(LeerMenu(db, nivel)) {
            case ABRIRMENU:
               // Nada que hacer.
			   break;
            case NUEVAEDITORIAL:
            case EDITAREDITORIAL:
            case BORRAREDITORIAL:
            case CONSULTAEDITORIAL:
               cout << "No implementado" << endl;
               break;
            case SALIR:
               salir = true;
        }
    } while(!salir);

    // Cerrar base de datos
    sqlite3_close(db);
    return 0;
}

bool VerificarTablas(sqlite3 *db) {
    char consulta[36];
    char *tabla[] = {
        "editorial",
        "libro",
        "autor",
        "tema",
        "ejemplar",
        "socio",
        "prestamo",
        "trata_sobre",
        "escrito_por"
    };
    char *create[] = {
        "CREATE TABLE editorial("
          "claveeditorial INTEGER PRIMARY KEY,"
          "editorial TEXT,"
          "direccion TEXT,"
          "telefono TEXT);",
        "CREATE TABLE libro("
          "clavelibro INTEGER PRIMARY KEY,"
          "titulo TEXT,"
          "idioma TEXT,"
          "formato TEXT,"
          "claveeditorial INTEGER "
          "REFERENCES editorial(claveeditorial) "
          "ON DELETE SET NULL "
          "ON UPDATE CASCADE);",
        "CREATE TABLE autor("
          "claveautor INTEGER PRIMARY KEY,"
          "autor TEXT);",
        "CREATE TABLE tema("
          "clavetema INTEGER PRIMARY KEY,"
          "tema TEXT);",
        "CREATE TABLE ejemplar("
          "clavelibro INTEGER "
          "REFERENCES libro(clavelibro) "
          "ON DELETE CASCADE "
          "ON UPDATE CASCADE,"
          "numeroorden INTEGER NOT NULL,"
          "edicion INTEGER,"
          "ubicacion TEXT,"
          "categoria TEXT,"
          "PRIMARY KEY(clavelibro,numeroorden));",
        "CREATE TABLE socio("
          "clavesocio INTEGER PRIMARY KEY,"
          "socio TEXT,"
          "direccion TEXT,"
          "telefono TEXT,"
          "categoria TEXT);",
        "CREATE TABLE prestamo("
          "clavesocio INTEGER "
          "REFERENCES socio(clavesocio) "
          "ON DELETE SET NULL "
          "ON UPDATE CASCADE,"
          "clavelibro INTEGER "
          "REFERENCES ejemplar(clavelibro) "
          "ON DELETE SET NULL "
          "ON UPDATE CASCADE,"
          "numeroorden INTEGER,"
          "fecha_prestamo DATE NOT NULL,"
          "fecha_devolucion DATE DEFAULT NULL,"
          "notas TEXT);",
        "CREATE TABLE trata_sobre("
          "clavelibro INTEGER NOT NULL "
          "REFERENCES libro(clavelibro) "
          "ON DELETE CASCADE "
          "ON UPDATE CASCADE,"
          "clavetema INTEGER NOT NULL "
          "REFERENCES tema(clavetema) "
          "ON DELETE CASCADE "
          "ON UPDATE CASCADE);",
        "CREATE TABLE escrito_por("
          "clavelibro INTEGER NOT NULL "
          "REFERENCES libro(clavelibro) "
          "ON DELETE CASCADE "
          "ON UPDATE CASCADE,"
          "claveautor INTEGER NOT NULL "
          "REFERENCES autor(claveautor) "
          "ON DELETE CASCADE "
          "ON UPDATE CASCADE);"
    };
	
    for(int i = 0; i < nTablas; i++) {
        sprintf(consulta, "SELECT COUNT(*) FROM %s;", tabla[i]);
        if(SQLITE_OK != sqlite3_exec(db, consulta, 0, 0, 0)) {
            cout << "La tabla " << tabla[i] << " no existe." << endl;
            if(SQLITE_OK != sqlite3_exec(db, create[i], 0, 0, 0)) {
               cout << "Error al crear la tabla " << tabla[i] << endl;
               return false;
            }
        }
    }
    return true;
}
