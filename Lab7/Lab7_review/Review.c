// FIRST STEP: Sonar using RC3, port 3 *************************************

// Analog Voltage Output: This pin outputs an analog 
// voltage scaled representation of the distance with 
// a scale factor of VCC/1024 per 5 mm.

// SECOND STEP: Sonar using RC2, port 2 ************************************

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

volatile unsigned short int adc_ready = 0;
volatile unsigned short int ccp_ready = 0;
volatile unsigned int t_a, t_b;

void main() {
    unsigned int adc_10bit;
    
    // Strings to print in the LCD:
    char txt[17];
    char num_txt[7];
    
    // Set Timer1:
    T1CON.TMR1CS1 = 0;
    T1CON.TMR1CS0 = 0;
    T1CON.T1RD16 = 1;
    T1CON.TMR1ON = 1;

    // Prescaler: 1/2
    T1CON.T1CKPS1 = 0;
    T1CON.T1CKPS0 = 1;

     //T1CON = 0b00010011;

    // Set Pin2 - RC2 as an digital input. IMPORTANT!!!!
    ANSELC.RC2 = 0;
    TRISC.RC2 = 1;

    // Set RC3 as analog input: -> AN15
    TRISC.RC3 = 1;
    ANSELC.RC3 = 1;

    // Set capture for RC2: CCP1-> CCPR1H:CCPR1L
    CCP1CON = 0b00000101; 

    // Timer selection: IMPORTANT TO DO IT!!
    CCPTMRS0.C1TSEL1 = 0;
    CCPTMRS0.C1TSEL0 = 0;
    
    // Rising edge: CCP1CON.CCP1M0 = 1;
    // Falling edge: CCP1CON.CCP1M0 = 0;
    PIE1.CCP1IE = 1;
    PIR1.CCP1IF = 0;

    // Set the sonar in free run:
    TRISC.RC6 = 0;
    LATC.RC6 = 1;

    // Set the ADC: 
    ADCON0 = 0b00111101;
    ADCON2 = 0b00100001;

    // Interrupts:
    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;

    // -----------LCD Initialization---------
    Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)
    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    Lcd_Out(2,1,"Waiting CCP ...");        // Print default first line (CCP)
    Lcd_Out(1,1,"Waiting ADC ...");        // Print default second line (ADC)


    INTCON.GIE = 1;
    ADCON0.GO_NOT_DONE = 1;

    while(1){
        if (adc_ready){
            adc_10bit = (ADRESH<<2)|(ADRESL>>6);
            strcpy(txt,"AVO[mm] =");
            IntToStr(adc_10bit*5,num_txt);
            strcat(txt,num_txt);
            Lcd_Out(1,1,txt);
            adc_ready = 0;
            ADCON0.GO_NOT_DONE = 1;
        }
        else if (ccp_ready){
            strcpy(txt,"PWO[mm] =");
            IntToStr((unsigned int)t_b-t_a,num_txt);
            strcat(txt,num_txt);
            Lcd_Out(2,1,txt);
            ccp_ready = 0;
        }

    }

}

void interrupt(){
    if (PIR1.ADIF){
        adc_ready = 1;
        PIR1.ADIF = 0;
    }
    else if (PIR1.CCP1IF){
        // Rising edge: CCP1CON.CCP1M0 = 1;
        // Falling edge: CCP1CON.CCP1M0 = 0;
        
        if (CCP1CON.CCP1M0 == 1){
            t_a= (CCPR1H<<8)|CCPR1L;
            CCP1CON.CCP1M0 = 0;
        }
        else if (CCP1CON.CCP1M0 == 0){
            t_b = (CCPR1H<<8)|CCPR1L;
            CCP1CON.CCP1M0 = 1;
            ccp_ready = 1;
        }
        PIR1.CCP1IF = 0;
    }

}