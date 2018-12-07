#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include "cliente.h"
#include "db_aux_def.h"
#include "utiles.h"

tipo_campo Cliente::info_campos[] = {
    tipo_campo::IS_INT,     // Codigo
    tipo_campo::IS_TXT,     // Razon social
    tipo_campo::IS_TXT,     // Domicilio
    tipo_campo::IS_TXT,     // Codigo Postal
    tipo_campo::IS_TXT,     // Poblacion
    tipo_campo::IS_TXT,     // Provincia
    tipo_campo::IS_TXT,     // Nif
    tipo_campo::IS_TXT,     // Tel1
    tipo_campo::IS_TXT,     // Tel2
    tipo_campo::IS_TXT,     // Fax
    tipo_campo::IS_INT,     // Forma de pago
    tipo_campo::IS_INT,     // Categoria
    tipo_campo::IS_TXT,     // Swift-BIC
    tipo_campo::IS_TXT      // IBAN
};

Cliente::Cliente()
{
    this->limpiar();
}

Cliente::Cliente(const Cliente &c)
{
    *this = c;
}

Cliente::~Cliente()
{

}

Cliente &Cliente::operator = (const Cliente &c)
{
    if(this != &c) {
        codigo          = c.codigo;
        razon_social    = c.razon_social;
        domicilio       = c.domicilio;
        codigo_postal   = c.codigo_postal;
        poblacion       = c.poblacion;
        provincia       = c.provincia;
        nif             = c.nif;
        tel1            = c.tel1;
        tel2            = c.tel2;
        fax             = c.fax;
        forma_pago      = c.forma_pago;
        categoria       = c.categoria;
        swift_bic       = c.swift_bic;
        iban            = c.iban;

        id              = c.id;
    }

    return *this;
}

void *Cliente::operator[](unsigned int i)
{
    switch(i) {
        case 0: return static_cast<void *>(&codigo);
        case 1: return static_cast<void *>(&razon_social);
        case 2: return static_cast<void *>(&domicilio);
        case 3: return static_cast<void *>(&codigo_postal);
        case 4: return static_cast<void *>(&poblacion);
        case 5: return static_cast<void *>(&provincia);
        case 6: return static_cast<void *>(&nif);
        case 7: return static_cast<void *>(&tel1);
        case 8: return static_cast<void *>(&tel2);
        case 9: return static_cast<void *>(&fax);
        case 10: return static_cast<void *>(&forma_pago);
        case 11: return static_cast<void *>(&categoria);
        case 12: return static_cast<void *>(&swift_bic);
        case 13: return static_cast<void *>(&iban);
        default: throw std::out_of_range("Cliente::operator[]");
    }
}

void Cliente::limpiar(void)
{
    codigo = 0;
    razon_social.clear();
    domicilio.clear();
    codigo_postal.clear();
    poblacion.clear();
    provincia.clear();
    nif.clear();
    tel1.clear();
    tel2.clear();
    fax.clear();
    forma_pago = 0;
    categoria = 0;
    swift_bic.clear();
    iban.clear();

    id = -1;
}

std::ostream &operator << (std::ostream &os, const Cliente &c)
{
    os << "Cliente:\n" <<
            "\tCod. Cliente: "  << utiles::to_string(c.codigo, 6)        << std::endl <<
            "\tRazon Social: "  << c.razon_social                       << std::endl <<
            "\tDomicilio:    "  << c.domicilio                          << std::endl <<
            "\tCod. Postal:  "  << c.codigo_postal                      << std::endl <<
            "\tPoblacion:    "  << c.poblacion                          << std::endl <<
            "\tProvincia:    "  << c.provincia                          << std::endl <<
            "\tNIF:          "  << c.nif                                << std::endl <<
            "\tTel1:         "  << c.tel1                               << std::endl <<
            "\tTel2:         "  << c.tel2                               << std::endl <<
            "\tFax:          "  << c.fax                                << std::endl <<
            "\tForma Pago:   "  << utiles::to_string(c.forma_pago, 2)    << std::endl <<
            "\tCategoria:    "  << utiles::to_string(c.categoria, 2)     << std::endl <<
            "\tSWIFT-BIC:    "  << c.swift_bic                          << std::endl <<
            "\tIBAN:         "  << c.iban                               << std::endl << std::endl;

    return os;
}

std::string Cliente::get_mysql_insert_str() const
{
    std::string temp;

    temp = "INSERT INTO cliente (cod_cliente, razon_social, domicilio, "
        " cod_postal, poblacion, provincia, nif, tel1, tel2, fax, "
        " forma_pago, categoria, swift_bic, iban) VALUES (";

    temp += utiles::to_string(codigo) + ",";
    temp += "\"" + razon_social + "\",";
    temp += "\"" + domicilio + "\",";
    temp += "\"" + codigo_postal + "\",";
    temp += "\"" + poblacion + "\",";
    temp += "\"" + provincia + "\",";
    temp += "\"" + nif + "\",";
    temp += "\"" + tel1 + "\",";
    temp += "\"" + tel2 + "\",";
    temp += "\"" + fax + "\",";
    temp += utiles::to_string(forma_pago) + ",";
    temp += utiles::to_string(categoria) + ",";
    temp += "\"" + swift_bic + "\",";
    temp += "\"" + iban + "\"";
    temp += ")";

    return temp;
}

std::string Cliente::get_mysql_update_str() const
{
    std::string temp = "UPDATE cliente SET";

    temp += " razon_social=\"" + razon_social + "\",";
    temp += " domicilio=\"" + domicilio + "\",";
    temp += " cod_postal=\"" + codigo_postal + "\",";
    temp += " poblacion=\"" + poblacion + "\",";
    temp += " provincia=\"" + provincia + "\",";
    temp += " nif=\"" + nif + "\",";
    temp += " tel1=\"" + tel1 + "\",";
    temp += " tel2=\"" + tel2 + "\",";
    temp += " fax=\"" + fax + "\",";
    temp += " forma_pago=" + utiles::to_string(forma_pago) + ",";
    temp += " categoria=" + utiles::to_string(categoria) + ",";
    temp += " swift_bic=\"" + swift_bic + "\",";
    temp += " iban=\"" + iban + "\"";
    temp += " WHERE cliente.cod_cliente=" + utiles::to_string(codigo);

    return temp;
}

void Cliente::fill_mysql_bind(data_bind_storage &data)
{
    int     str_count = 0,  // Controlamos cuantos datos llevamos incorporados
            int_count = 0;  // a la estructura.

    std::string *cadena;    // Para manejo de datos.

    // Utilizamos el operador [] que hemos definido, el cual nos devuelve
    // un puntero a void a cada uno de los campos del cliente.
    // Simplemente, siguiendo el orden, colocamos los datos en el lugar
    // adecuado, dependiendo del tipo de dato.
    // Para los enteros, copiamos el contenido (usando *puntero).
    // Para las cadenas, habremos de hacer una copia en la estructura.
    // Se da por supuesto que la estructura está inicializada y tenemos
    // espacio suficiente.
    for(int i = 0; i < db_consts::cliente_num_campos; ++i) {
        switch(info_campos[i]) {
            case tipo_campo::IS_INT :
                // Colocamos un entero. Copiamos en su lugar.
                data.int_data[int_count] = *static_cast<int *>(this->operator[](i));

                ++int_count;
                break;
            case tipo_campo::IS_TXT :
                // Colocamos una cadena. Copiamos la cadena e indicamos longitud.
                cadena = static_cast<std::string *>(this->operator[](i));
                std::strcpy(data.str_data[str_count], cadena->c_str());
                data.str_long[str_count] = cadena->size();

                ++str_count;
                break;
            case tipo_campo::IS_DEC :
                // En este caso, convertimos el dato a una cadena.
                /// TO DO ///
                break;
            default:
                break;
        }
    }
}

std::string Cliente::to_csv() const
{
    std::string temp {};

    temp += utiles::to_string(codigo, 6) + ";";
    temp += razon_social + ";";
    temp += domicilio + ";";
    temp += codigo_postal + ";";
    temp += poblacion + ";";
    temp += provincia + ";";
    temp += nif + ";";
    temp += tel1 + ";";
    temp += tel2 + ";";
    temp += fax + ";";
    temp += utiles::to_string(forma_pago, 2) + ";";
    temp += utiles::to_string(categoria) + ";";
    temp += swift_bic + ";";
    temp += iban + ";";

    return temp;
}
