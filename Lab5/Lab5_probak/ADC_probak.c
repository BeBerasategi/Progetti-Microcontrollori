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

unsigned short conv_done=0;
int adc;
int adc_10;

void main(){

    char txt[7];
    char res_text[17];


    // ----- Output ------
    TRISC = 0;
    TRISD = 0;
    // -------------------
    
    // Use PortB as an input
    ANSELB &= 0b00111111;
    TRISB |= 0b11000000;

    // -- Set CHS (AN0) --
    ADCON0.CHS0 = 0;
    ADCON0.CHS1 = 0;
    ADCON0.CHS2 = 0;
    ADCON0.CHS3 = 0;
    ADCON0.CHS4 = 0;
    // -------------------

    // ----- Set TAD -----
    // TAD (1;25) us
    // set TAD min = 1us


    //Fosc = 8MHz TAD
    // Fosc/8
    // ADCS = 001
    ADCON2.ADCS0 = 1;
    ADCON2.ADCS1 = 0;
    ADCON2.ADCS2 = 0;
    // -------------------

    // --- Set ACQT -----
    // TACQ = 7.45 us
    // TACQTmin = 8 TAD
    ADCON2.ACQT0 = 0;
    ADCON2.ACQT1 = 0;
    ADCON2.ACQT2 = 1;
    // -------------------

    // --- Just. Left ---
    ADCON2.ADFM = 0;
    // -------------------

    // ----- ADC ON -------
    ADCON0.ADON = 1;
    // -------------------

    Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)

    Lcd_Cmd(_LCD_CLEAR);    	// Clear display  
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF


   // Interrupt configuration
    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;
    INTCON.GIE = 1;


    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;


    
    //Lcd_Out(1,1,"Ciao");



    while(1){
      if (conv_done==1){
            conv_done = 0;
            LATD = adc_10>>2;
            strcpy(res_text,"RA0[mV]=");
            IntToStr(adc_10*5,txt);
            strcat(res_text, txt);
            Lcd_Out(1,1,res_text);
            ADCON0.CHS0 = 0;

            
      }
      else if (conv_done==2){
            conv_done = 0;
            LATC = adc;
            strcpy(res_text,"RA1[mV]=");
            IntToStr(adc*20,txt);
            strcat(res_text, txt);
            Lcd_Out(2,1,res_text);
            ADCON0.CHS0 = 1;


      }
      ADCON0.GO_NOT_DONE = 1;

      
    }

}

void interrupt(){
     if (PIR1.ADIF){ // ADC conversion finished
        if (ADCON0.CHS0==1){ //AN1
            adc_10 = (ADRESH<<2)+(ADRESL>>6);
            conv_done = 1;
        }
        else if (ADCON0.CHS0==0){ //AN0
            adc = ADRESH;
            conv_done = 2;
        }

        PIR1.ADIF = 0;
	}

}