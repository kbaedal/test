#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include "utiles.h"

void utiles::LeerTexto(const char *origen, int pos_inicio, int num_chars,
	char *destino, bool cerrar)
{
	for(int i = 0; i < num_chars; i++) {
        switch(origen[pos_inicio + i]) {
            case '¥':
                // Sustituimos caracteres especiales
                destino[i] = 'Ñ';
                break;
            case 'š':
                destino[i] = 'U';
                break;
            case '§':
                // Sustituimos caracteres extraños
                destino[i] = ' ';
                break;
            case '\0':
                // Finalizamos el bucle.
                i = num_chars;
                break;
            default:
                destino[i] = origen[pos_inicio + i];
                break;
        }
	}

	if(cerrar) destino[num_chars] = '\0';
}

void utiles::LeerNIF(const char *origen, int pos_inicio, int num_chars,
	char *destino, bool cerrar)
{
	int desp = 0;

	for(int i = 0; i < num_chars; i++)
	{
		if(origen[pos_inicio + desp + i] == '-')
			desp++;

		destino[i] = origen[pos_inicio + desp + i];
	}

	if(cerrar) destino[num_chars] = '\0';
}

void utiles::LeerNumero(char *origen, int pos_inicio, int num_chars,
	char *destino, bool cerrar)
{
	for(int i = 0, j = 0; i < num_chars; i++)
	{
		if(origen[pos_inicio + i + j] == '.')
			j++;
		else if(origen[pos_inicio + i + j] == ',')
			origen[pos_inicio + i + j] = '.';

		destino[i] = origen[pos_inicio + i + j];
	}
	if(cerrar) destino[num_chars] = '\0';
}

std::string &utiles::LimpiarNumero(std::string &s)
{
    std::string temp(s);
    s.clear();

    for(char &x : temp) {
        switch (x) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                s += x;
                break;
            case ',':
                s += '.';
                break;
            case '.':
                break;
            default:
                s.clear();
                return s;
        }
    }

    return s;
}

void utiles::VolcarNumero(char *destino, double numero, int pos_inicio, int pos_final)
{
	int 		total_char 	= pos_final - pos_inicio;
	int			longitud;
	char 		*temp 	    = new char[total_char];
	char 		*temp2;

	// Inicializamos la cadena con nulo.
	memset(temp, '\0', total_char);

	// Pasamos el numero a texto.
	sprintf(temp, "%.2f", fabs(numero));
	// Averiguamos la longitud de nuestra nueva cadena.
	longitud = strlen(temp);

	// Creamos una cadena nueva en la que copiaremos los datos, pero
	// sin el '.' del decimal.
	temp2 = new char[longitud+1];
	memset(temp2, '0', longitud);
	temp2[longitud] = '\0';

	// Eliminamos el '.' de la cadena, no tenemos que escribirlo.
	for(int i = (longitud - 1), desp = 0; i > 0; i--) {
		if(temp[i] == '.')
			desp = 1;
		temp2[i] = temp[i - desp];
	}

	// Rellenamos de 0 la cadena final, para sobreescribir solo lo necesario.
	for(int i = 0; i < (total_char + 1); i++)
		destino[pos_inicio + i] = '0';

	// Escribimos los numeros
	for(int i = longitud, j = 0; i > 0; i--, j++)
		destino[pos_final - j] = temp2[longitud -1 - j];

	delete [] temp;
	delete [] temp2;
}

void utiles::VolcarNumeroDecimal(char *destino, double numero, int pos_inicio, int pos_final)
{
	int 		total_char 	= pos_final - pos_inicio;
	int			longitud;
	char 		*temp 	    = new char[total_char];

	// Pasamos el numero a texto.
	sprintf(temp, "%.2f", numero);
	// Averiguamos la longitud de nuestra nueva cadena.
	longitud = strlen(temp);

	// Rellenamos de espacios la cadena final, para sobreescribir solo lo necesario.
	for(int i = 0; i < (total_char + 1); i++)
		destino[pos_inicio + i] = ' ';

	// Escribimos los numeros.
	for(int i = longitud, j = 0; i > 0; i--, j++) {
		// Sustituimos el punto por una coma.
		if(temp[longitud -1 - j] == '.') temp[longitud - 1 - j] = ',';
		destino[pos_final - j] = temp[longitud - 1 - j];
	}

	delete [] temp;
}

void utiles::VolcarCadena(char *destino, const char *origen, int pos_inicial, int num_chars)
{
	for(int i = 0; i < num_chars; i++) {
		if(origen[i] == '\0') // Si encontramos el final de la cadena, terminamos.
			i = num_chars;
		else
			destino[pos_inicial + i] = origen[i];
	}
}

void utiles::LeerLineaCSV(const std::string origen, std::vector<std::string> &lista_campos, char separador)
{
    size_t pos1, pos2;

    lista_campos.clear();

    pos1 = 0;
    while(pos1 < origen.size()) {
        pos2 = origen.find_first_of(separador, pos1);

        if(pos2 == std::string::npos) // La cadena no contiene el separador
            break;

        // Copiamos la subcadena en el vector.
        lista_campos.push_back(origen.substr(pos1, pos2 - pos1));

        // Para la siguiente búsqueda, desde el siguiente caracter.
        pos1 = pos2 + 1;
    }

    // En el caso de que el último campo no acabe en separador, lo guardamos.
    // Si acabara en separador, pos1 seria igual que origen.size(), lo que
    // quiere decir que el campo ya está guardado.
    if(pos1 < origen.size())
        lista_campos.push_back(origen.substr(pos1, origen.size() - pos1));
}

void utiles::RellenaCadena(char *cadena, char c, int num_chars)
{
	// Rellenamos con el caracter indicado la cadena.
	memset(cadena, c, num_chars - 1);
	// Y ponemos el ultimo caracter a '\0' para cerrarla.
	cadena[num_chars - 1] = '\0';
}

std::string utiles::FltToStr(float f)
{
    std::stringstream ss;

    ss << f;

    return ss.str();
}

std::string utiles::IntToStr(int n, int width, char fillchar)
{
    std::stringstream ss;

    if(width > 0)
        ss << std::setfill(fillchar) << std::setw(width) << n;
    else
        ss << n;

    return ss.str();
}

int utiles::StrToInt(std::string s)
{
    int res;

    std::stringstream is(s);

    is >> res;

    return res;
}

int utiles::StrToInt(char *cadena)
{
     return StrToInt(std::string(cadena));
}

int utiles::StrToInt(char caracter)
{
     return StrToInt(std::string(1, caracter));
}

int utiles::restoEntero(std::string dividendo, int divisor)
{
    unsigned int    i = 0;
    int             temp,
                    resto;
    std::string     dividendo_temp;

    // Recorremos la cadena de numeros, e iremos obteniendo los
    // restos hasta llegar al final. Los restos se componen
    // del resto de la division anterior más el siguiente
    // caracter de la cadena. En cuanto tengamos en la cadena
    // temporal suficientes caracteres para dividir entre el
    // divisor, tomamos el resto y repetimos el proceso.
    // Al final, tendremos en la cadena strTemp el resto de
    // la division.
    while(i < dividendo.length()) {
        // Vamos añadiendo caracteres a la cadena.
        dividendo_temp += dividendo[i];
        temp = StrToInt(dividendo_temp);
        // Hasta que el numero sea mayor que el divisor.
        // Es decir, hasta que al dividir tengamos un resto.
        if(temp > divisor) {
            // Tomamos el resto de esa division y seguimos.
            resto = temp % divisor;
            dividendo_temp = IntToStr(resto);
        }
        else {
            // Como no es lo suficientemente grande, pasamos
            // al siguiente numero.
            dividendo_temp = IntToStr(temp);
        }
        i++;
    }

    resto = StrToInt(dividendo_temp);
    return resto;
}

void utiles::swapChars(char c_orig, char c_dest, char *cadena, int max_pos)
{
    for(int i = 0; i < max_pos; i++) {
        if(cadena[i] == c_orig)
            cadena[i] = c_dest;
    }
}

std::string utiles::formatearImporte(std::string importe)
{
    int         longitud = importe.length() - 1;
    std::string inversa,
                formateado;
    bool        test = true;

    // Copiamos la cadena a la inversa, añadiendo en los
    // lugares adecuados la coma y los puntos.
    for(int i = longitud, j = 0; i >= 0; i--, j++) {
        if(j == 2)
            inversa += ",";

        if((((j + 1) % 3) == 0) && (j != 2))
            inversa += ".";

        inversa += importe[i];
    }

    // Examinamos la cadena, empezando por el final.
    // Iremos recorriendo esta cadena en sentido
    // inverso para eliminar los ceros sobrantes.
    // Si encontramos un 0 o un ., decrementamos el
    // contador de caracteres a copiar y seguimos.
    // En caso de encontrar un numero, salimos.
    // En caso de encontrar una , quiere decir
    // que nuestro número no tiene parte entera,
    // con lo que aumentamos el contador para
    // incluir el 0 de la parte entera y salimos.
    longitud = inversa.length()-1;
    while(test) {
        switch(inversa[longitud]) {
            case '0':
            case '.':
                longitud--;
                break;
            case ',':
                longitud++;
                test = false;
                break;
            default:
                test = false;
                break;
        }
    }

    // Invertimos de nuevo la cadena, y añadimos EUR al final.
    formateado = std::string(inversa.rbegin(), inversa.rend());
    formateado += " EUR";

    return formateado;
}

std::string utiles::formatearImporte(int importe)
{
    return formatearImporte(IntToStr(importe));
}

std::string utiles::formatearFecha(std::string fecha, const char *separador)
{
    // Formato ISO 8601 básico: AAAAMMDD
    // Por ejemplo, 27 de Septiembre de 1978: 19780927.

    std::string formateada;

    formateada  = fecha.substr(6, 2);
    formateada += separador;
    formateada += fecha.substr(4, 2);
    formateada += separador;
    formateada += fecha.substr(0, 4);

    return formateada;
}

std::string &utiles::fechaISO8601(std::string &fecha, const char *separador)
{
    std::string temp;

    temp = fecha.substr(6, 4);
    temp += separador;
    temp += fecha.substr(3, 2);
    temp += separador;
    temp += fecha.substr(0, 2);

    fecha = temp;

    return fecha;
}

std::string utiles::calculaIBAN(std::string ccc)
{
    // Cálculo de los digitos de control:
    //  - Añadir detrás 142800 (ES00, donde E es 14 y S es 28)
    //  - Obtener el resto de dividir entre 97.
    //  - A 98 le restamos el resultado, y ese es nuestro DC.

    std::string temp;
    int         resto,
                dc;

    // Primero comprobaremos que el CCC es correcto.
    if(!checkCCC(ccc))
        return std::string("");

    // Construimos la cadena que luego convertiremos en entero:
    temp    = ccc + "142800";
    resto   = restoEntero(temp, 97);
    dc      = 98 - resto;

    // Ya tenemos el DC calculado.
    if(dc < 10)
        temp = std::string("0") + IntToStr(dc);
    else
        temp = IntToStr(dc);

    return  "ES" + temp + ccc;
}

bool utiles::checkIBAN(std::string iban)
{
    // Para comprobarlo, calcularemos el IBAN de la
    // cuenta proporcionada, y compararemos lo obtenido
    // con lo que se nos ha pasado.

    std::string temp;

    if(iban.size() >= 24 )
        temp = calculaIBAN(iban.substr(4, 20));
    else
        // No es un IBAN, no tiene el tamaño minimo.
        return false;

    if(temp.length() < 1) // El CCC es incorrecto.
        return false;

    // Comparamos los digitos de control.
    if(temp.substr(2, 2) == iban.substr(2, 2))
        return true;
    else
        return false;
}

bool utiles::checkCCC(std::string ccc)
{
    if(ccc.size() == 20) {
        // Obtenemos los dígitos de la Entidad
        int e1 = StrToInt(ccc[0]);
        int e2 = StrToInt(ccc[1]);
        int e3 = StrToInt(ccc[2]);
        int e4 = StrToInt(ccc[3]);
        // Los de la Oficina
        int o1 = StrToInt(ccc[4]);
        int o2 = StrToInt(ccc[5]);
        int o3 = StrToInt(ccc[6]);
        int o4 = StrToInt(ccc[7]);
        // Y los de la cuenta
        int c01 = StrToInt(ccc[10]);
        int c02 = StrToInt(ccc[11]);
        int c03 = StrToInt(ccc[12]);
        int c04 = StrToInt(ccc[13]);
        int c05 = StrToInt(ccc[14]);
        int c06 = StrToInt(ccc[15]);
        int c07 = StrToInt(ccc[16]);
        int c08 = StrToInt(ccc[17]);
        int c09 = StrToInt(ccc[18]);
        int c10 = StrToInt(ccc[19]);

        // Calculamos el primer dígito de control. Para ello
        // multiplicamos:
        //      - Digito 1 de la Entidad por 4
        //      - Digito 2 de la Entidad por 8
        //      - Digito 3 de la Entidad por 5
        //      - Digito 4 de la Entidad por 10
        //      - Digito 1 de la Oficina por 9
        //      - Digito 2 de la Oficina por 7
        //      - Digito 3 de la Oficina por 3
        //      - Digito 4 de la Oficina por 6
        // Lo sumamos todo y tomamos el resto de dividir entre 11.
        // A 11 le restamos este resto y ese será nuestro dígito
        // de control. Si la resta nos da 10, el dígito es el 1.
        int dc1 = ( // Multiplicacion, suma y resto.
            (e1 * 4) + (e2 * 8) + (e3 * 5) + (e4 * 10) +
            (o1 * 9) + (o2 * 7) + (o3 * 3) + (o4 * 6)
        ) % 11;

        dc1 = 11 - dc1;

        if(dc1 == 10)
            dc1 = 1;
        else if(dc1 == 11)
            dc1 = 0;

        // Comprobamos:
        if(dc1 != StrToInt(ccc[8]))
            return false;

        // Calculamos el segundo dígito de control. Multiplicamos:
        //      - Digito 1 de la Cuenta por 1
        //      - Digito 2 de la Cuenta por 2
        //      - Digito 3 de la Cuenta por 4
        //      - Digito 4 de la Cuenta por 8
        //      - Digito 5 de la Cuenta por 5
        //      - Digito 6 de la Cuenta por 10
        //      - Digito 7 de la Cuenta por 9
        //      - Digito 8 de la Cuenta por 7
        //      - Digito 9 de la Cuenta por 3
        //      - Digito 10 de la Cuenta por 6
        // Sumamos, nos quedamos el módulo de dividir entre 11.
        // A 11 le restamos el resultado, y ese es el segundo
        // dígito de control. Como antes, si es 10, el dígito es 1.
        int dc2 = ( //Multiplicacion, suma y resto
            (c01 * 1) + (c02 * 2) + (c03 * 4) + (c04 * 8) + (c05 * 5) +
            (c06 * 10) + (c07 * 9) + (c08 * 7) + (c09 * 3) + (c10 * 6)
        ) % 11;

        dc2 = 11 - dc2;

        if(dc2 == 10)
            dc2 = 1;
        else if(dc2 == 11)
            dc2 = 0;

        // Comprobamos:
        if(dc2 != StrToInt(ccc[9]))
            return false;

        // Si llegamos aquí, ambos dígitos de control son correctos.
        return true;
    }
    else
        // No tiene el tamaño adecuado.
        return false;
}

int utiles::checkID(std::string id)
{
    switch(id[0]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'X':
        case 'Y':
        case 'Z': if (checkNIF(id)) return 1; break;
        default: if (checkCIF(id)) return 2; break;
    }

    return -1;
}

bool utiles::checkNIF(std::string nif)
{
    // Formato NIF: NNNNNNNNC
    //  - NNNNNNNN: Número asignado.
    //  - C: Dígito de control. Para calcularlo:
    //      - Tomar el resto de dividir NNNNNNNN entre 23 y tomar
    //        el valor correspondiente de la siguiente tabla:
    //
    //  Resto:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22
    // -----------------------------------------------------------------------------
    //  Letra:  T  R  W  A  G  M  Y  F  P  D  X  B  N  J  Z  S  Q  V  H  L  C  K  E
    //
    //      - Ej. 21457965T:
    //          - Tomamos 21457965, y calculamos resto de dividir entre 23:
    //              - 21457965 mod 23 = 0
    //          - Sustituimos el 0 por el valor indicado en la lista: T
    //          - Resultado: 21457965T
    //
    // Formato CIF: TNNNNNNNC
    //  - T: Puede ser X, Y ó Z
    //  - NNNNNNN: Número asignado.
    //  - C: Dígito de control. Para calcularlo:
    //      - Sustituir T (X=0, Y=1, Z=2)
    //      - Realizar la misma operacion que para NIF.
    //
    //      - Ej. Z3662885W
    //          - Tomamos Z3662885 y sustituimos la Z por 2: 23662885
    //          - Calculamos el resto de dividir por 23:
    //              - 23662885 mod 23 = 2
    //          - Sustituimos el 2 por el valor indicado en la lista: W
    //          - Resultado: Z3662885W

    std::string dc("TRWAGMYFPDXBNJZSQVHLCKE");
    int resto;

    // Comprobamos que el NIF/NIE a comprobar tenga los digitos adecuados
    if(nif.length() != 9)
        return false;

    // Comprobamos que el primer digito sea un numero o X, Y, Z
    switch(nif[0]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': break;
        case 'X': nif[0] = '0'; break;
        case 'Y': nif[0] = '1'; break;
        case 'Z': nif[0] = '2'; break;
        default:
            return false;
            break;
    }

    // Y que el resto de dígitos son numeros.
    for(int i = 0; i < 7; i++) {
        switch(nif[1+i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': break;
            default:
                return false;
                break;
        }
    }

    // Calculamos el resto de dividir entre 23
    resto = restoEntero(nif, 23);

    // Si lo obtenido no coincide con la letra del NIF/NIE, este no es correcto.
    if(nif[8] != dc[resto])
        return false;

    // NIF/NIE válido.
    return true;
}

bool utiles::checkCIF(std::string cif)
{
    // Formato CIF: TPPNNNNNC
    //  - T: Tipo (A, B, C, D, E, F, G, H, J, N, P, Q, R, S, U, V, W)
    //  - PP: Provincia (1-66, 68, 70-86, 90-99)
    //  - NNNNN: Número asignado.
    //  - C: Dígito de control. Para calcularlo:
    //      - Tomar PPNNNNN, dividirlo en posiciones pares e impares:
    //          P1 P2 N3 N4 N5 N6 N7
    //      - Sumar las posiciones pares (P2 + N4 + N6 = R1)
    //      - Para las posiciones impares:
    //          - Multiplicar por dos y sumar digitos:
    //              - P1 * 2 = M -> M1 M2 -> S1 = M1 + M2
    //          - Sumar los resultados:
    //              - S1 + S3 + S5 + S7 = R2
    //      - Sumar los resultados de los pares y los impares:
    //          - D = R1 + R2
    //      - Tomar el dígito de las unidades del resultado:
    //          - D -> D1 D2 -> D2
    //      - A 10 restarle el dígito calculado, excepto si es 0:
    //          - C = 10 - D ó C = 0 si D = 0
    //      - Si el tipo es: A, B, C, D, E, F, G, H, J, U:
    //          - El dígito de control es el número calculado (C).
    //      - Si el tipo es: N, P, 1, R, S, V, W:
    //          - Sustituir el valor calculado de acuerdo a la tabla:
    //
    //              Valor: 0 1 2 3 4 5 6 7 8 9
    //              ---------------------------
    //              Letra: J A B C D E F G H I
    //
    //      - Ej. A14552046
    //          - Tomamos 1455204.
    //          - Sumamos las posiciones pares:
    //              - 4 + 5 + 0 = 9
    //          - Multiplicamos por 2 las impares, y sumamos sus digitos:
    //              - 1 * 2 = 2
    //              - 5 * 2 = 10, 1 + 0 = 1
    //              - 2 * 2 = 4
    //              - 0 * 2 = 0
    //          - Sumamos los resultados de las multiplicaciones:
    //              - 2 + 1 + 4 + 0 = 7
    //          - Sumamos resultados de pares e impares:
    //              - 9 + 7 = 14
    //          - Tomamos el dígito de las unidades: 4
    //          - A 10 le restamos 4:
    //              - 10 - 4 = 6
    //          - El tipo es A, luego le corresponde un numero.
    //          - Resultado A14552046

    std::string dc_letra("JABCDEFGHI"),
                dc_numero("0123456789"),
                temp;
    int     p1, p2,                 // Codigo de provincia.
            n3, n4, n5, n6, n7,     // Numero del documento.
            r1, r2,                 // Resultados para pares e impares.
            s1, s3, s5, s7,         // Sumas impares.
            d2,                     // Resultado de las sumas de par y impar.
            c,                      // Digito de control
            cod_provincia;          // Para comprobar el codigo de provincia.

    // Comprobamos que el CIF tenga los digitos correctos.
    if(cif.length() != 9)
        return false;

    // Comprobamos que el tipo sea correcto:
    switch(cif[0]) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'J':
        case 'N':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'U':
        case 'V':
        case 'W': break;
        default:
            return false;
            break;
    }

    // Y que el resto de dígitos son numeros.
    for(int i = 0; i < 7; i++) {
        switch(cif[i + 1]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': break;
            default:
                return false;
                break;
        }
    }

    // Comprobamos el código de provincia.
    cod_provincia = StrToInt(cif.substr(1, 2));

    if(cod_provincia < 1 || cod_provincia > 99)
        return false;
    if(cod_provincia == 67 || cod_provincia == 69)
        return false;
    if(cod_provincia > 86 && cod_provincia < 90)
        return false;

    // Separamos los digitos para los calculos.
    p1 = StrToInt(cif[1]);
    p2 = StrToInt(cif[2]);
    n3 = StrToInt(cif[3]);
    n4 = StrToInt(cif[4]);
    n5 = StrToInt(cif[5]);
    n6 = StrToInt(cif[6]);
    n7 = StrToInt(cif[7]);

    // Suma de los digitos pares.
    r1 = p2 + n4 + n6;

    // Multiplicacion y suma de los digitos impares.
    s1 = p1 * 2;
    temp = IntToStr(s1);
    if(temp.length() == 2)
        s1 = StrToInt(temp[0]) + StrToInt(temp[1]);
    else
        s1 = StrToInt(temp);

    s3 = n3 * 2;
    temp = IntToStr(s3);
    if(temp.length() == 2)
        s3 = StrToInt(temp[0]) + StrToInt(temp[1]);
    else
        s3 = StrToInt(temp);

    s5 = n5 * 2;
    temp = IntToStr(s5);
    if(temp.length() == 2)
        s5 = StrToInt(temp[0]) + StrToInt(temp[1]);
    else
        s5 = StrToInt(temp);

    s7 = n7 * 2;
    temp = IntToStr(s7);
    if(temp.length() == 2)
        s7 = StrToInt(temp[0]) + StrToInt(temp[1]);
    else
        s7 = StrToInt(temp);

    // Resultado de las multiplicaciones de los impares.
    r2 = s1 + s3 + s5 + s7;

    // Calculamos el digito de la suma de los resultados.
    d2 = r1 + r2;
    temp = IntToStr(d2);
    if(temp.length() == 2)
        d2 = StrToInt(temp[1]);
    else
        d2 = StrToInt(temp);

    // Calculamos el digito de control.
    if(d2 != 0)
        c = 10 - d2;
    else
        c = 0;

    // Comprobamos con el digito de control, ya sea numero o letra.
    switch(cif[0]) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'J':
        case 'U':
            if(cif[8] != dc_numero[c])
                return false;
            break;
        case 'N':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'V':
        case 'W':
            if(cif[8] != dc_letra[c])
                return false;
            break;
        default:
            return false;
            break;
    }

    // Todo correcto. CIF bien formado.
    return true;
}

void utiles::setTextToISO20022(std::string &cadena)
{
    // Valores validos para ISO20022:
    // ABCDEFGHIJKLMNOPQRSTUVWXYZ
    // abcdefghijklmnopqrstuvwxyz
    // 0123456789
    // /-?:().,'
    // Espacio
    std::string temp;

    for(unsigned int i = 0; i < cadena.length(); ++i) {
        if(
            !( // Si no está entre los valores, devolvemos ?
                (cadena[i] >= 65 && cadena[i] <=  90) ||    // Entre A y Z
                (cadena[i] >= 97 && cadena[i] <= 122) ||    // Entre a y z
                (cadena[i] >= 48 && cadena[i] <=  57) ||    // Entre 0 y 9
                (cadena[i] >= 39 && cadena[i] <=  41) ||    // Es ' ( )
                (cadena[i] >= 44 && cadena[i] <=  47) ||    // Es , - . /
                (cadena[i] == 58 || cadena[i] ==  63) ||    // Es : ?
                (cadena[i] == 32)                           // Es espacio
            )
        )
            temp += '?';
        else
            temp += cadena[i];
    }

    cadena = temp;
}


bool utiles::checkNum(std::string numero)
{
    /*
	// C++11 only. GCC 4.9.2+
    try {
        // Probamos a convertir la cadena a un double
        std::stod(numero);
    }
    catch (...) {
        // Pueden lanzarse dos excepciones:
        //  - std::argument_invalid: No puede convertirse.
        //  - std::out_of_range: Sí puede, pero se saldría de rango.
        return false;
    }

    // Si no se ha lanzado excepción, conversión correcta, numero valido.
    return true;
    */

    // Para que una cadena contenga un float válido deben darse las
	// siguientes condiciones:
	//	- Puede haber espacios en blanco antes del primer caracter.
	//	- También puede haber espacios en blanco tras el último.
	//	- El primer caracter debe ser el signo, si lo hay, (+ o -).
	//	- Tras el signo solo puede haber números, nunca otro signo.
	//	- Puede haber una coma para indicar los decimales.
	//	- Tras la coma no puede haber otra coma.

	unsigned int    i = 0;
    bool	        en_numero 	= false,
                    fin_numero 	= false,
                    coma 		= false,
                    signo     	= false;

	while(i < numero.length()) {
		switch(numero[i]) {
			case ' ':
				if(en_numero)
					// Permitimos los espacios al principio y al final
					// de la cadena.
					fin_numero = true;
				break;
			case ',':
				if(!coma && !fin_numero) {
					// Hemos encontrado el punto decimal.
					coma = true;
					// Y puede que no haya cifra por delante, pero aún
					// así es un numero válido: ,56 por ejemplo.
					en_numero = true;
				}
				else
					return false;
				break;
			case '+':
			case '-':
				if(!en_numero && !signo && !fin_numero) {
					// Encontramos el signo antes de cualquier cifra.
					signo = true;
					// Y ya tenemos que procesar el número.
					en_numero = true;
				}
				else
					// Hemos encontrado un signo en un lugar en el que
					// no debería estar.
					return false;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if(!en_numero && !fin_numero)
					// Una cifra. Encontrada por primera vez nos marcará
					// el inicio del número, a partir de aquí entran en
					// juego las demás reglas.
					en_numero = true;
				else if(fin_numero)
					return false;
				break;
			default:
				return false;
				break;
		}
		// Avanzamos en la cadena.
		++i;
	}
	// Si hemos llegado hasta aquí, quiere decir que la cadena leída
	// tiene el formato adecuado para ser convertida a float.
	return true;
}

std::string utiles::pasarMayusculas(const std::string &s)
{
    std::string n;

    for(unsigned int i = 0; i < s.length(); ++i) {
        if(s[i] >= 97 && s[i] <= 122)
            n += s[i] - 32;
        else
            n += s[i];
    }

    return n;
}

char *utiles::GIDToUTF8(const char *s)
{
    unsigned int    pos = 0;
    char            *resultado;
    std::string     buffer("");

    while(s[pos] != '\0') {
        if((s[pos] & 0xFF) == 0xA5) { // Letra Ñ
            buffer.append(1, 0xC3);
            buffer.append(1, 0x91);
        } else if((s[pos] & 0xFF) == 0xA6) { // Simbolo º
            buffer.append(1, 0xC2);
            buffer.append(1, 0xAA);
        } else if((s[pos] & 0xFF) == 0xA7) { // Símbolo ª
            buffer.append(1, 0xC2);
            buffer.append(1, 0xBA);
        } else if((s[pos] & 0xFF) == 0xD1) { // Letra Ñ
            buffer.append(1, 0xC3);
            buffer.append(1, 0x91);
        } else if((s[pos] & 0xFF) == 0x9A) { // Letra Ü
            buffer.append(1, 0xC3);
            buffer.append(1, 0x9C);
        } else if((s[pos] & 0xFF) == 0xD3) { // Letra Ë
            buffer.append(1, 0xC3);
            buffer.append(1, 0x8B);
        } else {
            buffer.append(1, s[pos]);
        }
        pos++;
    }

    resultado = new char[buffer.length() + 1];
    strcpy(resultado, buffer.c_str());

    return resultado;
}

std::string &utiles::GID_to_UTF8(std::string &s)
{
    std::string temp;

    for(size_t i = 0; i < s.size(); ++i) {
        if((s[i] & 0xFF) == 0xA5) { // Letra Ñ
            temp += 0xC3;
            temp += 0x91;
        } else if((s[i] & 0xFF) == 0xA6) { // Simbolo º
            temp += 0xC2;
            temp += 0xAA;
        } else if((s[i] & 0xFF) == 0xA7) { // Símbolo ª
            temp += 0xC2;
            temp += 0xBA;
        } else if((s[i] & 0xFF) == 0xD1) { // Letra Ñ
            temp += 0xC3;
            temp += 0x91;
        } else if((s[i] & 0xFF) == 0x9A) { // Letra Ü
            temp += 0xC3;
            temp += 0x9C;
        } else if((s[i] & 0xFF) == 0xD3) { // Letra Ë
            temp += 0xC3;
            temp += 0x8B;
        }
        else {
            temp += s[i];
        }
    }

    s = temp;

    return s;
}

bool utiles::UTF8ToISO20022(const std::string origen, std::string &destino)
{
    unsigned int    pos = 0;

    destino.clear();

    while(pos < origen.length()) {
        // Buscamos caracteres que tengan su bit más
        // significativo a 1, lo que indica, en una
        // cadena codificada en UTF8, un símbolo multibyte.
        if((origen[pos] & 0x80) == 0x80) { // 0x80 = 1000 0000
            // El primero es 1, comprobamos ahora los tres primeros.
            if((origen[pos] & 0xE0) == 0xC0 ) { // 0xE0 = 1110 0000, 0xC0 = 1100 0000
                // Es un caracter del rango UNICODE 000080-0007FF
                // ignoramos el siguiente byte, ya que el código se
                // compone de 2 bytes.
                destino.append(1, '?');
                pos += 1;
            } else if((origen[pos] & 0xF0) == 0xE0 ) { // 0xF0 = 1111 0000, 0xE0 = 1110 0000
                // Es un caracter del rango UNICODE 000800-00FFFF
                // ignoramos los dos siguientes bytes, ya que el código se
                // compone de 3 bytes.
                destino.append(1, '?');
                pos += 2;
            } else if((origen[pos] & 0xF8) == 0xF0 ) { // 0xF8 = 1111 1000, 0xF0 = 1111 0000
                // Es un caracter del rango UNICODE 010000-10FFFF
                // ignoramos los tres siguientes bytes, ya que el código se
                // compone de 4 bytes.
                destino.append(1, '?');
                pos += 3;
            } else {
                // Es un código de control ASCII (1xxx xxxx) u otra
                // codificación desconocida.
                destino.append(1, '?');
            }
        } else {
            // El caracter ya está en ASCII (0xxx xxxx).
            destino.append(1, origen[pos]);
        }

        // Avanzamos por la cadena de caracteres.
        pos++;
    }

    return true;
}
