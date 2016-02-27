#include <exception>

#include "decimal.h"
#include "cosas.h"
#include "utiles.h"

decimal::decimal(unsigned int _c, unsigned int _d) : cifs(_c), decs(_d), ents(_c - _d)
{
    representacion.clear();

    if(decs > cifs) {
        decs = cifs;
        ents = 0;
    }
    else if(ents == 1) {
        parte_entera = new uint8_t;
        *parte_entera = 0x00;

        representacion = "0";
    }
    else {
        parte_entera = new uint8_t[ents];

        for(std::size_t i = 0; i < ents; ++i) {
            parte_entera[i] = 0x00;
            representacion += "0";
        }
    }

    if(decs == 1) {
        parte_decimal = new uint8_t;
        *parte_decimal = 0x00;

        representacion += ".0";
    }
    else if (decs != 0) {
        parte_decimal = new uint8_t[decs];

        representacion += ".";

        for(std::size_t i = 0; i < decs; ++i) {
            parte_decimal[i] = 0x00;

            representacion += "0";
        }
    }
}

decimal::decimal(unsigned int _c, unsigned int _d, const int &num) : decimal(_c, _d)
{
    // Asignamos el entero tras convertirlo a string
    this->assign(utiles::IntToStr(num));
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
    // Así ya podemos calcular si el número pasado "encaja" en la
    // definicion de este decimal. Si cabe, lo procesamos y convertimos
    // al formato intero. Si no, lanzamos excepción.
    unsigned int c, d, e;

    size_t  pos_punto;

    if(es_numero(num)) {
        // Calculamos numero de cifras, parte entera y parte decimal.
        if((pos_punto = num.find_first_of('.')) != std::string::npos) {
            c = num.size() - 1;
            d = c - pos_punto;
            e = c - d;
        }
        else {
            // El numero pasado no contiene decimales.
            c = num.size();
            d = 0;
            e = c;
        }

        // Comprobamos que el numero pasado "encaja" en este decimal.
        // Pasarmos la parte entera, si encaja, y truncamos la parte
        // decimal si hay más decimales de los precisos.
        //if((c > cifs) || (d > decs) || (e > ents)) {
        if(e > ents) {
            throw std::out_of_range("El numero pasado no encaja en este decimal.");
        }
        else {
            // Procesamos la cadena y la convertimos al formato interno.

            /*
             * ¿Qué pasa con el redondeo de decimales si truncamos?
             */
            convertir(num);
        }
    }
    else {
        throw std::invalid_argument("La cadena no contiene un numero valido.");
    }
}

void decimal::convertir(const std::string &str)
{
    // Trabajamos sobre una copia.
    std::string temp = str;

    // Si tenemos parte entera y decimal, o solo decimal,
    // insertamos ceros por delante de str hasta que las
    // posiciones de los puntos se igualen.
    // Si solamente tenemos parte entera, insertamos ceros
    // por delante hasta que el tamaño de str sea igual
    // que la parte entera de nuestro decimal.

    if(temp.find_first_of('.') != std::string::npos)
        while(temp.find_first_of('.') != representacion.find_first_of('.'))
            temp.insert(0, "0");
    else
        while(temp.size() < ents)
            temp.insert(0, "0");

    // Reemplazamos en nuestra representacion interna.
    for(size_t i = 0; i < temp.size(); ++i)
        representacion[i] = temp[i];

    // Actualizamos las estructuras.
}






