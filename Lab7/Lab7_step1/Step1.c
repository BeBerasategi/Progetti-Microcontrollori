// Sonar using RC3, port 3. 

// Analog Voltage Output: This pin outputs an analog 
// voltage scaled representation of the distance with 
// a scale factor of VCC/1024 per 5 mm.


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

// Global variables --------------------------
volatile unsigned  adc_10bit = 0;
volatile unsigned  adc_8bit = 0;
volatile unsigned short int lcd_refresh = 0;
// --------------------------------------------

void main() {

    char num_txt[7];
    char txt[17];

    // Set Pin3 - RC3 as an analog input.
    ANSELC.RC3 = 1;
    TRISC.RC3 = 1; 
    
    // Set Pin4 - RC6 to 1 (free run)
    TRISC.RC6 = 0;
    LATC.RC6 = 1;

    // Configure PORTD for debug output:
    TRISD = 0;
    LATD = 0;

    // Set the ADC: RC3 = AN15.
    ADCON0 = 0b00111101;
    // For simulations in proteus: AN0:
    // ADCON0 = 0b00000001;

    // ADCON1
    ADCON2 = 0b00100001;

    // Set the ADC interrupt
    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;

    // -----------LCD Initialization---------
    Lcd_Init();		            // Init the display library (This will set also the PORTB Configuration)

    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------

    // From the correction:
    Lcd_Out(1,1,"Waiting CCP ...");	// Print default first line (CCP)
    Lcd_Out(2,1,"Waiting ADC ...");	// Print default second line (ADC)


    INTCON.GIE = 1;
    ADCON0.GO_NOT_DONE = 1; // Start ADC Acquisition

    while (1){
        if (lcd_refresh){
            // Print values in the ADC:
            // adc_10bit = (adc_10bit << 2) | (adc_8bit >> 6); // Long thing to do... better out of the interrupt.
            IntToStr(adc_10bit*5, num_txt); 
            // adc_10bit*5 = distance [mm]. Why??
            lcd_refresh = 0;
            strcpy(txt,"ADC[mm] =");
            strcat(txt,num_txt);
            Lcd_Out(1,1,"CCP is working!");
            Lcd_Out(2,1,txt);
            ADCON0.GO_NOT_DONE = 1; 
        }
    }

}

void interrupt(){ // ISR
    if (PIR1.ADIF){
        PIR1.ADIF = 0; 

        LATD = ADRESH;
        adc_10bit = ADRESH;
        adc_8bit = ADRESL;

        // In the correction they do directly:
        adc_10bit = (adc_10bit << 2) | (adc_8bit >> 6);

        lcd_refresh = 1;
    }

}