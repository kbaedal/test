#ifndef DECOART_H_INCLUDED
#define DECOART_H_INCLUDED

#include <string>
#include <vector>

struct desc_campo
{
    int     tipo,   // 1 - Entero, 2 - entero peq, 3 - fecha, 4 - char, 5 - decimal.
            offset, // Desplazamiento desde el principio del registro.
            tam,    // Tamaño en bytes.
            decs;   // Numero de cifras racionales.
};

std::string interpretar_entero(const char *datos, int long_buffer);
std::string interpretar_decimal(const char *datos, int long_buffer, int num_racionales);

bool extraer_datos_articulo(const char *buffer, std::vector<std::string> &datos);
bool extraer_stock_articulo(const char *buffer, std::vector<std::string> &datos);
bool extraer_unid_medida(const char *buffer, std::vector<std::string> &datos);

#endif // EXPART_H_INCLUDED
