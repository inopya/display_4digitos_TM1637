/*
 #       _\|/_   A ver..., ¿que tenemos por aqui?
 #       (O-O)
 # ---oOO-(_)-OOo---------------------------------

 ##########################################################
 # ****************************************************** #
 # *             DOMOTICA PARA PRINCIPIANTES            * #
 # *       Termometro de max y min de bajo consumo      * #
 # *       (basado  en el termometro max-min v2.0)      * #
 # *            Autor: Eulogio López Cayuela            * #
 # *                                                    * #
 # *          Versión 2.3    Fecha: 07/04/2018          * #
 # ****************************************************** #
 ##########################################################



 ===== NOTAS IMPORTANTES / OPCIONES DISPONIBLES ===== 

10- implementada funcion para monitorizar la carga de la bateria

 9- Implementando temporizadores para llevar la cuenta de tiempo de funcionamiento
   (por supuesto como es con temporizadores no es fiable)
 
 8- Implemetado control de reinicios por falta de alimentacion.
   
 7- se muestra la version del programa en el display al reiniciar.
  - Modificadas ligeramente y renombradas las funciones de acceso al display. 
 
 6- Añadida lectura del estado inicial del pulsador para 
   evitar reset de los minimos y maximos por accidente.
 
 5- Añadida la opcion para que el pulsador active una interrupcion
   que saque al micro del estado de reposo  y active el display 
   para mostrar la temperatura actual, el minimo y el Maximo 
   a demanda del usuario. En caso contrario permanece apagado.
   
 4- Consumo actual 7mA en reposo, 14mA tomando muestras, 22mA mostrando datos,
 
 3- Dorminos al micro durante 128 segundos entre toma de muestras (16x8).

 2- Tratando de bajar el exagerado consumo de entre 45-31mA a 5v
   para que pueda ser autonomo con bateria (de movil) y placa solar (de juguete).

 1- La idea basica para ahorrar energia es pasar el micro a modo 'DORMIDO' de bajo consumo (SLEEP mode)
   durante ciertos periodos de tiempo y del saldremos mediante el WDT (WATCH DOG TIMER)
   con la 'instruccion' --> LowPower.powerDown(SLEEP_xxxx, ADC_OFF, BOD_OFF);
   teniendo SLEEP_xxxx las siguientes posibilidades:
      SLEEP_15MS
      SLEEP_30MS
      SLEEP_60MS
      SLEEP_120MS
      SLEEP_250MS    
      SLEEP_500MS    
      SLEEP_1S 
      SLEEP_2S 
      SLEEP_4S 
      SLEEP_8S 
 
      ** SLEEP_FOREVER **  <--- ¡¡ ojito !!  de esta opción, Solo se puede salir con una INTERRUPCION HARDWARE
      * y por tanto no es deaseable para mi toma de datos con el BMP180
*/




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//------------------------------------------------------
// Algunas definiciones personales para mi comodidad al escribir codigo
//------------------------------------------------------
#define AND &&
#define OR ||
#define NOT !
#define ANDbit &
#define ORbit |
#define NOTbit ~
#define XORbit ^

//------------------------------------------------------
// Otras definiciones para pines y variables
//------------------------------------------------------

#define MEM_POS_horas       6       //posicion de la eeprom para almacenar las horas ON
#define MEM_POS_dias        7       //posicion de la eeprom para almacenar los dias ON
#define MEM_POS_reinicios   0       //posiciond e la eeprom para almacenar los datos de cuelgues

#define MEM_POS_tempe_MIN   8       //posiciond e la eeprom para almacenar los datos de min
#define MEM_POS_tempe_MAX   9       //posiciond e la eeprom para almacenar los datos de max

#define ERROR_TEMPERATURA   3       //valor que se suma a la lectura de este BMP180
                                    //parece tener una desviacion de -3 grados


#define PIN_LED 13                  //led OnBoard de Arduino UNO

#define PIN_PULSADOR_SELECCION 2    //pin para un pulsador tactil (o de otro tipo)

#define CLK 11                      //definicion de pines de datos para el TM1637    
#define DIO 10                      //Se pueden cambiar a otros si se desea


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//  Definiciones para los segmentos del display
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#define SEG_A 0b00000001                //   -- A --
#define SEG_B 0b00000010                //  |      |
#define SEG_C 0b00000100                //  F      B
#define SEG_D 0b00001000                //   -- G --
#define SEG_E 0b00010000                //  E      C
#define SEG_F 0b00100000                //  |      |
#define SEG_G 0b01000000                //   -- D --


#define SIMBOLO_NEGATIVO   0b01000000   //  -  por comodidad al nombrarlo, (equivale a un SEG_G)
#define SIMBOLO_PUNTOS     0b10000000   //  :  solo es valido para el digito 1
#define SIMBOLO_GRADO      0b01100011   //  º 
#define SIMBOLO_INTERROGA  0b01010011   //  ?
#define SIMBOLO_BARRA_BAJA 0b00001000   //  _
#define SIMBOLO_PORCENTAJE 0b00010010   //  %   //otra version 0b01010010
#define SIMBOLO_NULO       0b00000000   //  'apaga ese digito'  


/*                0b0GFEDCBA   */
#define SIMBOLO_A 0b01110111            //  A
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
#define SIMBOLO_M 0b00110111            //  'N' o 'M'
#define SIMBOLO_N 0b00110111            //  'N'
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


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// IMPORTAMOS LAS BIBLIOTECAS NECESARIAS
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#include <Wire.h>           //biblioteca para comunicaciones I2C
#include <SFE_BMP180.h>     //biblioteca del sensor de presion y temperatura
#include <EEPROM.h>         //biblioteca para el control de la EEPROM
#include <LowPower.h>       //biblioteca para manejar el modo sleep
#include <TM1637.h>         //biblioteca para el display 7 segmentos x4


//------------------------------------------------------
// Creamos las instancia de los objetos:
//------------------------------------------------------

//Creamos el objeto sensor BMP180
SFE_BMP180 sensorBMP180;   //Creamos el objeto 'sensorBMP180' como una instancia del tipo "SFE_BMP180"

//Creamos el objeto display 4x7
TM1637 display_tm1637(CLK,DIO);


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//     VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

float ALTITUD = 407.0;              //(Sin uso) Altitud de Sorbas en metros
float altitud_estimada;             //(Sin uso) Calculo de altitud estimada en funcion de la presion
float PresionRelativaCotaCero = 0;  //(Sin uso)
float PresionABS = 0;               //(Sin uso) variable para la presion leida del BMP180

float Temperatura = 0;      //variable para la temperatura leida del BMP180
float TempeMAXIMA = -200;   // variable para la temperatura maxima y minima ...
float TempeMINIMA = 200;    // ... inicializadas a fuera de rango :)


volatile boolean FLAG_estado_pulsador = false;    //bandera para el control del estado del pulsador

int SEGUNDOS = 0;     // variables para llevar el control de tiempo de funcionamiento ...
byte MINUTOS = 0;     // ... No se utiliza reloj RTC con lo que los tiempos son aproximados
byte HORAS = 0;       // ...
byte DIAS = 0;        // ...
byte REINICIOS = 0;   // ...



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//     PROTOTIPADO DE FUNCIONES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

int estado_bateria(byte modo=100);  //por defecto devuelve porcentaje de carga




//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup()
{

  /*  programar los dias de uso ante borrados accidentales  */
//  EEPROM.write(MEM_POS_reinicios, 253);
//  EEPROM.write(MEM_POS_dias, 98);
//  EEPROM.write(MEM_POS_horas, 3);
//  while(true){};
  
  REINICIOS = EEPROM.read(MEM_POS_reinicios);
  REINICIOS += 1;
  EEPROM.write(MEM_POS_reinicios, REINICIOS);
  
  DIAS = EEPROM.read(MEM_POS_dias);
  HORAS = EEPROM.read(MEM_POS_horas);
                    
  //apagamos el led 'On Board'
  pinMode(13, OUTPUT);                //PIN13 como salida
  digitalWrite(13, LOW);              //apagar PIN13

  sensorBMP180.begin();
  
  display_tm1637.init();
  display_tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  
  //mostrar version del programa("v2.3")
  display_tm1637.displaySegments(0,SIMBOLO_v);          //letra v 
  display_tm1637.displaySegments(1,SIMBOLO_2);          //numero 2
  display_tm1637.displaySegments(2,SIMBOLO_BARRA_BAJA); //_
  display_tm1637.displaySegments(3,SIMBOLO_3);          //numero 3
  delay(1500);  //pausa de 1'5 segundos
  apagar_display();

  mostar_estado_bateria();
  
  mostar_info_timer();  //reinicios y tiempo de uso
      
  //definimos el pulsador de seleccion para mostrar max y min
  pinMode(PIN_PULSADOR_SELECCION, INPUT);
  
  //recuperar ultimos valores almacenados al reiniciar
  leerEEPROMtempeMaxMin();
  
  leerDatosSensorBMP180();                                //mirar la temperatura actual
  Temperatura = int(Temperatura) + ERROR_TEMPERATURA;     //eliminar decimales y corregir el error del bmp180
  comprobarMaxMin();                                      //comprobar max y minimos por si hay cambios
  mostar_cifra_grados(Temperatura);                       //mostar temperatura actual en el display (muestra el simbolo de º)
  delay(2000);
  apagar_display();
  
  attachInterrupt(digitalPinToInterrupt(PIN_PULSADOR_SELECCION), atenderInterrupcion, RISING); //  FALLING
  
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
  /*LEER LOS SENSORES y ACTUALIZAR VARIABLES*/   
  leerDatosSensorBMP180();  //actualiza Temperatura (y la presion, que no se usa)

  //eliminar decimales y corregir el puto error del bmp180
  Temperatura = int(Temperatura) + ERROR_TEMPERATURA;      

  comprobarMaxMin();        //comprueba cambios en los min/max y grabar datos en la eeprom, si procede

  /*-----  mostar los valores del temperaturas  -----*/
  if(FLAG_estado_pulsador == true){   // una interrupcion por parte del pulsador
    delay(30);
    unsigned long temporizador = 0;   //control de tiempos par en display
    FLAG_estado_pulsador = false;

    /*-----  mostar la temperatura actual  -----*/
    mostar_cifra_grados(Temperatura);       //muestra temperatura con el simbolo de º
    temporizador = millis() + 3000;
    
    /*-----  reset de los valores Min y Max si procede  -----*/
    delay(70);
    byte pulsador = leer_Pulsador(PIN_PULSADOR_SELECCION);
    if (pulsador == 2){
    reset_min_max();
    SEGUNDOS += 1;
    }
    
    while(FLAG_estado_pulsador == false AND millis()<temporizador){
    }
    //sumamos los segundos que tardamos en esta seccion
    if(FLAG_estado_pulsador == false){SEGUNDOS += 3;}
    
    /*-----  mostar la minima  -----*/
    mostar_min_max(TempeMINIMA);
    display_tm1637.displaySegments(0,SIMBOLO_L);
    temporizador = millis() + 3000;
    while(FLAG_estado_pulsador == false AND millis()<temporizador){
    }
    //sumamos los segundos que tardamos en esta seccion
    if(FLAG_estado_pulsador == false){SEGUNDOS += 3;}
    
    /*-----  mostar la maxima  -----*/
    mostar_min_max(TempeMAXIMA);
    display_tm1637.displaySegments(0,SIMBOLO_H);
    temporizador = millis() + 3000;
    while(FLAG_estado_pulsador == false AND millis()<temporizador){
    }       
    //sumamos los segundos que tardamos en esta seccion
    if(FLAG_estado_pulsador == false){SEGUNDOS += 4;}
    
    apagar_display();       //apagar el DISPLAY 7 SEGMENTOS X4        
  }

  /*-----  mostar estado de la carga de la bateria  -----*/
  if(FLAG_estado_pulsador == true){   // nueva interrupcion por parte del pulsador
    delay(70);
    FLAG_estado_pulsador = false;
    mostar_estado_bateria();
    apagar_display();  //por seguridad (es dedundante)
  }
  
  /*-----  mostar reinicios y tiempo de uso  -----*/
  if(FLAG_estado_pulsador == true){   // nueva interrupcion por parte del pulsador
    delay(70);
    FLAG_estado_pulsador = false;
    mostar_info_timer();
  }
  
  /*-----  reset de los valores del timer  -----*/
  if(FLAG_estado_pulsador == true){   // nueva interrupcion por parte del pulsador
    delay(70);
    FLAG_estado_pulsador = false;
    reset_info_timer();
  }


  // Entrar en modo de ahorro de energia durante 128 segundos
  for (int sleep_cicles=0; sleep_cicles<16; sleep_cicles++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    SEGUNDOS += 8;
    if(FLAG_estado_pulsador == true){
      break;
    }
  }

  delay (250);  //esperar 250-500ms para que se activen todos los sensores y evitar rebotes
  
  while(SEGUNDOS>59){
    SEGUNDOS-=60;
    MINUTOS+=1;
  }
  if(MINUTOS>59){
    MINUTOS -= 60;
    HORAS += 1;
    EEPROM.write(MEM_POS_horas, HORAS);
    if(HORAS>23){
      HORAS -= 23;
      DIAS += 1;
      EEPROM.write(MEM_POS_dias, DIAS);
    }
  }
  
}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BLOQUE DE FUNCIONES: LECTURA SENSORES, TOMA DE DECISIONES,...
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    TECLADO / PULSADORES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//  FUNCION PARA CONTROLAR EL ESTADO DEL PULSADOR 
//========================================================
byte leer_Pulsador(byte pin_pulsador)
/*
 * Funcion para atender la pulsacion de la tecla OK/seleccion
 */
{
  boolean FLAG_pulsacion = false;
  unsigned long momento_pulsar_boton = 0;
  unsigned long momento_soltar_boton = 0;
  byte tipo_pulsacion = 0;                        // no ha habido pulsacion 
    
  boolean pulsacion = digitalRead(pin_pulsador);  // leemos el estado del pulsador
  if (pulsacion==true){   
    momento_pulsar_boton = millis();              //'anotamos' el momento de la pulsacion
    delay(25);                                    //pausa para evitar rebotes
    while(pulsacion==true){
      pulsacion = digitalRead(pin_pulsador);      // leemos el estado del pulsador
    }
    momento_soltar_boton = millis();
    FLAG_pulsacion = true; 
  }
  
  unsigned long duracion_pulsacion = momento_soltar_boton - momento_pulsar_boton;
  //determinar la duracion de pulsacion para saber si es normal o larga

  if (FLAG_pulsacion==true){          //hay pulsacion...
    if (duracion_pulsacion >= 450){  
      tipo_pulsacion = 2;             //pulsacion larga
    }
    else{
      tipo_pulsacion = 1;             //pulsacion normal/corta
    }
  }
  return tipo_pulsacion;
}




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//  CONTROL DE INTERRUPCIONES 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//  FUNCION ATENDER INTERRUPCIONES DEL PUSADOR
//========================================================
void atenderInterrupcion()
{
  FLAG_estado_pulsador = true;
} 




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

  display_tm1637.displaySegments(0,SIMBOLO_NULO);
  display_tm1637.displaySegments(1,SIMBOLO_NULO);
  display_tm1637.displaySegments(2,SIMBOLO_NULO);
  display_tm1637.displaySegments(3,SIMBOLO_NULO);
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


//========================================================
//  FUNCION PARA ESCRIBIR DIGITOS EN EL DISPLAY
//========================================================
void mostar_min_max( int cifra)
{
  /*
   *  OJO el display se enumera de izquierda a derecha (digito 0,1,2,3)
   * permite el acceso a segmentos individuales a traves 
   * de la libreria modificada por mi con:  "xxx.displaySegments('Numero de digito','Segmentos a mostrar')"
   * Esta funcion es mostrar los maximos y minimos,
   * Muestras los datos en los tres digitos de la derecha 
   * siendo de esos 3, el mas a la izquierda el 
   * que mostrara signo si hay valores negativos
   * Se reserva el de la izquierda del display para escribir 
   * el simbolo  L o H segun corresponda a minima o maxima
   */
  int Num = abs(cifra);
  int8_t unidades = Num %10 ;                 //unidades
  int8_t decenas = (Num % 100) / 10 ;         //decenas
  int8_t centenas = (Num % 1000) / 100 ;      //centenas
  int8_t millares = Num  / 1000  ;            //unidades de millar
  
  display_tm1637.displaySegments(0,SIMBOLO_NULO);        //apagar el digito de la izquierda

  if(cifra<0){
    display_tm1637.displaySegments(1,SIMBOLO_NEGATIVO);  //mostar SEG_NEGATIVO
  }
  else{
    display_tm1637.displaySegments(1,SIMBOLO_NULO);      //o apagar ese digito
  }

  display_tm1637.display(2, decenas);
  display_tm1637.display(3, unidades);
}


//========================================================
//  MOSTAR TEMPERATURA CON EL SIMBOLO º
//========================================================
void mostar_cifra_grados( int cifra)
{
  /* 
   * OJO el display los numera de izquierda a derecha (digito 0,1,2,3)
   * permite el acceso a segmentos individuales atraves 
   * de la libreria modificada por mi con: "xxx.displaySegments('Numero de digito','Segmentos a mostrar')"
   * Esta version es para escribir al temperatura mostrando el simbolo de º
   */
  int Num = abs(cifra);
  int8_t unidades = Num %10 ;                 //unidades
  int8_t decenas = (Num % 100) / 10 ;         //decenas
  //int8_t centenas = (Num % 1000) / 100 ;    //centenas
  //int8_t millares = Num  / 1000  ;          //unidades de millar


  if(cifra<0){
    display_tm1637.displaySegments(0,SIMBOLO_NEGATIVO); // signo negativo (-) ...
  }
  else{
    display_tm1637.displaySegments(0,SIMBOLO_NULO);     // ... o apagar ese digito
  }

  display_tm1637.display(1, decenas);                   //decenas
  display_tm1637.display(2, unidades);                  //unidades

  display_tm1637.displaySegments(3,SIMBOLO_GRADO);      //el simbolode grado º
}


//========================================================
//  MOSTAR DATOS DE TIEMPOS Y REINICIOS
//========================================================
void mostar_info_timer()
{
  unsigned long temporizador = 0;   //control de tiempos para en display
  delay(100);
    
  /*-----  mostar nº de reinicios  -----*/
  mostarCifras(REINICIOS);
  display_tm1637.displaySegments(0,SIMBOLO_r);      //letra 'r'
  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();

  /*-----  mostar nº de dias ON  -----*/
  mostarCifras(DIAS);
  display_tm1637.displaySegments(0,SIMBOLO_d);      //letra 'd'
  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();

  /*-----  mostar nº de horas ON  -----*/
  mostarCifras(HORAS);
  display_tm1637.displaySegments(0,SIMBOLO_h);      //letra 'h'
  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();
}


//========================================================
//  RESET DE TIEMPOS Y REINICIOS
//========================================================
void reset_min_max()
{
  TempeMAXIMA = -200;   //variables para la temperatura maxima y minima
  TempeMINIMA = 200;    //inicializadas a valores actualizables :)
  display_tm1637.displaySegments(0,SIMBOLO_NEGATIVO);  // signo NEGATIVO
  display_tm1637.displaySegments(1,SIMBOLO_NEGATIVO);  // signo NEGATIVO
  display_tm1637.displaySegments(2,SIMBOLO_NEGATIVO);  // signo NEGATIVO
  display_tm1637.displaySegments(3,SIMBOLO_NEGATIVO);  // signo NEGATIVO
  comprobarMaxMin();                        //comprueba cambios en los min/max y grabar datos en la eeprom, si procede
  delay(750);
}


//========================================================
//  RESET DE TIEMPOS Y REINICIOS
//========================================================
void reset_info_timer()
{
 /* muestra en display  (rc ?)   */
  display_tm1637.displaySegments(0,SIMBOLO_r);          // letra 'r'
  display_tm1637.displaySegments(1,SIMBOLO_c);          // letra 'c'
  display_tm1637.displaySegments(2,SIMBOLO_NULO);       // caracter "vacio/borrado"
  display_tm1637.displaySegments(3,SIMBOLO_INTERROGA);  // ?
  
  /*-----  reset de los contadores si procede  -----*/
  delay(2000);
  SEGUNDOS += 2;
  
  byte pulsador = leer_Pulsador(PIN_PULSADOR_SELECCION);
  
  if (pulsador == 2){
    REINICIOS = 0;
    DIAS = 0;
    HORAS = 0;
     
    EEPROM.write(MEM_POS_reinicios, 0);
    EEPROM.write(MEM_POS_dias, 0);
    EEPROM.write(MEM_POS_horas, 0);
    
    /* muestra en display  (oooo)   */ 
    display_tm1637.displaySegments(0,SIMBOLO_o);  //letra 'o'
    display_tm1637.displaySegments(1,SIMBOLO_o);  
    display_tm1637.displaySegments(2,SIMBOLO_o);
    display_tm1637.displaySegments(3,SIMBOLO_o);
  
    delay(2000); 
    apagar_display();
    SEGUNDOS += 3;
  }
  
  apagar_display();
  
}




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    TERMOMETROS  (CONSULTA DE SENSORES Y ACTUALIZACION DE VARIABLES)
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//  TERMOMETRO / BAROMETRO, usando sensor BMP180
//========================================================
void leerDatosSensorBMP180()
{
  char estado;
  double T,P,p0,a;
  boolean FLAG_error_presion = false;
  boolean FLAG_error_temperatura = false;

  /* Primero se debe hacer una lectura de la temepratura para poder hacer una medida de presion.
  Se inicia el proceso de lectura de la temperatura.
  Si se realiza sin errores, se devuelve un numero de (ms) de espera, si no, la funcion devuelve 0.
  */
  
  estado = sensorBMP180.startTemperature();
  if (estado != 0)
  {
    // pausa para que se complete la medicion en el sensor.
    delay(estado);

    // Obtencion de la medida de temperatura que se almacena en T:
    // Si la lectura el correcta la funcion devuelve 1, si se producen errores, devuelve 0.

    estado = sensorBMP180.getTemperature(T);
    if (estado != 0)
    {
      Temperatura = T;  //Asignacion a la variable global Temperatura
      
      /* Se inicia el proceso de lectura de la presion.
         El parametro para la resolucion de muestreo varia de 0 a 3 (a mas resolucion, mayor tiempo necesario).
         Si se realiza sin errores, se devuelve un numero de (ms) de espera, si no, la funcion devuelve 0.
      */

      estado = sensorBMP180.startPressure(3);
      if (estado != 0)
      { 
        delay(estado); // pausa para que se complete la medicion en el sensor.
        
        // Obtencion de la medida de Presion que se almacena en P:
        // Si la lectura el correcta la funcion devuelve 1, si se producen errores, devuelve 0.
        estado = sensorBMP180.getPressure(P,T);

        if (estado != 0)
        {
          PresionABS = P;  //Asignacion a variable global

          /* 
          El sensor devuelve presion absoluta. Para compensar el efecto de la altitud
          usamos la funcion interna de la libreria del sensor llamada: 'sealevel'
          P = presion absoluta en (mb) y ALTITUD = la altitud del punto en que estomos (m).
          Resultado: p0 = presion compensada a niveldel mar en (mb)
          */

          p0 = sensorBMP180.sealevel(P,ALTITUD); // 407 metros (SORBAS)
          PresionRelativaCotaCero= p0;  //Asignacion a variable global
        }
        else FLAG_error_presion = true;
      }
      else FLAG_error_presion = true;
    }
    else FLAG_error_temperatura = true;
  }
  else FLAG_error_temperatura = true;
}




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    GESTION DE INFORMACION EN LA EEPROM (Max, Min)
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//  FUNCION PARA ACTUALIZAR MAX y MIN
//========================================================

void comprobarMaxMin()
{ 
  byte dato = 0;  //para el control de signo 
  if (Temperatura < TempeMINIMA){
    TempeMINIMA = Temperatura;    
    writeEEPROM_signo(MEM_POS_tempe_MIN, TempeMINIMA);
    }
  if (Temperatura > TempeMAXIMA){
    TempeMAXIMA = Temperatura;
    writeEEPROM_signo(MEM_POS_tempe_MAX, TempeMAXIMA);
    }
}


//========================================================
// ESCRIBIR  EN LA EEPROM CON SIGNO
//========================================================

void writeEEPROM_signo(int posicion, int valor)
{
  byte dato = byte (abs(valor));
  if(valor<0){
    dato+=128;
  }
  EEPROM.write(posicion, dato);
}


//========================================================
// LEER  DE LA EEPROM CON SIGNO
//========================================================

int readEEPROM_signo(int posicion)
{
  int dato = EEPROM.read(posicion);
  if(dato > 128){         //si es mayor de 128, sabemos que es negatido
    dato -= 128;          //restamos 128 para encontrar el valor real
    dato *= -1;           //le asignamos el signo negativo que le corresponde
  }
  return dato;
}


//========================================================
// LEER DE EEPROM temperaturas MAX y MIN
//========================================================

void leerEEPROMtempeMaxMin()
{
  //recuperar ultimos valores almacenados al reiniciar
  TempeMINIMA = readEEPROM_signo(MEM_POS_tempe_MIN);
  TempeMAXIMA = readEEPROM_signo(MEM_POS_tempe_MAX);
}




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//     CONTROL DEL ESTADO DE LA BATERIA
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//   FUNCION PARA MONITORIZAR EL ESTADO DE LA BATERIA
//========================================================

int estado_bateria(byte modo) 
{
  /* leer la referecia interna de  1.1V  para calcular Vcc */
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0) ;
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2);                         // pausa para que Vref se estabilice
  ADCSRA |= _BV(ADSC);              // iniciar medicion
  while (bit_is_set(ADCSRA,ADSC));  // proceso de medicion propiamente dicho
 
  uint8_t low  = ADCL; // leer ADCL
  uint8_t high = ADCH; // leer ADCH
 
  long lecturaACD = (high<<8) | low;
 
  long milivoltios = 1125300L / lecturaACD;           // Calcular Vcc en mV (1125300 = 1.1*1023*1000)
  float voltaje = milivoltios/1000.0;                 // estado de la bateria en mV 
  float porcentage_carga = (100-(4.2-voltaje)*100);   // una LiPo al 100% tiene 4'2V, al 0% --> 3'2V
  
  if(porcentage_carga<0){ porcentage_carga = 0;}      // ojito la bateria estaria por debajo de 3'2 voltios
  if(porcentage_carga>100){ porcentage_carga = 100;}  // ojito la bateria estaria por encima de 4'2 voltios

  if(modo == 0){ return int(milivoltios); }           // Voltaje de la bateria en milivoltios

  if(modo > 0){ return int(porcentage_carga); }       // carga restante en porcentaje  
}



//========================================================
//   MOSTRAR EL ESTADO DE LA BATERIA EN EL DISPLAY
//========================================================

void mostar_estado_bateria()
{
  unsigned long temporizador = 0;         //control de tiempos para en display
  
  int carga = estado_bateria();           //peticion del estado de la bateria (en %)
  carga = abs(carga);                     //por seguridad, pero nunca debe devolver un numero negativo

  int8_t decenas = (carga % 100) / 10 ;   //decenas (del porcentaje)
  int8_t unidades = carga %10 ;           //unidades (del porcentaje)

  
  if(carga<=99){ 
    display_tm1637.display(0, decenas);   //decenas de la cifra de porcentaje
    display_tm1637.display(1, unidades);  //unidades de la cifra de porcentaje
    display_tm1637.displaySegments(2, SIMBOLO_NULO); 
    }
  else{ //si por casualidad el porcentaje es mayor de 99, mostramos '100'
    display_tm1637.displaySegments(0, SIMBOLO_1);   // 
    display_tm1637.displaySegments(1, SIMBOLO_0);   //      | --->  100
    display_tm1637.displaySegments(2, SIMBOLO_0);   //
    }
    
  display_tm1637.displaySegments(3, SIMBOLO_PORCENTAJE); // simbolo '%' 

  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  
  //sumamos los segundos que tardamos en esta seccion
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();

  
  int voltaje = estado_bateria(0);            //peticion del estado de la vateria (en mV)
  voltaje = abs(voltaje);                     //por seguridad, pero nunca debe devolver un numero negativo

  int8_t millares = voltaje  / 1000  ;        //unidades de voltio
  int8_t centenas = (voltaje % 1000) / 100 ;  //decimas de voltio
  //decenas  = (voltaje % 100) / 10 ;         //sin uso (representa centesimas de voltio)
  //unidades = voltaje %10 ;                  //sin uso (representa milesimas de voltio)

  byte listaVoltios[6] = {SIMBOLO_0, SIMBOLO_1,SIMBOLO_2,SIMBOLO_3,SIMBOLO_4,SIMBOLO_5};
  
  display_tm1637.displaySegments(0, SIMBOLO_NULO);         //digito apagado
  byte voltios = listaVoltios[millares] + SIMBOLO_PUNTOS;  //union de 'voltios' y los ':'
  display_tm1637.displaySegments(1, voltios);              //voltios (parte entera)            
  display_tm1637.display(2, centenas);                     //voltios (parte decimal)  
  display_tm1637.displaySegments(3, SIMBOLO_v);            //letra 'v'
  
  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  
  //sumamos los segundos que tardamos en esta seccion
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();
}



//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************



