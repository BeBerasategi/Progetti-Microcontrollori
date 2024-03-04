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

unsigned short int adc_ready = 0;
unsigned int adc_res = 0;

unsigned short int sonar_ready = 0;

unsigned char reverse(unsigned char b);

void main() {

    unsigned short int conteggio = 0;
    unsigned short int button_pressed = 0;
    unsigned short int refresh_lcd = 0;

    char txt[17];
    char num_txt[7];

    // Etxean probak egiteko
    OSCCON = 0b01100010;

    // On PR RA5 and RA<3:0> are configured as analog inputs
    ANSELA.RA3 = 0;
    ANSELA.RA2 = 0;
    ANSELA.RA1 = 0;

    // Port RA0 for the ADC:
    // ANSELA.RA0 = 1;
    // TRISA.RA0 = 1;

    // For debug:
    TRISD = 0;
    LATD = 0;
    
    // -----------LCD Initialization---------
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF

    Lcd_Out(1,1, "Cont = 0");
    Lcd_Out(2,1, "Waiting ADC...");
    // --------------------------------------

    // ----------------- ADC settings ----------
    // T_AS=2 us
    // T_ACQT ~ 7,45 us
    // 10 bit
    // Use interrupts

    ADCON0 = 0b00000001;
    // ADCON1 = 0b00000000;
    ADCON2 = 0b00010101;

    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;
    // INTCON.GIE = 1;
    // -----------------------------------------

    // ---------------- PWM ---------------------
    // Use CCP5 (RE2) and Timer 6

    // Max period: PreS, PostS = 1/16, PR6 = 255.
    T6CON = 0b01111110;

    TRISE.RE2 = 1;
    CCPTMRS1 = 0b00001000;
    PR6 = 255;
    CCP5CON = 0b00001100;
    CCPR5L = 0;

    TRISE.RE2 = 0;
    // ---------------------------------------------

    // --------------------SONAR-------------------
    // Set Analog Voltage Output: 300 mm - 5000 mm
    // ANSELC.RC3 = 1;
    // Set  AN15:
    // ADCON0 |= 0b00111100 for AN15
    // ADCON0 |= 0b11000011 for AN0

    TRISC.RC6 = 0;
    LATC.RC6 = 1;

    TRISC.RC3 = 1;
    ANSELC.RC3 = 1;

    // --------------------------------------------

    INTCON.GIE = 1;
    ADCON0.GO_NOT_DONE = 1;

    while (1){

        // Polling on RA <3:0>
        if ((PORTA & ~button_pressed) & 0b00000010){ // RA1
            if (conteggio > 0){
                conteggio--;
            }
            button_pressed |= 0b00000010;
            refresh_lcd = 1;
        }
        else if ((PORTA & ~button_pressed) & 0b00000100){ // RA2
            conteggio = 0;
            button_pressed |= 0b00000100;
            refresh_lcd = 1;
        }
        else if ((PORTA & ~button_pressed) & 0b00001000){ // RA3
            if (conteggio < 255){
                conteggio++;
            }
            button_pressed |= 0b00001000;
            refresh_lcd = 1;
        }
        button_pressed &= PORTA;

        if (refresh_lcd){
            strcpy(txt,"Cont =");
            IntToStr((int)conteggio, num_txt);
            strcat(txt, num_txt);
            Lcd_Out(1,1,txt);
            refresh_lcd = 0;
        }
        if (adc_ready && (sonar_ready == 0)){
            adc_res = (ADRESH<<2)|(ADRESL>>6); // BE VERY VERY CAREFUL TO USE THE PARENTHESIS
            strcpy(txt,"ADC[mV] =");
            IntToStr((int)adc_res*5, num_txt);
            strcat(txt, num_txt);
            Lcd_Out(2,1,txt);
            adc_ready = 0;
            ADCON0.GO_NOT_DONE = 1;
        }
        if (adc_ready && sonar_ready){
            CCPR5L = ADRESH;
            LATD = ADRESH;
            adc_res = (ADRESH<<2)|(ADRESL>>6);
            // strcpy(txt,"Sonar =");
            // IntToStr((int)adc_res*5, num_txt);
            // strcat(txt, num_txt);
            // Lcd_Out(1,1,txt);
            ADCON0.GO_NOT_DONE = 1;
            adc_ready = 0;
        }

    }
}

void interrupt(){ // ISR
    if (PIR1.ADIF){
        adc_ready = 1;
        PIR1.ADIF = 0;
        if (ADCON0.CHS0 == 0){ // AN0
            sonar_ready = 0;
            // Change to AN15:
            ADCON0 |= 0b00111100;
        }
        else if (ADCON0.CHS0 == 1){ // AN15
            sonar_ready = 1;
            // Change to AN0:
            ADCON0 &= 0b11000011;
        }
    }


}

unsigned char reverse(unsigned char b) {  // [b7 b6 b5 b4 b3 b2 b1 b0]
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; // [b3 b2 b1 b0 b7 b6 b5 b4]
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2; // [b1 b0 b3 b2 b5 b4 b7 b6]
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1; // [b0 b1 b2 b3 b4 b5 b6 b7]
   return b;
}