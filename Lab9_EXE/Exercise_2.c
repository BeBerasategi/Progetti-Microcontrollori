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


volatile unsigned int time_ms;
volatile unsigned int antibounce_ms;
volatile unsigned int count_ms;
volatile unsigned short int antibounce = 0;
volatile unsigned short int count_on = 0;

void fmt_time(char *txt, unsigned short s, unsigned short min, unsigned short h);

void main() {

    // Variables for the timer:
    unsigned short int s=0, min=0, h=0;
    char txt_out[9];

    // Flags:
    unsigned short int refresh_lcd = 0;
    unsigned short int pressed = 0;

    unsigned short int antibounce_val;

    // Status timer:
    enum status {ON, OFF, ALARM} state;

    // To correctly set the oscillator at 8MHz:
    OSCCON = 0b01100010;

    // Debug:
    TRISD = 0;
    LATD = 0;

    // Polling port C--> Use port B, RB7 and RB6.
    // Set as digital input by default.

    // Timer0 configuration:
    T0CON = 0b11000111;
    INTCON.TMR0IE = 1;
    INTCON.TMR0IF = 0;

    // -----------LCD Initialization---------
    Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)

    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------

    INTCON.GIE = 1;

    while(1){
        while (time_ms >= 1000){
            s++;
            LATD = s;
            time_ms -= 1000;
            if (s>=60){
                min++;
                s = 0;
            }
            if (min >= 60){
                    h++;
                    min = 0;
                }
            refresh_lcd = 1;
        }

        if (refresh_lcd){
            fmt_time(txt_out, s, min, h);
            Lcd_Out(1,5,txt_out);
            refresh_lcd = 0;
        }

        if (!antibounce){
            antibounce_val = PORTB;
            antibounce_ms = 0;
            antibounce = 1;
        }
        else if (antibounce_ms>30){
            if(((antibounce_val & ~pressed)&0b10000000)&&((antibounce_val & ~pressed)&0b01000000)){ //RB6-7: h
                pressed |= 0b11000000;
                h++;
                count_on = 0b11000000;
                refresh_lcd = 1;    
            }
            else if ((antibounce_val & ~pressed)&0b10000000){ // RB7: min
                pressed |= 0b10000000;
                min++;
                count_on = 0b10000000;
                refresh_lcd = 1;  
            }
            else if((antibounce_val & ~pressed)&0b01000000){ //RB6: s
                pressed |= 0b01000000;
                s++;
                count_on = 0b01000000;
                refresh_lcd = 1;  
            }
            pressed &= PORTB;
            if (count_on & ~pressed){
                count_on = 0;
                count_ms = 0;
            }
            else if ( (count_ms)>=500){
                count_ms -= 500;
                refresh_lcd = 1;
                if ((count_on&0b10000000)&&(count_on&0b01000000)){
                    h++;
                }
                else if (count_on&0b10000000){
                    min++;
                }
                else if (count_on&0b01000000){
                    s++;
                }
            }
            antibounce = 0;
        }

    }

}

void interrupt(){
    if (INTCON.TMR0IF){
        time_ms += 33;
        if (antibounce){
            antibounce_ms += 33;
        }
        if (count_on){
            count_ms += 33;
        }
        INTCON.TMR0IF = 0;
    }

}


void fmt_time(char *txt, unsigned short s, unsigned short min, unsigned short h){
    char temp_txt[7];

    IntToStr(h, temp_txt);
    txt[0] = (temp_txt[4]==' ') ? '0' : temp_txt[4];
    txt[1] = temp_txt[5];
    txt[2] = ':'; 

    IntToStr(min, temp_txt);
    txt[3] = (temp_txt[4]==' ') ? '0' : temp_txt[4];
    txt[4] = temp_txt[5];
    txt[5] = ':'; 

    IntToStr(s, temp_txt);
    txt[6] = (temp_txt[4]==' ') ? '0' : temp_txt[4];
    txt[7] = temp_txt[5];
    txt[8] = '\0'; 
}