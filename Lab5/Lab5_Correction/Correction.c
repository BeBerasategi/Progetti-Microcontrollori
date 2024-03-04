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


signed short int adc_flag = -1; // do NOT print
// using this flag makes the code a bit more human-readable,
// even if a bit more of memory is used
int adc_10bit;          //ADRESH*4 + ADSREL/64 => (ADRESH<<2) + (ADSREL >> 6)
int adc_8bit;           //ADRESH


void main() {

    char txt[17];
    char numtxt[7];

    // ----- Output ------
    TRISC = 0;
    TRISD = 0;
    // -------------------



    // ---- AN0 = RA0 ----
    // Default is analog input; i.e., digital input buffer off
    // ANSELA.RA0 = 1;
    // ANSELA.RA1 = 1;
    // -------------------


    // Fast ADC Settings: Fosc = 8MHz (PLL on)
    ADCON2 = 0b00100001;
    //ADCON1 = 0;
    ADCON0=0b00000001; // Default An0
 
    // LCD
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);    	
    Lcd_Cmd(_LCD_CURSOR_OFF);  

    // Interrupt
    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;
    INTCON.GIE = 1;

    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;


    while(1){

            // Print AN0
            if (adc_flag==1){
                adc_flag = 0;

                // Print 8 MSBs on PORTC
                LATC = adc_10bit>>2;

                strcpy(txt,"RA0[mV]=");
                IntToStr(adc_10bit * 5, numtxt);
                strcat(txt,numtxt);
                Lcd_Out(1,1,txt);

                // Set AN1
                ADCON0.CHS0 = 1; 

                
            }

            // Print AN1
            else if (adc_flag==2){
                adc_flag = 0;

                // Print 8 MSBs on PORTC
                LATD = adc_8bit;
                
                strcpy(txt,"RA1[mV]=");
                IntToStr(adc_8bit * 20, numtxt);
                strcat(txt,numtxt);
                Lcd_Out(2,1,txt);

                // set AN0  
                ADCON0.CHS0 = 0; 
            }

            ADCON0.GO_NOT_DONE = 1;

            /*
            Changing ADCON0.CHS0, ADCON0.GO_NOT_DONE... needs to be here now
            */

       }

    }




void interrupt(){
    if (PIR1.ADIF){
        PIR1.ADIF = 0;  // Only the IOCB needs to put the flag down after reading the ports.
                        // In this situation is better to do it as fast as possible.
                        // However... on another correction they do it in the end.

        if (ADCON0.CHS0==0){
            adc_10bit = (ADRESH<<2) + (ADRESL >> 6);
            adc_flag = 1;
        }

        // Se AN1
        else if (ADCON0.CHS0==1){
            adc_8bit = ADRESH;
            adc_flag = 2;

        }
    }

// DO NOT DARE TO EVEN CONSIDER WRITING THE LCD_OUT FUNCTION
// INSIDE THE INTERRUPT. IT IS TOO SLOW!!
}