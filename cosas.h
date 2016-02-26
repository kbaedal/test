#ifndef COSAS_H_INCLUDED
#define COSAS_H_INCLUDED

#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include <mysql.h>

#include "utiles.h"

#include "cliente.h"



bool leerClientesGID(std::string nombre_fichero, std::vector<Cliente> &lista_clientes);

bool leerLineaTXTGID(std::fstream &fichero, std::string &info, std::string patron);

bool extraerDatosBlInfCli(BloqueInfoCliente bic_info, Cliente &datos_cliente);

void extraerPobProv(std::string origen, std::string &poblacion, std::string &provincia);

/* ------------------------------- */
/*        COSAS NUEVAS             */
/* --------------------------------*/

void print_mysql_error (MYSQL *my_con, const std::string &mensaje);

bool verificar_tablas(MYSQL *my_con);

bool existe_tabla(MYSQL *my_con, std::string &tabla);

bool existe_cliente(MYSQL *my_con, int codigo_cliente);

bool revisar_clientes(MYSQL *my_con);

int get_datos_cliente(MYSQL *my_con, int codigo, Cliente &c);

bool es_numero(const std::string &str);

#endif // COSAS_H_INCLUDED
