#include <exception>
#include <cstring>
#include <sstream>
#include <bitset>

#include "decimal.h"
#include "cosas.h"
#include "utiles.h"

decimal::decimal(unsigned int _c, unsigned int _d) : ents(_c - _d), decs(_d)
{
    // Reservamos 8 bits por cada dos cifras.
    long_ents   = static_cast<int>((ents - 1)/2) + 1;
    long_decs   = static_cast<int>((decs - 1)/2) + 1;
    long_buffer = long_ents + long_decs;

    buffer      = new uint8_t[long_buffer];

    std::memset(buffer, 0x00, sizeof(uint8_t[long_buffer]));
}

decimal::decimal(const decimal &d)
{
    ents        = d.ents;
    decs        = d.decs;
    long_ents   = d.long_ents;
    long_decs   = d.long_decs;
    long_buffer = d.long_buffer;

    buffer = new uint8_t[long_buffer];

    std::memset(buffer, 0x00, sizeof(uint8_t[long_buffer]));
    std::memcpy(buffer, d.buffer, sizeof(uint8_t[long_buffer]));
}

decimal::~decimal()
{
    delete [] buffer;
}

decimal &decimal::operator=(const decimal &d) {
    if(this != &d) {
        decimal temp(d);
        temp.resize(ents + decs, decs);

        std::memcpy(buffer, temp.buffer, sizeof(uint8_t[long_buffer]));
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
    std::string temp = val;

    size_t  pos_punto;

    if(es_numero(temp)) {
        // Comprobamos si es negativo.
        if(temp[0] == '-') {
            this->set_negative(true);
            temp.erase(0, 1);
        }
        // Calculamos numero de cifras, parte entera y parte decimal.
        if((pos_punto = temp.find_first_of('.')) != std::string::npos) {
            c = temp.size() - 1;
            d = c - pos_punto;
            e = c - d;
        }
        else {
            // El numero pasado no contiene decimales.
            c = temp.size();
            d = 0;
            e = c;
        }

        // Comprobamos que el numero pasado "encaja" en este decimal.
        // Si la parte entera es mayor de lo que podemos almacenar, tenemos
        // que comprobar que las cifras que sobran son 0. Si alguna de esas
        // cifras es distinta de 0, no podremos almacenarla y por lo tanto
        // devolvemos error.
        if(e > ents) {
            int i = 0;
            while(e > ents) {
                char cifra = temp[i];

                if(utiles::StrToInt(cifra) != 0)
                    throw std::out_of_range("El numero pasado no encaja en este decimal.");

                // Avanzamos por la cadena.
                ++i;
                // Una cifra menos por comprobar.
                --e;
            }
        }

        // Procesamos la cadena y la convertimos al formato interno.
        convertir(temp);
    }
    else {
        throw std::invalid_argument("La cadena no contiene un numero valido.");
    }

    return *this;
}

decimal &decimal::operator+=(const decimal &d)
{
    /***
     * ¡¡¡ OJO !!!
     *
     * Hay que tener en cuenta el signo de los operadores, que ahora
     * no lo tengo en cuenta. Implemento la resta y luego ya veo
     * por donde tirar con esto, que debe ser muy parecido.
     *
    ***/
    unsigned int max_val_e = ((ents % 2) != 0) ? 9 : 99;

    // Cambiamos el tamaño del decimal a sumar. Con esto garantizamos dos cosas:
    //  1. Que la suma se pueda realizar (mismo numero de enteros y decimales).
    //  2. Que se haya redondeado el decimal correctamente.
    decimal temp(d);
    temp.resize(ents + decs, decs);

    // Por cada pareja de numeros (posicion del buffer), empezando por el final:
    //  1. Sumamos ambos numeros.
    //  2. Si las suma pasa de 100 acarreamos para el siguiente par y restamos 100 a este par.
    bool acarreo = false;
    for(int i = long_buffer - 1; i > 0; --i) {
        int op1 = static_cast<int>(this->buffer[i]);
        int op2 = static_cast<int>(temp.buffer[i]);
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

    // Comprobamos que no nos hayamos salido de rango.
    if(buffer[0] > max_val_e)
        throw std::out_of_range("La suma ha producido desbordamiento.");

    return *this;
}


decimal &decimal::operator-=(const decimal &d)
{
    /***
     *  RESTA CHUNGA DE LA MUE-TE
     *
     *  Tenemos la resta típica:
     *
     *      M inuendo
     *    - S ustraendo
     *     ---
     *      R esultado
     *
     *  Dependiendo del signo de M y de S, y de su valor absoluto, tendremos que
     *  realizar una operación u otra:
     *      - Si M(+) y S(+):
     *          - Si abs(M) >= abs(S) R = ABS(M) - ABS(S), R(+).
     *          - Si abs(S) > abs(M) R = ABS(S) - ABS(M), R(-).
     *      - Si M(+) y S(-) R = ABS(M) + ABS(S), R(+).
     *      - Si M(-) y S(+):
     *          - Si abs(M) >= abs(S) R = ABS(M) + ABS(S), R(-).
     *          - Si abs(S) > abs(M) R = ABS(S) + ABS(M), R(+).
     *      - Si M(-) y S(-)
     *          - Si abs(M) >= abs(S) R = ABS(M) - ABS(S), R(-).
     *          - Si abs(S) > abs(M) R = ABS(S) - ABS(M), R(+).
     *
    ***/

    unsigned int max_val_e = ((ents % 2) != 0) ? 9 : 99;

    // Cambiamos el tamaño del decimal a sumar. Con esto garantizamos dos cosas:
    //  1. Que la suma se pueda realizar (mismo numero de enteros y decimales).
    //  2. Que se haya redondeado el decimal correctamente.
    decimal temp(d);
    temp.resize(ents + decs, decs);

    // Por cada pareja de numeros (posicion del buffer), empezando por el final:
    //  1. Sumamos ambos numeros.
    //  2. Si las suma pasa de 100 acarreamos para el siguiente par y restamos 100 a este par.
    bool acarreo = false;
    for(int i = long_buffer - 1; i > 0; --i) {
        int op1 = static_cast<int>(this->buffer[i]);
        int op2 = static_cast<int>(temp.buffer[i]);

        if(acarreo) ++op2;

        if(op1 < op2) {
            op1 += 100;
            acarreo = true;
        }
        else {
            acarreo = false;
        }

        int op3 = op1 - op2;

        this->buffer[i] = static_cast<uint8_t>(op3);
    }

    if(acarreo) this->set_negative(true);

    // Comprobamos que no nos hayamos salido de rango.
    if(buffer[0] > max_val_e)
        throw std::out_of_range("La resta ha producido desbordamiento.");

    return *this;
}

decimal &decimal::operator*=(const decimal &d)
{

}

decimal &decimal::operator/=(const decimal &d)
{

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
    unsigned int    lb = le + ld;
    uint8_t         *temp = new uint8_t[lb];

    unsigned int    max_val_e   = ((_e % 2) != 0) ? 9 : 99;
    unsigned int    max_val_d   = ((_d % 2) != 0) ? 94 : 99;

    std::memset(temp, 0x00, sizeof(uint8_t[lb]));

    // Empezaremos por los enteros, por si después con los decimales
    // es necesario aplicar redondeo.
    if(_e < ents) {
        // Tenemos que reducir el numero de enteros, asi que debemos
        // comprobar que no se produce desbordamiento.

        // Recorremos el buffer original en sentido inverso, desde la
        // primera posicion del nuevo tamaño, y comprobaremos que el
        // valor se puede copiar, además de seguir comprobando que el
        // resto de bytes están a 0.
        int dif = long_ents - le;

        if(buffer[dif] > max_val_e) {
            delete [] temp;
            throw std::out_of_range("Cambio de tamanio imposible.");
        }

        for(int i = dif - 1; i >= 0; --i) {
            if(buffer[i] != 0x00) {
                delete [] temp;
                throw std::out_of_range("Cambio de tamanio imposible.");
            }
        }
        // Llegados aquí, quiere decir que los enteros caben en el nuevo
        // tamaño, así que copiamos los datos desde ents-1 hasta diff.
        for(int i = long_ents - 1, j = le - 1; j >= 0; --i, --j)
            temp[j] = buffer[i];
    }
    else {
        for(int i = long_ents - 1, j = le - 1; i >= 0; --i, --j)
            temp[j] = buffer[i];
    }

    // Ahora los decimales.
    if(_d < decs) {
        // Tendremos que redondear, si es necesario.
        // TODO:
        //  1. Calcular la diferencia de bytes.
        //  2. Eliminar el último byte, y redondear todo el decimal.
        //  3. Repetir para todos los bytes a eliminar.
        unsigned int    dif         = long_decs - ld;

        bool            acarreo     = false;

        // Procesamos los bytes a eliminar, para ver si una vez eliminados
        // tenemos que acarrear al último byte del nuevo tamaño.
        for(int i = long_buffer - 1; i >= long_buffer - dif - 1; --i) {
            if(acarreo) ++buffer[i];

            if(buffer[i] > 49)
                acarreo = true;
            else
                acarreo = false;
        }

        // Copiamos la parte del buffer que si cabe en el temporal.
        for(int i = long_ents, j = le; j < lb; ++i, ++j) {
            temp[j] = buffer[i];
        }

        // Ahora procesamos todo el temporal, para comprobar que todo va bien.
        // Tendremos que comprobar que si el acarreo nos ha hecho incrementar
        // la parte entera, esta no se salga de rango.
        acarreo = false;
        for(int i = lb - 1; i >= 0; --i) {
            if(acarreo)
                ++temp[i];

            if(i == lb - 1) {
                // El último byte. Podrá tener un valor máximo
                // dependiendo del numero de cifras del decimal.
                if(temp[i] > max_val_d) {
                    // Nuestro último numero produce acarreo.
                    // Ponemos a 0 e indicamos.
                    temp[i] = 0;
                    acarreo = true;
                }
                else if((_d % 2) != 0) {
                    // En caso de que el nuevo decimal solo pueda
                    // almacenar un numero impar de cifras, hay que
                    // poner a 0 la menos significativa del par.
                    temp[i] /= 10;
                    temp[i] *= 10;
                }
            }
            else if(temp[i] >= 100) {
                temp[i] = 0;
                acarreo = true;
            }
            else {
                acarreo = false;
            }
        }

        // Comprobamos que no nos hayamos salido de rango.
        if(temp[0] > max_val_e) {
            delete [] temp;
            throw std::out_of_range("Cambio de tamanio imposible.");
        }
    }
    else {
        for(unsigned int i = long_ents, j = le; i < long_buffer; ++i, ++j)
            temp[j] = buffer[i];
    }

    // Actualizamos los datos.
    ents        = _e;
    decs        = _d;
    long_ents   = le;
    long_decs   = ld;
    long_buffer = lb;

    delete [] buffer;
    buffer = temp;
    temp = nullptr;
}

void decimal::convertir(const std::string &str)
{
    // Trabajamos sobre una copia, y comparamos
    // con la representacion de nuestro decimal.
    std::string temp = str,
                orig = this->to_str();

    // Eliminamos el guión, si lo hay.
    if(orig[0] == '-')
        orig.erase(0, 1);

    size_t      pos_punto_t = temp.find_first_of('.'),
                pos_punto_o = orig.find_first_of('.');

    // Equiparamos los tamaños de las partes entera y decimal.
    if(pos_punto_t != std::string::npos) {
        if(pos_punto_t > pos_punto_o)
            // La cadena pasada tiene más cifras en la parte entera de las que podemos
            // almacenar. Eliminamos tantas como sea necesario, ya que sabemos que
            // son solamente 0's, lo hemos comprobado antes.
            temp.erase(0, pos_punto_t - pos_punto_o);
        else
            // La cadena pasada tiene menos cifras en la parte entera, con lo que
            // rellenamos con 0.
            temp.insert(0, pos_punto_o - pos_punto_t, '0');
    }
    else {
        while(temp.size() < ents)
            temp.insert(0, "0");
    }

    // Ahora la parte decimal.
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
    // Habrá que tener cuidado con el primer byte, pues almacena también
    // el signo del decimal.
    for(size_t i = 0, j = 0; i < long_buffer; ++i, j += 2) {
        if(i == 0) {
            uint8_t x = static_cast<uint8_t>(utiles::StrToInt(temp.substr(j, 2)));
            buffer[i] &= 0x80;  // Borramos todo menos el bit más significativo.
            buffer[i] |= x;     // Copia bit a bit.
        }
        else {
            buffer[i] = static_cast<uint8_t>(utiles::StrToInt(temp.substr(j, 2)));
        }
    }
}

std::string decimal::to_str() const
{
    std::string temp;

    for(int i = 0; i < static_cast<int>(long_buffer); ++i) {
        uint8_t x = buffer[i];

        if(i == 0) {
            if(is_negative()) {
                // Si el decimal es negativo, imprimimos el guión
                // y eliminamos el bit de signo para imprimir
                // luego el valor correcto.
                temp += '-';
                x &= 0x7F;
            }
        }

        if(i == static_cast<int>(long_ents))
            temp += '.';

        // Si el numero de enteros es impar, del primer par de numeros
        // tomaremos solamente una cifra.
        // Lo mismo para los decimales, pero con el ultimo par.
        if((i == 0) && ((ents % 2) != 0))
            temp += utiles::IntToStr(static_cast<int>(x), 2).substr(1, 1);
        else if((i == static_cast<int>(long_buffer - 1)) && ((decs % 2) != 0))
            temp += utiles::IntToStr(static_cast<int>(x), 2).substr(0, 1);
        else
            temp += utiles::IntToStr(static_cast<int>(x), 2);
    }

    return temp;
}
