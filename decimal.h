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

namespace fpt {

class decimal {
    public:
        // Constructores
        decimal(unsigned int _c, unsigned int _d);
        decimal(const decimal &d);

        // Destructor
        ~decimal();

        // Operadores - Asignación.
        decimal &operator=(const decimal &d);
        decimal &operator=(const std::string &val);

        // Operadores - Suma y Resta.
        decimal &operator+=(const decimal &d);
        friend decimal operator+(decimal a, const decimal &b)
        {
            a += b;
            return a;
        }
        friend decimal operator+(decimal a, const std::string &val)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            a += t;

            return a;
        }
        friend decimal operator+(const std::string &val, decimal a)
        {
            return a + val;
        }

        decimal &operator-=(const decimal &d);
        friend decimal operator-(decimal a, const decimal &b)
        {
            a -= b;
            return a;
        }
        friend decimal operator-(decimal a, const std::string &val)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            a -= t;

            return a;
        }
        friend decimal operator-(const std::string &val, decimal a)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            t -= a;

            return t;
        }

        // Operadores - Multiplicación y división.
        decimal &operator*=(const decimal &d);
        friend decimal operator*(decimal a, const decimal &b)
        {
            a *= b;
            return a;
        }
        friend decimal operator*(decimal a, const std::string &val)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            a *= t;

            return a;
        }
        friend decimal operator*(const std::string &val, decimal a)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            t *= a;

            return t;
        }

        decimal &operator/=(const decimal &d);
        friend decimal operator/(decimal a, const decimal &b)
        {
            a /= b;
            return a;
        }
        friend decimal operator/(decimal a, const std::string &val)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            a /= t;

            return a;
        }
        friend decimal operator/(const std::string &val, decimal a)
        {
            decimal t(a.ents + a.decs, a.decs);
            t = val;
            t /= a;

            return t;
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

        // Cambia el tamaño del decimal, redondeando si es necesario
        // o lanzando excepción si se produce desbordamiento.
        void resize(unsigned int _c, unsigned int _d);

        // Devuelve el decimal como cadena de caracteres.
        std::string to_str() const;

        // Devuelve un decimal con el valor absoluto de v.
        friend decimal abs(const decimal &v);

        // Devuelve un decimal con el valor maximo que puede tener v.
        friend decimal max(const decimal &v);

        // Devuelve un decimal con el valor minimo que puede tener v.
        friend decimal min(const decimal &v);

        // Devuelve un decimal de iguales caracteristicas que v, pero con valor 0.
        friend decimal zero(const decimal &v);

    private:
        uint8_t *buffer;

        unsigned int ents;          // Total enteros del número.
        unsigned int long_ents;     // Tamaño del array para los enteros: ((ents - 1)/2) + 1
        unsigned int decs;          // Total decimales del número.
        unsigned int long_decs;     // Tamaño del array para los decimales: ((decs - 1)/2) + 1

        unsigned int long_buffer;   // Tamaño total del buffer: long_ents + long_decs

        void convertir(const std::string &str);
        bool is_negative() const
        {
            // El bit más significativo del primer byte del array nos indicará
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

        // Obtiene el inverso multiplicativo, tal que i*d = 1.
        decimal inverse() const;
};

}; // Namespace fpt

#endif // DECIMAL_H_INCLUDED

