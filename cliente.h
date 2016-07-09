#ifndef CLIENTE_H_INCLUDED
#define CLIENTE_H_INCLUDED

#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include <mysql.h>

#include "utiles.h"

class Cliente {
    public:
        // Constructores
        Cliente();
        Cliente(const Cliente &c);

        // Destructor
        ~Cliente();

        // Operador de asignación.
        Cliente &operator = (const Cliente &c);

        // Operador de acceso.
        //std::string operator [](unsigned int i) const
        //std::string &operator [](unsigned int i);

        // Operador de acceso.
        void *operator[](unsigned int i);

        // Limpia los datos.
        void limpiar(void);

        // Impresion en stream
        friend std::ostream &operator << (std::ostream &os, const Cliente &c);

        // Devuelve una cadena para lanzar un INSERT contra la base de datos.
        std::string get_mysql_insert() const;

        // Devuelve una cadena para lanzar un INSERT contra la base de datos.
        std::string get_mysql_update() const;

        // Rellena la estructura para un prepared stament de mysql.
        void get_mysql_bind(MYSQL_BIND *my_bind);
        MYSQL_BIND *get_mysql_bind();

        // Libera los recursos adquiridos por get_mysql_bin.
        void free_mysql_bind();
        int free_mysql_bind(MYSQL_BIND *my_bind);

        int     id;
        char    tipo;   // F = persona física, J = persona jurídica.

        int     codigo,
                forma_pago,
                categoria;

        std::string razon_social,
                    domicilio,
                    codigo_postal,
                    poblacion,
                    provincia,
                    nif,
                    tel1,
                    tel2,
                    fax,
                    swift_bic,
                    iban;

    private:
        char            **datos;
        unsigned int    num_campos = 14;
};

class BloqueInfoCliente {
    public:
        BloqueInfoCliente()
        {
            this->limpiar();
        }

        // Operador de asignación.
        BloqueInfoCliente &operator = (const BloqueInfoCliente &bic)
        {
            if(this != &bic) {
                linea_codigo.assign(bic.linea_codigo);
                linea_razon.assign(bic.linea_razon);
                linea_nombre.assign(bic.linea_nombre);
                linea_domicilio.assign(bic.linea_domicilio);
                linea_poblacion.assign(bic.linea_poblacion);
                linea_c_postal.assign(bic.linea_c_postal);
                linea_observaciones.assign(bic.linea_observaciones);
            }

            return *this;
        }

        void limpiar(void)
        {
            linea_codigo.clear();
            linea_razon.clear();
            linea_nombre.clear();
            linea_domicilio.clear();
            linea_poblacion.clear();
            linea_c_postal.clear();
            linea_observaciones.clear();
        }

        std::string linea_codigo,
                    linea_razon,
                    linea_nombre,
                    linea_domicilio,
                    linea_poblacion,
                    linea_c_postal,
                    linea_observaciones;
};

#endif // CLIENTE_H_INCLUDED
