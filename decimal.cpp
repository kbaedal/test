#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "decimal.h"

namespace fpt {

decimal::decimal(unsigned int _c, unsigned int _d) : ents(_c - _d), decs(_d)
{
    // Reservamos 8 bits por cada dos cifras.
    long_ents   = ents != 0 ? static_cast<int>((ents - 1)/2) + 1 : 0;
    long_decs   = decs != 0 ? static_cast<int>((decs - 1)/2) + 1 : 0;
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
    if(this != &d) {
        decimal t(d);
        t.resize(ents + decs, decs);

        std::memcpy(buffer, t.buffer, sizeof(uint8_t[long_buffer]));
    }

    return *this;
}

decimal &decimal::operator=(const std::string &val)
{
    // Comprobamos que la cadena sea un numero, y la convertimos.
    // La funcion convertir llamar� a resize(), que lanzar� una
    // excepci�n si el nuevo contenido no cabe en el decimal.
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
     *  realizar una operaci�n u otra:
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

    unsigned int max_val_e = ((ents % 2) != 0) ? 9 : 99;

    // Cambiamos el tama�o del decimal a sumar. Con esto garantizamos dos cosas:
    //  1. Que la suma se pueda realizar (mismo numero de enteros y decimales).
    //  2. Que se haya redondeado el decimal correctamente.
    decimal s2(d);
    s2.resize(ents + decs, decs);

    if(!this->is_negative() && !s2.is_negative()) { // S1(+), S2(+)
        suma(s2.buffer);
    }
    else if(!this->is_negative() && s2.is_negative()) { // S1(+), S2(-)
        s2.set_positive(); // Ambos son positivos ahora.

        if(*this >= s2) {
            resta(s2.buffer);
        }
        else {
            s2.resta(buffer);
            *this = s2;
            this->set_negative();
        }
    }
    else if(this->is_negative() && !s2.is_negative()) { // S1(-), S2(+)
        this->set_positive(); // Ambos positivos ahora.

        if(*this >= s2) {
            resta(s2.buffer);
            this->set_negative();
        }
        else {
            s2.resta(buffer);
            *this = s2;
        }
    }
    else if(this->is_negative() && s2.is_negative()) { // S1(-), S2(-)
        // Ambos positivos.
        this->set_positive();
        s2.set_positive();

        suma(s2.buffer);
        this->set_negative();
    }

    // Comprobamos que no nos hayamos salido de rango.
    if((buffer[0] & 0x7F) > max_val_e)
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
     *  realizar una operaci�n u otra:
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

    unsigned int max_val_e = ((ents % 2) != 0) ? 9 : 99;

    // Cambiamos el tama�o del decimal a restar. Con esto garantizamos dos cosas:
    //  1. Que la resta se pueda realizar (mismo numero de enteros y decimales).
    //  2. Que se haya redondeado el decimal correctamente.
    decimal ss(d);
    ss.resize(ents + decs, decs);

    if(!this->is_negative() && !ss.is_negative()) { // M(+), S(+)
        if(*this >= ss) {
            resta(ss.buffer);
        }
        else {
            ss.resta(buffer);
            *this = ss;
            this->set_negative();
        }
    }
    else if(!this->is_negative() && ss.is_negative()) { // M(+), S(-)
        ss.set_positive(); // Ambos positivos.

        suma(ss.buffer);
    }
    else if(this->is_negative() && !ss.is_negative()) { // M(-), S(+)
        this->set_positive(); // Ambos positivos ahora.

        suma(ss.buffer);
        this->set_negative();
    }
    else if(this->is_negative() && ss.is_negative()) { // M(-), S(-)
        // Los hacemos positivos.
        this->set_positive();
        ss.set_positive();

        if(*this >= ss) {
            resta(ss.buffer);
            this->set_negative();
        }
        else {
            ss.resta(buffer);
            *this = ss;
        }
    }

    // Comprobamos que no nos hayamos salido de rango.
    if((buffer[0] & 0x7F) > max_val_e)
        throw std::out_of_range("La resta ha producido desbordamiento.");

    return *this;
}

void decimal::suma(const uint8_t *sum)
{
    // Por cada pareja de numeros (posicion del buffer), empezando por el final:
    //  1. Sumamos ambos numeros.
    //  2. Si las suma pasa de 100 acarreamos para el siguiente par y restamos 100 a este par.

    bool acarreo = false;

    for(int i = long_buffer - 1; i > 0; --i) {
        uint8_t op1 = buffer[i];
        uint8_t op2 = sum[i];
        uint8_t op3 = op1 + op2;

        if(acarreo) ++op3;

        if(op3 >= 100) {
            acarreo = true;
            op3 -= 100;
        }
        else {
            acarreo = false;
        }

        buffer[i] = op3;
    }
 }

void decimal::resta(const uint8_t *res)
{
    // Por cada pareja de numeros (posicion del buffer), empezando por el final:
    //  1. Comprobamos si operador 1 es mayor o igual operador 2.
    //  2. Si lo es, restamos. Si no, a�adimos 100, indicando acarreo para el siguiente, y restamos.

    bool acarreo = false;

    for(int i = long_buffer - 1; i > 0; --i) {
        uint8_t op1 = buffer[i];
        uint8_t op2 = res[i];

        if(acarreo) ++op2;

        if(op1 < op2) {
            op1 += 100;
            acarreo = true;
        }
        else {
            acarreo = false;
        }

        int op3 = op1 - op2;

        buffer[i] = op3;
    }
}

decimal &decimal::operator*=(const decimal &d)
{
    // Declaramos un temporal que tendr� siempre un numero
    // par de cifras, y que ser� como minimo igual de grande
    // que el decimal original. As� garantizamos que las
    // operaciones de multiplicacion se realizan siempre
    // sobre n�meros de dos cifras, y se almacenan correctamente
    // en el buffer temporal.
    int te  = (ents % 2) != 0 ? ents + 1 : ents;
    int td  = (decs % 2) != 0 ? decs + 1 : decs;
    int tlb = long_buffer * 2;
    decimal r((te + td) * 2, td * 2);

    // Y un buffer donde almacenar nuestras multiplicaciones.
    uint8_t *res_mult = new uint8_t[tlb];
    std::memset(res_mult, 0x00, sizeof(uint8_t[tlb]));

    // Hacemos que el multiplicador sea del mismo tama�o.
    decimal t(d);
    t.resize(ents+decs, decs);

    // Calculamos el signo del resultado.
    // Negativo si signos diferentes, positivo si iguales.
    bool res_negativo = (this->is_negative() != t.is_negative());

    // Hacemos que ambos sean positivos.
    this->set_positive();
    t.set_positive();

    // Multiplicamos por parejas y sumamos, colocando el resultado
    // en el lugar adecuado.
    for(int i = long_buffer - 1; i >= 0; --i) {
        for(int j = long_buffer -1; j >= 0; --j) {
            // Limpiamos nuestro buffer temporal.
            std::memset(res_mult, 0x00, sizeof(uint8_t[tlb]));

            // Multiplicamos la pareja.
            // Sabemos que el valor m�ximo de cada par es 99, por lo que
            // el resultado m�ximo posible es 9801.
            int m   = static_cast<int>(buffer[i]);
            int n   = static_cast<int>(t.buffer[j]);
            int mxn = m * n;

            // Posicion a escribir en el buffer temporal.
            int x = j + i + 1;

            // En cada byte, su valor correspondiente.
            uint8_t l_par = static_cast<uint8_t>(mxn % 10 + ((mxn / 10) % 10) * 10);
            uint8_t h_par = static_cast<uint8_t>((mxn / 100) % 10 + ((mxn / 1000) % 10) * 10);

            // Los colocamos en su lugar correspondiente del buffer.
            res_mult[x]     = l_par;
            res_mult[x - 1] = h_par;

            // Sumamos al resultado.
            r.suma(res_mult);
        }
    }

    r.resize(ents + decs, decs);
    *this = r;

    if(res_negativo)
        this->set_negative();

    delete [] res_mult;

    return *this;
}

decimal decimal::inverse() const
{
    // M�todo Newton-Raphson:
    //  - Obtener una estimacion x(0) de 1/decimal, para lo que calculamos
    //    estimaciones cada vez m�s precisas de x(1), x(2), ..., x(s).
    //  - Para aplicar el metodo se necesita una funcion f(x) que sea 0 cuando
    //    x = 1/decimal. En este caso, f(x) = (1/x) - d. Para esta funcion,
    //    el m�todo Newton-Raphson resuelve:
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
    //    Repetimos el c�lculo hasta que 1-p < d * x < 1+p, donde p es la
    //    precision que queremos en el c�lculo, que deber�a ser la m�xima
    //    posible. Para este caso, le daremos el valor del decimal m�s peque�o
    //    que se pueda almacenar. A�adimos un control para el caso de que
    //    la inversa no produzca el resultado esperado debido a la falta
    //    de redondeo de posiciones inferiores, que no calculamos.
    //
    //  - Codigo:
    //
    //      p = PRECISION;
    //      x = p;
    //      r = 0;
    //      while((x * d) > 1 + p || (x * d) < 1 - p) {
    //          r = x * (2 - d*x);
    //          if(r == x)
    //              break;
    //          else
    //              x = r;
    //      }
    //

    unsigned int    te = ents,
                    td = decs == 0 ? static_cast<int>(ents/2+1) : decs,
                    tc = te + td;

    int             i = 2;  // Veces que aumentaremos decimales para mejorar la
                            // precision. Equivale a a�adir 4 decimales para mejorar
                            // el c�lculo. A partir de aqu�, las p�rdidas de precision
                            // las consideramos insignificantes, aumentar decimales
                            // no va a producir un resultado mejor.

    decimal p(tc, td),  // Precision deseada.
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
            std::cerr << "p: " << p.to_str() << "\n";
            std::cerr << "x: " << x.to_str() << "\n";
            std::cerr << "r: " << r.to_str() << "\n";
            std::cerr << "c: " << c.to_str() << "\n";

            r = x * ("2.0" - c * x);

            // Comprobamos si hemos alcanzado el valor m�s pr�ximo posible
            // a la soluci�n correcta. Para obtener un resultado m�s preciso
            // deber�amos aumentar el n�mero de cifras decimales.
            if(r == x) {
                break;
            }
            else
                x = r;
        }

        // Puede ser que al redondear no obtengamos el mejor resultado posible,
        // as� que aumentamos dos cifras decimales y repetimos el c�lculo.
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
    // Utilizaremos un decimal con un numero par de cifras y
    // el doble de tama�o del original para asegurar la precisi�n
    // del c�culo.

    std::cerr << "Division: " << this->to_str() <<  " / " << d.to_str() << "\n";

    if(d.abs() == d.zero()) {
        throw std::invalid_argument("Division entre 0.");
    }
    else {
        int te  = (ents % 2) != 0 ? ents + 1 : ents;
        int td  = (decs % 2) != 0 ? decs + 1 : decs;
        int tc  = te + td;

        std::cerr << "1. (ents, decs, te, td, tc): ";
        std::cerr << int_to_s(ents) << ", ";
        std::cerr << int_to_s(decs) << ", ";
        std::cerr << int_to_s(te) << ", ";
        std::cerr << int_to_s(td) << ", ";
        std::cerr << int_to_s(tc) << "\n";

        decimal r((tc) * 2, td * 2),
                n((tc) * 2, td * 2);

        bool res_negativo = (this->is_negative() != d.is_negative());

        n = *this;
        r = d;

        n.set_positive();
        r.set_positive();

        r = r.inverse();

        std::cerr << "(n * r): " << n.to_str() <<  " * " << r.to_str() << "\n";

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

    // Comprobamos que realmente haya que cambiar el tama�o.

    if((nc != ents + decs) || (nd != decs)) {
        if((nc - nd) < 0)
            throw std::invalid_argument("Numero de decimales incompatible con numero de cifras.");

        unsigned int    ne = nc - nd;

        unsigned int    le = ne != 0 ? static_cast<int>((ne - 1)/2) + 1 : 0;
        unsigned int    ld = nd != 0 ? static_cast<int>((nd - 1)/2) + 1 : 0;
        unsigned int    lb = le + ld;
        uint8_t         *t = new uint8_t[lb];

        unsigned int    max_val_e   = ((ne % 2) != 0) ? 9 : 99;
        unsigned int    max_val_d   = ((nd % 2) != 0) ? 94 : 99;

        // Almacenamos el signo, y hacemos el numero positivo, para
        // las siguientes comprobaciones.
        bool signo = this->is_negative();
        this->set_positive();

        std::memset(t, 0x00, sizeof(uint8_t[lb]));

        // Empezaremos por los enteros, por si despu�s con los decimales
        // es necesario aplicar redondeo.
        if(ne < ents) {
            // Tenemos que reducir el numero de enteros, asi que debemos
            // comprobar que no se produce desbordamiento.

            // Recorremos el buffer original en sentido inverso, desde la
            // primera posicion del nuevo tama�o, y comprobaremos que el
            // valor se puede copiar, adem�s de seguir comprobando que el
            // resto de bytes est�n a 0.
            int dif = long_ents - le;

            if(buffer[dif] > max_val_e) {
                delete [] t;
                throw std::out_of_range("Cambio de tamanio imposible.");
            }

            for(int i = dif - 1; i >= 0; --i) {
                if(buffer[i] != 0x00) {
                    delete [] t;
                    throw std::out_of_range("Cambio de tamanio imposible.");
                }
            }
            // Llegados aqu�, quiere decir que los enteros caben en el nuevo
            // tama�o, as� que copiamos los datos desde ents-1 hasta diff.
            for(int i = long_ents - 1, j = le - 1; j >= 0; --i, --j)
                t[j] = buffer[i];
        }
        else {
            for(int i = long_ents - 1, j = le - 1; i >= 0; --i, --j)
                t[j] = buffer[i];
        }

        // Ahora los decimales.
        if(nd < decs) {
            // Tendremos que redondear, si es necesario.
            //  1. Calcular la diferencia de bytes.
            //  2. Eliminar el �ltimo byte, y redondear todo el decimal.
            //  3. Repetir para todos los bytes a eliminar.
            unsigned int dif = long_decs - ld;

            bool acarreo = false;

            // Procesamos los bytes a eliminar, para ver si una vez eliminados
            // tenemos que acarrear al �ltimo byte del nuevo tama�o.
            for(int i = long_buffer - 1; i >= static_cast<int>(long_buffer - dif - 1); --i) {
                if(acarreo) ++buffer[i];

                if(buffer[i] > 49)
                    acarreo = true;
                else
                    acarreo = false;
            }

            // Copiamos la parte del buffer que si cabe en el temporal.
            for(int i = long_ents, j = le; j < static_cast<int>(lb); ++i, ++j) {
                t[j] = buffer[i];
            }

            // Ahora procesamos todo el temporal, para comprobar que todo va bien.
            // Tendremos que comprobar que si el acarreo nos ha hecho incrementar
            // la parte entera, esta no se salga de rango.
            acarreo = false;

            for(int i = lb - 1; i >= 0; --i) {
                if(acarreo)
                    ++t[i];

                if(i == static_cast<int>(lb - 1)) {
                    // El �ltimo byte. Podr� tener un valor m�ximo
                    // dependiendo del numero de cifras del decimal.
                    if(t[i] > max_val_d) {
                        // Nuestro �ltimo numero produce acarreo.
                        // Ponemos a 0 e indicamos.
                        t[i] = 0;
                        acarreo = true;
                    }
                    else if((nd % 2) != 0) {
                        // En caso de que el nuevo decimal solo pueda
                        // almacenar un numero impar de cifras, hay que
                        // poner a 0 la menos significativa del par,
                        // y comprobar que esta �ltima cifra no produce
                        // acarreo a la siguiente.
                        if((t[i] % 10) > 4)
                            t[i] = (((t[i] / 10) % 10) + 1) * 10;
                        else
                            t[i] = ((t[i] / 10) % 10) * 10;
                    }
                }
                else if(t[i] >= 100) {
                    t[i] = 0;
                    acarreo = true;
                }
                else {
                    acarreo = false;
                }
            }

            // Comprobamos que no nos hayamos salido de rango.
            if((t[0] & 0x7F) > max_val_e) {
                delete [] t;
                throw std::out_of_range("Cambio de tamanio imposible.");
            }
        }
        else {
            for(unsigned int i = long_ents, j = le; i < long_buffer; ++i, ++j)
                t[j] = buffer[i];
        }

        // Actualizamos los datos.
        ents        = ne;
        decs        = nd;
        long_ents   = le;
        long_decs   = ld;
        long_buffer = lb;

        delete [] buffer;
        buffer  = t;
        t       = nullptr;

        if(signo)
            this->set_negative();
    }
}

void decimal::convertir(const std::string &str)
{
    // Nuevo plan: Averiguamos enteros y decimales, creamos
    // un decimal de esas caracter�sticas, cambiamos el tama�o
    // para que se acomode a este y ya lo tenemos hecho.
    std::string t = str;
    bool        neg = false;
    size_t      pos_punto;
    int         ne = 0,
                nd = 0;

    // Comprobamos el signo.
    if(t[0] == '-') {
        neg = true;
        t.erase(0, 1);
    }

    // Buscamos el punto decimal
    pos_punto = t.find_first_of('.');

    if(pos_punto != std::string::npos) {
        // Tenemos punto decimal, contamos enteros y decimales.
        ne = pos_punto;
        nd = t.size() - pos_punto - 1;

        // Y lo eliminamos.
        t.erase(pos_punto, 1);
    }
    else {
        // No tenemos punto, todo enteros.
        ne = t.size();
    }

    // A�adimos 0 por delante o por detras, seg�n sea necesario, para
    // tener pares de numero a convertir y colocar facilmente en el
    // buffer del decimal temporal.
    if(ne % 2 != 0) { // Numero de enteros impar, a�adimos al principio.
        t.insert(0, 1, '0');
        ++ne;
    }

    if(nd % 2 != 0) { // Numero de enteros impar, a�adimos al final.
        t.push_back('0');
        ++nd;
    }

    // Creamos un decimal temporal para el nuevo contenido.
    decimal x(ne + nd, nd);

    // Almacenamos en formato interno.
    for(int i = 0, j = 0; j < ne + nd; ++i, j += 2)
        x.buffer[i] = s_to_int(t.substr(j, 2));

    // Y lo cambiamos al tama�o que necesitamos.
    x.resize(ents+decs, decs);
    *this = x;

    if(neg)
        this->set_negative();
}

bool decimal::validar_cadena(const std::string &str, std::string &t)
{
    const char  *ws = " \t\n\r\f\v"; // Caracteres a eliminar.

    t = str;

    // Eliminamos espacios, tabuladores, retornos y otros caracteres
    // especiales, tanto del principio como del final de la cadena.
    t.erase(t.find_last_not_of(ws) + 1);
    t.erase(0, t.find_first_not_of(ws));

    // Comprobamos que solo contenga n�meros, signos m�s o menos, y puntos.
    size_t valid_chars = t.find_first_not_of("+-0123456789.");

    // Contamos el n�mero de puntos. Como m�ximo habr� uno.
    size_t num_points = std::count(t.begin(), t.end(), '.');

    // Averiguamos la posici�n del signo menos, si lo hay.
    size_t pos_minus = t.find_first_of('-');

    // Averiguamos la posici�n del signo m�s, si lo hay.
    size_t pos_plus = t.find_first_of('+');

    // Si todos son caracteres v�lidos, hay como m�ximo un punto, y si
    // hay un menos o un mas al principio, tenemos un numero v�lido.
    return (valid_chars == std::string::npos) && (num_points <= 1) && (
        (pos_minus == 0 && pos_plus == std::string::npos) || // Hay un signo menos, y no hay signo m�s.
        (pos_plus == 0 && pos_minus == std::string::npos) || // Hay un singo m�s, y no hay signo menos.
        ((pos_minus == std::string::npos) && (pos_plus == std::string::npos)) // No hay signo alguno.
    );
}

std::string decimal::to_str(bool format) const
{
    std::string t;

    for(int i = 0; i < static_cast<int>(long_buffer); ++i) {
        uint8_t x = buffer[i];

        if(i == 0 && this->is_negative())
            // Eliminamos el bit de signo para imprimir el valor correcto.
            x &= 0x7F;

        if(i == static_cast<int>(long_ents))
            t += '.';

        // Si el numero de enteros es impar, del primer par de numeros
        // tomaremos solamente una cifra.
        // Lo mismo para los decimales, pero con el ultimo par.
        // Hay que controlar si tenemos enteros solamente, si tenemos
        // tanto enteros como decimales, o si solamente tenemos
        // decimales.

        if((i == 0) && ((ents % 2) != 0))
            t += int_to_s(static_cast<int>(x), 2).substr(1, 1);
        else if((i == static_cast<int>(long_buffer - 1)) && ((decs % 2) != 0))
            t += int_to_s(static_cast<int>(x), 2).substr(0, 1);
        else
            t += int_to_s(static_cast<int>(x), 2);
    }

    // Para imprimir:
    //  1. Imprimir todo el buffer por pares en una cadena temporal.
    //  2. Comprobar si tenemos enteros y decimales o no.
    //      a. ENTEROS Y DECIMALES: Buscar la posicion del punto, y colocar.
    //      b. SOLO DECIMALES: Colocar el punto al principio.
    //      c. SOLO ENTEROS: No colocar el punto.
    //  3. Eliminar, si sobran, la cifra inicial y/o la final.
    //  4. Colocar el signo.

    if(format) {
        // Formateamos el n�mero para que sea legible. Es decir,

        // Eliminamos los ceros a la izquierda, dejandos los decimales.
        t.erase(0, t.find_first_not_of("0"));

        // Si no tenemos enteros, ponemos un 0 al principio.
        if(t.size() != 0) {
            if(t[0] == '.')
                t.insert(0, 1, '0');
        }
        else {
            // Si no tenemos ning�n n�mero, ponemos un 0.
            // Este caso se puede dar al formatear un decimal que tenga
            // solamente parte entera y est� a 0.
            t += "0";
        }
    }

    if(this->is_negative())
        // Si es necesario, insertamos el signo.
        t.insert(0, 1, '-');

    return t;
}

bool operator==(const decimal &a, const decimal &b)
{
    // 1. Comparamos signos. Si son diferentes devolvemos falso.
    // 2. Cambiamos el tama�o de b para que coincida con el de a.
    // 3. Comparamos byte a byte.

    if(a.is_negative() != b.is_negative()) return false;

    decimal t(b);
    t.resize(a.decs + a.ents, a.decs);

    for(int i = 0; i < static_cast<int>(a.long_buffer); ++i)
        if(a.buffer[i] != t.buffer[i])
            return false;

    return true;
}

bool operator==(const decimal &a, const std::string &b)
{
    decimal t(a.ents + a.decs, a.decs);

    t = b;

    return a == t;
}

bool operator>=(const decimal &a, const decimal &b)
{
    // 1. Comparamos signos. Si son distintos, el positivo es el mayor.
    // 2. Cambiamos el tama�o de b para que coincida con el de a.
    // 3. Comparamos byte a byte, y devolvemos en consecuencia.

    if(a.is_negative() != b.is_negative())
        return !a.is_negative();

    decimal t(b);
    t.resize(a.decs + a.ents, a.decs);

    bool    a_mayor = false,
            iguales = true;

    for(int i = 0; i < static_cast<int>(a.long_buffer); ++i) {
        if(a.buffer[i] != t.buffer[i]) {
            iguales = false;
            if(a.buffer[i] > t.buffer[i]) {
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
    // 2. Cambiamos el tama�o de b para que coincida con el de a.
    // 3. Comparamos byte a byte, y devolvemos en consecuencia.

    if(a.is_negative() != b.is_negative())
        return a.is_negative();

    decimal t(b);
    t.resize(a.decs + a.ents, a.decs);

    bool    a_menor = false,
            iguales = true;

    for(int i = 0; i < static_cast<int>(a.long_buffer); ++i) {
        if(a.buffer[i] != t.buffer[i]) {
            iguales = false;
            if(a.buffer[i] < t.buffer[i]) {
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
    // Ojo, varias posibilidades:
    //  1. Solo decimales.
    //  2. Solo enteros.
    //  3. Decimales y enteros.

    decimal t(*this);

    uint8_t max_val_inicio  = ((t.ents % 2) != 0) ? 9 : 99;
    uint8_t max_val_final   = ((t.decs % 2) != 0) ? 90 : 99;
    uint8_t max_val_num     = 99;

    for(int i = 0; i < static_cast<int>(t.long_buffer); ++i) {
        if(i == 0 && t.ents != 0)
            t.buffer[i] = max_val_inicio;
        else if(i == static_cast<int>(t.long_buffer - 1) && t.decs != 0)
            t.buffer[i] = max_val_final;
        else
            t.buffer[i] = max_val_num;
    }

    return t;
}

decimal decimal::min() const
{
    // Poner todo a cero y el ultimo byte a lo que
    // corresponda seg�n las cifras que tengamos y si
    // hay decimales o no.
    decimal t(*this);

    for(int i = 0; i < static_cast<int>(t.long_buffer); ++i)
        t.buffer[i] = 0;

    if(t.decs != 0)
        t.buffer[t.long_buffer-1] = ((t.decs % 2) != 0) ? 10 : 01;
    else
        t.buffer[t.long_buffer-1] = 1;

    return t;
}

decimal decimal::zero() const
{
    decimal t(*this);

    std::memset(t.buffer, 0x00, sizeof(uint8_t[t.long_buffer]));

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

}; // namespace fpt

