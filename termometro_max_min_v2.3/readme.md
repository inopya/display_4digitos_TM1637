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
