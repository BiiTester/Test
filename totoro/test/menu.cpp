
#include <iostream>
#include "totoro/test/menu.h"

stmenu menu[] = {
    // Nivel, letra, texto, submenú, acción
    {1,"-","---MENU PRINCIPAL---",0,TITULO},
    {1, "1", "Maestros >", 2, ABRIRMENU},
    {1, "2", "Libros >", 3, ABRIRMENU},
    {1, "3", "Socios >", 4, ABRIRMENU},
    {1, "4", "Prestamos >", 5, ABRIRMENU},
    {1, "0", "Salir", 0, SALIR},
    {2,"-","---TABLAS MAESTRAS---",0,TITULO},
    {2, "1", "Editoriales >", 6, ABRIRMENU},
    {2, "2", "Autores >", 7, ABRIRMENU},
    {2, "3", "Temas >", 8, ABRIRMENU},
    {2, "0", "Salir <", 1, ABRIRMENU},
    {6,"-","---MENU EDITORIALES---",0,TITULO},
    {6, "1", "Nuevo", 0, NUEVAEDITORIAL},
    {6, "2", "Editar", 0, EDITAREDITORIAL},
    {6, "3", "Borrar", 0, BORRAREDITORIAL},
    {6, "4", "Consultar", 0, CONSULTAEDITORIAL},
    {6, "0", "Salir <", 2, ABRIRMENU},
    {3,"-","---MENU LIBROS---",0,TITULO},
    {3, "0", "Salir <", 1, ABRIRMENU},
    {4,"-","---MENU SOCIOS---",0,TITULO},
    {4, "0", "Salir <", 1, ABRIRMENU},
    {5,"-","---MENU PRESTAMOS---",0,TITULO},
    {5, "0", "Salir <", 1, ABRIRMENU},
    {7,"-","---MENU AUTORES---",0,TITULO},
    {7, "0", "Salir <", 2, ABRIRMENU},
    {8,"-","---MENU TEMAS---",0,TITULO},
    {8, "0", "Salir <", 2, ABRIRMENU}
};

using namespace std;

bool IniciarMenu(sqlite3 *db) {
    sqlite3_stmt *ppStmt;
    int rc;
    char consulta[100];

    if(SQLITE_OK != sqlite3_exec(db, "SELECT COUNT(*) FROM menu", 0, 0, 0)) {
        // if(SQLITE_OK != sqlite3_exec(db, "DROP TABLE menu;", 0, 0, 0)) return false;
        if(SQLITE_OK != sqlite3_exec(db, "CREATE TABLE menu(iditem INTEGER PRIMARY KEY,"
                                     "idmenu INTEGER,letra TEXT,texto TEXT,hijo INTEGER,accion INTEGER);", 0, 0, 0)) return false;
        if(SQLITE_OK != sqlite3_exec(db, "BEGIN;", 0, 0, 0)) return false;

        strcpy(consulta, "INSERT INTO menu(idmenu,letra,texto,hijo,accion) VALUES(@mid,@let,@txt,@hij,@acc);");
        rc = sqlite3_prepare_v2(db, consulta, -1, &ppStmt, NULL);
        if( rc!=SQLITE_OK ){
            cout << sqlite3_errmsg(db) << endl;
            return false;
        } else {
            for(int i = 0; i < sizeof(menu)/sizeof(stmenu); i++) {
                sqlite3_bind_int(ppStmt, sqlite3_bind_parameter_index(ppStmt, "@mid"), menu[i].idmenu);
                sqlite3_bind_text(ppStmt, sqlite3_bind_parameter_index(ppStmt, "@let"), menu[i].letra, -1, SQLITE_STATIC);
                sqlite3_bind_text(ppStmt, sqlite3_bind_parameter_index(ppStmt, "@txt"), menu[i].texto, -1, SQLITE_STATIC);
                sqlite3_bind_int(ppStmt, sqlite3_bind_parameter_index(ppStmt, "@hij"), menu[i].hijo);
                sqlite3_bind_int(ppStmt, sqlite3_bind_parameter_index(ppStmt, "@acc"), menu[i].accion);
                sqlite3_step(ppStmt);
                sqlite3_reset(ppStmt);
            }
            sqlite3_finalize(ppStmt);
        }
        if(SQLITE_OK != sqlite3_exec(db, "COMMIT;", 0, 0, 0)) return false;
    }
    return true;
}

void MostrarMenu(sqlite3 *db, int nivel) {
    sqlite3_stmt *ppStmt;
    int rc;
    char consulta[80];
    int nLineas;
    char titulo[64];

    // Contar items:
    sprintf(consulta, "SELECT COUNT(*) FROM menu WHERE idmenu=%d AND accion!=-1;", nivel);
    rc = sqlite3_prepare_v2(db, consulta, -1, &ppStmt, NULL);
    if( rc==SQLITE_OK ){
        if(SQLITE_ROW == sqlite3_step(ppStmt)) {
            nLineas = sqlite3_column_int(ppStmt, 0);
        }
        sqlite3_finalize(ppStmt);
    }

    // 24 líneas en blanco (Borrar pantalla):
    for(int i = 0; i < 24; i++) cout << endl;

    // Titulo:
    strcpy(titulo, "---MENU---"); // Titulo por defecto
    sprintf(consulta, "SELECT texto FROM menu WHERE idmenu=%d AND accion=-1;", nivel);
    rc = sqlite3_prepare_v2(db, consulta, -1, &ppStmt, NULL);
    if( rc==SQLITE_OK ){
        if(SQLITE_ROW == sqlite3_step(ppStmt)) {
            strcpy(titulo, (const char*)sqlite3_column_text(ppStmt, 0));
        }
        sqlite3_finalize(ppStmt);
    }

    cout << "\t\t" << titulo << "\n" << endl;

    strcpy(consulta, "SELECT letra,texto FROM menu WHERE idmenu=@mid AND accion!=-1;");
    rc = sqlite3_prepare_v2(db, consulta, -1, &ppStmt, NULL);
    if( rc!=SQLITE_OK ){
        cout << "Error: " << sqlite3_errmsg(db) << endl;
    } else {
        sqlite3_bind_int(ppStmt, sqlite3_bind_parameter_index(ppStmt, "@mid"), nivel);
        while(SQLITE_ROW == sqlite3_step(ppStmt)) {
            cout << sqlite3_column_int(ppStmt, 0) << ") " << sqlite3_column_text(ppStmt, 1) << endl;
        }
        sqlite3_finalize(ppStmt);
    }
    // Más líneas en blanco:
    for(int i = 0; i < (18-nLineas); i++) cout << endl;
    cout << "\tOpcion: ";
}

int LeerMenu(sqlite3 *db, int& nivel) {
    char resp[2];
    sqlite3_stmt *ppStmt;
    int rc;
    char consulta[64];
    int retval;

    cin >> resp;
    sprintf(consulta, "SELECT hijo,accion FROM menu WHERE letra='%s' AND idmenu=%d;", resp, nivel);
    rc = sqlite3_prepare_v2(db, consulta, -1, &ppStmt, NULL);
    if( rc!=SQLITE_OK ){
        cout << "Error: " << sqlite3_errmsg(db) << endl;
    } else {
        if(SQLITE_ROW == sqlite3_step(ppStmt)) {
            retval = sqlite3_column_int(ppStmt, 1);
            if(retval == ABRIRMENU) nivel = sqlite3_column_int(ppStmt, 0);
        }
        sqlite3_finalize(ppStmt);
    }
    return retval;
}
