#include <exception>
#include <cstring>

#include "decimal.h"
#include "cosas.h"
#include "utiles.h"

decimal::decimal(unsigned int _e, unsigned int _d) : ents(_e), decs(_d)
{
    representacion.clear();

    // Reservamos 8 bits por cada dos cifras de la parte entera.
    if(ents > 0) {
        parte_entera = new uint8_t[static_cast<int>((ents - 1)/2) + 1];
        std::memset(parte_entera, 0x00, sizeof parte_entera);
    }
    else {
        parte_entera = nullptr;
    }

    // Lo mismo para la parte decimal.
    if(decs > 0) {
        parte_decimal = new uint8_t[static_cast<int>((decs - 1)/2) + 1];
        std::memset(parte_decimal, 0x00, sizeof parte_decimal);
    }
    else {
        parte_decimal = nullptr;
    }

    // Rellenamos la representacion del numero.
    for(size_t i = 0; i < ents; ++i)
        representacion += "0";

    representacion += ".";

    for(size_t i = 0; i < decs; ++i)
        representacion += "0";
}

decimal::decimal(unsigned int _e, unsigned int _d, const int &num) : decimal(_e, _d)
{
    // Asignamos el entero tras convertirlo a string
    this->assign(utiles::IntToStr(num));
}

decimal::~decimal()
{
    if(parte_entera != nullptr)
        delete [] parte_entera;

    if(parte_decimal != nullptr)
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
    for(int i = representacion.find_first_of('.') - 1, j = 0; i >= 0; i -= 2, ++j) {
        std::cout << "Convertimos representacion.substr(" << i-1 << ", 2) para parte_entera[" << j << "]" << std::endl;
        parte_entera[j] = utiles::StrToInt(representacion.substr(i-1, 2));
    }

    for(int i = representacion.find_first_of('.') + 1, j = 0; i <= representacion.length(); i += 2, ++j) {
        std::cout << "Convertimos representacion.substr(" << i << ", 2) para parte_decimal[" << j << "]" << std::endl;
        parte_decimal[j] = utiles::StrToInt(representacion.substr(i, 2));
    }

    std::cout << "Ents: ";
    for(int i = 0; i < ents/2; ++i)
        std::cout << utiles::IntToStr(static_cast<int>(parte_entera[i])) << " ";
    std::cout << std::endl;

    std::cout << "Decs: ";
    for(int i = 0; i < decs/2; ++i)
        std::cout << utiles::IntToStr(static_cast<int>(parte_decimal[i])) << " ";
    std::cout << std::endl;
}







