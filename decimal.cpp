#include <exception>
#include <cstring>
#include <sstream>

#include "decimal.h"
#include "cosas.h"
#include "utiles.h"

decimal::decimal(unsigned int _c, unsigned int _d) : ents(_c - _d), decs(_d)
{
    // Reservamos 8 bits por cada dos cifras.
    long_ents = static_cast<int>((ents - 1)/2) + 1;
    long_decs = static_cast<int>((decs - 1)/2) + 1;
    buffer = new uint8_t[long_ents + long_decs];
    std::memset(buffer, 0x00, sizeof(uint8_t[long_ents + long_decs]));
}

decimal::decimal(decimal &d)
{
    ents = d.ents;
    decs = d.decs;
    long_ents = d.long_ents;
    long_decs = d.long_decs;

    buffer = new uint8_t[long_ents + long_decs];
    std::memset(buffer, 0x00, sizeof(uint8_t[long_ents + long_decs]));
    std::memcpy(buffer, d.buffer, sizeof(uint8_t[long_ents + long_decs]));
}

decimal::~decimal()
{
    delete [] buffer;
}

decimal &decimal::operator=(const decimal &d) {
    if(this != &d) {
        if(d.ents > ents)
            throw std::out_of_range("El numero pasado no encaja en este decimal.");



        this->operator=(d.to_str());
    }
    return *this;
}

decimal &decimal::operator=(const std::string &val)
{
    // Calculamos la posicion del punto, y desde ahí vemos la parte
    // que corresponde a la entera y a la decimal.
    // Así ya podemos calcular si el número pasado "encaja" en la
    // definicion de este decimal. Si cabe, lo procesamos y convertimos
    // al formato interno. Si no, lanzamos excepción.
    unsigned int c, d, e;

    size_t  pos_punto;

    if(es_numero(val)) {
        // Calculamos numero de cifras, parte entera y parte decimal.
        if((pos_punto = val.find_first_of('.')) != std::string::npos) {
            c = val.size() - 1;
            d = c - pos_punto;
            e = c - d;
        }
        else {
            // El numero pasado no contiene decimales.
            c = val.size();
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
            convertir(val);
        }
    }
    else {
        throw std::invalid_argument("La cadena no contiene un numero valido.");
    }

    return *this;
}

void decimal::convertir(const std::string &str)
{
    // Trabajamos sobre una copia, y comparamos
    // con la representacion de nuestro decimal.
    std::string temp = str,
                orig = this->to_str();

    // Si tenemos parte entera y decimal, o solo decimal,
    // insertamos ceros por delante de str hasta que las
    // posiciones de los puntos se igualen.
    // Si solamente tenemos parte entera, insertamos ceros
    // por delante hasta que el tamaño de str sea igual
    // que la parte entera de nuestro decimal.
    if(temp.find_first_of('.') != std::string::npos)
        while(temp.find_first_of('.') != orig.find_first_of('.'))
            temp.insert(0, "0");
    else
        while(temp.size() < ents)
            temp.insert(0, "0");

    // Igualamos los tamaños, si es necesario, añadiendo ceros al final.
    if(temp.size() < orig.size()) {
        while(temp.size() < orig.size()) {
            // Si no existe el punto decimal, lo añadimos.
            if(temp.find_first_of('.') == std::string::npos)
                temp += '.';

            temp += '0';
        }
    }
    else if (temp.size() > orig.size()) {
        // La parte decimal de temp es mayor de lo que podemos almacenar,
        // así que debemos truncarla, redondeando los valores correctamente.
        char    cifra;
        int     numero;
        bool    acarreo = false;

        while(temp.size() > orig.size()) {
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
        // Seguimos con esta operación siempre que sea necesario.
        size_t i = temp.size() - 1;
        while(acarreo) {
            // Comprobamos que no nos salgamos de rango.
            if(i < 0)
                throw std::out_of_range("El acarreo ha producido desbordamiento.");

            // Nos saltamos el punto, si lo encontramos.
            if(temp[i] == '.') --i;

            // Tomamos la cifra.
            cifra   = temp[i];
            numero  = utiles::StrToInt(cifra) + 1;

            if(numero > 4)
                acarreo = true;
            else
                acarreo = false;

            // Reducimos el numero a una sola cifra, si es necesario.
            if(numero == 10) numero = 0;

            temp[i] = utiles::IntToStr(numero)[0];

            --i;
        }
    }

    // Si la parte entera tiene un numero de cifras impar, añadimos un 0 por delante.
    if((ents % 2) != 0)
        temp.insert(0, 1, '0');

    // Lo mismo para la parte decimal, pero añadiendo al final.
    if((decs % 2) != 0)
        temp += '0';

    // Buscamos el punto y lo eliminamos.
    temp.erase(temp.find_first_of('.'), 1);

    // Convertimos los valores de la cadena a nuestro formato interno.
    for(size_t i = 0, j = 0; i < long_ents + long_decs; ++i, j += 2)
        buffer[i] = utiles::StrToInt(temp.substr(j, 2));
}

std::string decimal::to_str() const
{
    std::string temp;

    for(int i = 0; i < static_cast<int>(long_ents + long_decs); ++i) {
        if(i == static_cast<int>(long_ents))
            temp += std::string(".");

        // Si el numero de enteros es impar, del primer par de numeros
        // tomaremos solamente una cifra.
        // Lo mismo para los decimales, pero con el ultimo par.
        if((i == 0) && ((ents % 2) != 0))
            temp += utiles::IntToStr(static_cast<int>(buffer[i]), 2).substr(1, 1);
        else if((i == static_cast<int>(long_ents + long_decs - 1)) && ((decs % 2) != 0))
            temp += utiles::IntToStr(static_cast<int>(buffer[i]), 2).substr(0, 1);
        else
            temp += utiles::IntToStr(static_cast<int>(buffer[i]), 2);
    }

    return temp;
}

decimal &decimal::operator+=(const decimal &d)
{
    if(d.ents > this->ents)
        throw std::out_of_range("El numero a sumar no encaja en este decimal.");

    // Creamos un decimal del mismo tamaño que nuestro decimal, y le asignamos
    // el dato del decimal a sumar. Nos aseguramos así de hacer el redondeo
    // de los decimales si es necesario.
    decimal temp(ents + decs, ents);
    temp = d.to_str();

    // Por cada pareja de numeros (posicion del buffer), empezando por el final:
    //  1. Sumamos ambos numeros.
    //  2. Si las suma pasa de 100 acarreamos para el siguiente par y restamos 100 a este par.
    bool acarreo = false;
    for(int i = long_ents + long_decs - 1; i > 0; --i) {
        int op1 = static_cast<int>(this->buffer[i]);
        int op2 = static_cast<int>(d.buffer[i]);
        int op3 = op1 + op2;

        if(acarreo) ++op3;

        if(op3 >= 100) {
            acarreo = true;
            op3 -= 100;
        }
        else {
            acarreo = false;
        }

        this->buffer[i] = static_cast<uint8_t>(op3);
    }

    return *this;
}

void decimal::resize(unsigned int _c, unsigned int _d)
{
    // TODO:
    //  1. Comprobar el numero de enteros. Si es mayor que el
    //      que tenemos, no hay problema. En caso contrario,
    //      comprobar el desboradamiento.
    //  2. Para los decimales, simplemente tendremos que tener
    //      ciudado con el redondeo.
    if((_c - _d) < 0)
        throw std::invalid_argument("Numero de decimales incompatible con numero de cifras.");

    unsigned int    _e = _c - _d;
    unsigned int    le = static_cast<int>((_e - 1)/2) + 1;
    unsigned int    ld = static_cast<int>((_d - 1)/2) + 1;
    uint8_t         *temp = new uint8_t[le + ld];

    std::memset(temp, 0x00, sizeof(uint8_t[le + ld]));

    if(_e < ents) {
        // Tenemos que reducir el numero de enteros, asi que debemos
        // comprobar que no se produce desbordamiento.

        // Calculamos el máximo valor que puede tener el primer byte
        // de nuestro nuevo decimal.
        unsigned int max_val = 0;

        if((_e % 2) != 0)
            max_val = 9;
        else
            max_val = 99;

        // Recorremos el buffer original en sentido inverso, desde la
        // primera posicion del nuevo tamaño, y comprobaremos que el
        // valor se puede copiar, además de seguir comprobando que el
        // resto de bytes están a 0.
        int dif = long_ents - le;

        if(buffer[dif] > max_val) {
            std::cout << "buffer[dif] = " << utiles::IntToStr(dif) << ", " << utiles::IntToStr(buffer[dif]) << "\n";
            delete [] temp;
            throw std::out_of_range("Cambio de tamanio imposible.");
        }

        for(int i = dif - 1; i >= 0; --i) {
            if(buffer[i] != 0x00) {
                std::cout << "buffer[i] = " << utiles::IntToStr(buffer[i]) << "\n";
                delete [] temp;
                throw std::out_of_range("Cambio de tamanio imposible.");
            }
        }
        // Llegados aquí, quiere decir que los enteros caben en el nuevo
        // tamaño, así que copiamos los datos desde ents-1 hasta diff.
        for(int i = ents - 1; i >= dif; --i)
            temp[i-dif] = buffer[i];
    }

    // Actualizamos los datos.
    ents = _e;
    decs = _d;
    long_ents = le;
    long_decs = ld;

    delete [] buffer;
    buffer = temp;
    temp = nullptr;
}

