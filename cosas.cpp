#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <mysql.h>

#include "utiles.h"
#include "cosas.h"
#include "cliente.h"

void print_mysql_error (MYSQL *my_con, const std::string &mensaje)
{
    std::cerr << mensaje << std::endl;

    if (my_con != NULL) {
        std::cerr << "Error " << mysql_errno (my_con);
        std::cerr << "(" << mysql_sqlstate (my_con);
        std::cerr << "): " << mysql_error (my_con) << std::endl;
    }
}

bool leer_tablas(const std::string &archivo, std::vector<std::string> &nombre, std::vector<std::string> &cuerpo)
{
    std::ifstream   data_source (archivo);
    std::string     linea, temp;

    if(!data_source.is_open()) {
        std::cout << "Imposible leer tablas de " << archivo << std::endl;
        return false;
    }

    while(std::getline(data_source, linea)) {
        if(linea[0] == '#') // Si encontramos comentario, seguimos.
            continue;

        if(linea.compare("<table>") == 0) { // Entramos en un bloque de tablas
            bool first_line = true;

            while(std::getline(data_source, linea)) { // Leemos el contenido de la tabla.
                if(linea[0] == '#') continue;

                if(linea.compare("</table>") != 0) { // Hasta encontrar el final.
                    if(first_line) { // Primera linea, nombre de tabla.
                        utiles::trim(linea);
                        temp = "CREATE TABLE " + linea + " (";
                        nombre.push_back(linea);
                        first_line = false;
                    }
                    else {
                        temp += linea + ", ";
                    }
                }
                else {
                    // Fin de la tabla. Cerramos y a por la siguiente.
                    // Reemplazmos la coma y el espacio finales por el cierre
                    // de la sentencia CREATE.
                    temp.replace(temp.end()-2, temp.end(), ");");

                    cuerpo.push_back(temp);
                    break;
                }
            }
        }
    }

    return true;
}

bool existe_tabla(MYSQL *my_con, std::string &tabla)
{
    std::string consulta = "SHOW TABLES FROM sepyadb LIKE \'" + tabla + "\'";
    MYSQL_RES   *my_res;
    bool        valor_ret = true;

    if(mysql_query(my_con, consulta.c_str()) != 0) {
        std::string err_string("Fallo al consultar existencia de la tabla " + tabla);
        print_mysql_error(my_con, err_string);

        valor_ret = false;
    }
    else {
        if((my_res = mysql_store_result(my_con)) != NULL) {
            // Procesar resultados
            if(!mysql_num_rows(my_res)) valor_ret = false;
            // Liberar el resultado de la consulta:
            mysql_free_result(my_res);
        }
    }

    return valor_ret;
}

bool existe_cliente(MYSQL *my_con, int codigo_cliente)
{
    std::string consulta = "SELECT * FROM CLIENTE WHERE cod_cliente=" + utiles::IntToStr(codigo_cliente, 6);
    MYSQL_RES   *my_res;
    bool        valor_ret = false;

    if(mysql_query(my_con, consulta.c_str()) != 0) {
        std::string err_string("Fallo al consultar existencia del cliente " + utiles::IntToStr(codigo_cliente, 6));
        print_mysql_error(my_con, err_string);

        valor_ret = false;
    }
    else {
        if((my_res = mysql_store_result(my_con)) != NULL) {
            // Procesar resultados
            if(mysql_num_rows(my_res) != 0) valor_ret = true;
            // Liberar el resultado de la consulta:
            mysql_free_result(my_res);
        }
    }

    return valor_ret;
}

bool verificar_tablas(MYSQL *my_con)
{
    std::vector<std::string>    tablas, creates;

    if(!leer_tablas("c:/prog/test/mydb.dat", tablas, creates))
    {
        std::cout << "Error leyendo las tablas." << std::endl;
        return -1;
    }

    for(size_t i = 0; i < tablas.size(); ++i) {

        if(!existe_tabla(my_con, tablas[i])) {
            // Creamos la tabla.
            std::cout << "Creamos la tabla: " << tablas[i] << std::endl;

            if(mysql_query(my_con, creates[i].c_str())) {
                std::string err_string("Error al crear la tabla " + tablas[i]);
                print_mysql_error(my_con, err_string);
            }
        }
        else {
            std::cout << "La tabla " << tablas[i] << " ya existe." << std::endl;
        }

    }

    std::cout << "Salimos de verificar_tablas()." << std::endl;

    return true;
}

bool revisar_clientes(MYSQL *my_con)
{
    // Plan:
    //  1. Leer los clientes del archivo, almacenarlos en una lista.
    //  2. Comprobar, uno por uno, si existen en la base de datos.
    //      2.a. Si existe, a la lista de clientes a actulizar.
    //      2.b. Si no existe, a la lista de clientes a insertar.
    //  3. Si hay clientes que actualizar, hacerlo.
    //  4. Si hay clientes que insertar, hacerlo.
    //  5. Limpiar la casa, y retornar.

    std::vector<Cliente>    lista_clientes, clientes_actualizar, clientes_insertar;
    //MYSQL_RES               *my_res;

    std::cout << "Leyendo fichero de clientes... " << std::endl;
    if(!leerClientesGID("c:/cli.txt", lista_clientes)) {
        std::cout << "\tNo se ha podido leer el fichero de clientes." << std::endl;
        return false;
    }
    else {
        std::cout << "\tFichero de clientes leido. Total clientes: ";
        std::cout << lista_clientes.size() << std::endl;
    }

    for(auto c : lista_clientes) {
        if(existe_cliente(my_con, c.codigo)) {
            clientes_actualizar.push_back(c); // El cliente existe ya, actualizar.
        }
        else {
            clientes_insertar.push_back(c); // El cliente no existe, insertar.
        }
    }

    // Actualización de cliente.
    std::cout << "Actualizando " << clientes_actualizar.size() << " clientes... ";
    for(auto c : clientes_actualizar) {
        std::string consulta = c.get_mysql_update();

        if(mysql_query(my_con, consulta.c_str())) {
            std::string err_string("Error al actualizar cliente " + utiles::IntToStr(c.codigo));
            print_mysql_error(my_con, err_string);
        }
    }
    std::cout << "Hecho." << std::endl;

    std::cout << "Insertando " << clientes_insertar.size() << " clientes... ";
    for(auto c : clientes_insertar) {
        std::string consulta = c.get_mysql_insert();

        if(mysql_query(my_con, consulta.c_str())) {
            std::string err_string("Error al insertar cliente " + utiles::IntToStr(c.codigo));
            print_mysql_error(my_con, err_string);
        }
    }
    std::cout << "Hecho." << std::endl;

    return true;
}

bool leerClientesGID(std::string nombre_fichero, std::vector<Cliente> &lista_clientes)
{
    std::fstream        fichero;        // Manejo el fichero.
    bool                codigo;         // Indicara si tenemos codigo o no.
    BloqueInfoCliente   bic_info;       // Almacenaremos la informacion leida
                                        // para despues procesarla.
    Cliente             datos_cliente;  // Datos a insertar en la BBDD.
    std::string         linea_fichero;  // Para la lectura del fichero.

    // Abrimos el fichero.
    fichero.open(nombre_fichero.c_str(), std::ios::in);

    if(!fichero.good())
        // Imposible abrir el fichero.
        return false;

	// Procesamos los clientes
	while(!fichero.eof()) // Hasta fin de fichero.
	{
	    codigo = false;
		do // Mientras que no encontremos un cliente.
		{
			std::getline(fichero, linea_fichero);

            if(linea_fichero.length() > 10) // Linea con datos, no en blanco.
                if(linea_fichero.substr(2, 10).compare("CODIGO....") == 0)
                    codigo = true;

        } while(!codigo && !fichero.eof());

		if(!fichero.eof()) // Nos aseguramos de que no estamos en el final del fichero.
		{
		    // Copiamos los datos de la primera linea al almacen
		    bic_info.linea_codigo.assign(linea_fichero);

            // Seguimos con el resto de lineas.

            // Linea RAZON SOC.
            if(!leerLineaTXTGID(fichero, bic_info.linea_razon, std::string("RAZON SOC.")))
                return false;

            // Linea NOMBRE....
            if(!leerLineaTXTGID(fichero, bic_info.linea_nombre, std::string("NOMBRE....")))
                return false;

            // Linea DOMICILIO.
            if(!leerLineaTXTGID(fichero, bic_info.linea_domicilio, std::string("DOMICILIO.")))
                return false;

            // Linea POBLACION.
            if(!leerLineaTXTGID(fichero, bic_info.linea_poblacion, std::string("POBLACION.")))
                return false;

            // Linea COD.POST..
            if(!leerLineaTXTGID(fichero, bic_info.linea_c_postal, std::string("COD.POST..")))
                return false;

            // Linea OBSERVAC..
            if(!leerLineaTXTGID(fichero, bic_info.linea_observaciones, std::string("OBSERVAC..")))
                return false;

            // Limpiamos el almacen de datos.
            datos_cliente.limpiar();

            // Extraemos los datos.
            if(!extraerDatosBlInfCli(bic_info, datos_cliente))
                return false;

            // Insertamos los datos en la lista.
            lista_clientes.push_back(datos_cliente);
		}
	}

	// Cerramos el fichero.
	fichero.close();

    // Proceso correcto.
	return true;
}

bool leerLineaTXTGID(std::fstream &fichero, std::string &info, std::string patron)
{
    // Buscamos la siguiente linea. Podremos encontrar cuatro cosas:
	//  - Linea en blanco: Seguimos buscando.
	//  - Linea compuesta de guiones: Seguimos buscando.
	//  - Linea de encabezado: Seguimos buscando.
	//  - Linea con patron en posiciones 2 a 12: Bingo.
	// Cualquier otra cosa encontrada, salimos devolviendo false.

    std::string linea_fichero;

    while(!fichero.eof()) {
        std::getline(fichero, linea_fichero);

        if(linea_fichero.length() < 10)
            // Linea en blanco, siguiente.
            continue;
        else if(linea_fichero.substr(2, 7).compare("-------") == 0)
            // Linea con guiones, siguiente.
            continue;
        else if(linea_fichero.substr(1, 7).compare("MARCIAL") == 0)
            // Linea de encabezado, siguiente.
            continue;
        else if(linea_fichero.substr(2, 10).compare(patron) == 0)
            // La linea que estamos buscando. Salimos.
            break;
        else
            // Lo que se haya leido no debería estar ahí. Error.
            return false;
    }

    if(!fichero.eof())
        // Almacenamos la linea en su sitio y seguimos.
        info = linea_fichero;
    else
        // Fin de fichero inesperado.
        return false;

    return true;
}

bool extraerDatosBlInfCli(BloqueInfoCliente bic_info, Cliente &datos_cliente)
{
    std::string iban,
                ccc,
                bic;

    // Linea: CODIGO
    // Leemos el código del cliente, y lo convertimos a int.
    datos_cliente.codigo = utiles::StrToInt(bic_info.linea_codigo.substr(14, 6));

	// La categoría, también a int.
	datos_cliente.categoria = utiles::StrToInt(bic_info.linea_codigo.substr(143, 1));

    // Linea: RAZON SOCIAL
    // La razón social está equivocada con el Nombre Comercial,
    // así que lo ignoramos.
    // DatCliente.strRazonSocial.assign(bicInfo.strLineaRazon, 14, 40);

    // Forma de pago, a int.
    datos_cliente.forma_pago = utiles::StrToInt(bic_info.linea_razon.substr(82, 3));

    // SWIFT-BIC
    // Habrá que comprobar que es correcto.
    datos_cliente.swift_bic.assign(bic_info.linea_razon, 172, 11);
    //utiles::setTextToISO20022(datos_cliente.swift_bic);
    utiles::GID_to_UTF8(datos_cliente.swift_bic);

    // Linea: NOMBRE
    // Razón social, que está extrañamente en esta linea.
    datos_cliente.razon_social.assign(bic_info.linea_nombre, 14, 40);
    utiles::trim(datos_cliente.razon_social);
    //utiles::setTextToISO20022(datos_cliente.razon_social);
    utiles::GID_to_UTF8(datos_cliente.razon_social);

    // Teléfono 1.
    datos_cliente.tel1.assign(bic_info.linea_nombre, 57, 12);
    utiles::trim(datos_cliente.tel1);

    // Linea: DOMICILIO
    // Domicilio.
    datos_cliente.domicilio.assign(bic_info.linea_domicilio, 14, 40);
    utiles::trim(datos_cliente.domicilio);
    //utiles::setTextToISO20022(datos_cliente.domicilio);
    utiles::GID_to_UTF8(datos_cliente.domicilio);

    // Teléfono 2.
    datos_cliente.tel2.assign(bic_info.linea_domicilio, 57, 12);
    utiles::trim(datos_cliente.tel2);

    // Leemos el CCC.
    // Se necesitará comprobar que es correcto.
    datos_cliente.iban.assign(bic_info.linea_domicilio, 189, 4);
    datos_cliente.iban.append(bic_info.linea_domicilio, 194, 4);
    datos_cliente.iban.append(bic_info.linea_domicilio, 199, 2);
    datos_cliente.iban.append(bic_info.linea_domicilio, 206, 10);

    // Cambiamos los espacios, si los hay, por ceros.
    for(size_t i = 0; i < datos_cliente.iban.size(); i++)
        if(datos_cliente.iban[i] == ' ')
            datos_cliente.iban[i] = '0';

    // Calculamos el IBAN correspondiente. Si el CCC no es válido o no
    // existe, lo eliminamos para evitar errores.
    if(datos_cliente.iban.compare("00000000000000000000") != 0 && utiles::checkCCC(datos_cliente.iban))
        datos_cliente.iban.assign(utiles::calculaIBAN(datos_cliente.iban));
    else
        datos_cliente.iban.clear();

    // Linea POBLACION.
    // Extraemos la poblacion y la provincia de la cadena szTemp.
    extraerPobProv(bic_info.linea_poblacion.substr(14, 40), datos_cliente.poblacion, datos_cliente.provincia);
    //utiles::setTextToISO20022(datos_cliente.poblacion);
    //utiles::setTextToISO20022(datos_cliente.provincia);
    utiles::GID_to_UTF8(datos_cliente.poblacion);
    utiles::GID_to_UTF8(datos_cliente.provincia);

    datos_cliente.fax.assign(bic_info.linea_poblacion, 57, 12);
    utiles::trim(datos_cliente.fax);

    // Linea COD.POST..
    // Código postal
    datos_cliente.codigo_postal.assign(bic_info.linea_c_postal, 14, 5);
    // Cambiamos los espacios en blanco, si los hay, por 0.
    for(size_t i = 0; i < datos_cliente.codigo_postal.size(); i++)
        if(datos_cliente.codigo_postal[i] == ' ')
            datos_cliente.codigo_postal[i] = '0';

    // NIF
    datos_cliente.nif.assign(bic_info.linea_c_postal, 32, 16);
    utiles::trim(datos_cliente.nif);

    // Eliminamos el guión del NIF, si lo hay, y los espacios.
    for(size_t i = 0; i < datos_cliente.nif.size(); i++)
        if(datos_cliente.nif[i] == '-' || datos_cliente.nif[i] == ' ')
            datos_cliente.nif.erase(i, 1);

    // Todo correcto.
	return true;
}

void extraerPobProv(std::string origen, std::string &poblacion, std::string &provincia)
{
    int     inicio_provincia = 0,
            fin_provincia = 0;      // Almacenaremos incio y final de la provincia.

    poblacion.clear();
    provincia.clear();

    // Buscamos el inicio de la provincia.
    // Almacenaremos en nInicioProv la última
    // posición en la que encontremos un '('.
    for(size_t i = 0; i < origen.size(); i++) {
        if(origen[i] == '(') {
            inicio_provincia = i;
        }
    }

    // Copiamos la población, controlando que no sobrepasemos
    // la capacidad de szPob.
    if(inicio_provincia != 0) {
        // Si es cero, quiere decir que no hemos encontrado
        // ningún paréntesis, luego todo sería población.

        // Buscamos ahora el final.
        for(size_t i = inicio_provincia; i < origen.size(); i++) {
            if(origen[i] == ')') {
                fin_provincia = i - 1; // Marcamos el caracter anterior.
            }
        }

        // Hasta el primer parentesis: poblacion.
        poblacion.assign(origen.substr(0, inicio_provincia - 1));
        utiles::trim(poblacion);

        // Desde el primer parentesis, hasta el cierre de parentesis: provincia.
        provincia.assign(origen.substr(inicio_provincia + 1, fin_provincia - inicio_provincia));
        utiles::trim(provincia);
    }
    else {
        // No hemos encontrado ningún paréntesis.
        poblacion.assign(origen);
        utiles::trim(poblacion);
    }

    utiles::setTextToISO20022(poblacion);
    utiles::setTextToISO20022(provincia);
}

int get_datos_cliente(MYSQL *my_con, int codigo, Cliente &c)
{
    std::string     consulta = "SELECT * FROM cliente WHERE COD_CLIENTE=" + utiles::IntToStr(codigo, 6);
    MYSQL_RES       *resultado;
    MYSQL_ROW       fila;
    int             num_col;

    if(mysql_query(my_con, consulta.c_str()) != 0) {
        print_mysql_error(my_con, "Fallo al seleccionar un cliente.");
        return 1;
    }
    else {
        if((resultado = mysql_store_result(my_con)) == NULL) {
            print_mysql_error(my_con, "Fallo al almacenar el resultado.");
            return 2;
        }
        else {
            num_col = static_cast<int>(mysql_num_fields(resultado));

            while((fila = mysql_fetch_row(resultado)) != NULL) {
                for(int i = 0; i < num_col; ++i) {
                    if(fila[i] != NULL)
                        std::cout << fila[i] << " ";
                    else
                        std::cout << "NULL";
                }
                std::cout << std::endl;
            }

            mysql_free_result(resultado);
        }
    }

    return 0;
}
