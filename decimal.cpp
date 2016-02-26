#include "decimal.h"
#include "cosas.h"

decimal::decimal(unsigned int _c, unsigned int _d) : cifs(_c), decs(_d), ents(_c - _d)
{
    if(decs > cifs) {
        decs = cifs;
        ents = 0;
    }
    else if(e == 1) {
        parte_entera = new uint8_t;
        *parte_entera = 0x00;
    }
    else {
        parte_entera = new uint8_t[ents];

        for(std::size_t i = 0; i < ents; ++i)
            parte_entera[i] = 0x00;
    }

    if(d == 1) {
        parte_decimal = new uint8_t;
        *parte_decimal = 0x00;
    }
    else {
        parte_decimal = new uint8_t[decs];

        for(std::size_t i = 0; i < d; ++i)
            parte_decimal[i] = 0x00;
    }
}

decimal::decimal(unsigned int _c, unsigned int _d, const int &num) : cifs(_c), decs(_d), ents(_c - _d)
{

}

decimal::~decimal()
{
    if(ents == 1)
        delete parte_entera;
    else if(ents > 1)
        delete [] parte_entera;

    if(decs == 1)
        delete parte_decimal;
    else if(decs > 1)
        delete [] parte_decimal;
}

void decimal::assign(const std::string &num)
{
    // Calculamos la posicion del punto, y desde ahí vemos la parte
    // que corresponde a la entera y a la decimal.
    // Así ya podemos calcular si el número pasado "cabe" en la
    // definicion de este decimal. Si cabe

    size_t  pos_punto;

    if(es_numero(num)) {
        if((pos_punto = num.find_first_of('.')) != std::string::npos) {
            // Calculamos parte entera y decimal.
            // Vemos si "cabe" en este decimal.
            c = num.size() - 1;
            d = c - (pos_punto + 1);
            e = c - (d + 1);
        }
        else {
            // El numero pasado no contiene decimales.
            c = num.size();
            d = 0;
            e = c;
        }
    }
    else {
        // TODO: What? throw exception : out of range.
    }
}





