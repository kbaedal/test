#include "decimal.h"

decimal::decimal(unsigned int _c, unsigned int _d) : c(_c), d(_d), e(_c - _d)
{
    if(d > c) {
        d = c;
        e = 0;
    }
    else if(e == 1) {
        parte_entera = new uint8_t;
        *parte_entera = 0x00;
    }
    else {
        parte_entera = new uint8_t[e];

        for(std::size_t i = 0; i < e; ++i)
            parte_entera[i] = 0x00;
    }

    if(d == 1) {
        parte_decimal = new uint8_t;
        *parte_decimal = 0x00;
    }
    else {
        parte_decimal = new uint8_t[d];

        for(std::size_t i = 0; i < d; ++i)
            parte_decimal[i] = 0x00;
    }
}

decimal::decimal(unsigned int _c, unsigned int _d, const int &num) : c(_c), d(_d), e(_c - _d)
{

}

decimal::~decimal()
{
    if(e == 1)
        delete parte_entera;
    else if(e > 1)
        delete [] parte_entera;

    if(d == 1)
        delete parte_decimal;
    else if(d > 1)
        delete [] parte_decimal
}

