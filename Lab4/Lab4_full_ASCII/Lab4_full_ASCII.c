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

// Time and delay
unsigned int tempo_0 = 0;
unsigned int tempo_lcd = 0;
unsigned int delay_0 = 500;    // ms
unsigned int delay_lcd = 10; //1000;    // ms

// A flag for the timer of the lcd
unsigned short tmr_lcd_f = 0;

void init(short *a,short *b,short *c,short *d,short *e,short *f,short *g,short *h){
    *a=48;
    *b=48;
    *c=48;
    *d=48;
    *e=48;
    *f=48;
    *g=48;
    *h=48;
}

void main() {

    const unsigned short states[8] = {0,129,195,231,255,231,195,129}; // declare them as constants!
    short i=0;

    // Flags. 1 when a button has been pressed.
    unsigned short RA0_F = 0;
    unsigned short RA1_F = 0;
    unsigned short RA2_F = 0;
    unsigned short RA3_F = 0;
    unsigned short RA4_F = 0;

    // Time formatting: h10h1:min10min1:s10s1:cs10cs1
    // This will allow to have each digit on a different variable, going from 0 to 9.
    // In fact, these variables will keep the ASCII code corresponding to each variable!
    // Equivalence: '0'-48, '1'-49, '2'-50, '3'-51, '4'-52, '5'-53,'6'-54,'7'-55, '8'-56, '9'-57 

    // Function we will use: void Lcd_Chr(char row, char column, char out_char);

    unsigned short cs10 = 48;
    unsigned short cs1 = 48;
    unsigned short s10 = 48;
    unsigned short s1 = 48;
    unsigned short min10 = 48;
    unsigned short min1 = 48;
    unsigned short h10 = 48;
    unsigned short h1 = 48;

    char time_fmt[12]="hh:mm:ss:cs";

    TRISC = 0;
    TRISD = 0;
    LATD = 0;
    LATC = 0;

    // Configure Port A to be an input.
    ANSELA = 0;
    TRISA = 0b11111111; // Be careful, writing 1 here we were doing: TRISA = 0b00000001.

    // TMR0, GPIO configuration.
    T0CON = 0b11000111;
    INTCON = 0b10100000;

    // Set up the LCD
    Lcd_Init();                 // Init the display library (This will set also the PORTB Configuration)
    Lcd_Cmd(_LCD_CLEAR);        // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF

    // Intial message
    Lcd_Out(1,1,time_fmt);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1,"00:00:00:00");   

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

            cs1++;
            if (cs1==58){
                cs1 = 48;
                cs10++;
                if (cs10==58){  
                    cs10 = 48;
                    s1++;
                    if(s1==58){
                        s1 = 48;
                        s10++;
                        if (s10==54){
                            s10 = 48;
                            min1++;
                            if (min1==58){
                                min1 = 48;
                                min10++;
                                if (min10==54){
                                    min10 = 48;
                                    h1++;
                                    if(h1==58){
                                        h1=48;
                                        h10++;
                                        if(h10==58){
                                            h10=48;
                                        }
                                        Lcd_Chr(1,1,h10);
                                    }
                                    Lcd_Chr(1,2,h1);
                                }
                                Lcd_Chr(1,4,min10);
                            }
                            Lcd_Chr(1,5,min1);
                        }
                        Lcd_Chr(1,7,s10);
                    }
                    Lcd_Chr(1,8,s1);
                }  
                Lcd_Chr(1,10,cs10);
            }
            Lcd_Chr(1,11,cs1);
        }

        // Change kitt periods
        if (PORTA.RA3 && !RA3_F){
            RA3_F = 1;
            delay_0 += 100;
        }
        else if (!PORTA.RA3 && RA3_F){
            RA3_F = 0;
        }
        if (PORTA.RA4 && !RA4_F){
            RA4_F = 1;
            delay_0 -= 100;
        }
        else if (!PORTA.RA4 && RA4_F){
            RA4_F = 0;
        }


        // Change the stopwatch
        // RA0 = start, RA1 = stop, RA2 = reset
        if (PORTA.RA0 && !RA0_F){
            RA0_F = 1;
            tmr_lcd_f = 1;
            RA1_F = 0;
            RA2_F = 0;
        }
        else if (PORTA.RA1 && !RA1_F){
            RA1_F = 1;
            tmr_lcd_f = 0;
            RA0_F=0;
        }
        else if (PORTA.RA2 && !RA2_F){
            RA2_F = 1;
            // tmr_lcd_f = 0; Set this if we want the clock to stop after resetting it.
            init(&cs1,&cs10,&s1,&s10,&min1,&min10,&h1,&h10);
            tempo_lcd = 0;
            RA0_F=0;
            Lcd_Out(1,1,"00:00:00:00"); 
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