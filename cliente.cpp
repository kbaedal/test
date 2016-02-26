#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include <mysql.h>

#include "cliente.h"
#include "utiles.h"



Cliente::Cliente()
{
    this->limpiar();
    datos = new char*[num_campos];

    for(size_t i = 0; i < num_campos; ++i)
        datos[i] = nullptr;
}

Cliente::Cliente(const Cliente &c)
{
    datos = new char*[num_campos];

    for(size_t i = 0; i < num_campos; ++i)
        datos[i] = nullptr;

    *this = c;
}

Cliente::~Cliente()
{
    for(size_t i = 0; i < num_campos; ++i)
        if(datos[i] != nullptr)
            delete [] datos[i];

    delete [] datos;
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

//std::string operator [](unsigned int i) const
//{
//    switch(i) {
//        case 1: return razon_social;
//        case 2: return domicilio;
//        case 3: return codigo_postal;
//        case 4: return poblacion;
//        case 5: return provincia;
//        case 6: return nif;
//        case 7: return tel1;
//        case 8: return tel2;
//        case 9: return fax;
//        case 12: return swift_bic;
//        case 13: return iban;
//        default: throw std::out_of_range("Cliente:string");
//    }
//}
//
//std::string &operator [](unsigned int i)
//{
//    switch(i) {
//        case 1: return razon_social;
//        case 2: return domicilio;
//        case 3: return codigo_postal;
//        case 4: return poblacion;
//        case 5: return provincia;
//        case 6: return nif;
//        case 7: return tel1;
//        case 8: return tel2;
//        case 9: return fax;
//        case 12: return swift_bic;
//        case 13: return iban;
//        default: throw std::out_of_range("Cliente::string");
//    }
//}

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
        default: throw std::out_of_range("Cliente::string");
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
            "\tCod. Cliente: "  << utiles::IntToStr(c.codigo, 6)        << std::endl <<
            "\tRazon Social: "  << c.razon_social                       << std::endl <<
            "\tDomicilio:    "  << c.domicilio                          << std::endl <<
            "\tCod. Postal:  "  << c.codigo_postal                      << std::endl <<
            "\tPoblacion:    "  << c.poblacion                          << std::endl <<
            "\tProvincia:    "  << c.provincia                          << std::endl <<
            "\tNIF:          "  << c.nif                                << std::endl <<
            "\tTel1:         "  << c.tel1                               << std::endl <<
            "\tTel2:         "  << c.tel2                               << std::endl <<
            "\tFax:          "  << c.fax                                << std::endl <<
            "\tForma Pago:   "  << utiles::IntToStr(c.forma_pago, 2)    << std::endl <<
            "\tCategoria:    "  << utiles::IntToStr(c.categoria, 2)     << std::endl <<
            "\tSWIFT-BIC:    "  << c.swift_bic                          << std::endl <<
            "\tIBAN:         "  << c.iban                               << std::endl << std::endl;

    return os;
}

std::string Cliente::get_mysql_insert()
{
    std::string temp;

    temp = "INSERT INTO cliente (cod_cliente, razon_social, domicilio, "
        " cod_postal, poblacion, provincia, nif, tel1, tel2, fax, "
        " forma_pago, categoria, swift_bic, iban) VALUES (";

    temp += utiles::IntToStr(codigo) + ",";
    temp += "\"" + razon_social + "\",";
    temp += "\"" + domicilio + "\",";
    temp += "\"" + codigo_postal + "\",";
    temp += "\"" + poblacion + "\",";
    temp += "\"" + provincia + "\",";
    temp += "\"" + nif + "\",";
    temp += "\"" + tel1 + "\",";
    temp += "\"" + tel2 + "\",";
    temp += "\"" + fax + "\",";
    temp += utiles::IntToStr(forma_pago) + ",";
    temp += utiles::IntToStr(categoria) + ",";
    temp += "\"" + swift_bic + "\",";
    temp += "\"" + iban + "\"";
    temp += ")";

    return temp;
}

std::string Cliente::get_mysql_update()
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
    temp += " forma_pago=" + utiles::IntToStr(forma_pago) + ",";
    temp += " categoria=" + utiles::IntToStr(categoria) + ",";
    temp += " swift_bic=\"" + swift_bic + "\",";
    temp += " iban=\"" + iban + "\"";
    temp += " WHERE cliente.cod_cliente=" + utiles::IntToStr(codigo);

    return temp;
}

// Rellena la estructura para un prepared stament de mysql.
// Ojo, cualquier modificación de los datos posterior a esta
// llamada invalida estos datos.
// Asegurarse de llamar a free_sql_bind() tras el uso de
// los datos.
void Cliente::get_mysql_bind(MYSQL_BIND *my_bind)
{
    // Para acceder a los datos utilizamos un medio un poco ofuscado.
    // Podemos acceder con el operador [] a todos, solo tendremos
    // que separar los que son enteros de los que son cadenas.
    // Utilizamos la versión funcion del operador, ya que así
    // accedemos desde el puntero this, asegurandonos que tenemos
    // lo que queremos.
    // El operador [] devuelve void*, justo lo que necesitamos.
    void                *buffer;
    enum_field_types    buffer_type;

    // Limpiamos las cadenas, por si ya se están usando.
    this->free_mysql_bind();

    for(size_t i = 0; i < num_campos; ++i) {
        if((i == 0) || (i ==  10) || (i == 11))  {
            // Enteros. Asignamos y seguimos.
            buffer_type = MYSQL_TYPE_LONG;
            buffer      = this->operator[](i);
        }
        else {
            // Cadenas de caracteres.
            // Reservamos memoria para copiar las cadenas desde
            // las std::strings que tenemos a cadenas c-style.
            // Como hemos reservado memoria para incluir hasta
            // num_campos cadenas en la variable **datos, utilizamos
            // el mismo contador del bucle para no tener que
            // preocuparnos de controlar otra variable más.
            // El consumo de memoria es mínimo (dejamos unos pocos
            // punteros a nullptr) y la legibilidad del código es mayor.

            std::string *cadena = static_cast<std::string *>(this->operator[](i));
            datos[i] = new char[cadena->size() + 1];
            std::strcpy(datos[i], cadena->c_str());

            buffer_type = MYSQL_TYPE_STRING;
            buffer      = static_cast<void *>(datos[i]);
        }

        // Rellenamos la estructura y salimos.
        my_bind[i].buffer_type   = buffer_type;
        my_bind[i].buffer        = buffer;
        my_bind[i].is_unsigned   = 0;
        my_bind[i].is_null       = 0;
    }
}

// Rellena la estructura para un prepared stament de mysql.
// Ojo, cualquier modificación de los datos posterior a esta
// llamada invalida estos datos.
// Asegurarse de llamar a free_sql_bind() tras el uso de
// los datos.
MYSQL_BIND *Cliente::get_mysql_bind(void)
{
    // Para acceder a los datos utilizamos un medio un poco ofuscado.
    // Podemos acceder con el operador [] a todos, solo tendremos
    // que separar los que son enteros de los que son cadenas.
    // Utilizamos la versión funcion del operador, ya que así
    // accedemos desde el puntero this, asegurandonos que tenemos
    // lo que queremos.
    // El operador [] devuelve void*, justo lo que necesitamos.
    void                *buffer;
    enum_field_types    buffer_type;
    MYSQL_BIND          *my_bind = new MYSQL_BIND[num_campos];

    for(size_t i = 0; i < num_campos; ++i) {
        if((i == 0) || (i ==  10) || (i == 11))  {
            // Enteros. Asignamos y seguimos.

            int *ptrdato = new int;
            *ptrdato = *static_cast<int *>(this->operator[](i));

            buffer_type = MYSQL_TYPE_LONG;
            buffer      = static_cast<void *>(ptrdato);
        }
        else {
            // Cadenas de caracteres.
            // Reservamos memoria para copiar las cadenas desde
            // las std::strings que tenemos a cadenas c-style.

            std::string *cadena = static_cast<std::string *>(this->operator[](i));
            char        *ptrdato = new char[cadena->size() + 1];
            std::strcpy(ptrdato, cadena->c_str());

            buffer_type = MYSQL_TYPE_STRING;
            buffer      = static_cast<void *>(ptrdato);
        }

        // Rellenamos la estructura y salimos.
        my_bind[i].buffer_type   = buffer_type;
        my_bind[i].buffer        = buffer;
        my_bind[i].is_unsigned   = 0;
        my_bind[i].is_null       = 0;
    }

    return my_bind;
}

void Cliente::free_mysql_bind()
{
    for(size_t i = 0; i < num_campos; ++i) {
        if(datos[i] != nullptr) {
            delete [] datos[i];
            datos[i] = nullptr;
        }
    }
}

int Cliente::free_mysql_bind(MYSQL_BIND *my_bind)
{
    for(size_t i; i < num_campos; ++i) {
        if((i == 0) || (i ==  10) || (i == 11)) {
            int *ptrdato = static_cast<int *>(my_bind[i].buffer);
            delete ptrdato;
        }
        else {
            char *ptrdato = static_cast<char *>(my_bind[i].buffer);
            delete [] ptrdato;
        }
    }

    delete [] my_bind;

    return 0;
}
