////////////////////////////////////////////////////////////////////////////////
//
//  Project:    Fixed point type and arithmetics.
//  File:       decimal.h
//  Author:     Alfonso Vicente Zurdo (kbaedal at gmail dot com)
//  Date:       2016-04-28
//  Version:    1.0
//  Brief:      Simple fixed point type and arithmetics, to cope with
//              light problems, like basic economic operations, and other
//              type of situations where a floating point arithmetics
//              can lead to an unacepptable loss of precission.
//              This is not intended to be included in a complex system where
//              calculations needs to be quick and extremely precisse. There
//              are better solutions for that kind of scenario (see GMP library).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DECIMAL_H_INCLUDED
#define DECIMAL_H_INCLUDED

#include <cstdint>
#include <string>
#include <iostream>

namespace fpt {

class decimal {
    public:
        // Constructors
        decimal(unsigned int nc, unsigned int nd);
        decimal(const decimal &d);

        // Destructor
        ~decimal();

        // Operators - Asignation.
        decimal &operator=(const decimal &d);
        decimal &operator=(const std::string &val);

        // Operators: Negation (unary minus).
        decimal operator-()
        {
            decimal t(*this);

            if(t.is_negative())
                t.set_positive();
            else
                t.set_negative();

            return t;
        }

        // Operador: Unary plus.
        decimal operator+()
        {
            decimal t(*this);

            return t;
        }

        // Operators - Add and sustract.
        decimal &operator+=(const decimal &d);
        friend decimal operator+(decimal a, const decimal &b)
        {
            a += b;

            return a;
        }
        friend decimal operator+(decimal a, const std::string &val)
        {
            decimal t(a.cifs, a.decs);
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
            decimal t(a.cifs, a.decs);
            t = val;
            a -= t;

            return a;
        }
        friend decimal operator-(const std::string &val, decimal a)
        {
            decimal t(a.cifs, a.decs);
            t = val;
            t -= a;

            return t;
        }

        // Operators - Multiplication and division.
        decimal &operator*=(const decimal &d);
        friend decimal operator*(decimal a, const decimal &b)
        {
            a *= b;

            return a;
        }
        friend decimal operator*(decimal a, const std::string &val)
        {
            decimal t(a.cifs, a.decs);
            t = val;
            a *= t;

            return a;
        }
        friend decimal operator*(const std::string &val, decimal a)
        {
            decimal t(a.cifs, a.decs);
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

        // Operators - Comparision.
        friend bool operator==(const decimal &a, const decimal &b);
        friend bool operator==(const decimal &a, const std::string &b);
        friend bool operator!=(const decimal &a, const decimal &b)
        {
            return !(a == b);
        }
        friend bool operator!=(const decimal &a, const std::string &b)
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

        // Changes decimal size, applying rounding if needed.
        // Throws an exception on overflow.
        void resize(unsigned int nc, unsigned int nd);
        void resize(const decimal &d)
        {
            this->resize(d.cifs, d.decs);
        }

        // Rebuilds decimal structure and data from d.
        void rebuild(const decimal &d)
        {
            this->resize(d);
            *this = d;
        }

        // Formats the decimal as a string.
        // With format = false, it shows all cyphers, even non-significant zeros.
        std::string to_str(bool format = true) const;

        // Formats the decimal as a string with economics look, adding decimal point
        // and separator comas in every place needed.
        std::string to_smoney(std::string suffix = " EUR" ) const;

        // Formats the decimal as a string without decimal point or negative sign.
        std::string to_splain() const;

        // Returns absoulte value.
        decimal abs() const;

        // Returns maximum positive value.
        decimal max() const;

        // Returns minimum positive value.
        decimal min() const;

        // Returns a 0 filled decimal.
        decimal zero() const;

    private:
        uint8_t *buffer;            // PBCD data storage.

        unsigned int ents;          // Total enteros del número.
        unsigned int decs;          // Total racionales del número.
        unsigned int cifs;          // Total cifras del número.

        unsigned int long_buffer;   // Tamaño total del buffer.

        uint8_t status;             // Flags de status del decimal.
                                    // Bit 0: Signo (0 positivo, 1 negativo).
                                    // Bit 1: Overflow (0 no hay, 1 sí hay).

        // Comprueba que str contenga un numero válido, ignorando espacios
        // y otros caracteres especiales tanto al principio como al final.
        // Si es un número válido, devuelve true y la cadena limpia, sin los
        // caracteres especiales, en t.
        bool validar_cadena(const std::string &str, std::string &t);

        // Convierte el número a formato interno.
        void convertir(const std::string &str);

        std::string int_to_str(const int &n, int width = 0) const;

        int str_to_int(const std::string &s) const;

        bool is_negative() const
        {
            return (status & 0x01);
        }

        void set_negative()
        {
            status |= 0x01;
        }

        void set_positive()
        {
            status &= 0xFE; // 1111 1110
        }

        bool overflow()
        {
            return (status & 0x02); // 0000 0010
        }

        // Suma al decimal los datos de sum. Condiciones:
        //  1. sizeof(sum.buffer) == sizeof(this->buffer)
        //  2. Ambos datos deben ser positivos.
        void suma(const decimal &sum);

        // Resta del decimal los datos de res. Condiciones:
        //  1. sizeof(res.buffer) == sizeof(this->buffer)
        //  2. Ambos datos deben ser positivos.
        //  3. La resta debe dar un resultado positivo.
        void resta(const decimal &res);

        // Obtiene el inverso multiplicativo, tal que inverse(d)*d = 1.
        decimal inverse() const;

        // Devuelve un digito codificado en pbcd.
        // Si high = true en el nybble alto, en el bajo si false.
        // En caso de que la cifra no se pueda convertir, devuelve 0xFF.
        uint8_t char_to_pbcd(char cifra, bool high) const;

        // Devuelve un caracter que corresponde al dígito almacenado
        // en pbcd. Si high = true decodifica el nybble alto, el bajo
        // en caso contrario.
        char pbcd_to_char(uint8_t cifra, bool high) const;

        // Devuelve el valor de la cifra en la posicion indicada. La primera
        // posicion será la 0, y la última cifs - 1.
        // Siempre devuelve el valor en el nybble bajo.
        uint8_t get_cifra(unsigned int pos) const;

        // Coloca el valor del nybble bajo de val en la posicion indicada.
        void set_cifra(uint8_t val, unsigned int pos);
};

}; // Namespace fpt

#endif // DECIMAL_H_INCLUDED

