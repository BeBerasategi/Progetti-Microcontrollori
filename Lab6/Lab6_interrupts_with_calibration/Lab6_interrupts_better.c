/*
    CCP5 = RE2.
    TIMER2 -> PR2

    Duty cycle: \delta = CCPR5 / (PR2+1)

    Period:
    T = (PR2+1)*TMR2PS*4/F_osc

    Calibration:
    PR2 = MAX - MIN
    CCPR5 = ADRESH - MIN


*/

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



volatile unsigned int time_mikros = 0; // 0-65535

void main() {
     // For the calibration of the ADC:
      unsigned short max_val = 200;
      unsigned short min_val = 10;
      
      unsigned short button_pressed = 0;
      unsigned short calibration_on = 0;

      char numtotxt[7];
      char txt[17];

    // ---- AN0 = RA0 ----
    // Default is analog input; i.e., digital input buffer off
    // ANSELA.RA0 = 1;
    // ANSELA.RA1 = 1;
    // -------------------


    // - Disaple CCP Out -
    TRISE.RE2 = 1;  // During the configuration of the CCP, we need to DISABLE THE OUTPUT.
                    // This could be dangerous if we think in real systems...
        // -------------------


    // --- TMR2 - CCP5 ---
    CCPTMRS1.C5TSEL1 = 0;
    CCPTMRS1.C5TSEL0 = 0;
        // -------------------


    // --- Set Period ----
    PR2 = 255;
        // -------------------

    // --- PWM on CCP5 ---
    CCP5CON = 0b00001100;
    //CCP5CON.CCP5M3 = 1;
    //CCP5CON.CCP5M2 = 1;
        // -------------------

    // ---- Set Ton ------
        // d = CCPR5L /(PR2 +1)
    CCPR5L = 0; // It will go from 0/256 to 255/256. Then it restarts.
        // -------------------

    // Set the ADC
    // Fast ADC Settings: Fosc = 32MHz (PLLL on)
    ADCON2 = 0b00100010;
    //ADCON1 = 0;
    ADCON0=0b00000001; // Default An0

    // Interrupt for ADC
    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;


    // ---- Set TMR2 -----
    // TMR2 ON, Max Prescaler
    T2CON = 0b00000111;
        // -------------------

        
    // -----------LCD Initialization---------
    Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)

    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------


    Lcd_Out(1,1,"Ciao");

    // Interrupt configuration for TMR2.
    PIE1.TMR2IE = 1;
    PIR1.TMR2IF = 0;
    // INTCON.PEIE = 1;
    // INTCON.GIE = 1;

    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;


    // Which of the following two should be done first?

    // Global interrupt enable
    INTCON.GIE = 1;

    // - Output Enable --
    TRISE.RE2 = 0;
        // -------------------

    // In this version of the project we will find a way to call the ADC interrupts less frequently. 
    // Instead of working in free run, we will use a counter "out_ms" so that only a conversion is done every 100ms.
    // TMR2 will be used to controll that.

    // A calibration option will be given too, using PORTC.RC7.
    // Set is as an input, press once to enter calibration mode, press it again to go out.

    ANSELC &= 0b01111111;
    TRISC |= 0b10000000;
    

    while(1){
        while (time_mikros>10000){ // 10ms have passed
            time_mikros -= 10000;
            CCPR5L = ADRESH - min_val;
            Lcd_Out(1,1,"ADC for PWM:");
            IntToStr((CCPR5L-min_val)*20, numtotxt);
            strcpy(txt,"ADC[mV]=");
            strcat(txt,numtotxt);
            Lcd_Out(2,1,txt);

            ADCON0.GO_NOT_DONE = 1;
        }

        if ((PORTC & ~button_pressed) & 0b10000000){
            calibration_on = ~calibration_on;
            button_pressed |= 0b10000000;
            Lcd_Cmd(_LCD_CLEAR);
        }
        button_pressed &= PORTC;

        while (calibration_on){
            Lcd_Out(1,1,"Calibrating");
            ADCON0.GO_NOT_DONE = 1;
            while (ADCON0.GO_NOT_DONE){
                // Wait;
            }
            if (ADRESH > max_val){
                max_val = ADRESH;
            }
            else if (ADRESH < min_val){
                min_val = ADRESH;
            }

            PR2 = max_val - min_val;
            IntToStr(PR2, numtotxt);
            strcpy(txt,"PR2=");
            strcat(txt,numtotxt);
            Lcd_Out(2,1,txt);

            if ((PORTC & ~button_pressed) & 0b10000000){
                calibration_on = ~calibration_on;
                button_pressed |= 0b10000000;
                Lcd_Cmd(_LCD_CLEAR);
            }
            button_pressed &= PORTC;
        }
    
    }


}

/*
unsigned short map(unsigned short x, unsigned short in_min, unsigned short in_max, unsigned short out_min, unsigned short out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
*/

void interrupt(){
    if (PIR1.TMR2IF){
        PIR1.TMR2IF = 0;
        time_mikros += 512; // We can count up to 65 ms.
    }
    else if (PIR1.ADIF){
        PIR1.ADIF = 0;   // Prima possibile
        // ADCON0.GO_NOT_DONE = 1; // We will set it to 1 every 10ms.
    }
}