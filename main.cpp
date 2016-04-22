#include <iostream>
#include <string>

//#include <mysql.h>

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
    fpt::decimal a(12, 0);
    fpt::decimal b(12, 0);

    a = "321";
    b = "123";

    fpt::bcddec c(10, 5);
    fpt::bcddec d(10, 5);

    c = "4.123";
    d = "1.234";

    std::cout << c.to_str() << " * " << d.to_str() << " = " << (c*d).to_str() << "\n";
    // std::cout << a.to_str() << " - " << b.to_str() << " = " << (a-b).to_str() << "\n";
    // std::cout << a.to_str() << " * " << b.to_str() << " = " << (a*b).to_str() << "\n";
    // std::cout << a.to_str() << " / " << b.to_str() << " = " << (a/b).to_str() << "\n";

    /*
    std::cout << "max(a) :  " << a.max().to_str() << "\n";
    std::cout << "min(a) :  " << a.min().to_str() << "\n";
    std::cout << "abs(a) :  " << a.abs().to_str() << "\n";
    std::cout << "zero(a) : " << a.zero().to_str() << "\n";
    std::cout << "-a:       " << (-a).to_str() << "\n";
    */

    return 0;
}
