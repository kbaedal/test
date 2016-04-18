#ifndef DECIMAL_H_INCLUDED
#define DECIMAL_H_INCLUDED

#include <cstdint>
#include <string>
#include <iostream>

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

        // Operadores - Asignaci�n.
        decimal &operator=(const decimal &d);
        decimal &operator=(const std::string &val);

        // Operadors - Negaci�n
        decimal operator-()
        {
            decimal t(*this);

            if(t.is_negative())
                t.set_positive();
            else
                t.set_negative();

            return t;
        }

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

        // Operadores - Multiplicaci�n y divisi�n.
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

        // Cambia el tama�o del decimal, redondeando si es necesario
        // o lanzando excepci�n si se produce desbordamiento.
        void resize(unsigned int _c, unsigned int _d);

        // Devuelve el decimal como cadena de caracteres.
        // Por defecto le dara un formato para mostrar por pantalla,
        // eliminado 0 por la izquerda y acomodando decimales y signo.
        // Para obtener la representacion interna format = false.
        std::string to_str(bool format = true) const;

        // Devuelve un decimal con el valor absoluto de v.
        decimal abs() const;

        // Devuelve un decimal con el valor maximo que puede tener v.
        decimal max() const;

        // Devuelve un decimal con el valor minimo que puede tener v.
        decimal min() const;

        // Devuelve un decimal de iguales caracteristicas que v, pero con valor 0.
        decimal zero() const;

    private:
        uint8_t *buffer;

        unsigned int ents;          // Total enteros del n�mero.
        unsigned int long_ents;     // Tama�o del array para los enteros: ((ents - 1)/2) + 1
        unsigned int decs;          // Total decimales del n�mero.
        unsigned int long_decs;     // Tama�o del array para los decimales: ((decs - 1)/2) + 1

        unsigned int long_buffer;   // Tama�o total del buffer: long_ents + long_decs

        // Comprueba que str contenga un numero v�lido, ignorando espacios y otros
        // caracteres especiales tanto al principio como al final. Devuelve la
        // cadena sin estos caracteres en t.
        bool validar_cadena(const std::string &str, std::string &t);
        void convertir(const std::string &str); // Convierte el n�mero a formato interno.

        std::string int_to_s(const int &n, int width = 0) const;

        int s_to_int(const std::string &s) const;

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

        // Obtiene el inverso multiplicativo, tal que i*d = 1.
        decimal inverse() const;
};

class bcddec {
    // Nuevo experimento. Esta vez codificando las cifras en bcd,
    // y almacenando el n�mero en orden l�gico, es decir, la ultima
    // cifra (la menos significativa) queda almacenada en el la
    // primera posici�n del buffer.
    //
    // Es probable que este orden facilite las cosas, pero ya veremos.

    public:
        // Constructores
        bcddec(unsigned int _c, unsigned int _d);
        bcddec(const bcddec &bcd);

        // Destructor
        ~bcddec();

        // Operadores - Asignaci�n.
        bcddec &operator=(const bcddec &d);
        bcddec &operator=(const std::string &val);

        // Operadors - Negaci�n
        bcddec operator-()
        {
            bcddec t(*this);

            if(t.is_negative())
                t.set_positive();
            else
                t.set_negative();

            return t;
        }

        // Operadores - Suma y Resta.
        bcddec &operator+=(const bcddec &d);
        friend bcddec operator+(bcddec a, const bcddec &b)
        {
            a += b;
            return a;
        }
        friend bcddec operator+(bcddec a, const std::string &val)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            a += t;

            return a;
        }
        friend bcddec operator+(const std::string &val, bcddec a)
        {
            return a + val;
        }

        bcddec &operator-=(const bcddec &d);
        friend bcddec operator-(bcddec a, const bcddec &b)
        {
            a -= b;
            return a;
        }
        friend bcddec operator-(bcddec a, const std::string &val)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            a -= t;

            return a;
        }
        friend bcddec operator-(const std::string &val, bcddec a)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            t -= a;

            return t;
        }

        // Operadores - Multiplicaci�n y divisi�n.
        bcddec &operator*=(const bcddec &d);
        friend bcddec operator*(bcddec a, const bcddec &b)
        {
            a *= b;
            return a;
        }
        friend bcddec operator*(bcddec a, const std::string &val)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            a *= t;

            return a;
        }
        friend bcddec operator*(const std::string &val, bcddec a)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            t *= a;

            return t;
        }

        bcddec &operator/=(const bcddec &d);
        friend bcddec operator/(bcddec a, const bcddec &b)
        {
            a /= b;
            return a;
        }
        friend bcddec operator/(bcddec a, const std::string &val)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            a /= t;

            return a;
        }
        friend bcddec operator/(const std::string &val, bcddec a)
        {
            bcddec t(a.ents + a.decs, a.decs);
            t = val;
            t /= a;

            return t;
        }

        // Operadores - Comparacion.
        friend bool operator==(const bcddec &a, const bcddec &b);
        friend bool operator==(const bcddec &a, const std::string &b);
        friend bool operator!=(const bcddec &a, const bcddec &b)
        {
            return !(a == b);
        }
        friend bool operator!=(const bcddec &a, const std::string &b)
        {
            return !(a == b);
        }

        friend bool operator>=(const bcddec &a, const bcddec &b);
        friend bool operator<=(const bcddec &a, const bcddec &b);

        friend bool operator>(const bcddec &a, const bcddec &b)
        {
            return !(a <= b);
        }

        friend bool operator<(const bcddec &a, const bcddec &b)
        {
            return !(a >= b);
        }

        friend std::ostream &operator << (std::ostream &os, const bcddec &d)
        {
            os << d.to_str();

            return os;
        }

        // Cambia el tama�o del bcddec, redondeando si es necesario
        // o lanzando excepci�n si se produce desbordamiento.
        void resize(unsigned int _c, unsigned int _d);

        // Devuelve el bcddec como cadena de caracteres.
        // Por defecto le dara un formato para mostrar por pantalla,
        // eliminado 0 por la izquerda y acomodando bcddeces y signo.
        // Para obtener la representacion interna format = false.
        std::string to_str(bool format = true) const;

        // Devuelve un bcddec con el valor absoluto de v.
        bcddec abs() const;

        // Devuelve un bcddec con el valor maximo que puede tener v.
        bcddec max() const;

        // Devuelve un bcddec con el valor minimo que puede tener v.
        bcddec min() const;

        // Devuelve un bcddec de iguales caracteristicas que v, pero con valor 0.
        bcddec zero() const;

    private:
        uint8_t *buffer;

        unsigned int ents;          // Total enteros del n�mero.
        unsigned int long_ents;     // Tama�o del array para los enteros: ((ents - 1)/2) + 1
        unsigned int decs;          // Total bcddeces del n�mero.
        unsigned int long_decs;     // Tama�o del array para los bcddeces: ((decs - 1)/2) + 1

        unsigned int long_buffer;   // Tama�o total del buffer: long_ents + long_decs

        // Comprueba que str contenga un numero v�lido, ignorando espacios y otros
        // caracteres especiales tanto al principio como al final. Devuelve la
        // cadena sin estos caracteres en t.
        bool validar_cadena(const std::string &str, std::string &t);
        void convertir(const std::string &str); // Convierte el n�mero a formato interno.

        std::string int_to_s(const int &n, int width = 0) const;

        int s_to_int(const std::string &s) const;

        bool is_negative() const
        {
            // El bit m�s significativo del primer byte del array nos indicar�
            // si el bcddec es negatvo o positivo.
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

        // Suma al bcddec los datos de sum. Condiciones:
        //  1. sizeof(sum) == sizeof(buffer)
        //  2. Ambos datos deben ser positivos.
        void suma(const uint8_t *sum);

        // Resta del bcddec los datos de res. Condiciones:
        //  1. sizeof(res) == sizeof(buffer)
        //  2. Ambos datos deben ser positivos.
        //  3. La resta debe dar un resultado positivo.
        void resta(const uint8_t *res);

        // Obtiene el inverso multiplicativo, tal que i*d = 1.
        bcddec inverse() const;

        // Devuelve un digito codificado en BCD, en el nybble alto
        // si high = true, en el bajo de lo contrario.
        // En caso de que la cifra no se pueda convertir, devuelve 0xFF.
        uint8_t char_to_bcd(char cifra, bool high);

        // Devuelve un caracter que corresponde al d�gito almacenado
        // en bcd. Si high = true decodifica el nybble alto, el bajo
        // en caso contrario.
        char bcd_to_char(uint8_t cifra, bool high);
};

}; // Namespace fpt

#endif // DECIMAL_H_INCLUDED

