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


unsigned int tempo_0 = 0;
unsigned int tempo_lcd = 0;
unsigned int delay_0 = 500;    // ms
unsigned int delay_lcd = 1000;    // ms
// A flag for the timer of the lcd
unsigned short tmr_lcd_f = 0;

void main() {

    const unsigned short states[8] = {0,129,195,231,255,231,195,129}; // declare them as constants!
    short i=0;
    unsigned short a = 0; // 0-255
    unsigned short b = 0; // 0-255
    short int dir = 1;
    
    unsigned short count;
    char count_str[7];

    // For try3: flags for the buttons. 
    unsigned short RA0_F = 0;
    unsigned short RA1_F = 0;
    unsigned short RA2_F = 0;
    unsigned short RA3_F = 0;
    unsigned short RA4_F = 0;


    TRISC = 0;
    TRISD = 0;
    LATD = 0;
    LATC = 0;

    // For try 3 and on, configure Port A to be an input.
    ANSELA = 0;
    TRISA = 1;

    T0CON = 0b11000111;
    INTCON = 0b10100000;


    Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)

    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF

    Lcd_Out(1,1,"Start!");
    
    // Initialize counter
    count = 0;
    IntToStr(count, count_str);

    while(1){

        // FIRST TRY: this works
        /* if(tempo_0 >= delay_0){
            tempo_0 -= delay_0;
            i++;
            if (i>=8)
                i=i-8;
            LATD = states[i];
            Lcd_Out(1,1,count_str);
            count++;
            if (count >= 256)
                count = 0;
            IntToStr(count, count_str);
        } */
        

       /* SECOND TRY: kitt, on a more elegant way 
        if(tempo_0 >= delay_0){
            tempo_0 -= delay_0;

            if (a == 0){ 
                a = 0b00000001;
                b = 0b10000000;
                dir = 1;
            }
            else if (a == 15){ // 0b00001111 = 15
                a = 0b00000111;
                b = 0b11100000;
                dir = -1;
            } 
            else if (dir == 1){
                a = a|(a<<1);
                b = b|(b>>1);
            }
            else if (dir == -1){
                a >>= 1;
                b <<= 1;
            }

            // Set the output to:
            LATD = a|b;

            Lcd_Out(1,1,count_str);
            count++;
            if (count >= 256)
                count = 0;
            IntToStr(count, count_str);
            
        } */
        

        //THIRD TRY: using two different timers

        // Hemen interesgarria izan liteke while-ak erabiltzea.
        if(tempo_0 >= delay_0){ // Kitt car effect
            tempo_0 -= delay_0;
            i++;
            if (i>=8)
                i=i-8;
            LATD = states[i];  
        } 

        if(tempo_lcd >= delay_lcd){ // LCD 
            tempo_lcd -= delay_lcd;

            Lcd_Out(1,1,count_str);
            count++;
            if (count >= 256)
                count = 0;
            IntToStr(count, count_str);
        }


        // Change kitt periods
        if (PORTA.RA3 && (RA3_F==0) ){
            RA3_F = 1;
            delay_0 += 100;
        }
        else if (PORTA.RA3==0 && RA3_F){
            RA3_F = 0;
        }
        if (PORTA.RA4 && (RA4_F==0) ){
            RA4_F = 1;
            delay_0 -= 100;
        }
        else if (PORTA.RA4==0 && RA4_F){
            RA4_F = 0;
        }

    
        // Change the stopwatch
        // RA0 = start, RA1 = stop, RA2 = reset
        if (PORTA.RA0 && (RA0_F==0) ){
            RA0_F = 1;
            tmr_lcd_f = 1;
            RA1_F = 0;
            RA2_F = 0;
        }
        else if (PORTA.RA1 && (RA1_F==0)){
            RA1_F = 1;
            tmr_lcd_f = 0;
            RA0_F=0;
        }
        else if (PORTA.RA2 && (RA2_F==0)){
            RA2_F = 1;
            // tmr_lcd_f = 0; Set this if we want the clock to stop after resetting it.
            count = 0;
            tempo_lcd = 0;
            RA0_F=0;

            IntToStr(count, count_str);
            Lcd_Out(1,1,count_str);
        }

    } 

    
}


void interrupt(){
    if(INTCON.TMR0IF){
        INTCON.TMR0IF = 0; // Zuzenketan bukaeran...
        tempo_0 += 33;
        if (tmr_lcd_f)
            tempo_lcd += 33;
    }
}