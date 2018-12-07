#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <mysql.h>

#include "cliente.h"
#include "decimal.h"
#include "cosas.h"

#include "decoart.h"
#include "articulo.h"

bool artdata_to_csv();
bool artstock_to_csv();
bool unimed_to_csv();

bool artdata_to_list(std::list<articulo> &lista_articulos);
bool artstock_to_list(std::list<stock_art> &lista_stocks);

bool compare_art(const articulo &a, const articulo &b) {
    int procod_a = utiles::to_integer(a.get_data(0)),
        procod_b = utiles::to_integer(b.get_data(0)),
        artcod_a = utiles::to_integer(a.get_data(1)),
        artcod_b = utiles::to_integer(b.get_data(1));

    if(procod_a == procod_b)
        return artcod_a < artcod_b;
    else
        return procod_a < procod_b;
}

bool compare_stock(const stock_art &a, const stock_art &b) {
    int procod_a = utiles::to_integer(a.get_data(0)),
        procod_b = utiles::to_integer(b.get_data(0)),
        artcod_a = utiles::to_integer(a.get_data(1)),
        artcod_b = utiles::to_integer(b.get_data(1));

    if(procod_a == procod_b)
        return artcod_a < artcod_b;
    else
        return procod_a < procod_b;
}

int main()
{
    std::list<articulo>     lista_articulos;
    std::list<stock_art>    lista_stocks;

    std::fstream    fs_out;
    std::string     file_path { "c:/Develop/projects/test/" },
                    file_out { "art_data.csv" };

    /*
    MYSQL *my_con = mysql_init(NULL);

    // Incializamos la estructura de la conexion.
    if (my_con == NULL) {
        std::cout << mysql_error(my_con) << std::endl;
        return 1;
    }

    // Indicamos que nuestra base de datos se almacena en utf8.
    mysql_options(my_con, MYSQL_SET_CHARSET_NAME, "utf8");

    // Conectamos.
    if (mysql_real_connect(my_con, "backupserver", "sepyauser", "sepyapwd",
            "sepyadb", 0, NULL, 0) == NULL) {
        std::cout << mysql_error(my_con) << std::endl;
        mysql_close(my_con);
        return 1;
    }
    else {
        std::cout << "Conexion correcta." << std::endl;
    }

    //verificar_tablas(my_con);

    //revisar_clientes(my_con);

    mysql_close(my_con);
    */

    /*
    std::cout << "Procesando datos de articulos...\n\n";
    if(!artdata_to_csv())
        return -1;
    */

    /*
    std::cout << "\n\nProcesando stock de articulos...\n\n";
    if(!artstock_to_csv())
        return -1;

    std::cout << "\n\nProcesando unidades de medida...\n\n";
    if(!unimed_to_csv())
        return -1;
    */


    std::cout << "Procesando datos de articulos...\n\n";
    if(!artdata_to_list(lista_articulos))
        return -1;

    std::cout << "Procesando datos de stock...\n\n";
    if(!artstock_to_list(lista_stocks))
        return -1;

    fs_out.open(file_path + file_out, std::ios::out | std::ios::binary);

    if(!fs_out.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_out << "\n";
        return -1;
    }

    // Escribimos encabezado:
    fs_out << "cod_prov;cod_art;ref_prov;descripcion;cod_familia;EAN13;ud_medida;PVP;Margen1;Margen2;Margen3;Margen4;Margen5;Precio_coste;Dto1;Dto2;Dto3;Coste_real;Margen_benef;PVP_IVA;Tipo_IVA;stock_actual" << std::endl;

    // Plan:
    //  - Leemos lista articulos cod_art y cod_pro.
    //  - Comparamos con lista sotcks cod_art y cod_pro.
    //  - Si coinciden, grabamos y avanzamos ambos contadores.
    //  - Si no coinciden, aumentamos contador de stocks y repetimos.
    //  - Si llegamos al final de lista stocks sin haber llegado al final
    //      de lista articulos, error.
    //  - Si llegamos al final de lista articulos, fin.
    std::list<articulo>::iterator   i = lista_articulos.begin();
    std::list<stock_art>::iterator  j = lista_stocks.begin();

    int     num_art = lista_articulos.size(),   // Registros a procesar.
            art_procesados  = 0,                // Registros procesados.
            stock_procesado = 0;


    std::cout << "\n\n\n";

    while(i != lista_articulos.end()) {
        std::cout << "\rCombinando " << num_art << " articulos... ";
        std::cout << utiles::to_string(art_procesados, 6, ' ') << "/" << utiles::to_string(stock_procesado, 6, ' ') << "/" << utiles::to_string(num_art, 6, ' ');

        if(((*i).procod == (*j).procod) &&
           ((*i).artcod == (*j).artcod))
        {
            // Grabamos registro, aumentamos contadores.

            // Datos del articulo.
            for(int k = 0; k < 21; ++k)
                fs_out << (*i).get_data(k) << ";";

            // Stock.
            fs_out << (*j).get_data(3) << ";";

            fs_out << std::endl;

            ++i;
            ++j;
            ++art_procesados;
            ++stock_procesado;
        }
        else {
            // Aumentamos contador stock.
            ++j;
            ++stock_procesado;
            if(j == lista_stocks.end()) {
                // Error, hemos pasado el final de la lista de stocks.
                std::cerr << "\n\nERROR: Stock no econtrado.\n\n";

                // Salimos del bucle.
                i = lista_articulos.end();
                break;
            }
        }
    }

    return 0;
}

bool artdata_to_csv()
{
    std::vector<std::string> datos;

    int             reg_size = 1078;
    char            *buffer = new char[reg_size];
    std::fstream    fs_in, fs_out;
    std::string     file_path { "c:/Develop/projects/test/" },
                    file_in { "arti1893.dat" },
                    file_out { "arti1893.csv" };
    std::streampos  file_size;

    fs_in.open(file_path + file_in, std::ios::in | std::ios::binary | std::ios::ate);
    fs_out.open(file_path + file_out, std::ios::out | std::ios::binary);
    //fs_out.open("c:/Develop/projects/test/arti1893p.dat", std::ios::out | std::ios::binary);

    if(!fs_in.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_in << "\n";
        return -1;
    }

    if(!fs_out.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_out << "\n";
        return -1;
    }

    file_size = fs_in.tellg();
    fs_in.seekg(0, std::ios::beg);

    // Escribimos encabezado:
    fs_out << "cod_prov;cod_art;ref_prov;descripcion;cod_familia;EAN13;ud_medida;PVP;Margen1;Margen2;Margen3;Margen4;Margen5;Precio_coste;Dto1;Dto2;Dto3;Coste_real;Margen_benef;PVP_IVA;Tipo_IVA;" << std::endl;

    // Para la informacion por pantalla.
    //int num_registros = 100,
    int     num_registros = static_cast<int>(file_size/reg_size), // Registros en el archivo origen.
            j = 0,  // Registros procesados.
            k = 0;  // Registros guardados en archivo.

    // Empezamos a leer y procesar registros
    for(long int i = 0; i < num_registros; ++i) {
        std::cout << "\rExtrayendo " << num_registros << " registros... ";
        std::cout << utiles::to_string(++j, 6, ' ') << "/" << utiles::to_string(num_registros, 6, ' ');

        fs_in.read(buffer, reg_size);

        //fs_out.write(buffer, 1078);
        switch(buffer[reg_size - 1]) {
            case 0x0A:
                // Registro con datos. Procesamos.
                datos.clear();

                if(!extraer_datos_articulo(buffer, datos)) {
                    std::cerr << "GNIE!!!!\n" << std::flush;
                    return false;
                }

                for(size_t i = 0; i < datos.size(); ++i)
                        fs_out << datos[i] << ";";

                fs_out << std::endl;

                ++k;
                break;
            case 0x00:
                // Registro nulo. Ignoramos.
                break;
            default:
                // Algo no ha ido como se esperaba.
                return false;
        }
    }

    std::cout << "\n\nRegistros guardados en archivo/registros procesados: "
        << utiles::to_string(k) << "/" << utiles::to_string(num_registros) << "\n\n";

    // Cerramos streams.
    fs_in.close();
    fs_out.close();

    delete [] buffer;

    return true;
}

bool artstock_to_csv()
{
    std::vector<std::string> datos;

    int             reg_size = 280;
    char            *buffer = new char[reg_size];
    std::fstream    fs_in, fs_out;
    std::string     file_path { "c:/Develop/projects/test/" },
                    file_in { "alma1241.dat" },
                    file_out { "alma1241.csv" };
    std::streampos  file_size;

    fs_in.open(file_path + file_in, std::ios::in | std::ios::binary | std::ios::ate);
    fs_out.open(file_path + file_out, std::ios::out | std::ios::binary);
    //fs_out.open("c:/Develop/projects/test/arti1893p.dat", std::ios::out | std::ios::binary);

    if(!fs_in.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_in << "\n";
        return -1;
    }

    if(!fs_out.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_out << "\n";
        return -1;
    }

    file_size = fs_in.tellg();
    fs_in.seekg(0, std::ios::beg);

    // Escribimos encabezado:
    fs_out << "cod_prov;cod_art;almacen;stock_actual;" << std::endl;

    // Para la informacion por pantalla.
    // int num_registros = 286895,     // Registros totales
    int     num_registros = static_cast<int>(file_size/reg_size), // Registros en el archivo origen.
            j = 0,  // Registros procesados.
            k = 0;  // Registros guardados en archivo.

    // Empezamos a leer y procesar registros
    for(long int i = 0; i < num_registros; ++i) {
        std::cout << "\rExtrayendo " << num_registros << " registros... ";
        std::cout << utiles::to_string(++j, 6, ' ') << "/" << utiles::to_string(num_registros, 6, ' ');

        fs_in.read(buffer, reg_size);

        //fs_out.write(buffer, 1078);
        switch(buffer[reg_size - 1]) {
            case 0x0A:
                // Registro con datos. Procesamos.
                datos.clear();

                if(!extraer_stock_articulo(buffer, datos)) {
                    std::cerr << "GNIE!!!!\n" << std::flush;
                    return false;
                }

                for(size_t i = 0; i < datos.size(); ++i)
                        fs_out << datos[i] << ";";

                fs_out << std::endl;

                ++k;
                break;
            case 0x00:
                // Registro nulo. Ignoramos.
                break;
            default:
                // Algo no ha ido como se esperaba.
                return false;
        }
    }

    std::cout << "\n\nRegistros guardados en archivo/registros procesados: "
        << utiles::to_string(k) << "/" << utiles::to_string(num_registros) << "\n\n";

    // Cerramos streams.
    fs_in.close();
    fs_out.close();

    delete [] buffer;

    return true;
}

bool unimed_to_csv()
{
    std::vector<std::string> datos;

    int             reg_size = 38;
    char            *buffer = new char[reg_size];
    std::fstream    fs_in, fs_out;
    std::string     file_path { "c:/Develop/projects/test/" },
                    file_in { "unim1851.dat" },
                    file_out { "unim1851.csv" };
    std::streampos  file_size;

    fs_in.open(file_path + file_in, std::ios::in | std::ios::binary | std::ios::ate);
    fs_out.open(file_path + file_out, std::ios::out | std::ios::binary);
    //fs_out.open("c:/Develop/projects/test/arti1893p.dat", std::ios::out | std::ios::binary);

    if(!fs_in.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_in << "\n";
        return -1;
    }

    if(!fs_out.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_out << "\n";
        return -1;
    }

    file_size = fs_in.tellg();
    fs_in.seekg(0, std::ios::beg);


    // Escribimos encabezado:
    fs_out << "cod_unid;desc_unid;" << std::endl;

    // Para la informacion por pantalla.
    int     num_registros = static_cast<int>(file_size/reg_size), // Registros en el archivo origen.
            j = 0,  // Registros procesados.
            k = 0;  // Registros guardados en archivo.

    // Empezamos a leer y procesar registros
    for(long int i = 0; i < num_registros; ++i) {
        std::cout << "\rExtrayendo " << num_registros << " registros... ";
        std::cout << utiles::to_string(++j, 6, ' ') << "/" << utiles::to_string(num_registros, 6, ' ');

        fs_in.read(buffer, reg_size);

        //fs_out.write(buffer, 1078);
        switch(buffer[reg_size - 1]) {
            case 0x0A:
                // Registro con datos. Procesamos.
                datos.clear();

                if(!extraer_unid_medida(buffer, datos)) {
                    std::cerr << "GNIE!!!!\n" << std::flush;
                    return false;
                }

                for(size_t i = 0; i < datos.size(); ++i)
                        fs_out << datos[i] << ";";

                fs_out << std::endl;

                ++k;
                break;
            case 0x00:
                // Registro nulo. Ignoramos.
                break;
            default:
                // Algo no ha ido como se esperaba.
                return false;
        }
    }

    std::cout << "\n\nRegistros guardados en archivo/registros procesados: "
        << utiles::to_string(k) << "/" << utiles::to_string(num_registros) << "\n\n";

    // Cerramos streams.
    fs_in.close();
    fs_out.close();

    delete [] buffer;

    return true;
}

bool artdata_to_list(std::list<articulo> &lista_articulos)
{
    std::vector<std::string>    datos;
    //std::list<articulo>         lista_articulos;

    int             reg_size = 1078;
    char            *buffer = new char[reg_size];
    std::fstream    fs_in, fs_out;
    std::string     file_path { "c:/Develop/projects/test/" },
                    file_in { "arti1893.dat" };

    std::streampos  file_size;

    articulo        a;

    fs_in.open(file_path + file_in, std::ios::in | std::ios::binary | std::ios::ate);

    if(!fs_in.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_in << "\n";
        return -1;
    }

    file_size = fs_in.tellg();
    fs_in.seekg(0, std::ios::beg);

    // Para la informacion por pantalla.
    int     num_registros = static_cast<int>(file_size/reg_size), // Registros en el archivo origen.
            j = 0,  // Registros procesados.
            k = 0;  // Registros guardados en el archivo.

    // Empezamos a leer y procesar registros
    for(long int i = 0; i < num_registros; ++i) {
        std::cout << "\rExtrayendo " << num_registros << " registros... ";
        std::cout << utiles::to_string(++j, 6, ' ') << "/" << utiles::to_string(num_registros, 6, ' ');

        fs_in.read(buffer, reg_size);

        switch(buffer[reg_size - 1]) {
            case 0x0A:
                // Registro con datos. Procesamos.
                datos.clear();

                if(!extraer_datos_articulo(buffer, datos)) {
                    std::cerr << "GNIE!!!!\n" << std::flush;
                    return false;
                }

                for(size_t l = 0; l < datos.size(); ++l)
                    a.put_data(datos[l], l);

                lista_articulos.push_back(a);

                ++k;

                break;
            case 0x00:
                // Registro nulo. Ignoramos.
                break;
            default:
                // Algo no ha ido como se esperaba.
                return false;
        }
    }

    std::cout << "\n\nOrdenando datos ... ";
    lista_articulos.sort(compare_art);
    std::cout << "Hecho.\n\n";

    // Cerramos streams.
    fs_in.close();

    delete [] buffer;

    return true;
}

bool artstock_to_list(std::list<stock_art> &lista_stocks)
{
    std::vector<std::string> datos;

    int             reg_size = 280;
    char            *buffer = new char[reg_size];
    std::fstream    fs_in, fs_out;
    std::string     file_path { "c:/Develop/projects/test/" },
                    file_in { "alma1241.dat" };

    std::streampos  file_size;

    stock_art       a;

    fs_in.open(file_path + file_in, std::ios::in | std::ios::binary | std::ios::ate);

    if(!fs_in.good()) {
        std::cout << "Fallo al abrir el fichero " << file_path << file_in << "\n";
        return -1;
    }

    file_size = fs_in.tellg();
    fs_in.seekg(0, std::ios::beg);

    // Para la informacion por pantalla.
    int     num_registros = static_cast<int>(file_size/reg_size), // Registros en el archivo origen.
            j = 0,  // Registros procesados.
            k = 0;  // Registros guardados en archivo.

    // Empezamos a leer y procesar registros
    for(long int i = 0; i < num_registros; ++i) {
        std::cout << "\rExtrayendo " << num_registros << " registros... ";
        std::cout << utiles::to_string(++j, 6, ' ') << "/" << utiles::to_string(num_registros, 6, ' ');

        fs_in.read(buffer, reg_size);

        //fs_out.write(buffer, 1078);
        switch(buffer[reg_size - 1]) {
            case 0x0A:
                // Registro con datos. Procesamos.
                datos.clear();

                if(!extraer_stock_articulo(buffer, datos)) {
                    std::cerr << "GNIE!!!!\n" << std::flush;
                    return false;
                }

                // Ignoramos los datos que no sean del almacen 1.
                if(utiles::to_integer(datos[2]) == 1) {
                    for(size_t l = 0; l < datos.size(); ++l)
                        a.put_data(datos[l], l);

                    lista_stocks.push_back(a);

                    ++k;
                }
                break;
            case 0x00:
                // Registro nulo. Ignoramos.
                break;
            default:
                // Algo no ha ido como se esperaba.
                return false;
        }
    }

    std::cout << "\n\nOrdenando datos ... ";
    lista_stocks.sort(compare_stock);
    std::cout << "Hecho.\n\n";

    // Cerramos streams.
    fs_in.close();

    delete [] buffer;

    return true;
}


