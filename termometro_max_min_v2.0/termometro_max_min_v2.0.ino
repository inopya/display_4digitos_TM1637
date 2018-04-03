/*
 #       _\|/_   A ver..., ¿que tenemos por aqui?
 #       (O-O)
 # ---oOO-(_)-OOo---------------------------------

 ##########################################################
 # ****************************************************** #
 # *             DOMOTICA PARA PRINCIPIANTES            * #
 # *       Termometro de max y min de bajo consumo      * #
 # *            Autor: Eulogio López Cayuela            * #
 # *                                                    * #
 # *          Versión 2.0    Fecha: 21/01/2018          * #
 # ****************************************************** #
 ##########################################################



 ===== NOTAS IMPORTANTES / OPCIONES DISPONIBLES ===== 

 9- Implementando temporizadores para llevar la cuenta de tiempo de funcionamiento
   (por supuesto como es con temporizadores no es fiable)
 
 8- Implemetado control de reinicios por falta de alimentacion.
   
 7- se muestra la version del programa en el display al reiniciar.
 
 
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

#define MEM_POS_horas     6     //posicion de la eeprom para almacenar las horas ON
#define MEM_POS_dias      7     //posicion de la eeprom para almacenar los dias ON
#define MEM_POS_reinicios 0     //posiciond e la eeprom para almacenar los datos de cuelgues

#define MEM_POS_tempe_MIN 8     //posiciond e la eeprom para almacenar los datos de min
#define MEM_POS_tempe_MAX 9     //posiciond e la eeprom para almacenar los datos de max

#define ERROR_TEMPERATURA 3     //valor que se suma a la lectura de mi sensor BMP180
                                //parece tener una desviacion de -3 grados
                                //(hecho esto, es bastante preciso)


#define PIN_LED 13              //led OnBoard de Arduino UNO

#define PIN_PULSADOR_SELECCION 2    //pin para un pulsador tactil (o de otro tipo)

#define CLK 11                      //definicion de pines de datos para el TM1637    
#define DIO 10                      //Se pueden cambiar a otros si se desea


//definiciones para los segmentos del display
#define SEG_A 0b00000001            //   -- A --
#define SEG_B 0b00000010            //  |      |
#define SEG_C 0b00000100            //  F      B
#define SEG_D 0b00001000            //   -- G --
#define SEG_E 0b00010000            //  E      C
#define SEG_F 0b00100000            //  |      |
#define SEG_G 0b01000000            //   -- D --

#define SEG_NEGATIVO 0b01000000     //  -  por comodidad al nombrarlo, (equivale a un SEG_G)
#define SEG_PUNTOS 0b10000000       //  :  solo es valido para el digito 1
#define SEG_GRADO 0b01100011        //  º
/*           0b 0GFEDCBA   */
#define SEG_L 0b00111000            //  muestra una L
#define SEG_H 0b01110110            //  muestra una H


//------------------------------------------------------
// Importamos las librerias necesarias
//------------------------------------------------------
#include <Wire.h>           //libreria para comunicaciones I2C
#include <SFE_BMP180.h>     //libreria del sensor de presion y temperatura
#include <EEPROM.h>         //libreria para el control de la EEPROM
#include <LowPower.h>       //libreria para el uso del modo sleep
#include <TM1637.h>         //libreria para el uso del display 7 segmentos x4


//------------------------------------------------------
// Creamos las instancia de los objetos:
//------------------------------------------------------

//Creamos el objeto sensor BMP180
SFE_BMP180 sensorBMP180;   //Creamos el objeto 'sensorBMP180' como una instancia del tipo "SFE_BMP180"

//Creamos el objeto display 4x7
TM1637 display_tm1637(CLK,DIO);

//------------------------------------------------------
// Variables GLOBALES
//------------------------------------------------------

float ALTITUD = 407.0;      // Altitud de Sorbas en metros
float altitud_estimada;     //guarda el calculo de altitud estimada en funcion de la presion Sin utilidad practica
float PresionRelativaCotaCero = 0;
float PresionABS = 0;       //variable para la presion leida del BMP180

float Temperatura = 0;      //variable para la temperatura leida del BMP180
float TempeMAXIMA = -200;   //variables para la temperatura maxima y minima
float TempeMINIMA = 200;    //inicializadas a valores actualizables :)



volatile boolean FLAG_estado_pulsador = false;
int SEGUNDOS = 0;
byte MINUTOS = 0;
byte HORAS = 0;
byte DIAS = 0;
byte REINICIOS = 0;





//*******************************************************
//         FUNCION DE CONFIGURACION
//*******************************************************

void setup()
{

  /* por si queremos programar el tiempo de uso ante borrados accidentales  */
//  EEPROM.write(MEM_POS_reinicios, 255);  // ai en el siguente reinicio el contador se pone a CERO
//  EEPROM.write(MEM_POS_dias, 70);
//  EEPROM.write(MEM_POS_horas, 8);
//  while(true){};
  
  REINICIOS = EEPROM.read(MEM_POS_reinicios);
  REINICIOS += 1;
  EEPROM.write(MEM_POS_reinicios, REINICIOS);
  
  DIAS = EEPROM.read(MEM_POS_dias);
  HORAS = EEPROM.read(MEM_POS_horas);
                    
  //apagamos el led 'On Board'
  pinMode(13, OUTPUT);                //PIN13 como salida
  digitalWrite(13, LOW);              //apagar PIN13  DISPLAY_POWER

  sensorBMP180.begin();
  
  display_tm1637.init();
  display_tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  
  //mostrar version del programa("v3.0")
  int simbolo = SEG_C + SEG_D + SEG_E;      //v simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(0,simbolo);//simbolo 
  display_tm1637.display(1,2);              //numero 2
  display_tm1637.displaySegments(2,SEG_D);  //_
  display_tm1637.display(3,0);              //numero 0
  delay(1500);  //1500
  apagar_display();
  
  mostar_info_timer();  //reinicios y tiempo de uso
      
  //definimos el pulsador de seleccion para mostrar max y min
  pinMode(PIN_PULSADOR_SELECCION, INPUT);
  
  //recuperar ultimos valores almacenados al reiniciar
  leerEEPROMtempeMaxMin();
  
  
  //display_tm1637.clearDisplay();
  
  leerDatosSensorBMP180();              //mirar la temperatura actual
  Temperatura = int(Temperatura) + ERROR_TEMPERATURA;      //eliminar decimales y corregir el puto error del bmp180
  comprobarMaxMin();                    //comprobar max y minimos por si hay cambios
  mostar_cifra_grados(Temperatura);     //mostar temperatura actual en el display v3 muestra el simbolo de º
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
    display_tm1637.displaySegments(0,SEG_L);
    temporizador = millis() + 3000;
    while(FLAG_estado_pulsador == false AND millis()<temporizador){
    }
    //sumamos los segundos que tardamos en esta seccion
    if(FLAG_estado_pulsador == false){SEGUNDOS += 3;}
    
    /*-----  mostar la maxima  -----*/
    mostar_min_max(TempeMAXIMA);
    display_tm1637.displaySegments(0,SEG_H);
    temporizador = millis() + 3000;
    while(FLAG_estado_pulsador == false AND millis()<temporizador){
    }       
    //sumamos los segundos que tardamos en esta seccion
    if(FLAG_estado_pulsador == false){SEGUNDOS += 4;}
    
    apagar_display();       //apagar el DISPLAY 7 SEGMENTOS X4        
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
//  BLOQUE DE FUNCIONES: LECTURA SENSORES, TOMA DE DECISIONES, ACCESO EEPROM...
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    TECLADO
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
//  INTERRUPCIONES
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


//========================================================
//  FUNCION PARA ESCRIBIR DIGITOS EN EL DISPLAY
//========================================================
void mostar_min_max( int cifra)
{
  /*
   *  OJO el display se enumera de izquierda a derecha (digito 0,1,2,3)
   * permite el acceso a segmentos individuales atraves 
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
  
  display_tm1637.displaySegments(0,0);        //apagar el digito de la izquierda

  if(cifra<0){
    display_tm1637.displaySegments(1,SEG_G);  //mostar SEG_NEGATIVO
  }
  else{
    display_tm1637.displaySegments(1,0);      //o apagar ese digito
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
  int8_t unidades = Num %10 ;               //unidades
  int8_t decenas = (Num % 100) / 10 ;       //decenas
  //int8_t centenas = (Num % 1000) / 100 ;    //centenas
  //int8_t millares = Num  / 1000  ;          //unidades de millar


  if(cifra<0){
    display_tm1637.displaySegments(0,SEG_G);      //mostar SEG_NEGATIVO
  }
  else{
    display_tm1637.displaySegments(0,0);          //o apagar ese digito
  }

  display_tm1637.display(1, decenas);             //decenas
  display_tm1637.display(2, unidades);            //unidades

  display_tm1637.displaySegments(3,SEG_GRADO);    //el simbolode grado º
}


//========================================================
//  MOSTAR DATOS DE TIEMPOS Y REINICIOS
//========================================================
void mostar_info_timer()
{
  int simbolo = 0;
  unsigned long temporizador = 0;   //control de tiempos par en display
  delay(100);
    
  /*-----  mostar nº de reinicios  -----*/
  mostarCifras(REINICIOS);
  simbolo = SEG_E + SEG_G;                    //r simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(0,simbolo);  //simbolo
  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();

  /*-----  mostar nº de dias ON  -----*/
  mostarCifras(DIAS);
  simbolo = SEG_B + SEG_C + SEG_D + SEG_E + SEG_G;  //d simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(0,simbolo);        //simbolo
  temporizador = millis() + 2000;
  while(FLAG_estado_pulsador == false AND millis()<temporizador){
  }
  if(FLAG_estado_pulsador == false){SEGUNDOS += 2;}
  apagar_display();

  /*-----  mostar nº de horas ON  -----*/
  mostarCifras(HORAS);
  simbolo = SEG_C + SEG_E + SEG_F + SEG_G;    //h simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(0,simbolo);  //simbolo
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
  display_tm1637.displaySegments(0,SEG_G);  //SEG_NEGATIVO
  display_tm1637.displaySegments(1,SEG_G);  //SEG_NEGATIVO
  display_tm1637.displaySegments(2,SEG_G);  //SEG_NEGATIVO
  display_tm1637.displaySegments(3,SEG_G);  //SEG_NEGATIVO
  comprobarMaxMin();                        //comprueba cambios en los min/max y grabar datos en la eeprom, si procede
  delay(750);
}


//========================================================
//  RESET DE TIEMPOS Y REINICIOS
//========================================================
void reset_info_timer()
{
  int simbolo = 0;
  simbolo = SEG_E + SEG_G;                          //r simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(0,simbolo);
  simbolo = SEG_D + SEG_E + SEG_G;                  //c simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(1,simbolo);  
  simbolo = 0;
  display_tm1637.displaySegments(2,simbolo);        // caracter "vacio/borrado"
  simbolo = SEG_A + SEG_B + SEG_E + SEG_G;          //? simbolo compuesto de estos segmentos
  display_tm1637.displaySegments(3,simbolo);
  
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
    simbolo = 0;
    simbolo = SEG_E + SEG_G + SEG_C + SEG_D;          //o simbolo compuesto de estos segmentos
    display_tm1637.displaySegments(0,simbolo);
    display_tm1637.displaySegments(1,simbolo);  
    display_tm1637.displaySegments(2,simbolo);
    display_tm1637.displaySegments(3,simbolo);
  
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
//    GESTION DE Max y Min EN EEPROM
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


//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************

