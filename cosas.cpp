#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <mysql.h>

#include "cosas.h"
#include "cliente.h"
#include "proveedor.h"
#include "db_aux_def.h"
#include "utiles.h"

void print_mysql_error (MYSQL *my_con, const std::string &mensaje)
{
    std::cerr << mensaje << std::endl;

    if (my_con != NULL) {
        std::cerr << "Error " << mysql_errno (my_con);
        std::cerr << "(" << mysql_sqlstate (my_con);
        std::cerr << "): " << mysql_error (my_con) << std::endl;
    }
}

void print_mysql_stmt_error (MYSQL_STMT *stmt, const std::string &mensaje)
{
    std::cerr << mensaje << std::endl;

    if (stmt != NULL) {
        std::cerr << "Error " << mysql_stmt_errno (stmt);
        std::cerr << "(" << mysql_stmt_sqlstate (stmt);
        std::cerr << "): " << mysql_stmt_error (stmt) << std::endl;
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

bool cli_to_csv()
{
    std::vector<Cliente>    lista_clientes;

    std::fstream            fichero;        // Manejo el fichero.

    std::cout << "Leyendo fichero de clientes... " << std::endl;
    if(!leerClientesGID("c:/cli.txt", lista_clientes)) {
        std::cout << "\tNo se ha podido leer el fichero de clientes." << std::endl;
        return false;
    }
    else {
        std::cout << "\tFichero de clientes leido. Total clientes: ";
        std::cout << lista_clientes.size() << std::endl;
    }

    // Abrimos el fichero.
    fichero.open("c:/clipb.csv", std::ios::out);

    int i = 0;
    for(auto c : lista_clientes) {
        std::cout << "\rEscribiendo " << lista_clientes.size() << " clientes... ";
        std::cout << utiles::to_string(++i, 6, ' ') << "/" << utiles::to_string(lista_clientes.size(), 6, ' ');

        fichero << c.to_csv() << "\n";
    }

    fichero.close();

    return true;
}

bool pro_to_csv()
{
    std::vector<Proveedor>  lista_proveedores;

    std::fstream            fichero;        // Manejo el fichero.

    std::cout << "Leyendo fichero de proveedores... " << std::endl;
    if(!leerProveedoresGID("c:/pro.txt", lista_proveedores)) {
        std::cout << "\tNo se ha podido leer el fichero de proveedores." << std::endl;
        return false;
    }
    else {
        std::cout << "\tFichero de proveedores leido. Total proveedores: ";
        std::cout << lista_proveedores.size() << std::endl;
    }

    // Abrimos el fichero.
    fichero.open("c:/propb.csv", std::ios::out);

    // Encabezado.
    fichero << "codigo;tipo;razon_social;domicilio;codigo_postal;poblacion_provincia;cif;tel1;tel2;fax;forma_pago;banco;" << "\n";

    int i = 0;
    for(auto p : lista_proveedores) {
        std::cout << "\rEscribiendo " << lista_proveedores.size() << " proveedores... ";
        std::cout << utiles::to_string(++i, 6, ' ') << "/" << utiles::to_string(lista_proveedores.size(), 6, ' ');

        fichero << p.to_csv() << "\n";
    }

    fichero.close();

    return true;
}

bool revisar_clientes(MYSQL *my_con)
{
    // Plan:
    //  1. Leer los clientes del archivo, almacenarlos en una lista.
    //  2. Comprobar, uno por uno, si existen en la base de datos.
    //      2.a. Si existe, a la lista de clientes a actualizar.
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

    // Actualización de clientes.
    if(clientes_actualizar.size() > 0)
        actualizar_clientes(my_con, clientes_actualizar);

    // Inserción de clientes
    if(clientes_insertar.size() > 0)
        insertar_clientes(my_con, clientes_insertar);

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
    //utiles::GID_to_UTF8(datos_cliente.razon_social);

    // Teléfono 1.
    datos_cliente.tel1.assign(bic_info.linea_nombre, 57, 12);
    utiles::trim(datos_cliente.tel1);

    // Linea: DOMICILIO
    // Domicilio.
    datos_cliente.domicilio.assign(bic_info.linea_domicilio, 14, 40);
    utiles::trim(datos_cliente.domicilio);
    //utiles::setTextToISO20022(datos_cliente.domicilio);
    //utiles::GID_to_UTF8(datos_cliente.domicilio);

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
    // Quedan convertidas a UTF8 directamente.
    extraerPobProv(bic_info.linea_poblacion.substr(14, 40), datos_cliente.poblacion, datos_cliente.provincia);

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

bool extraerDatosBlInfPro(BloqueInfoProveedor bip_info, Proveedor &datos_proveedor)
{
    std::string iban,
                ccc,
                bic;

    // Linea: CODIGO
    // Leemos el código del cliente, y lo convertimos a int.
    datos_proveedor.codigo = utiles::StrToInt(bip_info.linea_codigo.substr(14, 4));

	// El tipo, también a int.
	datos_proveedor.tipo = bip_info.linea_codigo.substr(24, 1)[0];

    // Linea: NOMBRE
    // La razón social del proveedor.
    datos_proveedor.razon_social.assign(bip_info.linea_nombre, 14, 40);
    utiles::trim(datos_proveedor.razon_social);
    utiles::GID_to_LATIN(datos_proveedor.razon_social);

    // El teléfono 1.
    datos_proveedor.tel1.assign(bip_info.linea_nombre, 57, 12);
    utiles::trim(datos_proveedor.tel1);

    // Linea: DOMICILIO
    // El domicilio.
    datos_proveedor.domicilio.assign(bip_info.linea_domicilio, 14, 40);
    utiles::trim(datos_proveedor.razon_social);
    utiles::GID_to_LATIN(datos_proveedor.razon_social);

    // El teléfono 2.
    datos_proveedor.tel2.assign(bip_info.linea_domicilio, 57, 12);
    utiles::trim(datos_proveedor.tel2);

    // Linea: POBLACION
    // Extraemos pobalcion y provincia.
    extraerPobProv(
        bip_info.linea_poblacion.substr(14, 40),
        datos_proveedor.poblacion,
        datos_proveedor.provincia
    );

    // Y el fax.
    datos_proveedor.fax.assign(bip_info.linea_poblacion, 57, 12);
    utiles::trim(datos_proveedor.fax);

    // Linea: COD.POST
    // El código postal.
    datos_proveedor.codigo_postal.assign(bip_info.linea_c_postal, 14, 5);
    // Cambiamos los espacios en blanco, si los hay, por 0.
    for(size_t i = 0; i < datos_proveedor.codigo_postal.size(); ++i)
        if(datos_proveedor.codigo_postal[i] == ' ')
            datos_proveedor.codigo_postal[i] = '0';

    // Y el CIF.
    datos_proveedor.cif.assign(bip_info.linea_c_postal, 38, 16);
    utiles::trim(datos_proveedor.cif);
    // Eliminamos el guion o los espacios en blanco del CIF, si los hay.
    for(size_t i = 0; i < datos_proveedor.cif.size(); ++i)
        if(datos_proveedor.cif[i] == '-' || datos_proveedor.cif[i] == ' ')
            datos_proveedor.cif.erase(i, 1);

    // Linea FORMA PAGO
    // La forma de pago.
    datos_proveedor.forma_pago = utiles::StrToInt(bip_info.linea_f_pago.substr(14, 2));

    // Linea BANCO
    // El código del banco.
    datos_proveedor.banco = utiles::StrToInt(bip_info.linea_banco.substr(14, 4));

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

    //utiles::GID_to_UTF8(poblacion);
    //utiles::GID_to_UTF8(provincia);
    utiles::GID_to_LATIN(poblacion);
    utiles::GID_to_LATIN(provincia);
}

bool leerProveedoresGID(std::string nombre_fichero, std::vector<Proveedor> &lista_proveedores)
{
    std::fstream        fichero;            // Manejo el fichero.
    bool                codigo;             // Indicara si tenemos codigo o no.
    BloqueInfoProveedor bip_info;           // Almacenaremos la informacion leida
                                            // para despues procesarla.
    Proveedor           datos_proveedor;    // Datos a insertar en la BBDD.
    std::string         linea_fichero;      // Para la lectura del fichero.

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
		    bip_info.linea_codigo.assign(linea_fichero);

            // Seguimos con el resto de lineas.

            // Linea NOMBRE....
            if(!leerLineaTXTGID(fichero, bip_info.linea_nombre, std::string("NOMBRE....")))
                return false;

            // Linea DOMICILIO.
            if(!leerLineaTXTGID(fichero, bip_info.linea_domicilio, std::string("DOMICILIO.")))
                return false;

            // Linea POBLACION.
            if(!leerLineaTXTGID(fichero, bip_info.linea_poblacion, std::string("POBLACION.")))
                return false;

            // Linea COD.POST..
            if(!leerLineaTXTGID(fichero, bip_info.linea_c_postal, std::string("COD.POST..")))
                return false;

            // Linea FORMA PAGO
            if(!leerLineaTXTGID(fichero, bip_info.linea_f_pago, std::string("FORMA PAGO")))
                return false;

            // Linea BANCO.....
            if(!leerLineaTXTGID(fichero, bip_info.linea_banco, std::string("BANCO.....")))
                return false;

            // Limpiamos el almacen de datos.
            datos_proveedor.limpiar();

            // Extraemos los datos.
            if(!extraerDatosBlInfPro(bip_info, datos_proveedor))
                return false;

            // Insertamos los datos en la lista.
            lista_proveedores.push_back(datos_proveedor);
		}
	}

	// Cerramos el fichero.
	fichero.close();

    // Proceso correcto.
	return true;
}

int get_datos_cliente(MYSQL *my_con, int codigo, Cliente &c)
{
    std::string     consulta = "SELECT * FROM cliente WHERE COD_CLIENTE=" + utiles::to_string(codigo, 6);
    MYSQL_RES       *resultado;
    MYSQL_ROW       fila;

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
            if(static_cast<int>(mysql_num_rows(resultado)) > 1) {
                print_mysql_error(my_con, "Varios clientes con el mismo codigo.");
                return 3;
            }

            if(static_cast<int>(mysql_num_fields(resultado)) != 15) {
                print_mysql_error(my_con, "El numero de columnas no coincide.");
                return 4;
            }

            // Columna      Contenido       Tipo
            // -------      ---------       ----
            //  0           id_cliente      Entero (No usable, interno BD)
            //  1           cod_cliente     Entero
            //  2           razon_social    Texto
            //  3           domicilio       Texto
            //  4           cod_postal      Texto
            //  5           poblacion       Texto
            //  6           provincia       Texto
            //  7           nif             Texto
            //  8           tel1            Texto
            //  9           tel2            Texto
            //  10          fax             Texto
            //  11          forma_pago      Entero
            //  12          categoria       Entero
            //  13          swift_bic       Texto
            //  14          iban            Texto

            while((fila = mysql_fetch_row(resultado)) != NULL) {
                c.codigo        = (fila[1] != NULL) ? utiles::to_integer(fila[1]) : 0;
                c.razon_social  = fila[2];
                c.domicilio     = fila[3];
                c.codigo_postal = fila[4];
                c.poblacion     = fila[5];
                c.provincia     = fila[6];
                c.nif           = fila[7];
                c.tel1          = fila[8];
                c.tel2          = fila[9];
                c.fax           = fila[10];
                c.forma_pago    = (fila[11] != NULL) ? utiles::to_integer(fila[11]) : 0;
                c.categoria     = (fila[12] != NULL) ? utiles::to_integer(fila[12]) : 0;
                c.swift_bic     = fila[13];
                c.iban          = fila[14];
            }

            mysql_free_result(resultado);
        }
    }

    return 0;
}

int update_datos_cliente(MYSQL *my_con, const Cliente &c)
{
    std::string     consulta = c.get_mysql_update_str();

    if(mysql_query(my_con, consulta.c_str()) != 0) {
        print_mysql_error(my_con, "Fallo al actualizar cliente.");
        return 1;
    }

    return 0;
}

bool insertar_clientes(MYSQL *my_con, std::vector<Cliente> &lista_clientes)
{
    MYSQL_STMT          *stmt;          // Manejador de la consulta preparada.
    MYSQL_BIND          my_bind[14];    // Estructura para los datos de la consulta.
    data_bind_storage   cmb(db_consts::cliente_nc_txt,      // Reservamos espacio para
                            db_consts::cliente_nc_int,      // almacenar los datos
                            db_consts::cliente_max_txt);    // de un cliente.

    // Ponemos la estructura para los datos a 0.
    std::memset(static_cast<void *>(my_bind), 0, sizeof(my_bind));

    // Reservamos manejador
    if((stmt = mysql_stmt_init(my_con)) == NULL)
    {
        print_mysql_error(my_con, "Imposible inicializar el manejador de la consulta.");
        return false;
    }

    // Preparamos la consulta.
    if(mysql_stmt_prepare(stmt, db_queries::ins_cliente.c_str(), db_queries::ins_cliente.size()) != 0)  {
        print_mysql_stmt_error(stmt, "Imposible preparar consulta INSERT.");
        return false;
    }

    // Asociamos la estructura my_bind al almacen de datos.
    set_mysql_bind(my_bind, cmb, db_binds::cliente_ins_bind);

    // Asociamos los datos a la consulta.
    if(mysql_stmt_bind_param(stmt, my_bind) != 0) {
        print_mysql_stmt_error(stmt, "Imposible asociar los datos a la consulta.");
        return false;
    }

    int i = 1;
    for(auto &&c : lista_clientes) {
        std::cout << "\rInsertando " << lista_clientes.size() << " clientes... ";
        std::cout << utiles::to_string(i, 6, ' ') << "/" << utiles::to_string(lista_clientes.size(), 6, ' ');

        // Rellenamos el "receptaculo" con los datos del cliente. La estructura
        // my_bind está asociada a esta otra estructura, luego la consulta enviará
        // los datos de esta estructura a traves de la estructura my_bind.
        c.fill_mysql_bind(cmb);

        // Procesamos la consulta con los datos.
        if(mysql_stmt_execute(stmt) != 0) {
            print_mysql_stmt_error(stmt, "Imposible ejecutar consulta preparada.");
            return false;
        }

        ++i;
    }

    std::cout << "\tHecho.\n\n";

    if(mysql_stmt_close(stmt) != 0) {
        print_mysql_stmt_error(stmt, "Imposible cerrar el manejador de la consulta.");
        return false;
    }

    return true;
}

bool actualizar_clientes(MYSQL *my_con, std::vector<Cliente> &lista_clientes)
{
    MYSQL_STMT          *stmt;          // Manejador de la consulta preparada.
    MYSQL_BIND          my_bind[14];    // Estructura para los datos de la consulta.
    data_bind_storage   cmb(db_consts::cliente_nc_txt,      // Reservamos espacio para
                            db_consts::cliente_nc_int,      // almacenar los datos
                            db_consts::cliente_max_txt);    // de un cliente.

    // Ponemos la estructura para los datos a 0.
    std::memset(static_cast<void *>(my_bind), 0, sizeof(my_bind));

    // Reservamos manejador
    if((stmt = mysql_stmt_init(my_con)) == NULL)
    {
        print_mysql_error(my_con, "Imposible inicializar el manejador de la consulta.");
        return false;
    }

    // Preparamos la consulta.
    if(mysql_stmt_prepare(stmt, db_queries::upd_cliente.c_str(), db_queries::upd_cliente.size()) != 0)  {
        print_mysql_stmt_error(stmt, "Imposible preparar consulta UPDATE.");
        return false;
    }

    // Asociamos la estructura my_bind al almacen de datos.
    set_mysql_bind(my_bind, cmb, db_binds::cliente_upd_bind);

    // Asociamos los datos a la consulta.
    if(mysql_stmt_bind_param(stmt, my_bind) != 0) {
        print_mysql_stmt_error(stmt, "Imposible asociar los datos a la consulta.");
        return false;
    }

    int i = 1;
    for(auto &&c : lista_clientes) {
        std::cout << "\rActualizando " << lista_clientes.size() << " clientes... ";
        std::cout << utiles::to_string(i, 6, ' ') << "/" << utiles::to_string(lista_clientes.size(), 6, ' ');

        // Rellenamos el "receptaculo" con los datos del cliente. La estructura
        // my_bind está asociada a esta otra estructura, luego la consulta enviará
        // los datos de esta estructura a traves de la estructura my_bind.
        c.fill_mysql_bind(cmb);

        // Procesamos la consulta con los datos.
        if(mysql_stmt_execute(stmt) != 0) {
            print_mysql_stmt_error(stmt, "Imposible ejecutar consulta preparada.");
            return false;
        }

        ++i;
    }

    std::cout << "\tHecho.\n\n";

    mysql_stmt_close(stmt);

    return true;
}

//bool set_insert_client_struct(MYSQL_BIND *my_bind, cliente_mysql_bind &data)
bool set_insert_client_struct(MYSQL_BIND *my_bind, data_bind_storage &data)
{
    int     str_count = 0,  // Cadenas copiadas.
            int_count = 0;  // Enteros copiados.

    // El orden de los campos depende de la consulta.
    for(size_t i = 0; i < 14; ++i) {
        if((i == 0) || (i ==  10) || (i == 11)) {
            // Enteros. Asignamos y seguimos.
            my_bind[i].buffer_type  = MYSQL_TYPE_LONG;
            my_bind[i].buffer       = static_cast<void *>(&data.int_data[int_count]);

            ++int_count;
        }
        else {
            // Cadenas de caracteres.
            my_bind[i].buffer_type     = MYSQL_TYPE_STRING;
            // Indicamos la direccion del primer caracter de la cadena, para
            // asegurarnos de que buffer contiene la dirección correcta.
            my_bind[i].buffer          = static_cast<void *>(&data.str_data[str_count][0]);
            my_bind[i].buffer_length   = 40;
            my_bind[i].is_null         = 0;
            my_bind[i].length          = &data.str_long[str_count];

            ++str_count;
        }
    }

    return true;
}

//bool set_update_client_struct(MYSQL_BIND *my_bind, cliente_mysql_bind &data)
bool set_update_client_struct(MYSQL_BIND *my_bind, data_bind_storage &data)
{
    int     str_count = 0,  // Cadenas copiadas.
            int_count = 1;  // Enteros copiados. A 1 porque dejamos el primero para el final.

    // El orden de los campos depende de la consulta.
    for(size_t i = 0; i < 13; ++i) { // Ignoramos el ultimo campo, queda para el final.
        if((i ==  9) || (i == 10))  {
            // Enteros. Asignamos y seguimos.
            my_bind[i].buffer_type  = MYSQL_TYPE_LONG;
            my_bind[i].buffer       = static_cast<void *>(&data.int_data[int_count]);

            ++int_count;
        }
        else {
            // Cadenas de caracteres.
            my_bind[i].buffer_type     = MYSQL_TYPE_STRING;
            // Indicamos la direccion del primer caracter de la cadena, para
            // asegurarnos de que buffer contiene la dirección correcta.
            my_bind[i].buffer          = static_cast<void *>(&data.str_data[str_count][0]);
            my_bind[i].buffer_length   = 40;
            my_bind[i].is_null         = 0;
            my_bind[i].length          = &data.str_long[str_count];

            ++str_count;
        }
    }

    // Ahora solo nos queda el último, el codigo de cliente, que esta
    // en la posicion 0 de la matriz int_data.
    my_bind[13].buffer_type   = MYSQL_TYPE_LONG;
    my_bind[13].buffer        = static_cast<void *>(&data.int_data[0]);

    return true;
}

bool set_mysql_bind(MYSQL_BIND *my_bind, data_bind_storage &data, const std::vector<info_campo> &info_bind)
{

    for(size_t i = 0; i < info_bind.size(); ++i) {
        switch(info_bind[i].data_type) {
            case tipo_campo::IS_INT :
                // Tipo de dato: entero.
                my_bind[info_bind[i].bind_pos].buffer_type = MYSQL_TYPE_LONG;

                // Asociacion al buffer correcto
                my_bind[info_bind[i].bind_pos].buffer =
                    static_cast<void *>(&data.int_data[info_bind[i].data_pos]);

                break;
            case tipo_campo::IS_TXT :
                // Tipo de dato: cadenas de caracteres.
                my_bind[info_bind[i].bind_pos].buffer_type = MYSQL_TYPE_STRING;

                // Indicamos la direccion del primer caracter de la cadena, para
                // asegurarnos de que buffer contiene la dirección correcta.
                my_bind[info_bind[i].bind_pos].buffer =
                    static_cast<void *>(&data.str_data[info_bind[i].data_pos][0]);

                my_bind[info_bind[i].bind_pos].buffer_length    = 40; /// QUE HAGO CON ESTO?
                my_bind[info_bind[i].bind_pos].is_null          = 0;

                my_bind[info_bind[i].bind_pos].length =
                    &data.str_long[info_bind[i].data_pos];

                break;
            case tipo_campo::IS_DEC :
                /// TO DO ///
                break;
        }
    }

    return true;
}
