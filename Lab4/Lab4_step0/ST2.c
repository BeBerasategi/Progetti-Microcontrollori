unsigned int tempo = 0;
int delay = 500;
void main() {
    short int dir = 1;
    TRISD = 0;
    TRISC = 0;
    LATD = 0;

    T0CON = 0b11000111;
    IOCB = 0b11000000;
    INTCON = 0b10101000;

    while(1){
        if(tempo >= delay){
            tempo = 0;
            if (dir==1)
                LATC <<= 1;
            else
                LATC >>= 1;
            
            if (LATC == 1)
                dir = 1;
            else if (LATC == 128)
                dir = -1;
        }
    }
}

void interrupt(){

    if(INTCON.TMR0IF){
        INTCON.TMR0IF = 0;
        tempo += 33;
        LATD++;
    }
    if(INTCON.RBIF){
        if(PORTB.RB7)
            delay += 100;
        if(PORTB.RB6)
            delay -= 100;
        
        if(delay < 100)
            delay = 100;
        if(delay > 1000)
            delay = 1000;
        INTCON.RBIF = 0;
    }
}