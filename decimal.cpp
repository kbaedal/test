#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "decimal.h"

namespace fpt {

decimal::decimal(unsigned int _c, unsigned int _d) :  ents(_c - _d), decs(_d), cifs(_c)
{
    if(cifs < 1)
        throw std::out_of_range("Numero de cifras incorrecto.");

    // Estado del decimal, por defecto todos los flags a 0.
    status = 0x00;

    // Necesitamos 4 bits por cifra, 1 byte por cada 2 cifras.
    // Si cifs es impar, añadimos un byte por el redondeo.
    long_buffer = ((cifs % 2) != 0) ? ((cifs/2) + 1) : (cifs/2);

    buffer      = new uint8_t[long_buffer];

    std::memset(buffer, 0x00, sizeof(uint8_t[long_buffer]));

    // Si tenemos un numero de cifras impar, ponemos el nybble superior
    // del ultimo byte a F, indicando que ese espacio no es usable.
    if((cifs % 2) != 0)
        buffer[long_buffer - 1] = 0xF0;
}

decimal::decimal(const decimal &d)
{
    cifs        = d.cifs;
    ents        = d.ents;
    decs        = d.decs;

    status      = d.status;

    long_buffer = d.long_buffer;

    buffer      = new uint8_t[long_buffer];

    std::memset(buffer, 0x00, sizeof(uint8_t[long_buffer]));
    std::memcpy(buffer, d.buffer, sizeof(uint8_t[long_buffer]));
}

decimal::~decimal()
{
    delete [] buffer;
}

decimal &decimal::operator=(const decimal &d)
{
    if(this != &d) {    // Autoasignacion
        decimal t(d);
        t.resize(cifs, decs);

        std::memcpy(buffer, t.buffer, sizeof(uint8_t[long_buffer]));

        if(t.is_negative())
            this->set_negative();
    }

    return *this;
}

decimal &decimal::operator=(const std::string &val)
{
    // Comprobamos que la cadena sea un numero, y la convertimos.
    // La funcion convertir llamará a resize(), que lanzará una
    // excepción si el nuevo contenido no cabe en el decimal.
    std::string tv = val;

    if(validar_cadena(tv, tv))
        convertir(tv);
    else
        throw std::invalid_argument("La cadena no contiene un numero valido.");

    return *this;
}

decimal &decimal::operator+=(const decimal &d)
{
    /*
     *  Suma:
     *
     *      S1
     *    + S2
     *     ----
     *       R
     *
     *  Dependiendo del signo de S1 y de S2, y de su valor absoluto, tendremos que
     *  realizar una operación u otra:
     *      - Si S1(+) y S2(+): R = abs(S1) + abs(S2), R(+)
     *      - Si S1(+) y S2(-):
     *          - Si abs(S1) >= abs(S2): R = abs(S1) - abs(S2), R(+).
     *          - Si abs(S1) < abs(S2): R = abs(S2) - abs(S1), R(-).
     *      - Si S1(-) y S2(+):
     *          - Si abs(S1) >= abs(S2): R = abs(S1) - abs(S2), R(-).
     *          - Si abs(S1) < abs(S2): R = abs(S2) - abs(S1), R(+).
     *      - Si S1(-) y S2(-): R = abs(S1) + abs(S2), R(-)
     *
     */

    // Cambiamos el tamaño del decimal a sumar. Con esto garantizamos dos cosas:
    //  1. Que la suma se pueda realizar (mismo numero de enteros y racionales).
    //  2. Que se ha redondeado correctamente.
    decimal s2(d);
    s2.resize(cifs, decs);

    if(!this->is_negative() && !s2.is_negative()) { // S1(+), S2(+)
        suma(s2);
    }
    else if(!this->is_negative() && s2.is_negative()) { // S1(+), S2(-)
        s2.set_positive(); // Ambos son positivos ahora.

        if(*this >= s2) {
            resta(s2);
        }
        else {
            s2.resta(*this);
            *this = s2;
            this->set_negative();
        }
    }
    else if(this->is_negative() && !s2.is_negative()) { // S1(-), S2(+)
        this->set_positive(); // Ambos positivos ahora.

        if(*this >= s2) {
            resta(s2);
            this->set_negative();
        }
        else {
            s2.resta(*this);
            *this = s2;
        }
    }
    else if(this->is_negative() && s2.is_negative()) { // S1(-), S2(-)
        // Ambos positivos.
        this->set_positive();
        s2.set_positive();

        suma(s2);
        this->set_negative();
    }

    // Comprobamos que no nos hayamos salido de rango.
    if(this->get_cifra(cifs-1) > 9)
        throw std::out_of_range("La suma ha producido desbordamiento.");

    return *this;
}


decimal &decimal::operator-=(const decimal &d)
{
    /*
     *  Resta:
     *
     *      M inuendo
     *    - S ustraendo
     *     ---
     *      R esultado
     *
     *  Dependiendo del signo de M y de S, y de su valor absoluto, tendremos que
     *  realizar una operación u otra:
     *      - Si M(+) y S(+):
     *          - Si abs(M) >= abs(S) R = abs(M) - abs(S), R(+).
     *          - Si abs(M) < abs(S) R = abs(S) - abs(M), R(-).
     *      - Si M(+) y S(-): R = abs(M) + abs(S), R(+).
     *      - Si M(-) y S(+): R = abs(M) + abs(S), R(-).
     *      - Si M(-) y S(-):
     *          - Si abs(M) >= abs(S) R = abs(M) - abs(S), R(-).
     *          - Si abs(M) < abs(S) R = abs(S) - abs(M), R(+).
     *
     */

    // Cambiamos el tamaño del decimal a restar. Con esto garantizamos dos cosas:
    //  1. Que la resta se pueda realizar (mismo numero de enteros y decimales).
    //  2. Que se haya redondeado el decimal correctamente.
    decimal ss(d);
    ss.resize(cifs, decs);

    if(!this->is_negative() && !ss.is_negative()) { // M(+), S(+)
        if(*this >= ss) {
            resta(ss);
        }
        else {
            ss.resta(*this);
            *this = ss;
            this->set_negative();
        }
    }
    else if(!this->is_negative() && ss.is_negative()) { // M(+), S(-)
        ss.set_positive(); // Ambos positivos.

        suma(ss);
    }
    else if(this->is_negative() && !ss.is_negative()) { // M(-), S(+)
        this->set_positive(); // Ambos positivos ahora.

        suma(ss);
        this->set_negative();
    }
    else if(this->is_negative() && ss.is_negative()) { // M(-), S(-)
        // Los hacemos positivos.
        this->set_positive();
        ss.set_positive();

        if(*this >= ss) {
            resta(ss);
            this->set_negative();
        }
        else {
            ss.resta(*this);
            *this = ss;
        }
    }

    // Comprobamos que no nos hayamos salido de rango.
    if(this->get_cifra(cifs - 1) > 9)
        throw std::out_of_range("La resta ha producido desbordamiento.");

    return *this;
}

void decimal::suma(const decimal &sum)
{
    // Por cada cifra del decimal, empezando por el final:
    //  1. Sumamos ambos numeros.
    //  2. Si la suma pasa de 10 acarreamos para el siguiente par y restamos 10 a este par.

    bool acarreo = false;

    for(int i = 0; i < cifs; ++i) {
        uint8_t op1 = this->get_cifra(i);
        uint8_t op2 = sum.get_cifra(i);
        uint8_t op3 = op1 + op2;

        if(acarreo) ++op3;

        if(op3 >= 10) {
            acarreo = true;
            op3 -= 10;
        }
        else {
            acarreo = false;
        }

        // Control para el overflow.
        if(acarreo && (i == cifs - 1))
            this->set_cifra(10, i);
        else
            this->set_cifra(op3, i);
    }
}

void decimal::resta(const decimal &res)
{
    // Por cada cifra del decimal, empezando por el final:
    //  1. Comprobamos si operador 1 es mayor o igual operador 2.
    //  2. Si lo es, restamos. Si no, añadimos 10, indicando acarreo para el siguiente, y restamos.

    bool acarreo = false;

    for(int i = 0; i < cifs; ++i) {
        uint8_t op1 = this->get_cifra(i);
        uint8_t op2 = res.get_cifra(i);

        if(acarreo) ++op2;

        if(op1 < op2) {
            op1 += 10;
            acarreo = true;
        }
        else {
            acarreo = false;
        }

        int op3 = op1 - op2;

        this->set_cifra(op3, i);
    }
}

decimal &decimal::operator*=(const decimal &d)
{

    // Declaramos un temporal que tendrá el doble de cifras
    // que los numeros que estamos multiplicando. Cambiaremos
    // el tamaño del multiplicador si es necesario para que
    // coincida con el del multiplicando.
    // Y la multiplicacion se reduce a ir haciendo multiplicaciones
    // parciales y acumulando el resultado, siempre controlando
    // en que lugar colocamos el resultado.

    uint8_t res     = 0,
            acarreo = 0;

    decimal  mtdr(d),                    // Multiplicador, para poder cambiar tamaño.
            t1(2 * cifs, 2 * decs),     // Temporal donde colocaremos las multiplicaciones parciales.
            t2(2 * cifs, 2 * decs);     // Acumulador. Sumamos aquí las multiplicaciones.

    bool res_negativo = (this->is_negative() != d.is_negative());

    mtdr.resize(cifs, decs);

    for(int i = 0; i < cifs; ++i) {     // Por cada cifra del multiplicando
        t1 = "0";
        if(this->get_cifra(i) != 0) {
            for(int j = 0; j < cifs; ++j) { // Multiplicamos por cada cifra del multiplicador
                res = this->get_cifra(i) * mtdr.get_cifra(j);

                res += acarreo;
                acarreo = 0;

                if(res >= 10) {
                    acarreo = (res / 10) % 10;
                    res = res % 10;
                }

                t1.set_cifra(res, i + j);
            }
            t2 += t1;   // Acumulamos resultados.
        }
    }

    // Cambiamos de tamaño para acomodarlo.
    t2.resize(cifs, decs);
    // Y copiamos el resultado.
    *this = t2;

    if(res_negativo)
        this->set_negative();
    else
        this->set_positive();

    return *this;
}

decimal decimal::inverse() const
{
    // Método Newton-Raphson:
    //  - Obtener una estimacion x(0) de 1/decimal, para lo que calculamos
    //    estimaciones cada vez más precisas de x(1), x(2), ..., x(s).
    //  - Para aplicar el metodo se necesita una funcion f(x) que sea 0 cuando
    //    x = 1/decimal. En este caso, f(x) = (1/x) - d. Para esta funcion,
    //    el método Newton-Raphson resuelve:
    //
    //                        f(x(i))
    //      x(i+1) = x(i) - ----------
    //                       f'(x(i))
    //    Sustituyendo:
    //
    //                        1/x(i) - d
    //      x(i+1) = x(i) - ------------- = x(i) + x(i) * (1 - d * x(i))
    //                       -1/(x(i)^2)
    //
    //    Luego:
    //
    //      x(i+1) = x(i) * (2 - d * x(i))
    //
    //    Repetimos el cálculo hasta que 1-p < d * x < 1+p, donde p es la
    //    precision que queremos en el cálculo, que debería ser la máxima
    //    posible. Para este caso, le daremos el valor del decimal más pequeño
    //    que se pueda almacenar. Añadimos un control para el caso de que
    //    la inversa no produzca el resultado esperado debido a la falta
    //    de redondeo de posiciones inferiores, que no calculamos.
    //
    //  - Codigo:
    //
    //      p = PRECISION;
    //      x = p;
    //      r = 0;
    //      d = numero;
    //      while((x * d) > 1 + p || (x * d) < 1 - p) {
    //          r = x * (2 - d*x);
    //          if(r == x)
    //              break;
    //          else
    //              x = r;
    //      }
    //
    unsigned int    te = ents,
                    td = decs,
                    tc = cifs;

    int             i = 2;  // Veces que aumentaremos racionales para mejorar la
                            // precision. Equivale a añadir 4 decimales para mejorar
                            // el cálculo. A partir de aquí, las pérdidas de precision
                            // las consideramos insignificantes, aumentar decimales
                            // no va a producir un resultado mejor.

    decimal  p(tc, td),  // Precision deseada.
            x(tc, td),  // Resultado.
            r(tc, td),  // Variable para controlar el resultado.
            c(tc, td);  // Almacenamos el contenido actual del decimal.

    // Le damos a p el menor valor posible.
    p = p.min();

    // E inicializamos el resultado con este valor.
    x = p;

    // Y la variable de control a 0.
    r = r.zero();

    c = *this;

    // Aplicamos Newton-Raphson, y repetimos hasta i veces para mejorar la precision.
    while(--i) {
        while((x * c) > ("1.0" + p) || (x * c) < ("1.0" - p)) {
            r = x * ("2.0" - c * x);

            // Comprobamos si hemos alcanzado el valor más próximo posible
            // a la solución correcta. Para obtener un resultado más preciso
            // deberíamos aumentar el número de cifras decimales.
            if(r == x) {
                break;
            }
            else
                x = r;
        }

        // Puede ser que al redondear no obtengamos el mejor resultado posible,
        // así que aumentamos dos cifras decimales y repetimos el cálculo.
        if((x * c) == "1.0")
            break;
        else {
            tc += 2;
            td += 2;

            p.resize(tc, td);
            p = p.min();

            x.resize(tc, td);
            r.resize(tc, td);
            c.resize(tc, td);
        }
    }

    return r;
}

decimal &decimal::operator/=(const decimal &d)
{
    // Utilizaremos un decimal con el  doble de tamaño del original
    // para asegurar la precisión del cáculo.
    // No dividimos, calculamos el inverso multiplicativo de d,
    // y lo multiplicamos por el decimal.

    if(d.abs() == d.zero()) {
        throw std::invalid_argument("Division entre 0.");
    }
    else {
        int ne  = (ents != 0) ? ents * 2 : 2;
        int nd  = (decs != 0) ? decs * 2 : 2;
        int nc  = ne + nd;

        decimal r(nc, nd),
               n(nc, nd);

        bool res_negativo = (this->is_negative() != d.is_negative());

        n = *this;
        r = d;

        n.set_positive();
        r.set_positive();

        r = r.inverse();

        *this = n * r;

        if(res_negativo)
            this->set_negative();

        return *this;
    }
}

void decimal::resize(unsigned int nc, unsigned int nd)
{
    // Proceso:
    //  1. Comprobar el numero de enteros. Si es mayor que el
    //      que tenemos, no hay problema. En caso contrario,
    //      comprobar el desboradamiento.
    //  2. Para los decimales, simplemente tendremos que tener
    //      cuidado con el redondeo.

    if(nd > nc)
        throw std::invalid_argument("Numero de decimales incompatible con numero de cifras.");

    unsigned int    ne  = nc - nd;  // Nuevo numero de enteros.

    // Comprobamos que realmente haya que cambiar el tamaño.
    if((ne != ents) || (nd != decs)) {
        decimal  t(nc, nd);      // Temporal para operaciones.

        // Comprobamos que las cifras por encima de lo que podemos almacenar sean 0.
        if(ne < ents)
            for(unsigned int i = decs + nc - nd; i < decs + ents; ++i)
                if(get_cifra(i) != 0)
                    throw std::out_of_range("El cambio de tamanio ha provocado desbordamiento (enteros).");

        // Llegados aquí, sabemos que el contenido cabe en el nuevo tamaño, así pues,
        // después, tras comprobar los decimales y el posible acarreo, podremos copiar
        // sin riesgo.

        // Ahora los decimales. Para ello habrá que comprobar si hemos de redondear.
        if(nd < decs) {
            // Tenemos que reducir el numero de decimales. Comprobamos los que tenemos
            // que eliminar, para controlar el acarreo.
            bool    acarreo = false;

            for(unsigned int i = 0; i < decs - nd; ++i) {
                // Desde el ultimo decimal del viejo número, hasta el anterior
                // al que tenemos que copiar, comprobamos si se produce acarreo.
                uint8_t x = this->get_cifra(i);

                if(acarreo)
                    ++x;

                if(x > 4)
                    acarreo = true;
                else
                    acarreo = false;
            }

            // Si se ha producido acarreo, lo añadimos a la hora de copiar el
            // contenido del viejo tamaño al nuevo tamaño.
            for(unsigned int k = 0, l = decs - nd; k < nc; ++k, ++l) {
                uint8_t x = 0;

                if(l < cifs)
                    x = this->get_cifra(l);

                if(acarreo)
                    ++x;

                if(x > 9) {
                    acarreo = true;
                    x -= 10;
                }
                else {
                    acarreo = false;
                }

                t.set_cifra(x, k);
            }

            // Si la última cifra válida del número es mayor de nueve, entonces
            // se ha producido un desbordamiento.
            if(t.get_cifra(nc - 1) > 9)
                throw std::out_of_range("El cambio de tamanio ha provocado desbordamiento (completo).");
        }
        else { // No se puede producir desbordamiento por redondeo. Copiamos.
            for(unsigned int i = 0, j = nd - decs; (i < cifs) && (j < nc); ++i, ++j)
                t.set_cifra(this->get_cifra(i), j);
        }

        // Actualizamos los datos.
        ents        = ne;
        decs        = nd;
        cifs        = nc;
        long_buffer = t.long_buffer;

        delete [] buffer;
        buffer = new uint8_t[long_buffer];

        std::memset(buffer, 0x00, sizeof(uint8_t[long_buffer]));
        std::memcpy(buffer, t.buffer, sizeof(uint8_t[long_buffer]));
    }
}

void decimal::convertir(const std::string &str)
{
    // Crearemos un decimal temporal con las caracterísiticas
    // del que se ha pasado por cadena. Una vez convertido,
    // cambiamos el tamaño para acomodarlo a las características
    // del decimal y asignamos.

    std::string t = str;
    bool        neg = false;
    size_t      pos_punto;
    uint8_t     lonybble, hinybble;
    int         ne = 0,
                nd = 0,
                i = 0;

    // Comprobamos el signo.
    if(t[0] == '-') {
        neg = true;
        t.erase(0, 1);
    }
    else
        neg = false;

    // Buscamos el punto.
    pos_punto = t.find_first_of('.');

    if(pos_punto != std::string::npos) {
        // Tenemos punto, contamos enteros y racionales.
        ne = pos_punto;
        nd = t.size() - pos_punto - 1;

        // Y lo eliminamos.
        t.erase(pos_punto, 1);
    }
    else {
        // No tenemos punto, todo enteros.
        ne = t.size();
    }

    // Creamos un decimal temporal con estos datos.
    decimal x(ne+nd, nd);

    // Sacamos los caracteres de la cadena por pares, los traducimos
    // a bcd, y los colocamos en su sitio.
    while(t.size() > 1) { // Mientras que tengamos al menos 2 caracteres.
        lonybble = char_to_bcd(t.back(), false);
        t.pop_back();
        hinybble = char_to_bcd(t.back(), true);
        t.pop_back();

        x.buffer[i] = hinybble | lonybble;

        ++i;
    }

    if(t.size() != 0) {
        // El último número de la cadena.
        lonybble = char_to_bcd(t.back(), false);
        t.pop_back();
        hinybble = 0xFF;

        x.buffer[x.long_buffer-1] = hinybble & lonybble;
    }

    // Cambiamos al temporal al tamaño que necesitamos.
    x.resize(cifs, decs);
    *this = x;

    if(neg)
        this->set_negative();
    else
        this->set_positive();
}

bool decimal::validar_cadena(const std::string &str, std::string &t)
{
    const char  *ws = " \t\n\r\f\v"; // Caracteres a eliminar.

    t = str;

    // Eliminamos espacios, tabuladores, retornos y otros caracteres
    // especiales, tanto del principio como del final de la cadena.
    t.erase(t.find_last_not_of(ws) + 1);
    t.erase(0, t.find_first_not_of(ws));

    // Comprobamos que solo contenga números, signos más o menos, y puntos.
    size_t valid_chars = t.find_first_not_of("+-0123456789.");

    // Contamos el número de puntos. Como máximo habrá uno.
    size_t num_points = std::count(t.begin(), t.end(), '.');

    // Averiguamos la posición del signo menos, si lo hay.
    size_t pos_minus = t.find_first_of('-');

    // Averiguamos la posición del signo más, si lo hay.
    size_t pos_plus = t.find_first_of('+');

    // Si todos son caracteres válidos, hay como máximo un punto, y si
    // hay un menos o un mas al principio, tenemos un numero válido.
    return (valid_chars == std::string::npos) && (num_points <= 1) && (
        (pos_minus == 0 && pos_plus == std::string::npos) || // Hay un signo menos, y no hay signo más.
        (pos_plus == 0 && pos_minus == std::string::npos) || // Hay un singo más, y no hay signo menos.
        ((pos_minus == std::string::npos) && (pos_plus == std::string::npos)) // No hay signo alguno.
    );
}

std::string decimal::to_str(bool format) const
{
    std::string t;

    for(unsigned int i = 0; i < cifs; ++i) {
        if(i == decs && decs != 0)
            t.insert(0, 1, '.');

        t.insert(0, 1, bcd_to_char(this->get_cifra(i), false));
    }

    // Para formatear el numero, eliminamos los 0 a las izquiera.
    if(format) {
        t.erase(0, t.find_first_not_of("0"));

        // Si no tenemos ningún entero, al menos dejamos un 0.
        if(t[0] == '.')
            t.insert(0, 1, '0');
    }

    if(this->is_negative())
        t.insert(0, 1, '-');

    return t;
}

bool operator==(const decimal &a, const decimal &b)
{
    // 1. Comparamos signos. Si son diferentes devolvemos falso.
    // 2. Cambiamos el tamaño de b para que coincida con el de a.
    // 3. Comparamos cifra a cifra.

    if(a.is_negative() != b.is_negative()) return false;

    decimal t(b);
    t.resize(a.cifs, a.decs);

    for(int i = 0; i < static_cast<int>(a.long_buffer); ++i)
        if(a.buffer[i] != t.buffer[i])
            return false;

    return true;
}

bool operator==(const decimal &a, const std::string &b)
{
    decimal t(a.cifs, a.decs);

    t = b;

    return a == t;
}

bool operator>=(const decimal &a, const decimal &b)
{
    // 1. Comparamos signos. Si son distintos, el positivo es el mayor.
    // 2. Cambiamos el tamaño de b para que coincida con el de a.
    // 3. Comparamos cifra a cifra y devolvemos en consecuencia.

    if(a.is_negative() != b.is_negative())
        return !a.is_negative();

    decimal t(b);
    t.resize(a.cifs, a.decs);

    bool    a_mayor = false,
            iguales = true;

    for(int i = a.cifs - 1; i >= 0; --i) {
        if(a.get_cifra(i) != t.get_cifra(i)) {
            iguales = false;
            if(a.get_cifra(i) > t.get_cifra(i)) {
                a_mayor = true;
            }
            break;
        }
    }

    if(iguales)
        return true;

    // Invertimos el resultado si a y b son negativos.
    return a.is_negative() ? !a_mayor : a_mayor;
}

bool operator<=(const decimal &a, const decimal &b)
{
    // 1. Comparamos signos. Si son distintos, el negativo es el menor.
    // 2. Cambiamos el tamaño de b para que coincida con el de a.
    // 3. Comparamos cifra a cifra, y devolvemos en consecuencia.

    if(a.is_negative() != b.is_negative())
        return a.is_negative();

    decimal t(b);
    t.resize(a.cifs, a.decs);

    bool    a_menor = false,
            iguales = true;

    for(int i = a.cifs - 1; i >= 0; --i) {
        if(a.get_cifra(i) != t.get_cifra(i)) {
            iguales = false;
            if(a.get_cifra(i) < t.get_cifra(i)) {
                a_menor = true;
            }
            break;
        }
    }

    if(iguales)
        return true;

    // Invertimos el resultado si a y b son negativos.
    return a.is_negative() ? !a_menor : a_menor;
}

decimal decimal::abs() const
{
    decimal t(*this);

    t.set_positive();

    return t;
}

decimal decimal::max() const
{
    // Todo a 9, signo positivo.

    decimal t(cifs, decs);

    for(unsigned int i = 0; i < t.cifs; ++i)
        t.set_cifra(9, i);

    return t;
}

decimal decimal::min() const
{
    // Todo a 0 excepto última cifra. Signo positivo.

    decimal t(cifs, decs);

    std::memset(t.buffer, 0x00, sizeof(uint8_t[t.long_buffer]));

    t.set_cifra(1, 0);

    return t;
}

decimal decimal::zero() const
{
    decimal t(cifs, decs);

    return t;
}

std::string decimal::int_to_s(const int &n, int width) const
{
    std::stringstream ss;

    if(width > 0)
        ss << std::setfill('0') << std::setw(width) << n;
    else
        ss << n;

    return ss.str();
}

int decimal::s_to_int(const std::string &s) const
{
    int res;

    std::stringstream is(s);

    is >> res;

    return res;
}

uint8_t decimal::char_to_bcd(char cifra, bool high) const
{
    int shift = high ? 4 : 0;

    switch(cifra) {
        case '0': return (0x00 << shift);
        case '1': return (0x01 << shift);
        case '2': return (0x02 << shift);
        case '3': return (0x03 << shift);
        case '4': return (0x04 << shift);
        case '5': return (0x05 << shift);
        case '6': return (0x06 << shift);
        case '7': return (0x07 << shift);
        case '8': return (0x08 << shift);
        case '9': return (0x09 << shift);
        default: return 0xFF;
    }
}

char decimal::bcd_to_char(uint8_t cifra, bool high) const
{
    // Si es necesario, desplazamos los bits.
    cifra = high ? cifra >> 4 : cifra;

    switch(cifra & 0x0F) { // Solo tenemos en cuenta el nybble inferior.
        case 0x00 : return '0';
        case 0x01 : return '1';
        case 0x02 : return '2';
        case 0x03 : return '3';
        case 0x04 : return '4';
        case 0x05 : return '5';
        case 0x06 : return '6';
        case 0x07 : return '7';
        case 0x08 : return '8';
        case 0x09 : return '9';
        default: return 'X';
    }
}

uint8_t decimal::get_cifra(unsigned int pos) const
{
    uint8_t res;

    if(pos >= cifs)
        throw std::out_of_range("Imposible devolver la cifra.");

    if((pos % 2) != 0) {
        // Posicion impar, devolvemos el nybble alto del byte correspondiente.
        res = buffer[pos/2] & 0xF0;
        res = res >> 4;
    }
    else {
        // Posicion par, nybble bajo.
        res = buffer[pos/2] & 0x0F;
    }

    return res;
}

void decimal::set_cifra(uint8_t val, unsigned int pos)
{
    if(pos >= cifs) {
        std::string exc_msg = "Imposible colocar la cifra. (val, pos): " + int_to_s(val) + ", " + int_to_s(pos);

        throw std::out_of_range(exc_msg);
    }

    if((pos % 2) != 0) {
        // Posicion impar, colocamos el valor en el nybble alto del byte correspondiente.
        val &= 0x0F;                // Solo los 4 bits del nybble inferior.
        val = val << 4;             // Datos al nybble superior.
        buffer[pos/2] &= 0x0F;      // Limpiamos posicion.
        buffer[pos/2] |= val;       // Colocamos cifra.
    }
    else {
        val &= 0x0F;                // Solo los 4 bits del nybble inferior.
        buffer[pos/2] &= 0xF0;      // Limpiamos posicion.
        buffer[pos/2] |= val;       // Colocamos la cifra.
    }
}

void decimal::print_buffer(std::string encab) const
{
    std::cout << encab << "Buffer[ ";
    for(int i = long_buffer - 1; i >= 0; --i) {
        print_bcd(this->buffer[i], '-');
        std::cout  << " ";
    }

    std::cout << "]\n";
}

void decimal::print_bcd(uint8_t val, char sep) const
{
    std::bitset<8> x(val);

    std::cout << x.to_string().substr(0, 4) << sep << x.to_string().substr(4, 4);
}

}; // namespace fpt
