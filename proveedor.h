#ifndef __PROVEEDOR_H__
#define __PROVEEDOR_H__

#include <iostream>
#include <cstring>
#include <string>
#include <stdexcept>

#include <mysql.h>

#include "db_aux_def.h"
#include "utiles.h"

class Proveedor {
	public:
	    Proveedor();
	    Proveedor(const Proveedor &p);

		~Proveedor();

		// Operador de asignación.
        Proveedor &operator = (const Proveedor &p);

        /*
        std::string operator [](unsigned int i) const
        {
            switch(i) {
                case 0: return codigo;
                case 1: return razon_social;
                case 2: return domicilio;
                case 3: return codigo_postal;
                case 4: return poblacion;
                case 5: return cif;
                case 6: return tel1;
                case 7: return tel2;
                case 8: return fax;
                case 9: return forma_pago;
                case 10: return banco;
                default: throw std::out_of_range("Proveedor");
            }
        }

        std::string &operator [](unsigned int i)
        {
            switch(i) {
                case 0: return codigo;
                case 1: return razon_social;
                case 2: return domicilio;
                case 3: return codigo_postal;
                case 4: return poblacion;
                case 5: return cif;
                case 6: return tel1;
                case 7: return tel2;
                case 8: return fax;
                case 9: return forma_pago;
                case 10: return banco;
                default: throw std::out_of_range("Proveedor");
            }
        }
        */

        void limpiar(void)
        {
            codigo = 0;
            tipo = ' ';
			razon_social.clear();
			domicilio.clear();
			codigo_postal.clear();
			poblacion.clear();
			provincia.clear();
			cif.clear();
			tel1.clear();
			tel2.clear();
			fax.clear();
			forma_pago = 0;
			banco = 0;

			id = -1;
        }

        std::string to_csv() const;

        int     id;
        char    tipo;   // P = Proveedor, A = Acreedor

        int     codigo,
                forma_pago,
                banco;

		std::string razon_social,
                    domicilio,
                    codigo_postal,
                    poblacion,
                    provincia,
                    cif,
                    tel1,
                    tel2,
                    fax;
};

class BloqueInfoProveedor {
    public:
        BloqueInfoProveedor()
        {
            this->limpiar();
        }

        // Operador de asignación.
        BloqueInfoProveedor &operator = (const BloqueInfoProveedor &bic)
        {
            if(this != &bic) {
                linea_codigo    = bic.linea_codigo;
                linea_nombre    = bic.linea_nombre;
                linea_domicilio = bic.linea_domicilio;
                linea_poblacion = bic.linea_poblacion;
                linea_c_postal  = bic.linea_c_postal;
                linea_f_pago    = bic.linea_f_pago;
                linea_banco     = bic.linea_banco;
            }

            return *this;
        }

        void limpiar(void)
        {
            linea_codigo.clear();
            linea_nombre.clear();
            linea_domicilio.clear();
            linea_poblacion.clear();
            linea_c_postal.clear();
            linea_f_pago.clear();
            linea_banco.clear();
        }

        std::string linea_codigo,
                    linea_nombre,
                    linea_domicilio,
                    linea_poblacion,
                    linea_c_postal,
                    linea_f_pago,
                    linea_banco;
};

#endif // __PROVEEDOR_H__
