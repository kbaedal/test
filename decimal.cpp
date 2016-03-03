#include <exception>
#include <cstring>

#include "decimal.h"
#include "cosas.h"
#include "utiles.h"

decimal::decimal(unsigned int _c, unsigned int _d) : ents(_c - _d), decs(_d)
{
    representacion.clear();

    // Reservamos 8 bits por cada dos cifras de la parte entera.
    if(ents > 0) {
        long_ents = static_cast<int>((ents - 1)/2) + 1;
        parte_entera = new uint8_t[long_ents];
        std::memset(parte_entera, 0x00, sizeof(uint8_t[long_ents]));
    }
    else {
        parte_entera = nullptr;
    }

    // Lo mismo para la parte decimal.
    if(decs > 0) {
        long_decs = static_cast<int>((decs - 1)/2) + 1;
        parte_decimal = new uint8_t[long_decs];
        std::memset(parte_decimal, 0x00, sizeof(uint8_t[long_decs]));
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

decimal &decimal::operator=(const std::string &str)
{
    this->assign(str);

    return *this;
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
    size_t      pos_punto;

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

    // Igualamos los tamaños, si es necesario, añadiendo ceros al final.
    if(temp.size() < representacion.size()) {
        while(temp.size() < representacion.size()) {
            // Si no existe el punto decimal, lo añadimos.
            if(temp.find_first_of('.') == std::string::npos)
                temp += '.';

            temp += '0';
        }
    }
    else if (temp.size() > representacion.size()) {
        // La parte decimal de temp es mayor de lo que podemos almacenar,
        // así que debemos truncarla, redondeando los valores correctamente.
        char    cifra;
        int     numero;
        bool    acarreo = false;

        while(temp.size() > representacion.size()) {
            cifra   = temp[temp.size() - 1];
            numero  = utiles::StrToInt(cifra);

            if(acarreo) ++numero;

            // Si el numero es 5 o mayor, incrementamos la siguiente cifra leida.
            if(numero > 4)
                acarreo = true;
            else
                acarreo = false;

            // Eliminamos esta cifra de la cadena.
            temp.pop_back();
        }

        // Si hay acarreo, incrementamos el ultimo valor.
        if(acarreo) {
            cifra   = temp[temp.size() - 1];
            numero  = utiles::StrToInt(cifra) + 1;

            temp.pop_back();
            temp += utiles::IntToStr(numero);
        }
    }

    // Reemplazamos en nuestra representacion interna.
    for(size_t i = 0; i < temp.size(); ++i)
        representacion[i] = temp[i];

    // Actualizamos las estructuras.

    // Tomamos la parte entera para procesarla.
    pos_punto = representacion.find_first_of('.');
    temp = representacion.substr(0, pos_punto);

    // Si el numero de digitos es impar, añadimos un 0 al principio
    // para hacerlo par y poder almacenar los numeros por parejas
    // en nuestro array de enteros.
    if((temp.size() % 2) != 0)
        temp.insert(0, "0");

    // Los pasamos al array.
    for(size_t i = 0, j = long_ents - 1; i < temp.size(); i += 2, --j)
        parte_entera[j] = utiles::StrToInt(temp.substr(i, 2));

    // Y ahora la parte decimal. Con la salvedad de que añadimos al final.
    temp = representacion.substr(pos_punto + 1, representacion.size() - pos_punto);
    if((temp.size() % 2) != 0)
        temp.append("0");

    for(size_t i = 0, j = 0; i < temp.size(); i += 2, ++j)
        parte_decimal[j] = utiles::StrToInt(temp.substr(i, 2));
}







