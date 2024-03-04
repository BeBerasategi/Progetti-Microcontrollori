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

void main() {

    unsigned short int conteggio = 0;
    unsigned short int button_pressed = 0;
    unsigned short int refresh_lcd = 0;

    char txt[17];
    char num_txt[7];

    // Etxean probak egiteko
    // OSCCON = 0b01100010;

    // On PR RA5 and RA<3:0> are configured as analog inputs
    ANSELA.RA3 = 0;
    ANSELA.RA2 = 0;
    ANSELA.RA1 = 0;

    // For debug:
    TRISD = 0;
    LATD = 0;
    
    // -----------LCD Initialization---------
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);    	// Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------


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
            LATD = conteggio;
            strcpy(txt,"Cont =");
            IntToStr((int)conteggio, num_txt);
            strcat(txt, num_txt);
            Lcd_Out(1,1,txt);
            refresh_lcd = 0;
        }

    }
}