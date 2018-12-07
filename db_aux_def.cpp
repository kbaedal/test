#include <tuple>

#include "db_aux_def.h"

// Constantes para cliente
namespace db_consts {
    extern const int    cliente_num_campos {14};   // Total campos en cliente
    extern const int    cliente_nc_txt     {11};   // Numero de campos de texto
    extern const int    cliente_max_txt    {40};   // Maxima longitud campo de texto
    extern const int    cliente_nc_int     {3};    // Numero de campos de enteros
};

namespace db_binds {

    // Posiciones de los datos a la hora de enlazarlos con la estructura MYSQL_BIND.
    // Columna      Dato
    // -------      ----
    //  1           Posicion dentro del buffer de tipo. Para cada tipo, debe ser
    //              correlativo, e igual al numero de campos de ese tipo declarados
    //              en las constantes. En este caso, el buffer de enteros debe
    //              contener 3 posiciones, de la 0 a la 2. El de cadenas llevaria
    //              11 posiciones (0 a 10).
    //  2           Posicion dentro de la estructura MYSQL_BIND. Tantas como total
    //              de campos para la consulta. En este caso 14 (0 a 13), no puede
    //              repetirse una posicion de esta columna, han de ser todas diferentes.
    //  3           Tipo de dato. Define el buffer del que tomaremos el dato para
    //              colocarlo en la posicion adecuada de la estructura MYSQL_BIND.
    extern const std::vector<info_campo> cliente_ins_bind {
        // Buffer enteros - data_bind_storage.int_data[n]
        {0,     0,      tipo_campo::IS_INT},
        {1,     10,     tipo_campo::IS_INT},
        {2,     11,     tipo_campo::IS_INT},

        // Buffer cadenas - data_bind_storage.str_data[n] y str_long[n]
        {0,     1,      tipo_campo::IS_TXT},
        {1,     2,      tipo_campo::IS_TXT},
        {2,     3,      tipo_campo::IS_TXT},
        {3,     4,      tipo_campo::IS_TXT},
        {4,     5,      tipo_campo::IS_TXT},
        {5,     6,      tipo_campo::IS_TXT},
        {6,     7,      tipo_campo::IS_TXT},
        {7,     8,      tipo_campo::IS_TXT},
        {8,     9,      tipo_campo::IS_TXT},
        {9,     12,     tipo_campo::IS_TXT},
        {10,    13,     tipo_campo::IS_TXT}
    };

    extern const std::vector<info_campo> cliente_upd_bind {
        // Enteros
        {0,     13,     tipo_campo::IS_INT},
        {1,     9,      tipo_campo::IS_INT},
        {2,     10,     tipo_campo::IS_INT},

        // Cadenas
        {0,     0,      tipo_campo::IS_TXT},
        {1,     1,      tipo_campo::IS_TXT},
        {2,     2,      tipo_campo::IS_TXT},
        {3,     3,      tipo_campo::IS_TXT},
        {4,     4,      tipo_campo::IS_TXT},
        {5,     5,      tipo_campo::IS_TXT},
        {6,     6,      tipo_campo::IS_TXT},
        {7,     7,      tipo_campo::IS_TXT},
        {8,     8,      tipo_campo::IS_TXT},
        {9,     11,     tipo_campo::IS_TXT},
        {10,    12,     tipo_campo::IS_TXT}
    };
};

namespace db_queries {
    extern const std::string ins_cliente {
        "INSERT INTO cliente (cod_cliente, razon_social, domicilio, cod_postal, "
        " poblacion, provincia, nif, tel1, tel2, fax, forma_pago, categoria, "
        " swift_bic, iban) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    };

    extern const std::string upd_cliente {         // Consulta a preparar.
        "UPDATE cliente SET razon_social=?, domicilio=?, cod_postal=?, poblacion=?, "
        "provincia=?, nif=?, tel1=?, tel2=?, fax=?, forma_pago=?, categoria=?, "
        "swift_bic=?, iban=? WHERE cliente.cod_cliente=?"
    };
};


