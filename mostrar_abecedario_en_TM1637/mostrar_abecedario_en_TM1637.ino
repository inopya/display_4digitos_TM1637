/*
 #       _\|/_   A ver..., ¿que tenemos por aqui?
 #       (O-O)
 # ---oOO-(_)-OOo---------------------------------

 ##########################################################
 # ****************************************************** #
 # *             DOMOTICA PARA PRINCIPIANTES            * #
 # *         mostar caracteres en Display TM1637        * #
 # *            Autor: Eulogio López Cayuela            * #
 # *                                                    * #
 # *          Versión 1.0    Fecha: 04/02/2018          * #
 # ****************************************************** #
 ##########################################################

*/

/*
 * NOTAS DE LA VERSION:
 * Uso de de la libreria del TM1637 con una pequeña modificacion con la 
 * que se le a añadido la funcion perosonal displaySegments()
 */

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//------------------------------------------------------
// Algunas definiciones personales para mi comodidad al escribir codigo
//------------------------------------------------------
#define AND &&
#define OR ||
#define NOT !
#define ANDbit &
#define ORbit |
#define XORbit ^
#define NOTbit ~


//------------------------------------------------------
// Otras definiciones para pines y variables
//------------------------------------------------------

#define PIN_LED 13                  //led OnBoard de Arduino UNO

#define CLK 11                      //definicion de pines de datos para el TM1637    
#define DIO 10                      //Se pueden cambiar a otros si se desea




//------------------------------------------------------
//definiciones para los segmentos del display
//------------------------------------------------------
#define SEG_A 0b00000001                //   -- A --    // ORDEN DE LOS SEGMENTOS DEL DISPLAY
#define SEG_B 0b00000010                //  |      |
#define SEG_C 0b00000100                //  F      B
#define SEG_D 0b00001000                //   -- G --
#define SEG_E 0b00010000                //  E      C
#define SEG_F 0b00100000                //  |      |
#define SEG_G 0b01000000                //   -- D --


//------------------------------------------------------
//definiciones de caracteres
//------------------------------------------------------
#define SIMBOLO_NEGATIVO  0b01000000    //  -  por comodidad al nombrarlo, (equivale a un SEG_G)
#define SIMBOLO_PUNTOS    0b10000000    //  :  solo es valido para el digito 1
#define SIMBOLO_GRADO     0b01100011    //  º 
#define SIMBOLO_interroga 0b01010011    //  ?
#define SIMBOLO_BARRA_BAJA 0b00001000   //  _
#define SIMBOLO_NULO      0b00000000    //  'apaga ese digito'  


/*                0b0GFEDCBA   */
#define SIMBOLO_A 0b01110111            //  A
#define SIMBOLO_a 0b01011111            //  a 
#define SIMBOLO_b 0b01111100            //  b
#define SIMBOLO_C 0b00111001            //  C
#define SIMBOLO_c 0b01011000            //  c
#define SIMBOLO_d 0b01011110            //  d
#define SIMBOLO_E 0b01111001            //  E
#define SIMBOLO_F 0b01110001            //  F
#define SIMBOLO_G 0b00111101            //  G
#define SIMBOLO_g 0b01101111            //  g
#define SIMBOLO_H 0b01110110            //  H
#define SIMBOLO_h 0b01110100            //  h
#define SIMBOLO_I 0b00110000            //  I
#define SIMBOLO_i 0b00010000            //  i
#define SIMBOLO_J 0b00011110            //  J
#define SIMBOLO_L 0b00111000            //  L 
#define SIMBOLO_N 0b00110111            //  'N' o 'M'
#define SIMBOLO_n 0b01010100            //  n 
#define SIMBOLO_O 0b00111111            //  O (0)
#define SIMBOLO_o 0b01011100            //  o  
#define SIMBOLO_P 0b01110011            //  P
#define SIMBOLO_q 0b01100111            //  q
#define SIMBOLO_r 0b01010000            //  r  
#define SIMBOLO_S 0b01101101            //  S (5)
#define SIMBOLO_t 0b01111000            //  t 
#define SIMBOLO_U 0b00111110            //  U 'V' 
#define SIMBOLO_v 0b00011100            //  v 
#define SIMBOLO_Y 0b01101110            //  Y  
#define SIMBOLO_Z 0b01011011            //  Z (2)

#define SIMBOLO_0 0b00111111            //  0
#define SIMBOLO_1 0b00000110            //  1  
#define SIMBOLO_2 0b01011011            //  2
#define SIMBOLO_3 0b01001111            //  3
#define SIMBOLO_4 0b01100110            //  4  
#define SIMBOLO_5 0b01101101            //  5
#define SIMBOLO_6 0b01111101            //  6 
#define SIMBOLO_7 0b00000111            //  7 
#define SIMBOLO_8 0b01111111            //  8 
#define SIMBOLO_9 0b01100111            //  9  



//------------------------------------------------------
// Importamos las librerias necesarias
//------------------------------------------------------
#include <TM1637.h>         //libreria para el uso del display 7 segmentos x4


//------------------------------------------------------
// Creamos las instancia de los objetos:
//------------------------------------------------------

//Creamos el objeto display 4x7
TM1637 display_tm1637(CLK,DIO);
 


//------------------------------------------------------
// Variables GLOBALES
//------------------------------------------------------
volatile boolean FLAG_estado_pulsador = false;

byte abecedario[] = {SIMBOLO_A, SIMBOLO_a, SIMBOLO_b, SIMBOLO_C, SIMBOLO_c, SIMBOLO_d, 
                     SIMBOLO_E, SIMBOLO_F, SIMBOLO_G, SIMBOLO_g, SIMBOLO_H, SIMBOLO_h, 
                     SIMBOLO_I, SIMBOLO_i, SIMBOLO_J, SIMBOLO_L, SIMBOLO_N, SIMBOLO_n, 
                     SIMBOLO_O, SIMBOLO_o, SIMBOLO_P, SIMBOLO_q, SIMBOLO_r, SIMBOLO_S, 
                     SIMBOLO_t, SIMBOLO_U, SIMBOLO_v, SIMBOLO_Y, SIMBOLO_Z, SIMBOLO_NULO,
                     SIMBOLO_0, SIMBOLO_1, SIMBOLO_2, SIMBOLO_3, SIMBOLO_4, SIMBOLO_5,
                     SIMBOLO_6, SIMBOLO_7, SIMBOLO_8, SIMBOLO_9, SIMBOLO_NEGATIVO, 
                     SIMBOLO_GRADO, SIMBOLO_interroga, SIMBOLO_BARRA_BAJA, };

int indice_abecedario = 0;



//***************************************************************************************************
//  SECCION OPERATIVA DEL PROGRAMA
//***************************************************************************************************


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup()
{
                    
  //apagamos el led 'On Board'
  pinMode(PIN_LED, OUTPUT);                //PIN13 como salida
  digitalWrite(PIN_LED, LOW);              //apagar PIN13  DISPLAY_POWER

  
  display_tm1637.init();
  /* --- Modificar la intensidad del brillo del display --- */
  //display_tm1637.set(n) // n=(-1,0,1...7,8) >>>  -1 o 8 apagado , 0 minimo brillo, 2 normal, 7 maximo brillo
  display_tm1637.set(2);
  
  //mostrar ("inoPYA")     
  display_tm1637.displaySegments(0,SIMBOLO_i);    //'i'
  display_tm1637.displaySegments(1,SIMBOLO_n);    //'n'
  display_tm1637.displaySegments(2,SIMBOLO_o);    //'o'
  display_tm1637.displaySegments(3,SIMBOLO_NULO); //' '
  delay(1200);  //1500
  display_tm1637.displaySegments(0,SIMBOLO_P);    //'P'
  display_tm1637.displaySegments(1,SIMBOLO_Y);    //'Y'
  display_tm1637.displaySegments(2,SIMBOLO_A);    //'A'
  display_tm1637.displaySegments(3,SIMBOLO_NULO); //' '
  delay(1200);  //1500
  apagar_display();
  
}


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BUCLE PRINCIPAL DEL PROGRAMA   (SISTEMA VEGETATIVO)
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void loop()
{ 

  for( byte n=0;n<(sizeof(abecedario)/4);n+=4){
    delay(1000);
    display_tm1637.displaySegments(0,abecedario[n]);
    display_tm1637.displaySegments(1,abecedario[n+1]);
    display_tm1637.displaySegments(2,abecedario[n+2]);
    display_tm1637.displaySegments(3,abecedario[n+3]);   
  }

}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BLOQUE DE FUNCIONES: LECTURA SENSORES, TOMA DE DECISIONES, MOSTAR DATOS, ACCESO EEPROM...
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    DISPLAY
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//  FUNCION PARA BORRAR/APAGAR EL DISPLAY
//========================================================
void apagar_display()
{
  /*
   * permite el apagado del display
   */

  display_tm1637.displaySegments(0,0);
  display_tm1637.displaySegments(1,0);
  display_tm1637.displaySegments(2,0);
  display_tm1637.displaySegments(3,0);
}


//========================================================
//  FUNCIONES BASE PARA ESCRIBIR DIGITOS EN EL DISPLAY
//========================================================
void mostarCifras( int cifra)
{
  //  OJO el display los numera de izquierda a derecha (digito 0,1,2,3)
  int Num = abs(cifra);
  
  int8_t unidades = Num %10 ;               //unidades
  int8_t decenas = (Num % 100) / 10 ;       //decenas
  int8_t centenas = (Num % 1000) / 100 ;    //centenas
  int8_t millares = Num  / 1000  ;          //unidades de millar

  
  display_tm1637.display(0, millares);
  display_tm1637.display(1, centenas);
  display_tm1637.display(2, decenas);
  display_tm1637.display(3, unidades);  
}



//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************


