#include <iostream>

#include "decoart.h"
#include "utiles.h"
#include "decimal.h"

std::vector<desc_campo> desc_articulos {
    { 2, 0, 2, 0 },     // procod
    { 1, 2, 4, 0 },     // artcod
    //{ 4, 18, 14, 0 }, // artrefe
    { 4, 1018, 30, 0 }, // artrefe2
    { 4, 32, 40, 0 },   // artdesc
    { 1, 80, 4, 0 },    // artfami
    //{ 5, 84, 8, 0 },  // artbarras
    { 4, 632, 13, 0 },  // artcharbarras
    { 4, 94, 4, 0 },    // artmedi
    { 5, 106, 8, 5 },   // artpvp
    { 5, 144, 8, 5 },   // artdto1
    { 5, 152, 8, 5 },   // artdto2
    { 5, 160, 8, 5 },   // artdto3
    { 5, 168, 8, 5 },   // artdto4
    { 5, 176, 8, 5 },   // artdto5
    { 5, 324, 8, 5 },   // artpucosp
    { 5, 332, 4, 2 },   // artdtoc1
    { 5, 336, 4, 2 },   // artdtoc2
    { 5, 340, 4, 2 },   // artdtoc3
    { 5, 360, 8, 5 },   // artpcreal
    { 5, 368, 5, 2 },   // artpbenef
    { 5, 424, 8, 5 },   // artpvpiva
    { 2, 530, 2, 0 }    // artcodiva
};

std::vector<desc_campo> desc_almart {
    { 2, 0, 2, 0 }, // procod
    { 1, 2, 4, 0 }, // artcod
    { 2, 6, 2, 0 }, // artalm
    { 5, 8, 6, 2 }  // artstock
};

std::vector<desc_campo> desc_unimed {
    { 4, 0, 4, 0 },    // unicod
    { 4, 4, 20, 0 }    // unides
};

std::string interpretar_entero(const char *datos, int long_buffer)
{
    unsigned const char   *ptr = reinterpret_cast<unsigned const char *>(datos);
    uint32_t        digitos = 0;

    if(long_buffer == 4) {
        digitos =   (ptr[0] << 24) |
                    (ptr[1] << 16) |
                    (ptr[2] << 8) |
                    ptr[3];
    }
    else if(long_buffer == 2) {
        digitos =   (ptr[0] << 8) |
                    ptr[1];
    }
    else
        return std::string("Error tamanio entero.");

    // Devolvemos los datos formateados.
    return utiles::to_string(digitos, long_buffer * 2, '0');

}

std::string interpretar_decimal(const char *datos, int long_buffer, int num_racionales)
{
    // Se recibe una cadena de bytes que almacenan los datos asi:
    //  - El primer byte es de la forma Cn, donde n indica el numero
    //      de bytes que conforman la parte real (a la izquierda de la coma).
    //  - Cada byte de la cadena representa un valor entre 0 y 99, en hexadecimal,
    //      excepto lógicamente el primero.

    //  Ejemplo:
    //      - datos = C6 07 4d 46 05 31 08 00 (valores en hexadecimal)
    //      - num_racionales = 0.
    //      - Tamaño de los datos: 1 byte de informacion + 7 de datos = 8 bytes.
    //      - C6, indica que la parte real son los 6 bytes siguientes: 07 4d 46 05 31 08.
    //      - 0x07 = 07, 0x4d = 77 ...
    //      - Colocamos y nos da: 077770054908.00
    //      - Como num_racionales = 0, eliminamos el .00
    //      - Resultado: 077770054908
    //
    //  Ejemplo:
    //      - datos = C1 31 18 63 1E 00 00 00 (valores en hexadecimal)
    //      - num_racionales = 5 (como va por bytes, necesitamos 3 para almacenarlos)
    //      - Tamaño de los datos: 1 byte de informacion + 7 de datos = 8 bytes
    //      - C1, indica que la parte real es el byte siguiente: 31
    //      - El resto de los datos, son la parte real: 18 63 1e 00 00 00
    //      - Convertimos cada par a decimal.
    //      - Colocamos y nos da: 49.249930000000
    //      - Como num_racionales = 5, eliminamos a partir de la quinta cifra.
    //      - Resultado: 49.24993

    unsigned const char   *ptr = reinterpret_cast<unsigned const char *>(datos);
    std::string     temp;
    uint16_t        digitos = 0,
                    coma = 0;
    const char      dec_coma = '.';

    // Positivo, negativo, rarito o nulo?
    if((ptr[0] & 0xF0) == 0xB0) { // Rarito.
        // Numero de la forma BN XX XX XX XX ...
        // Donde N = Numero de parejas de 0s tras la coma. Invertido. Hay que sumar 1.
        // XX = Parejas de enteros del numero. Normal.
        temp = "0";
        temp += dec_coma;

        // Invertimos el nybble bajo para saber los 0s tras la coma.
        coma = ~(0x000F & ptr[0]) & 0x000F;

        // Una pareja por cada posicion indicada.
        for(int i = 0; i < coma + 1; ++i)
            temp += "00";

        // Añadimos los digitos.
        for(int i = 1; i < long_buffer; ++i) {
            digitos = 0x00FF & ptr[i];
            temp += utiles::to_string(digitos, 2);
        }

        // Eliminamos los 0 que nos sobran por la derecha.
        // La coma está en el segundo caracter, luego contamos desde ahí.
        temp = temp.substr(0, 2 + num_racionales);
    }
    else if((ptr[0] & 0xF0) == 0xC0) { // Positivo
        // Numero de la forma CN XX XX XX XX ...
        // Donde C = Numero de parejas de cifras (1 byte) a la izquierda de la coma.
        // XX = Parejas de enteros del numero.

        coma = 0x000F & ptr[0];

        // Convertimos la informacion en binario a cadena de caracteres.
        for(int i = 1; i < long_buffer; ++i) {
            digitos = 0x00FF & ptr[i];
            temp += utiles::to_string(digitos, 2);
        }

        // Colocamos el punto decimal en su sitio.
        temp.insert(2 * coma, 1, dec_coma);

        // Eliminamos los digitos que nos sobran por la derecha.
        if(num_racionales > 0)
            temp = temp.substr(0, 2 * coma + num_racionales + 1);
        else // El numero no tiene parte racional, eliminamos desde la coma.
            temp = temp.substr(0, 2 * coma);
    }
    else if((ptr[0] & 0xF0) == 0x30) { // Negativo
        // Numero de la forma 3N XX XX XX XX
        // Donde N = Numero de bytes a la izquierda de la coma. Invertido.
        // XX = Parejas de enteros del numero.
        //
        coma = ~(0x000F & ptr[0]) & 0x000F; // Invertimos el nybble bajo para la posicion de la coma.

        // Convertimos la informacion en binario a cadena de caracteres.
        for(int i = 1; i < long_buffer; ++i) {
            digitos = 0x00FF & ptr[i];
            temp += utiles::to_string(digitos, 2);
        }

        // Colocamos el punto decimal en su sitio.
        // El caracter de la coma tiene que ser un '.', para poder
        // convertirlo luego a decimal sin problemas.
        temp.insert(2 * coma, 1, '.');

        // Ya tenemos el numero. Ahora creamos un decimal que tenga al menos 1 cifra
        // entera más y ponemos a 1 la primera cifra.
        // Le restamos el numero leido y lo hacemos negativo.
        // Ya solo queda convertirlo a cadena.
        fpt::decimal a(2 * coma + num_racionales, num_racionales);
        fpt::decimal b(2 * coma + num_racionales + 1, num_racionales);

        // Ya tenemos el numero leido en un decimal.
        a = temp;

        // Ahora, para tener el 1 en la primera cifra, multiplicamos 10 por si mismo
        // tantas veces como cifras tengamos a la izquierda de la coma.
        b = "1";
        for(int i = 0; i < 2 * coma; ++i)
            b = b * "10";

        // Restamos el numero leido.
        b = b - a;

        // Cambiamos el signo.
        b = b * "-1.0";

        // Devolvemos.
        temp = b.to_str(dec_coma);
    }
    else if((ptr[0] & 0xF0) == 0x00) { // Nulo.
        temp = "0";
        temp += dec_coma;
        temp += "00";
    }
    else { // Negativo
        temp = "Valor inesperado.";
    }

    // Si tenemos un cero a la izquiera que sobre, lo borramos.
    if((temp[0] == '0') && (temp[1] != dec_coma))
        temp.erase(0, 1);

    // Si el primer caracter es un punto, ponemos un 0 delante.
    if(temp[0] == dec_coma)
        temp.insert(0, 1, '0');

    // Devolvemos los datos formateados.
    return temp;
}

bool extraer_datos_articulo(const char *buffer, std::vector<std::string> &datos)
{
    std::string temp;

    for(size_t i = 0; i < desc_articulos.size(); ++i) {
        // Para hacerlo mas legible.
        int     tipo    = desc_articulos[i].tipo,
                offset  = desc_articulos[i].offset,
                tam     = desc_articulos[i].tam,
                decs    = desc_articulos[i].decs;

        switch(tipo){
            case 1: // Entero
            case 2: // Entero pequeño
                datos.push_back(interpretar_entero(&buffer[offset], tam));
                //temp =  interpretar_entero(&buffer[offset], tam);
                break;
            case 3: // Fecha
                datos.push_back(std::string("Pendiente implementar."));
                //temp = std::string("Pendiente implementar.");
                break;
            case 4: // Caracteres
                temp = std::string(&buffer[offset], tam);

                if(temp[0] == 0x00) { // Hemos leido una cadena nula, llenamos con espacios hasta tam.
                    temp.clear();
                    temp.insert(0, tam, ' ');
                }

                datos.push_back(utiles::GID_to_LATIN(temp));
                //temp = std::string(&buffer[offset], tam);
                break;
            case 5: // Decimal
                datos.push_back(interpretar_decimal(&buffer[offset], tam, decs));
                //temp = interpretar_decimal(&buffer[offset], tam, decs);
                break;
            default:
                return false;
        }
    }

    return true;
}

bool extraer_stock_articulo(const char *buffer, std::vector<std::string> &datos)
{
    std::string temp;

    for(size_t i = 0; i < desc_almart.size(); ++i) {
        // Para hacerlo mas legible.
        int     tipo    = desc_almart[i].tipo,
                offset  = desc_almart[i].offset,
                tam     = desc_almart[i].tam,
                decs    = desc_almart[i].decs;

        switch(tipo){
            case 1: // Entero
            case 2: // Entero pequeño
                datos.push_back(interpretar_entero(&buffer[offset], tam));
                //temp =  interpretar_entero(&buffer[offset], tam);
                break;
            case 3: // Fecha
                datos.push_back(std::string("Pendiente implementar."));
                //temp = std::string("Pendiente implementar.");
                break;
            case 4: // Caracteres
                temp = std::string(&buffer[offset], tam);

                datos.push_back(utiles::GID_to_LATIN(temp));
                //temp = std::string(&buffer[offset], tam);
                break;
            case 5: // Decimal
                datos.push_back(interpretar_decimal(&buffer[offset], tam, decs));
                //temp = interpretar_decimal(&buffer[offset], tam, decs);
                break;
            default:
                return false;
        }
    }

    return true;
}

bool extraer_unid_medida(const char *buffer, std::vector<std::string> &datos)
{
    std::string temp;

    for(size_t i = 0; i < desc_unimed.size(); ++i) {
        // Para hacerlo mas legible.
        int     tipo    = desc_unimed[i].tipo,
                offset  = desc_unimed[i].offset,
                tam     = desc_unimed[i].tam,
                decs    = desc_unimed[i].decs;

        switch(tipo){
            case 1: // Entero
            case 2: // Entero pequeño
                datos.push_back(interpretar_entero(&buffer[offset], tam));
                //temp =  interpretar_entero(&buffer[offset], tam);
                break;
            case 3: // Fecha
                datos.push_back(std::string("Pendiente implementar."));
                //temp = std::string("Pendiente implementar.");
                break;
            case 4: // Caracteres
                temp = std::string(&buffer[offset], tam);

                datos.push_back(utiles::GID_to_LATIN(temp));
                //temp = std::string(&buffer[offset], tam);
                break;
            case 5: // Decimal
                datos.push_back(interpretar_decimal(&buffer[offset], tam, decs));
                //temp = interpretar_decimal(&buffer[offset], tam, decs);
                break;
            default:
                return false;
        }
    }

    return true;
}
