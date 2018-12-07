#ifndef DB_AUX_DEF_H_INCLUDED
#define DB_AUX_DEF_H_INCLUDED

#include <vector>

// Idetificamos el tipo de campo.
enum class tipo_campo {
    IS_INT = 0,     // Tipo de dato int
    IS_TXT,         // Tipo de dato std::string
    IS_DEC          // Tipo de dato ftp::decimal
};

struct  info_campo {
    int     data_pos,       // Posicion en el objeto
            bind_pos;       // Posicion que ocupar en la estructura bind

    tipo_campo  data_type;  // Tipo de dato del campo.

    info_campo(int a, int b, tipo_campo c)
    {
        data_pos    = a;
        bind_pos    = b;
        data_type   = c;
    }
};

// Clase auxiliar, para almacenar los datos que se utilizaran
// durante una consulta preparada de mysql.
class data_bind_storage {
    public:
        char            **str_data;
        unsigned long   *str_long;
        int             *int_data;

        data_bind_storage(int num_txt_f, int num_int_f, int txt_max_long)
        {
            txt_fields = num_txt_f;

            str_data = new char*[txt_fields];
            for(int i = 0; i < txt_fields; ++i)
                str_data[i] = new char[txt_max_long];

            str_long = new unsigned long[txt_fields];
            int_data = new int[num_int_f];
        }
        ~data_bind_storage()
        {
            for(int i = 0; i < txt_fields; ++i)
                delete [] str_data[i];

            delete [] str_data;
            delete [] str_long;
            delete [] int_data;
        }
    private:
        int txt_fields;
};

namespace db_consts {
    // Cliente
    extern const int    cliente_num_campos;
    extern const int    cliente_nc_txt;
    extern const int    cliente_max_txt;
    extern const int    cliente_nc_int;
};

namespace db_binds {
    extern const std::vector<info_campo>    cliente_ins_bind;
    extern const std::vector<info_campo>    cliente_upd_bind;
};

namespace db_queries {
    extern const std::string    ins_cliente;
    extern const std::string    upd_cliente;
};

#endif // DB_AUX_DEF_H_INCLUDED
