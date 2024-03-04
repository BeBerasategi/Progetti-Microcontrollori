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
volatile unsigned int count_ms;
volatile unsigned short int count_on = 0;

void fmt_time(char *txt, unsigned short s, unsigned short min, unsigned short h);
void blink();

void main() {

    // Variables for the chrono:
    unsigned short int s=0, min=0, h=0;
    char txt_out[9];

    // Variables for the timer:
    short int st=0, mint=0, ht=0;
    char txt_outt[9];

    // Flags:
    unsigned short int refresh_lcd = 0b00000011;
    unsigned short int pressed = 0;

    unsigned short int antibounce_val;

    // Status timer:
    enum status {ON, OFF, ALARM, STOP} state;

    // Debug:
    TRISD = 0;
    LATD = 0;

    // Set Port C:
    ANSELC = 0;
    TRISC &= 0b11110111; 

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
    state = OFF;

    while(1){
        while (time_ms >= 1000){
            if (state==ON){
                LATC.RC3^=1;
            }
            s++;
            time_ms -= 1000;
            if (s>=60){
                min++;
                s = 0;
            }
            if (min >= 60){
                h++;
                min = 0;
            }
            if (h >= 24){
                h = 0;
            }
            if (state == ON){
                st--;
                if (st<0){
                    st = 59;
                    mint--;
                }
                if (mint<0){
                    mint = 59;
                    ht--;
                }
                if (ht<0){
                    ht = 23;
                }
                if ( (st==0)&&(mint==0)&&(ht==0) ){
                    state = ALARM;
                    LATD = 0xff;
                    LATC.RC3 = 1;
                }
                refresh_lcd |= 0b00000001;
            }
            refresh_lcd |= 0b00000010;
        }

        if (refresh_lcd&0b00000010){
            fmt_time(txt_out, s, min, h);
            Lcd_Out(1,5,txt_out);
            refresh_lcd &= 0b11111101;
            fmt_time(txt_out, st, mint, ht);
            Lcd_Out(2,1,txt_out);
        }
        if (refresh_lcd&0b00000001){
            fmt_time(txt_out, st, mint, ht);
            Lcd_Out(2,1,txt_out);
            refresh_lcd &= 0b11111110;
            if (state==ON){
                Lcd_Out(2,12,"   ON");
            }
            else if (state==OFF){
                Lcd_Out(2,12,"  OFF");
            }
            else if (state==ALARM){
                Lcd_Out(2,12,"ALARM");
            }
            
        }

        if(((PORTC & ~pressed)&0b00000100)){ 
            pressed |= 0b00000100;
            h++;
            count_on = 0b00000100;
            refresh_lcd = 0b00000010; 
            if (h >= 24){
                h = 0;
            }   
        }
        else if ((PORTC & ~pressed)&0b00000010){ 
            pressed |= 0b00000010;
            min++;
            count_on = 0b00000010;
            refresh_lcd = 0b00000010;  
            if (min >= 60){
                h++;
                min = 0;
            }
            if (h >= 24){
                h = 0;
            }
        }
        else if((PORTC & ~pressed)&0b00000001){ 
            pressed |= 0b00000001;
            s++;
            count_on = 0b00000001;
            refresh_lcd = 0b00000010;  
            if (s>=60){
                min++;
                s = 0;
            }
            if (min >= 60){
                h++;
                min = 0;
            }
            if (h >= 24){
                h = 0;
            }
        }
        else if(((PORTC & ~pressed)&0b01000000)){ 
            pressed |= 0b01000000;
            ht++;
            count_on = 0b01000000;
            refresh_lcd = 0b00000001;    
        }
        else if ((PORTC & ~pressed)&0b00100000){ 
            pressed |= 0b00100000;
            mint++;
            count_on = 0b00100000;
            refresh_lcd = 0b00000001;  
        }
        else if((PORTC & ~pressed)&0b00010000){ 
            pressed |= 0b00010000;
            st++;
            count_on = 0b00010000;
            refresh_lcd = 0b00000001;  
        }
        else if((PORTC & ~pressed)&0b10000000){ 
            pressed |= 0b10000000;
            if (state==OFF)
                state = ON;
            else if (state==ON){
                state = OFF;
                LATC.RC3 = 0;
                st=0;
                mint=0;
                ht=0;
            }
            else if (state==ALARM){
                state = OFF;
                LATC.RC3 = 0;
                LATD = 0;
            }
            refresh_lcd |= 0b00000001;
        }
        pressed &= PORTC;
        if (count_on & ~pressed){
            count_on = 0;
            count_ms = 0;
        }
        else if ( (count_ms)>=300){
            count_ms -= 300;
            if ((count_on&0b000000100)){
                h++;
                refresh_lcd = 0b00000010;
            }
            else if (count_on&0b00000010){
                min++;
                refresh_lcd = 0b00000010;
            }
            else if (count_on&0b00000001){
                s++;
                refresh_lcd = 0b00000010;
            }
            if ((count_on&0b010000000)){
                ht++;
                refresh_lcd = 0b00000001;
            }
            else if (count_on&0b001000000){
                mint++;
                refresh_lcd = 0b00000001;
            }
            else if (count_on&0b000010000){
                st++;
                refresh_lcd = 0b00000001;
            }
        }

    }

}

void interrupt(){
    if (INTCON.TMR0IF){
        time_ms += 33;
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