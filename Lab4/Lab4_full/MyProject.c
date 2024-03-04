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
unsigned int delay_lcd = 10; //1000;    // ms

// A flag for the timer of the lcd
unsigned short tmr_lcd_f = 0;

void convert(unsigned short i, char str){
    switch (i)
    {
        case 0:
            str='0';
        case 1:
            str='1';
        case 2:
            str='2';
        case 3:
            str='3';
        case 4:
            str='4';
        case 5:
            str='5';
        case 6:
            str='6';
        case 7:
            str='7';
        case 8:
            str='8';
        case 9:
            str='9';
    }

}


void main() {

    const unsigned short states[8] = {0,129,195,231,255,231,195,129}; // declare them as constants!
    short i=0;

    unsigned short count;
    char count_str[7];

    // For try3: flags for the buttons.
    unsigned short RA0_F = 0;
    unsigned short RA1_F = 0;
    unsigned short RA2_F = 0;
    unsigned short RA3_F = 0;
    unsigned short RA4_F = 0;


    // Time formatting
    unsigned short cs = 0;
    unsigned short s = 0;
    unsigned short min = 0;
    unsigned short h = 0;

    char time_fmt[12]="hh:mm:ss:cs";

    char str[7];
    char digit_str;
    char sep[2]=":";

    TRISC = 0;
    TRISD = 0;
    LATD = 0;
    LATC = 0;

    // For try 3 and on, configure Port A to be an input.
    ANSELA = 0;
    TRISA = 0b11111111; // Be careful, writing 1 here we were doing: TRISA = 0b00000001.

    T0CON = 0b11000111;
    INTCON = 0b10100000;


    Lcd_Init();                // Init the display library (This will set also the PORTB Configuration)

    Lcd_Cmd(_LCD_CLEAR);            // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF

    Lcd_Out(1,1,time_fmt);

    // count = 0;
    while(1){
        if(tempo_0 >= delay_0){ // Kitt car effect
            tempo_0 -= delay_0;
            i++;
            if (i>=8)
                i=i-8;
            LATD = states[i];
        }

        if(tempo_lcd >= delay_lcd){ // LCD
            tempo_lcd -= delay_lcd;

            cs++;

            /*
            convert(cs%10,digit_str);
            Lcd_Out(1,11,digit_str);
            convert(cs/10,digit_str);
            Lcd_Out(1,10,digit_str);
            Lcd_Out(1,9,sep);
            */

            ByteToStr(cs,str);
            Lcd_Out(1,9,str);
            Lcd_Out(1,9,sep);


            if (cs==100){
                cs = 0;
                s++; 
                ByteToStr(s,str);
                Lcd_Out(1,6,str);
                Lcd_Out(1,6,sep);
            }

            if (s==60){  
                s = 0;
                min++;
                ByteToStr(min,str);
                Lcd_Out(1,3,str);
                Lcd_Out(1,3,sep);
            }  
            if (min==60){
                min = 0;
                h++;
                IntToStr(h,str);
                Lcd_Out(1,1,str);
            }
            if (h==24){
                h = 0; 
            }
            

            //Lcd_Out(1,1,time_fmt);
            
            /*
            count++;
            if (count >= 256)
                count = 0;
            IntToStr(count, count_str);
            */

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
            Lcd_Cmd(_LCD_CLEAR);  
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
            cs=0;
            tempo_lcd = 0;
            RA0_F=0;
            // Lcd_Cmd(_LCD_CLEAR);  

            //IntToStr(count, count_str);
            //Lcd_Out(1,1,count_str);

            //time_fmt="hh:mm:ss:cs";
            //Lcd_Cmd(_LCD_CLEAR);
            //Lcd_Out(1,1,"hh:mm:ss:cs");
        }

    }


}


void interrupt(){
    if(INTCON.TMR0IF){
        INTCON.TMR0IF = 0;
        tempo_0 += 33;
        if (tmr_lcd_f)
            tempo_lcd += 33;
    }
}