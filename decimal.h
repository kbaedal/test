#ifndef DECIMAL_H_INCLUDED
#define DECIMAL_H_INCLUDED

#include <cstdint>
#include <string>


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

        // Destructor
        ~decimal();

        // Operadores
        decimal &operator=(const decimal &dec);
        decimal &operator=(const std::string &str);

        void assign(const std::string &num);

        friend std::ostream &operator << (std::ostream &os, const decimal &d)
        {
            os << d.representacion;

            return os;
        }

    private:
        uint8_t *parte_entera;
        uint8_t *parte_decimal;

        bool    positivo;

        unsigned int ents;      // Total enteros del número.
        unsigned int long_ents; // Tamaño del array para los enteros: ((ents - 1)/2) + 1
        unsigned int decs;      // Total decimales del número.
        unsigned int long_decs; // Tamaño del array para los decimales: ((decs - 1)/2) + 1


        std::string representacion; // Contiene la representacion del numero.

        void convertir(const std::string &str);


};

#endif // DECIMAL_H_INCLUDED

