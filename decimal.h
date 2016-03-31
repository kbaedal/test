#ifndef DECIMAL_H_INCLUDED
#define DECIMAL_H_INCLUDED

#include <cstdint>
#include <string>
#include <iostream>

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
        decimal(const decimal &d);

        // Destructor
        ~decimal();

        // Operadores - Asignaci�n.
        decimal &operator=(const decimal &d);
        decimal &operator=(const std::string &val);

        // Operadores - Suma y Resta.
        decimal &operator+=(const decimal &d);
        friend decimal operator+(decimal a, const decimal &b)
        {
            a += b;
            return a;
        }
        decimal &operator-=(const decimal &d);
        friend decimal operator-(decimal a, const decimal &b)
        {
            a -= b;
            return a;
        }

        // Operadores - Multiplicaci�n y divisi�n.
        decimal &operator*=(const decimal &d);
        friend decimal operator*(decimal a, const decimal &b)
        {
            a *= b;
            return a;
        }
        decimal &operator/=(const decimal &d);
        friend decimal operator/(decimal a, const decimal &b)
        {
            a /= b;
            return a;
        }

        // Operadores - Comparacion.
        friend bool operator==(const decimal &a, const decimal &b);
        friend bool operator!=(const decimal &a, const decimal &b)
        {
            return !(a == b);
        }

        friend bool operator>=(const decimal &a, const decimal &b);
        friend bool operator<=(const decimal &a, const decimal &b);

        friend bool operator>(const decimal &a, const decimal &b)
        {
            return !(a <= b);
        }

        friend bool operator<(const decimal &a, const decimal &b)
        {
            return !(a >= b);
        }

        friend std::ostream &operator << (std::ostream &os, const decimal &d)
        {
            os << d.to_str();

            return os;
        }

        // Devuelve el decimal como cadena de caracteres.
        std::string to_str() const;

        // Cambia el tama�o del decimal, redondeando si es necesario
        // o lanzando excepci�n si se produce desbordamiento.
        void resize(unsigned int _c, unsigned int _d);

        // Devuelve el valor absoluto.
        decimal abs() const;

    private:
        uint8_t *buffer;

        unsigned int ents;          // Total enteros del n�mero.
        unsigned int long_ents;     // Tama�o del array para los enteros: ((ents - 1)/2) + 1
        unsigned int decs;          // Total decimales del n�mero.
        unsigned int long_decs;     // Tama�o del array para los decimales: ((decs - 1)/2) + 1

        unsigned int long_buffer;   // Tama�o total del buffer: long_ents + long_decs

        void convertir(const std::string &str);
        bool is_negative() const
        {
            // El bit m�s significativo del primer byte del array nos indicar�
            // si el decimal es negatvo o positivo.
            return ((buffer[0] & 0x80) == 0x80);
        }

        void set_negative()
        {
            buffer[0] |= 0x80;
        }

        void set_positive()
        {
            buffer[0] &= 0x7F;
        }

        // Suma al decimal los datos de sum. Condiciones:
        //  1. sizeof(sum) == sizeof(buffer)
        //  2. Ambos datos deben ser positivos.
        void suma(const uint8_t *sum);

        // Resta del decimal los datos de res. Condiciones:
        //  1. sizeof(res) == sizeof(buffer)
        //  2. Ambos datos deben ser positivos.
        //  3. La resta debe dar un resultado positivo.
        void resta(const uint8_t *res);

        // Multiplica por 10 num veces.
        void mul10(unsigned int num);

        // Divide entre 10 num veces.
        void div10(unsigned int num);

};

#endif // DECIMAL_H_INCLUDED

