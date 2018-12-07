#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include "proveedor.h"
#include "db_aux_def.h"
#include "utiles.h"

Proveedor::Proveedor()
{
    this->limpiar();
}

Proveedor::Proveedor(const Proveedor &p)
{
    *this = p;
}

Proveedor::~Proveedor()
{

}

// Operador de asignación.
Proveedor &Proveedor::operator = (const Proveedor &p)
{
    if(this != &p) {
        codigo              = p.codigo;
        tipo                = p.tipo;
        razon_social        = p.razon_social;
        domicilio           = p.domicilio;
        codigo_postal       = p.codigo_postal;
        poblacion           = p.poblacion;
        provincia           = p.provincia;
        cif                 = p.cif;
        tel1                = p.tel1;
        tel2                = p.tel2;
        fax                 = p.fax;
        forma_pago          = p.forma_pago;
        banco               = p.banco;

        id                  = p.id;
    }

    return *this;
}

/*
bool BaseDatos::insertarProveedor(const Proveedor &p)
{
    const int   kcampos_txt = 9,                // Numero de campos de datos.
                kcampos_cons = 11;
	char        *datos_prov[kcampos_txt],       // Almacenamos los datos leidos.
                consulta[2048];               // Construimos la consulta.

    sqlite3_stmt    *ppStmt;

    // Comprobamos si tenemos una base de datos abierta.
    if(m_sqltBD == NULL)
        // No hay base de datos abierta.
        return false;

    // Comprobamos si ya existe el proveedor en la base de datos.
    sprintf(consulta, "SELECT ID_PROV FROM proveedor WHERE COD_PROV LIKE '%s';", p.codigo.c_str());
    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error en la consulta.
        // std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
       if(SQLITE_ROW == sqlite3_step(ppStmt)) {
           // El proveedor ya existe, no podemos insertarlo de nuevo.
           return false;
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "BEGIN", 0, 0, 0);
    strcpy(consulta,
        "INSERT INTO proveedor(COD_PROV, "
        "RAZON_SOCIAL, "
        "DOMICILIO, "
        "COD_POSTAL, "
        "POBLACION, "
        "CIF, "
        "TEL1, "
        "TEL2, "
        "FAX, "
        "FORMA_PAGO, "
        "BANCO) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    );

    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error en la consulta.
        //std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
        for(int i = 0, j = 1; j <= kcampos_cons; ++j, ++i) {
            switch(j) {
                case 1: case 2: case 3: case 4: case 5:
                case 6: case 7: case 8: case 9:
                    // Convertimos todas las cadenas a UTF8 antes de insertar.
                    datos_prov[i] = utiles::GIDToUTF8(p[i].c_str());

                    // Asociamos los datos a la consulta sql.
                    sqlite3_bind_text(ppStmt, j, datos_prov[i], -1, SQLITE_STATIC);
                    break;
                case 10: case 11:
                    ///// TEMPORAL //////
                    // Convertimos de cadena a entero. Lo suyo sería almacenarlo en la
                    // clase proveedor también como entero. Pero como soy un puto gilipollas
                    // de momento lo dejo en modo texto y ya veremos qué coño hacemos
                    // más adelante.

                    sqlite3_bind_int(ppStmt, j, utiles::StrToInt(p[i]));
                    ///// TEMPORAL /////
                    break;
                default:
                    break;
            }
        }

        // Insertamos.
        if(SQLITE_DONE != sqlite3_step(ppStmt)) {
            // Error en la insercion.
            // std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "END", 0, 0, 0);

    // Liberamos memoria.
    for(int i = 0; i < kcampos_txt; i++)
        delete [] datos_prov[i];

	return true;
}
*/

/*
bool BaseDatos::borrarProveedor(std::string codigo)
{
    char            consulta[2048];               // Construimos la consulta.

    sqlite3_stmt    *ppStmt;
    int             id;

    // Comprobamos si tenemos una base de datos abierta.
    if(m_sqltBD == NULL)
        // No hay base de datos abierta.
        return false;

    // Comprobamos si ya existe el cliente en la base de datos.
    sprintf(consulta, "SELECT ID_PROV FROM proveedor WHERE COD_PROV LIKE '%s';", codigo.c_str());
    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error en la consulta.
        // std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
       if(SQLITE_ROW != sqlite3_step(ppStmt)) {
            // El cliente no existe, no podemos borralo.
            return false;
        }
        else {
            // Recogemos la ID del cliente.
            id = sqlite3_column_int(ppStmt, 0);
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "BEGIN", 0, 0, 0);
    strcpy(consulta, "DELETE FROM proveedor WHERE ID_PROV=?;");

    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error en la consulta.
        //std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
        // Integramos los datos en la consulta sql.
        sqlite3_bind_int(ppStmt, 1, id);

        // Borramos.
        if(SQLITE_DONE != sqlite3_step(ppStmt)) {
            // Error en el borrado.
            // std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "END", 0, 0, 0);

	return true;
}
*/

/*
bool BaseDatos::actualizarProveedor(std::string codigo, const Proveedor &p)
{
    const int   kcampos_txt = 11,           // Numero de campos de datos.
                kcampos_cons = 12;
	char        *datos_prov[kcampos_txt],   // Almacenamos los datos leidos.
                consulta[2048];             // Construimos la consulta.

    sqlite3_stmt    *ppStmt;
    int             id;

    // Comprobamos si tenemos una base de datos abierta.
    if(m_sqltBD == NULL)
        // No hay base de datos abierta.
        return false;

    // Comprobamos si ya existe el cliente en la base de datos.
    sprintf(consulta, "SELECT ID_PROV FROM proveedor WHERE COD_PROV LIKE '%s';", codigo.c_str());
    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error en la consulta.
        // std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
       if(SQLITE_ROW != sqlite3_step(ppStmt)) {
            // El cliente no existe, no podemos actualizarlo.
            return false;
        }
        else {
            // Recogemos la ID del cliente.
            id = sqlite3_column_int(ppStmt, 0);
        }

        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "BEGIN", 0, 0, 0);
    strcpy(consulta, "UPDATE proveedor "
        "SET COD_PROV=?, "
        "RAZON_SOCIAL=?, "
        "DOMICILIO=?, "
        "COD_POSTAL=?, "
        "POBLACION=?, "
        "CIF=?, "
        "TEL1=?, "
        "TEL2=?, "
        "FAX=?, "
        "FORMA_PAGO=?, "
        "BANCO=? "
        "WHERE ID_PROV=?;"
    );

    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error en la consulta.
        //std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
        for(int i = 0, j = 1; j <= kcampos_cons; ++j, ++i) {
            switch(j) {
                case 1: case 2: case 3: case 4: case 5:
                case 6: case 7: case 8: case 9:
                    // Convertimos todas las cadenas a UTF8 antes de insertar.
                    datos_prov[i] = utiles::GIDToUTF8(p[i].c_str());

                    // Asociamos los datos a la consulta sql.
                    sqlite3_bind_text(ppStmt, j, datos_prov[i], -1, SQLITE_STATIC);
                    break;
                case 10: case 11:
                    ///// TEMPORAL /////
                    // Convertimos de cadena a entero. Lo suyo sería almacenarlo en la
                    // clase proveedor también como entero. Pero como soy un puto gilipollas
                    // de momento lo dejo en modo texto y ya veremos qué coño hacemos
                    // más adelante.

                    sqlite3_bind_int(ppStmt, j, utiles::StrToInt(p[i]));
                    ///// TEMPORAL /////
                    break;
                case 12:
                    sqlite3_bind_int(ppStmt, j, id);
                default:
                    break;
            }
        }

        // Insertamos.
        if(SQLITE_DONE != sqlite3_step(ppStmt)) {
            // Error en la insercion.
            // std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "END", 0, 0, 0);

    // Liberamos memoria.
    for(int i = 0; i < kcampos_txt; i++)
        delete [] datos_prov[i];

	return true;
}
*/

/*
bool BaseDatos::revisarProveedores(std::vector<Proveedor> &lista_provs)
{
    char            consulta[1024];
    sqlite3_stmt    *ppStmt;

    std::vector<Proveedor>  lista_provs_insertar;
    std::vector<Proveedor>  lista_provs_actualizar;
    std::vector<int>        lista_ids_actualizar;

    for(auto p : lista_provs) {
        sprintf(consulta, "SELECT ID_PROV FROM proveedor WHERE COD_PROV LIKE '%s';", p.codigo.c_str());
        if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
            // Error al preparar la consulta.
            std::string strError("Error consulta BaseDatos::revisarProveedores - ");
            strError += sqlite3_errmsg(m_sqltBD);

            wxLogMessage(strError.c_str());

            return false;
        } else {
           if(SQLITE_ROW == sqlite3_step(ppStmt)) {
                // El cliente existe en la base de datos, lo actualizaremos.
                lista_ids_actualizar.push_back(sqlite3_column_int(ppStmt, 0));
                lista_provs_actualizar.push_back(p);
            } else {
                // El cliente no existe, lo insertaremos.
                lista_provs_insertar.push_back(p);
            }
            sqlite3_finalize(ppStmt);
        }
    }

    // Actualizamos los clientes existentes.
    if(lista_provs_actualizar.size() > 0)
        if(!actualizarProveedoresMasivo(lista_ids_actualizar, lista_provs_actualizar))
            return false;

    // Insertamos los nuevos clientes.
    if(lista_provs_insertar.size() > 0)
        if(!insertarProveedoresMasivo(lista_provs_insertar))
            return false;

    return true;
}
*/

/*
bool BaseDatos::datosProveedor(std::string codigo, Proveedor &p)
{
    const int       kcampos_tabla = 12;
    char            consulta[1024];
    sqlite3_stmt    *ppStmt;

    sprintf(consulta, "SELECT * FROM proveedor WHERE COD_PROV LIKE '%s';", codigo.c_str());
    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error al preparar la consulta.
        //std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
       if(SQLITE_ROW == sqlite3_step(ppStmt)) {
            // Rellenamos los datos.
            p.limpiar();

            for(int i = 0, j = 0; j < kcampos_tabla; ++j) {
                switch(j) {
                    case 0:
                        p.id = sqlite3_column_int(ppStmt, j);
                        break;
                    case 1: case 2: case 3: case 4: case 5:
                    case 6: case 7: case 8: case 9:
                        utiles::UTF8ToISO20022(std::string((char *)sqlite3_column_text(ppStmt, j)), p[i]);
                        ++i;
                        break;
                    case 10: case 11:
                        p[i] = utiles::IntToStr(sqlite3_column_int(ppStmt, j));
                        ++i;
                        break;
                    default:
                        break;
                }
            }
        } else {
            // El cliente no existe.
            return false;
        }

        sqlite3_finalize(ppStmt);
    }

    return true;
}
*/

/*
bool BaseDatos::datosProveedor(int id, Proveedor &p)
{
    const int       kcampos_tabla = 12;
    char            consulta[1024];
    sqlite3_stmt    *ppStmt;

    sprintf(consulta, "SELECT * FROM proveedor WHERE ID_PROV=%d;", id);
    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error al preparar la consulta.
        //std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
       if(SQLITE_ROW == sqlite3_step(ppStmt)) {
            // Rellenamos los datos.
            p.limpiar();

            for(int i = 0, j = 0; j < kcampos_tabla; j++) {
                switch(j) {
                    case 0:
                        p.id = sqlite3_column_int(ppStmt, j);
                        break;
                    case 1: case 2: case 3: case 4: case 5:
                    case 6: case 7: case 8: case 9:
                        utiles::UTF8ToISO20022(std::string((char *)sqlite3_column_text(ppStmt, j)), p[i]);
                        ++i;
                        break;
                    case 10: case 11:
                        p[i] = utiles::IntToStr(sqlite3_column_int(ppStmt, j));
                        ++i;
                        break;
                    default:
                        break;
                }
            }
        } else {
            // El cliente no existe.
            return false;
        }

        sqlite3_finalize(ppStmt);
    }

    return true;
}
*/

/*
bool BaseDatos::listarProveedores(std::vector<Proveedor> &lista_provs)
{
    const int       kcampos_tabla = 12;
    char            consulta[1024];
    Proveedor       p;
    sqlite3_stmt    *ppStmt;

    sprintf(consulta, "SELECT * FROM proveedor");
    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error al preparar la consulta.
        //std::cout << "Error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
       while(SQLITE_ROW == sqlite3_step(ppStmt)) {
            p.limpiar();

            for(int i = 0, j = 0; j < kcampos_tabla; j++) {
                switch(j) {
                    case 0:
                        p.id = sqlite3_column_int(ppStmt, j);
                        break;
                    case 1: case 2: case 3: case 4: case 5:
                    case 6: case 7: case 8: case 9:
                        utiles::UTF8ToISO20022(std::string((char *)sqlite3_column_text(ppStmt, j)), p[i]);
                        ++i;
                        break;
                    case 10: case 11:
                        p[i] = utiles::IntToStr(sqlite3_column_int(ppStmt, j));
                        ++i;
                        break;
                    default:
                        break;
                }
            }

            lista_provs.push_back(p);
        }

        sqlite3_finalize(ppStmt);
    }

    return true;
}

bool BaseDatos::insertarProveedoresMasivo(std::vector<Proveedor> &lista_provs)
{
    const int   kcampos_txt = 11,                // Numero de campos de datos.
                kcampos_cons = 11;
	char        *datos_prov[kcampos_txt],    // Almacenamos los datos leidos.
                consulta[2048];               // Construimos la consulta.

    sqlite3_stmt    *ppStmt;

    sqlite3_exec(m_sqltBD, "BEGIN", 0, 0, 0);
    strcpy(consulta,
        "INSERT INTO proveedor(COD_PROV, "
        "RAZON_SOCIAL, "
        "DOMICILIO, "
        "COD_POSTAL, "
        "POBLACION, "
        "CIF, "
        "TEL1, "
        "TEL2, "
        "FAX, "
        "FORMA_PAGO, "
        "BANCO) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    );

    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error al preparar la consulta.
        return false;
    } else {
        for(auto p : lista_provs) {
            for(int i = 0, j = 1; j <= kcampos_cons; ++j, ++i) {
                switch(j) {
                    case 1: case 2: case 3: case 4: case 5:
                    case 6: case 7: case 8: case 9:
                        datos_prov[i] = utiles::GIDToUTF8(p[i].c_str());
                        sqlite3_bind_text(ppStmt, j, datos_prov[i], -1, SQLITE_STATIC);
                        break;
                    case 10: case 11:
                        sqlite3_bind_int(ppStmt, j, utiles::StrToInt(p[i]));
                        break;
                    default:
                        break;
                }
            }

            if(SQLITE_DONE != sqlite3_step(ppStmt)) {
                // Error en la insercion.
                return false;
            }
            sqlite3_reset(ppStmt);

            // Liberamos la memoria para la siguiente iteración.
            for(int i = 0; i < kcampos_txt; i++)
                delete [] datos_prov[i];
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "END", 0, 0, 0);

    return true;
}
*/

/*
bool BaseDatos::actualizarProveedoresMasivo(std::vector<int> lista_ids, std::vector<Proveedor> &lista_provs)
{
    const int   kcampos_txt = 9,            // Numero de campos de datos.
                kcampos_cons = 12;
	char        *datos_prov[kcampos_txt],   // Almacenamos los datos leidos.
	            consulta[2048];             // Construimos la consulta.

    sqlite3_stmt    *ppStmt;

    sqlite3_exec(m_sqltBD, "BEGIN", 0, 0, 0);
    strcpy(consulta, "UPDATE proveedor "
        "SET COD_PROV=?, "
        "RAZON_SOCIAL=?, "
        "DOMICILIO=?, "
        "COD_POSTAL=?, "
        "POBLACION=?, "
        "CIF=?, "
        "TEL1=?, "
        "TEL2=?, "
        "FAX=?, "
        "FORMA_PAGO=?, "
        "BANCO=? "
        "WHERE ID_PROV=?;"
    );


    if(SQLITE_OK != sqlite3_prepare_v2(m_sqltBD, consulta, -1, &ppStmt, NULL)) {
        // Error al preparar la consulta.
        return false;
    } else {
        for(size_t k = 0; k < lista_provs.size(); k++) {
            for(int i = 0, j = 1; j <= kcampos_cons; ++j, ++i) {
                switch(j) {
                    case 1: case 2: case 3: case 4: case 5:
                    case 6: case 7: case 8: case 9:
                        datos_prov[i] = utiles::GIDToUTF8(lista_provs[k][i].c_str());

                        sqlite3_bind_text(ppStmt, j, datos_prov[i], -1, SQLITE_STATIC);
                        break;
                    case 10: case 11:
                        sqlite3_bind_int(ppStmt, j, utiles::StrToInt(lista_provs[k][i]));
                        break;
                    case 12:
                        sqlite3_bind_int(ppStmt, j, lista_ids[k]);
                        break;
                    default:
                        break;
                }
            }

            if(SQLITE_DONE != sqlite3_step(ppStmt)) {
                // Error al insertar.
                return false;
            }
            sqlite3_reset(ppStmt);

            // Liberamos la memoria para la siguiente iteración.
            for(int i = 0; i < kcampos_txt; ++i)
                delete [] datos_prov[i];
        }
        sqlite3_finalize(ppStmt);
    }

    sqlite3_exec(m_sqltBD, "END", 0, 0, 0);

    return true;
}
*/

std::string Proveedor::to_csv() const
{
    std::string temp {};

    temp += utiles::to_string(codigo, 6) + ";";
    temp += tipo;
    temp += ";";
    temp += razon_social + ";";
    temp += domicilio + ";";
    temp += codigo_postal + ";";
    temp += poblacion + provincia + ";";
    temp += cif + ";";
    temp += tel1 + ";";
    temp += tel2 + ";";
    temp += fax + ";";
    temp += utiles::to_string(forma_pago, 2) + ";";
    temp += utiles::to_string(banco, 3) + ";";

    return temp;
}

