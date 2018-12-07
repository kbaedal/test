#ifndef COSAS_H_INCLUDED
#define COSAS_H_INCLUDED

#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include <mysql.h>

#include "utiles.h"

#include "cliente.h"
#include "proveedor.h"
#include "db_aux_def.h"

bool leerClientesGID(std::string nombre_fichero, std::vector<Cliente> &lista_clientes);

bool leerProveedoresGID(std::string nombre_fichero, std::vector<Proveedor> &lista_proveedores);

bool leerLineaTXTGID(std::fstream &fichero, std::string &info, std::string patron);

bool extraerDatosBlInfCli(BloqueInfoCliente bic_info, Cliente &datos_cliente);

bool extraerDatosBlInfPro(BloqueInfoProveedor bip_info, Proveedor &datos_proveedor);

void extraerPobProv(std::string origen, std::string &poblacion, std::string &provincia);

/* ------------------------------- */
/*        COSAS NUEVAS             */
/* --------------------------------*/

void print_mysql_error (MYSQL *my_con, const std::string &mensaje);

void print_mysql_stmt_error (MYSQL_STMT *stmt, const std::string &mensaje);

bool verificar_tablas(MYSQL *my_con);

bool existe_tabla(MYSQL *my_con, std::string &tabla);

bool existe_cliente(MYSQL *my_con, int codigo_cliente);

bool revisar_clientes(MYSQL *my_con);

bool insertar_clientes(MYSQL *my_con, std::vector<Cliente> &lista_clientes);

bool actualizar_clientes(MYSQL *my_con, std::vector<Cliente> &lista_clientes);

int get_datos_cliente(MYSQL *my_con, int codigo, Cliente &c);

int update_datos_cliente(MYSQL *my_con, const Cliente &c);

//bool set_insert_client_struct(MYSQL_BIND *my_bind, cliente_mysql_bind &data);
bool set_insert_client_struct(MYSQL_BIND *my_bind, data_bind_storage &data);

//bool set_update_client_struct(MYSQL_BIND *my_bind, cliente_mysql_bind &data);
bool set_update_client_struct(MYSQL_BIND *my_bind, data_bind_storage &data);

bool es_numero(const std::string &str);

bool set_mysql_bind(MYSQL_BIND *my_bind, data_bind_storage &data, const std::vector<info_campo> &info_bind);

// Pues eso, a csv.
bool cli_to_csv();
bool pro_to_csv();

#endif // COSAS_H_INCLUDED
