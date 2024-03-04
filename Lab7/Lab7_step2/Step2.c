// Sonar using RC2, port 2. 

// Pulse Width Output: 1us per 1mm (300us-5000us).
// Capture module will have to be used, ad 16 bit timer (1/3/5).

// Set a clock with a period of 1us to make conversion easier.

// Lcd module connections
sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End Lcd module connections

// ************************ IMPORTANT NOTE **********************************
// Once you see that the program works, try to integrate also the step1
// This is, the program using the ADC too. Manage the time with adc_ms.
// **************************************************************************

// --------- Global variables -----------
volatile unsigned int ta;
volatile unsigned int tb;
volatile unsigned int tmr1_ms = 0;
volatile unsigned int tmr1_delay = 500;
volatile unsigned int adc_ms = 0;
volatile unsigned int adc_delay = 100;
volatile unsigned  adc_10bit = 0;
volatile unsigned short refresh_CCP = 0;
volatile unsigned short refresh_ADC = 0;
// --------------------------------------

void main() {

    char num_txt[7];
    char txt[17];
    short dir = -1; // 1 -->, -1 <--

    // Set Pin2 - RC2 as an digital input.
    ANSELC.RC2 = 0;
    TRISC.RC2 = 1;

    // Set Pin3 - RC3 as an analog input.
    ANSELC.RC3 = 1;
    TRISC.RC3 = 1; 

    // Set Pin4 - RC6 to 1 (free run)
    TRISC.RC6 = 0;
    LATC.RC6 = 1;

    // Configure PORTD to run Kitt Car using TMR1.
    TRISD = 0;
    LATD = 1;

    // Set the ADC: RC3 = AN15.
    ADCON0 = 0b00111101;
    // ADCON1
    ADCON2 = 0b00100001;

    // Configure Timer 1: 
    /*
    Writes to TMRxH or TMRxL directly update the counter.
    Timer1/3/5 is enabled by configuring the TMRxON and
    TMRxGE bits in the TxCON and TxGCON registers.
    */ 

    // T1CON = 0b00 01 0 0 1 1; From the correction!

    // Clk source = Fosc/4
    T1CON.TMR1CS1 = 0;
    T1CON.TMR1CS0 = 0;

    // Set prescaler to 2:
    T1CON.T1CKPS1 = 0;
    T1CON.T1CKPS0 = 1;

    //  Enable register read/write of Timer1/3/5 in one 16-bit operation
    T1CON.T1RD16 = 1;

    // Timer1 ON:
    T1CON.TMR1ON = 1;

    // Capture module settings: CCP1 is used for RC2.
    CCP1CON.CCP1M3 = 0;
    CCP1CON.CCP1M2 = 1;
    CCP1CON.CCP1M1 = 0;
    CCP1CON.CCP1M0 = 1; // Every rising edge
    // CCP1CON.CCP1M0 = 0; // Every falling edge

    // Timer selection:
    CCPTMRS0.C1TSEL1 = 0;
    CCPTMRS0.C1TSEL0 = 0;

    // IOCB RB7 and RB6 to change kitt delays:
    ANSELB &= 0b00111111;
    // TRSISB |= 0b11000000;
    IOCB |= 0b11000000;

    // Set interrupts:
    PIE1.CCP1IE = 1;
    PIR1.CCP1IF = 0;
    // Also interrupts for TMR1:
    PIE1.TMR1IE = 1;
    PIR1.TMR1IF = 0;
    TMR1H = 0;
    TMR1L = 0;
    // For IOCB:
    INTCON.RBIE = 1;
    INTCON.RBIF = 0;
    // Set the ADC interrupt
    PIR1.ADIF = 0;
    PIE1.ADIE = 1;

    INTCON.PEIE = 1;
    // INTCON.GIE = 1;


    // Results in CCPR1H, CCPR1L.

    Lcd_Init();                           
    Lcd_Cmd(_LCD_CLEAR);          
    Lcd_Cmd(_LCD_CURSOR_OFF);  
    Lcd_Out(1,1,"Waiting CCP ...");        // Print default first line (CCP)
    Lcd_Out(2,1,"Waiting ADC ...");        // Print default second line (ADC)



    INTCON.GIE = 1;
    ADCON0.GO_NOT_DONE = 1; // Start ADC Acquisition

    while(1){
        if (refresh_CCP){
            INTCON.GIE = 0;
            strcpy(txt,"CCP[mm] = "); // 1LSB = 1us = 1mm
            IntToStr((unsigned int)tb-ta,num_txt) ;
            strcat(txt,num_txt);
            Lcd_Out(1,1,txt);
            refresh_CCP = 0;

            INTCON.GIE = 1;
        }
        if ( (adc_ms>adc_delay) && refresh_ADC){
            adc_ms -= adc_delay;
            IntToStr(adc_10bit*5, num_txt); 
            refresh_ADC = 0;
            strcpy(txt,"ADC[mm] = ");
            strcat(txt,num_txt);
            Lcd_Out(2,1,txt);
            ADCON0.GO_NOT_DONE = 1; 
        }

        // Kitt car:
        while (tmr1_ms>tmr1_delay){ // Every 1000ms.
            tmr1_ms -= tmr1_delay;
            // Dir: 1 -->, -1 <--
            if (dir == -1){
                LATD <<= 1;
                if (LATD == 0x80)
                    dir = 1;
            }
            else if (dir == 1){
                LATD >>= 1;
                if (LATD == 0x01)
                    dir = -1;
            }

        }

    }

}

void interrupt(){
    if (PIR1.TMR1IF){
        // IF rised every overflow, every 65535 us.
        // Approximatedly: every 66 ms.
        tmr1_ms += 66;
        adc_ms += 66;
        
        PIR1.TMR1IF = 0;
    }
    else if (PIR1.CCP1IF){
        PIR1.CCP1IF = 0;
        if (CCP1CON.CCP1M0 == 1){ // Rising edge
            ta = (CCPR1H<<8)| CCPR1L;
            CCP1CON.CCP1M0 = 0;
        }
        else if (CCP1CON.CCP1M0 == 0){ // Falling edge
            tb = (CCPR1H<<8)| CCPR1L;
            CCP1CON.CCP1M0 = 1;
            refresh_CCP = 1; // Do it only here!
        }
    }
    else if (PIR1.ADIF){
        PIR1.ADIF = 0; 

        // In the correction they do directly:
        adc_10bit = (ADRESH << 2) | (ADRESL >> 6);
        refresh_ADC = 1;
    }
    else if (INTCON.RBIF){
        if (PORTB.RB7){
            // Faster; decrease delay
            tmr1_delay -= 100;
            if (tmr1_delay < 100){
                tmr1_delay = 100;
            }
        }
        else if (PORTB.RB6){
            // Slower; increase delay
            tmr1_delay += 100;
            if (tmr1_delay > 2000){
                tmr1_delay = 2000;
            }
        }
        INTCON.RBIF = 0;
    }

}