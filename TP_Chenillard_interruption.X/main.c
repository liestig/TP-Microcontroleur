/* 
 * File:   main.c
 * Author: Lies
 *
 * Created on January 26, 2024, 8:56 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "leds.h"
#include "timer0.h" // Inclure le fichier d'en-t�te pour l'initialisation du Timer0
#include "button.h"
//#include "i2c.h"
//#include "lcd.h"


// PIC18F46K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO7    // Oscillator Selection bits (Internal oscillator block, CLKOUT function on OSC2)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
volatile unsigned char marche_arret = 0;

void __interrupt() isr(void) {
    if (INTCONbits.TMR0IF) {
        // R�initialiser le drapeau d'interruption du Timer 0
        INTCONbits.TMR0IF = 0;

        // Mettre en marche ou arr�ter le Timer 0 en fonction de la variable marche_arret
        if (marche_arret == 1) {
            T0CONbits.TMR0ON = 1;  // Mettre en marche le Timer 0
        } else {
            T0CONbits.TMR0ON = 0;  // Arr�ter le Timer 0
        }
    }

    if (INTCONbits.INT0IF) {
        // R�initialiser le drapeau d'interruption externe INT0
        INTCONbits.INT0IF = 0;

        // Inverser la valeur de la variable marche_arret
        marche_arret = !marche_arret;
    }
}

void turnOnLED(int ledNumber) {
    switch (ledNumber) {
        case 0:
            LED0_STATE = 1;
            break;
        case 1:
            LED1_STATE = 1;
            break;
        case 2:
            LED2_STATE = 1;
            break;
        case 3:
            LED3_STATE = 1;
            break;
        default:
            // Invalid LED number
            break;
    }
}

void delay_1s_with_timer() {
    while (!INTCONbits.TMR0IF) {
        // Attendez que le flag d'overflow du Timer0 soit d�fini
    }
    INTCONbits.TMR0IF = 0; // R�initialisez le flag d'overflow pour la prochaine temporisation
    TMR0L = 49911 & 0xFF;   // Les 8 bits les moins significatifs
    TMR0H = (49911 >> 8) & 0xFF;  // Les 8 bits les plus significatifs
    }

void turnOffLEDs() {
    LED0_STATE = 0;
    LED1_STATE = 0;
    LED2_STATE = 0;
    LED3_STATE = 0;
}

int main() {
    Nop();
    LED0_DIR = 0; // Set RB0 as output
    LED1_DIR = 0; // Set RB1 as output
    LED2_DIR = 0; // Set RB2 as output
    LED3_DIR = 0; // Set RB3 as output

    turnOffLEDs(); // Assurer que toutes les LED sont initialement �teintes

    initTimer0(); // Configurer le Timer0
    initButton(); // Configurer l'interruption associ�e au bouton poussoir

    while (1) {
        for (int i = 0; i < 4; i++) {
            turnOnLED(i);
            delay_1s_with_timer(); // Utilisez le Timer0 pour la temporisation
            turnOffLEDs();
        }
    }

    return 0;
}