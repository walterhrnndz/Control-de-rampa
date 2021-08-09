// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#include <xc.h>

#define _XTAL_FREQ 4000000

// Variables control

int activado = 0;
int MAX_TMR = 500;
int cont_tmr = 0;
int cantSube = 0;
int cantBaja = 0;

void __interrupt() ISR() {
    // Procesa la interrupcion del timer0
    if(INTCONbits.T0IF) {
        cont_tmr++;
        TMR0 = 60;
        INTCONbits.T0IF = 0;
    }
    
    // Si se cumplio el tiempo de espera resetea las salidas y las variables
    // de control
    if (cont_tmr > MAX_TMR) {
        GPIO0 = 0;
        GPIO1 = 0;
        cont_tmr = 0;
        cantSube = 0;
        cantBaja = 0;
        activado = 0;
    }
    
    // Procesa interrupcion  pin GPIO5
    if (INTCONbits.GPIF && GPIObits.GP5) {
        __delay_ms(150); // Retardo antirrebote
        if (GPIObits.GP5) {
            INTCONbits.GPIF = 0; // Apago el flag

            if (activado == 0) {
                // Si estan desactivadas las salidas enciende la salida 0 y suma 1
                GPIObits.GP0 = 1; 
                activado = 1;
                cantSube++;
                cont_tmr = 0;
            } else {
                // Si esta activada la salida y ya hay mas de un vehiculo subiendo
                // suma 1 para que se apague la salida 0 cuando hayan salido todos
                // resetea el tiempo del timer
                if (cantSube > 0) {
                    cantSube++;
                    cont_tmr = 0;
                }


                // Si hay vehiculos bajando resta 1
                if(cantBaja > 0) {
                    cantBaja--;
                    // Cuando llega a 0 apaga la salida 0 y desactiva el flag
                    if (cantBaja == 0) {
                        GPIObits.GP1 = 0;
                        activado = 0;
                    }
                }
            }
        }
        while(GPIObits.GP5) {
            
        }
    }
    
    // Procesa interrupcion pin GPIO4
    if (INTCONbits.GPIF && GPIObits.GP4) {
        __delay_ms(150); // Retardo antirrebote
        if (GPIObits.GP4) {
            INTCONbits.GPIF = 0; // Apago el flag

            if (activado == 0) {
                // Si estan desactivadas las salidas enciende la salida 1 y suma 1
                GPIObits.GP1 = 1; 
                activado = 1;
                cantBaja++;
                cont_tmr = 0;
            } else {
                // Si hay vehiculos subiendo resta 1
                if(cantSube > 0) {
                    cantSube--;
                    // Cuando llega a 0 apaga la salida 0 y desactiva el flag
                    if (cantSube == 0) {
                        GPIObits.GP0 = 0;
                        activado = 0;
                    }
                }


                // Si esta activada la salida y ya hay mas de un vehiculo bajando
                // suma 1 para que se apague la salida 0 cuando hayan salido todos
                // resetea el tiempo del timer
                if (cantBaja > 0) {
                    cantBaja++;
                    cont_tmr = 0;
                }
            }
        }
        while(GPIObits.GP4) {
            
        }
    }
}

void main(void) {
    
    // Configuracion del registro para usar el timer0 con el prescaler
    OPTION_REGbits.T0CS = 0; // Usa el clock interno
    OPTION_REGbits.PSA = 0;  // Activar el preescaler
    OPTION_REGbits.PS0 = 1;  // PS0 - PS2 en 1 = preescaler en 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;
    
     // Desactiva interrupciones globales para hacer las configuraciones
    INTCONbits.GIE = 0;
    
    // Bits de interrupcion para las entradas
    INTCONbits.GPIE = 1; // Habilitacion de las interrupciones IOC 
    INTCONbits.GPIF = 0; // Reset del flag
    IOCbits.IOC2 = 1;
    IOCbits.IOC3 = 1;
    IOCbits.IOC4 = 1;
    IOCbits.IOC5 = 1;
    
    // Bits de interrupcion del timer0
    INTCONbits.T0IE = 1; // Habilitacion de la interrupcion TMR0
    INTCONbits.T0IF = 0; // Reset del flag
    
    // Activa las interrupciones globales
    INTCONbits.GIE = 1;
    
    // Calibrar el oscilador interno
    //0x18
    
    // IO = 0x3C
    TRISIObits.TRISIO0  = 0; // 0b00111100 - GP 5, 4, 3, 2 entrada, 1 y 0 salida
    TRISIObits.TRISIO1  = 0;
    TRISIObits.TRISIO2  = 0;
    TRISIObits.TRISIO3  = 1;
    TRISIObits.TRISIO4  = 1;
    TRISIObits.TRISIO5  = 1;
    
    TMR0    = 0;
    
    CMCON   = 0x07; // Deshabilita el comparador analogico
    ANSEL   = 0;
    GPIO    = 0;
    
    while(1) {
        
    }
}