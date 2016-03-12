#include <iostream>
#include <string>

#include <mysql.h>

#include "cosas.h"
#include "decimal.h"



int main()
{
    /*
    MYSQL *my_con = mysql_init(NULL);

    // Incializamos la estructura de la conexion.
    if (my_con == NULL) {
        std::cout << mysql_error(my_con) << std::endl;
        return 1;
    }

    // Indicamos que nuestra base de datos se almacena en utf8.
    mysql_options(my_con, MYSQL_SET_CHARSET_NAME, "utf8");

    // Conectamos.
    if (mysql_real_connect(my_con, "backupserver", "sepyauser", "sepyapwd",
            "sepyadb", 0, NULL, 0) == NULL) {
        std::cout << mysql_error(my_con) << std::endl;
        mysql_close(my_con);
        return 1;
    }
    else {
        std::cout << "Conexion correcta." << std::endl;
    }

    //verificar_tablas(my_con);

    //revisar_clientes(my_con);
    Cliente c;

    get_datos_cliente(my_con, 100, c);

    mysql_close(my_con);
    */
    decimal a(10, 5);
    decimal b(10, 5);

    a = "15.00000";
    b = "1.99999";

    std::cout << a << " , " << b << std::endl;

    a = a + b;

    std::cout << a << " , " << b << std::endl;

    a.resize(8, 5);

    std::cout << a << " , " << b << std::endl;

    a.resize(10, 5);

    std::cout << a << " , " << b << std::endl;

    return 0;
}
