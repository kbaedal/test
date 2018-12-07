#include <string>

#include "utiles.h"

#include "articulo.h"

articulo::articulo() :
    artpvp(13, 5),
    artdto1(13, 5),
    artdto2(13, 5),
    artdto3(13, 5),
    artdto4(13, 5),
    artdto5(13, 5),
    artpucosp(13, 5),
    artdtoc1(5, 2),
    artdtoc2(5, 2),
    artdtoc3(5, 2),
    artpcreal(13, 5),
    artpbenef(7, 2),
    artpvpiva(13, 5)
{
    uint8_t ne = 0b00001100; // Imprimir signo, imprimir todas las cifras.

    artpvp.set_status(ne);
    artdto1.set_status(ne);
    artdto2.set_status(ne);
    artdto3.set_status(ne);
    artdto4.set_status(ne);
    artdto5.set_status(ne);
    artpucosp.set_status(ne);
    artdtoc1.set_status(ne);
    artdtoc2.set_status(ne);
    artdtoc3.set_status(ne);
    artpcreal.set_status(ne);
    artpbenef.set_status(ne);
    artpvpiva.set_status(ne);
}

articulo::articulo(const articulo &a) : articulo() {
    if(this != &a) // Autoasignacion
        *this = a;
}

void articulo::put_data(const std::string &data, size_t pos)
{
    switch(pos) {
        case 0: procod = utiles::to_integer(data); break;
        case 1: artcod = utiles::to_integer(data); break;
        case 2: artrefe = data; break;
        case 3: artdesc = data; break;
        case 4: artfami = utiles::to_integer(data); break;
        case 5: artbarras = data; break;
        case 6: artmedi = data; break;
        case 7: artpvp = data; break;
        case 8: artdto1 = data; break;
        case 9: artdto2 = data; break;
        case 10: artdto3 = data; break;
        case 11: artdto4 = data; break;
        case 12: artdto5 = data; break;
        case 13: artpucosp = data; break;
        case 14: artdtoc1 = data; break;
        case 15: artdtoc2 = data; break;
        case 16: artdtoc3 = data; break;
        case 17: artpcreal = data; break;
        case 18: artpbenef = data; break;
        case 19: artpvpiva = data; break;
        case 20: artcodiva = utiles::to_integer(data); break;
        default: break;
    }
}

std::string articulo::get_data(size_t pos) const
{
    switch(pos) {
        case 0: return utiles::to_string(procod, 4, '0');
        case 1: return utiles::to_string(artcod, 6, '0');
        case 2: return artrefe;
        case 3: return artdesc;
        case 4: return utiles::to_string(artfami, 6, '0');
        case 5: return artbarras;
        case 6: return artmedi;
        case 7: return artpvp.to_str(',');
        case 8: return artdto1.to_str(',');
        case 9: return artdto2.to_str(',');
        case 10: return artdto3.to_str(',');
        case 11: return artdto4.to_str(',');
        case 12: return artdto5.to_str(',');
        case 13: return artpucosp.to_str(',');
        case 14: return artdtoc1.to_str(',');
        case 15: return artdtoc2.to_str(',');
        case 16: return artdtoc3.to_str(',');
        case 17: return artpcreal.to_str(',');
        case 18: return artpbenef.to_str(',');
        case 19: return artpvpiva.to_str(',');
        case 20: return utiles::to_string(artcodiva, 2, '0');
        default: break;
    }
    return std::string("");
}

stock_art::stock_art() :
    stock(10, 2)
{
    uint8_t ne = 0b00001100; // Imprimir signo, imprimir todas las cifras.

    stock.set_status(ne);
}

stock_art::stock_art(const stock_art &a) : stock_art() {
    if(this != &a) // Autoasignacion
        *this = a;
}

void stock_art::put_data(const std::string &data, size_t pos)
{
    switch(pos) {
        case 0: procod = utiles::to_integer(data); break;
        case 1: artcod = utiles::to_integer(data); break;
        case 2: almacen = utiles::to_integer(data); break;
        case 3: stock = data; break;
        default: break;
    }
}

std::string stock_art::get_data(size_t pos) const
{
    switch(pos) {
        case 0: return utiles::to_string(procod, 4, '0');
        case 1: return utiles::to_string(artcod, 6, '0');
        case 2: return utiles::to_string(almacen, 2, '0');
        case 3: return stock.to_str(',');
        default: break;
    }
    return std::string("");
}
