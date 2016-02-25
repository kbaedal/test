/*! \file Biblioteca de utilidades. */

#ifndef __UTILES_H__
#define __UTILES_H__ 1

#include <string>
#include <vector>

/*! \namespace */
namespace utiles {

/*!
 * \brief Copia num_chars caracteres de origen a destino.
 *
 * \param origen Cadena origen.
 * \param pos_inicio Posicion desde la que copiar de la cadena origen.
 * \param num_chars Numero de caracteres a copiar.
 * \param destino Cadena destino.
 * \param cerrar Si TRUE, añadir '\0' al final de destino.
 *
 * Copia hasta num_chars caracteres de la cadena origen desde la posicion
 * pos_inicio en la cadena destino, eliminado caracteres extraños. Si la
 * variable cerrar es TRUE, se añadira un caracter nulo al final de destino.
 */
void LeerTexto(const char *origen, int pos_inicio, int num_chars,
	char *destino, bool cerrar);

/*!
 * \brief Lee un nif, eliminando el '-' si lo hay, pero no caracteres extraños.
 *
 * \param origen Cadena origen.
 * \param pos_inicio Posicion desde la que copiar.
 * \param destino Cadena destino.
 * \param cerrar Si TRUE, añadir '\0' al final de destino.
 */
void LeerNIF(const char *origen, int pos_inicio, int num_chars,
	char *destino, bool cerrar);

/*!
 * \brief Lee un numero, y lo transforma a un formato compatible con atof().
 *
 * \param origen Cadena origen.
 * \param pos_inicio Posicion desde la que copiar.
 * \param num_char Número de caracteres que ocupa el número en origen.
 * \param destino Cadena destino.
 * \param cerrar Si TRUE, añadir '\0' al final de destino.
 *
 * Lee una cadena de texto que debe contener un numero en el que los
 * separadores de millares son puntos y el de decimales es una coma.
 * Para que la funcion atof() se pueda utilizar con la cadena, debe
 * cumplir que no haya separadores de millares y el indicador del
 * decimal sea un punto. Es decir, para la entrada: 1.234.567,89
 * esta funcion devuelve: 1234567.89
 */
void LeerNumero(char *origen, int pos_inicio, int num_chars,
	char *destino, bool cerrar);

/*!
 * \brief Limpia un numero, y lo transforma a un formato compatible con atof().
 *
 * \param s Cadena que contiene el numero.
 * \return El número limpio, o una cadena vacia si no se pudo limpiar.
 *
 * Lee una cadena de texto que debe contener un numero en el que los
 * separadores de millares son puntos y el de decimales es una coma.
 * Para que la funcion atof() se pueda utilizar con la cadena, debe
 * cumplir que no haya separadores de millares y el indicador del
 * decimal sea un punto. Es decir, para la entrada: 1.234.567,89
 * esta funcion devuelve: 1234567.89
 */
std::string &LimpiarNumero(std::string &s);

/*!
 * \brief Vuelca un número en coma flotante en destino, alineado a la derecha.
 *
 * \param destino Cadena de destino del número.
 * \param numero Número a volcar en la cadena.
 * \param pos_inicio Posición de inicio en la cadena.
 * \param pos_final Posición final en la cadena, donde terminará el número.
 *
 * Vuelva un número en coma flotante en la cadena destino, alineandolo
 * a la derecha, eliminando el punto decimal y rellenando el resto de
 * la cadena con 0. El número terminará siempre en pos_final, y comenzará
 * como mucho en pos_inicio.
 */
void VolcarNumero(char *destino, double numero, int pos_inicio, int pos_final);

/*!
 * \brief Vuelca un número en coma flotante en destino.
 *
 * \param destino Cadena de destino del número.
 * \param numero Número a volcar en la cadena.
 * \param pos_inicio Posición de inicio en la cadena.
 * \param pos_final Posición final en la cadena, donde terminará el número.
 *
 * Vuelca un número en coma flotante en destino, cambiando el punto por una
 * coma, y rellena con un espacio los caracteres sobrantes. Alinea el numero
 * a la derecha.
 */
void VolcarNumeroDecimal(char *destino, double numero, int pos_inicio, int pos_final);

/*!
 * \brief Vuelca num_chars caracteres de origen en destino, desde pos_inicial.
 *
 * \param destino Cadena de destino del volcado.
 * \param origen Cadena de origen.
 * \param pos_inicial Posicion en destino desde la que copiar.
 * \param num_chars Número de caracteres a copiar.
 */
void VolcarCadena(char *destino, const char *origen, int pos_inicial, int num_chars);

/*!
 * \brief Lee una linea de campos de texto separado por el caracter separador.
 *
 * \param origen String que contiene la linea a leer.
 * \param lista_campos Vector de strings en el que se almacenarán los campos leidos.
 * \param separador Caracter separador de los campos.
 *
 * Lee una linea de campos de texto, y separa estos campos en un vector de
 * cadenas. Los campos irán separados por un caracter especificado por separador,
 * que por defecto será un ';'.
 */
void LeerLineaCSV(const std::string origen, std::vector<std::string> &lista_campos, char separador = ';');

/*!
 * \brief Rellena una cadena con tantos caracteres como se indique, y el ultimo a nulo.
 *
 * \param cadena Cadena a rellenar.
 * \param c Caracter con el que rellenarla.
 * \param num_chars Numero de caracteres a rellenar. cadena[num_chars - 1] sera puesto a nulo.
 */
void RellenaCadena(char *cadena, char c, int num_chars);

/*!
 * \brief Convierte un entero en una cadena.
 *
 * \param n Entero a convertir.
 * \param width Tamaño de la cadena devuelta, rellenado a 0 por la izquierda.
 * \return La cadena con el entero.
 */
std::string IntToStr(int n, int width = 0);

/*!
 * \brief Convierte una cadena a un entero.
 *
 * \param s String a convertir.
 * \return El entero correspondiente.
 */
int StrToInt(std::string s);

/*!
 * \brief Convierte una cadena terminada en ceros a un entero.
 *
 * \param cadena Cadena a convertir.
 * \return El entero correspondiente.
 */
int StrToInt(char *cadena);

/*!
 * \brief Convierte un caracter a un entero.
 *
 * \param caracter Caracter a convertir.
 * \return El entero correspondiente.
 */
int StrToInt(char caracter);

/*!
 * \brief Calcula el resto de la division de un entero largo, pasado como cadena.
 *
 * \param dividendo Cadena que contiene el dividendo.
 * \param divisor Entero con el divisor.
 * \return El resto de la division, como entero.
 */
int restoEntero(std::string dividendo, int divisor);

/*!
 * \brief Sustituye, hasta max_pos, c_orig por c_dest en cadena.
 *
 * \param c_orig Caracter que queremos sustituir.
 * \param c_dest Caracter sustituto.
 * \param cadena Cadena a la que hacer el cambio.
 * \param max_pos Maxima posicion en la cadena para cambiar.
 */
void swapChars(char cOrigen, char cDest, char *szCadena, int nNumChars);

/*!
 * \brief Convierte un numero pasado como cadena, en un formato de moneda.
 *
 * \param importe La cadena que contiene el numero a formatear.
 * \return Una cadena con el numero formateado a moneda.
 */
std::string formatearImporte(std::string importe);

/*!
 * \brief Convierte un numero entero en una aadena con formato de moneda.
 *
 * \param importe El numero a formatear.
 * \return Una cadena con el numero formateado a moneda.
 */
std::string formatearImporte(int importe);

/*!
 * \brief Convierte una fecha en formato ISO 8601 básico a humano común.
 *
 * \param fecha String que contiene la fecha en ISO 8601 básico.
 * \param separador Cadena terminada en \0 que separa los campos de la fecha.
 * \return String con la fecha en formato humano común.
 */
 std::string formatearFecha(std::string fecha, const char *separador = "-");

 /*!
 * \brief Convierte una fecha de la forma DD?MM?AAAA en AAAA-MM-DD, conforme a ISO 8601:2004.
 *
 * \param fecha String que contiene la fecha a transformar.
 * \param separador Cadena terminada en \0 que separa los campos de la fecha.
 * \return String con la fecha en formato ISO 8601:2004.
 */
 std::string &fechaISO8601(std::string &fecha, const char *separador = "-");


/*!
 * \brief Devuelve el IBAN asociado a un CCC.
 *
 * \param ccc String que contiene la cuenta bancaria sobre la que calcular el IBAN.
 * \return String con el IBAN calculado, o nulo si no se puede calcular.
 */
std::string calculaIBAN(std::string ccc);

/*!
 * \brief Calcula si un IBAN es correcto.
 *
 * \param iban IBAN que comprobar.
 * \return Verdadero si es correcto, falso si no lo es.
 */
bool checkIBAN(std::string iban);

/*!
 * \brief Calcula si un CCC es correcto.
 *
 * \param ccc CCC que comprobar.
 * \return Verdadero si es correcto, falso si no lo es.
 */
bool checkCCC(std::string ccc);

/*!
 * \brief Comprueba un número o código de identificacion fiscal.
 *
 * \param strID String con el dato a comprobar.
 * \return 1 si es NIF válido, 2 si es CIF válido, -1 si hay error.
 */
int checkID(std::string id);

/*!
 * \brief Comprueba un NIF es correcto (persona física).
 *
 * \param nif String con el nif a comprobar.
 * \return True si es correcto, false en caso contrario.
 */
bool checkNIF(std::string nif);

/*!
 * \brief Comprueba un CIF es correcto (persona jurídica).
 *
 * \param cif String con el cif a comprobar.
 * \return True si es correcto, false en caso contrario.
 */
bool checkCIF(std::string cif);

/*!
 * \brief Convierte una cadena al formato ISO 20022.
 *
 * \param cadena String con contiene la candena a transformar.
 */
void setTextToISO20022(std::string &cadena);

// Comprueba que los caracteres de strFloat sean
// válidos para convertirlos a un float.
/*!
 * \brief Comprueba que la cadena pasada contiene un numero real.
 *
 * \param numero Cadena con el número a comprobar.
 */
bool checkNum(std::string numero);

/*!
 * \brief Pasa a mayusculas, si es posible. Solo para ASCII.
 *
 * \param s Cadena a pasar a mayusculas.
 * \return La cadena en mayusculas.
 */
std::string pasarMayusculas(const std::string &s);

// Convierte una cadena exportada desde GID a UTF8.
/*!
 * \brief Convierte una cadena leida de GID a UTF8.
 *
 * \param s Cadena a convertir.
 * \return La cadena convertida.
 */
char *GIDToUTF8(const char *s);

// Convierte una cadena exportada desde GID a UTF8, version std::string
/*!
 * \brief Convierte una cadena leida de GID a UTF8.
 *
 * \param s Cadena a convertir.
 * \return La cadena convertida.
 */
std::string &GID_to_UTF8(std::string &s);

/*!
 * \brief Convierte una cadena con caracteres UTF8 a ISO20022.
 *
 * \param origen String que contiene la cadena de origen a transformar.
 * \param destino String que contendrá la cadena transformada.
 * \return True en caso de que la conversión haya sido posible, False en otro caso.
 */
bool UTF8ToISO20022(const std::string origen, std::string &destino);

/*!
 * \brief Elimina los espacios y otros caracteres de control del final de una cadena.
 *
 * \param s String que vamos a limpiar.
 * \param ws Caracteres a eliminar de la cadena.
 * \return String limpia de caracteres vacios al final.
 */
inline std::string &rtrim(std::string &s, const char *ws = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(ws) + 1);
    return s;
}

/*!
 * \brief Elimina los espacios y otros caracteres de control del principio de una cadena.
 *
 * \param s String que vamos a limpiar.
 * \param ws Caracteres a eliminar de la cadena.
 * \return String limpia de caracteres vacios al principio.
 */
inline std::string &ltrim(std::string &s, const char *ws = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(ws));
    return s;
}

/*!
 * \brief Combina ltrim y rtrim.
 *
 * \param s String que vamos a limpiar.
 * \param ws Caracteres a eliminar de la cadena.
 * \return String limpia de caracteres vacios al principio y al final.
 */
inline std::string &trim(std::string &s, const char *ws = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, ws), ws);
}

} // namespace utiles

#endif // __UTILES_H__
