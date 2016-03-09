#ifndef DECIMAL_H_INCLUDED
#define DECIMAL_H_INCLUDED

#include <cstdint>
#include <string>

#include "utiles.h"

// El plan es tener un tipo de dato decimal, similar al que se
// utiliza en algunas bases de datos para almacenar numeros
// con decimales de forma precisa, sin los problemas de
// exactitud de los tipos de coma flotante que ofrece C/C++.
// Ni puta idea de por donde engancharlo, pero y iremos
// viendo.

// Vale, declaramos como decimal numero(12,4), que viene a
// significar que queremos un numero de 12 cifras en total,
// de las cuales 4 son la parte decimal: EEEEEEEE.DDDD

class decimal {
    public:
        // Constructores
        decimal(unsigned int _c, unsigned int _d);
        decimal(decimal &d);

        // Destructor
        ~decimal();

        // Operadores
        decimal &operator=(const decimal &d);
        decimal &operator=(const std::string &val);

        decimal &operator+=(const decimal &d);
        friend decimal operator+(decimal a, const decimal &b)
        {
            a += b;
            return a;
        }

        friend std::ostream &operator << (std::ostream &os, const decimal &d)
        {
            os << d.to_str();

            return os;
        }

        // Devuleve el decimal como cadena de caracteres.
        std::string to_str() const;
        // Cambia el tamaño del decimal, redondeando si es necesario
        // o lanzando excepción si se produce desbordamiento.
        void resize(unsigned int _c, unsigned int _d);

    private:
        uint8_t *buffer;

        unsigned int ents;      // Total enteros del número.
        unsigned int long_ents; // Tamaño del array para los enteros: ((ents - 1)/2) + 1
        unsigned int decs;      // Total decimales del número.
        unsigned int long_decs; // Tamaño del array para los decimales: ((decs - 1)/2) + 1

        void convertir(const std::string &str);


};

#endif // DECIMAL_H_INCLUDED

