#ifndef ARTICULO_H_INCLUDED
#define ARTICULO_H_INCLUDED

#include "decimal.h"

struct articulo {               //  POS
    int             procod,     //  0
                    artcod;     //  1

    std::string     artrefe,    //  2
                    artdesc;    //  3

    int             artfami;    //  4

    std::string     artbarras,  //  5
                    artmedi;    //  6

    fpt::decimal    artpvp,     //  7
                    artdto1,    //  8
                    artdto2,    //  9
                    artdto3,    //  10
                    artdto4,    //  11
                    artdto5,    //  12
                    artpucosp,  //  13
                    artdtoc1,   //  14
                    artdtoc2,   //  15
                    artdtoc3,   //  16
                    artpcreal,  //  17
                    artpbenef,  //  18
                    artpvpiva;  //  19

    int             artcodiva;  //  20

    articulo();
    articulo(const articulo &a);

    void put_data(const std::string &data, size_t pos);
    std::string get_data(size_t pos) const;
};

struct stock_art {              // POS
    int             procod,     // 0
                    artcod,     // 1
                    almacen;    // 2

    fpt::decimal    stock;      // 3

    stock_art();
    stock_art(const stock_art &a);

    void put_data(const std::string &data, size_t pos);
    std::string get_data(size_t pos) const;
};

#endif // ARTICULO_H_INCLUDED
