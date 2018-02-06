# Ejemplos y utilidades para el TM1637
Algunos ejemplos y utilidades para el display de 4digitos, 7segmentos TM1637

- Libreria ligeramente modificada para representar segmentos individuales o componer caracteres con grupos de segmentos
  Incluye nueva funcion:
    displaySegments(uint8_t BitAddr,int8_t SegData);
    que permite direccionar segmentos individuales y bloques de segmentos

- Ejemplo en arduino para mostrar el abecedario y algunos simbolos mas
  (dentro de las limitaciones que supone un display de 7 segmentos)
