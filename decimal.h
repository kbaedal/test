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
    // Nuevo experimento. Esta vez codificando las cifras en bcd,
    // y almacenando el n�mero en orden l�gico, es decir, la ultima
    // cifra (la menos significativa) queda almacenada en el la
    // primera posici�n del buffer.
    //
    // Es probable que este orden facilite las cosas, pero ya veremos.

    public:
        // Constructores
        decimal(unsigned int _c, unsigned int _d);
        decimal(const decimal &d);

        // Destructor
        ~decimal();

        // Operadores - Asignaci�n.
        decimal &operator=(const decimal &d);
        decimal &operator=(const std::string &val);

        // Operador: Negaci�n.
        decimal operator-()
        {
            decimal t(*this);

            if(t.is_negative())
                t.set_positive();
            else
                t.set_negative();

            return t;
        }

        // Operador: + unario.
        decimal operator+()
        {
            decimal t(*this);

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

        // Operadores - Multiplicaci�n y divisi�n.
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
        unsigned int decs;          // Total racionales del n�mero.
        unsigned int cifs;          // Total cifras del n�mero.

        unsigned int long_buffer;   // Tama�o total del buffer.

        uint8_t status;             // Flags de status del decimal.
                                    // Bit 0: Signo (0 positivo, 1 negativo).
                                    // Bit 1: Overflow (0 no hay, 1 s� hay).

        // Comprueba que str contenga un numero v�lido, ignorando espacios y otros
        // caracteres especiales tanto al principio como al final. Devuelve la
        // cadena sin estos caracteres en t.
        bool validar_cadena(const std::string &str, std::string &t);
        void convertir(const std::string &str); // Convierte el n�mero a formato interno.

        std::string int_to_s(const int &n, int width = 0) const;

        int s_to_int(const std::string &s) const;

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

        // Obtiene el inverso multiplicativo, tal que i*d = 1.
        decimal inverse() const;

        // Devuelve un digito codificado en BCD, en el nybble alto
        // si high = true, en el bajo de lo contrario.
        // En caso de que la cifra no se pueda convertir, devuelve 0xFF.
        uint8_t char_to_bcd(char cifra, bool high) const;

        // Devuelve un caracter que corresponde al d�gito almacenado
        // en bcd. Si high = true decodifica el nybble alto, el bajo
        // en caso contrario.
        char bcd_to_char(uint8_t cifra, bool high) const;

        // Devuelve el valor de la cifra en la posicion indicada. La primera
        // posicion ser� la 0, y la �ltima cifs - 1.
        // Siempre devuelve el valor en el nybble bajo.
        uint8_t get_cifra(unsigned int pos) const;

        // Coloca el valor de la cifra en la posicion indicada.
        void set_cifra(uint8_t val, unsigned int pos);

        void print_buffer(std::string encab = "") const;

        void print_bcd(uint8_t val, char sep = ' ') const;
};

}; // Namespace fpt

#endif // DECIMAL_H_INCLUDED

