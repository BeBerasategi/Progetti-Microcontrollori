unsigned int counter = 0;
int n_overflows = 61;

void main() {
    // Kitt configuration:
    short dir = 1;

    // Configuration of the TIMER0
    T0CON.T08BIT = 1;
    T0CON.T0CS = 0;
    T0CON.PSA = 0; // Prescaler assigned

    T0CON |= 0b00000111; // PS at 256
    // T0CON.TOPS2 = 1 
    // T0CON.TOPS1 = 1
    // T0CON.TOPS0 = 1 
    TMR0L = 0b00000000;

    // Turn on TIMER0
    T0CON.TMR0ON = 1;

    // Configure the interrupt 
    INTCON.TMR0IE = 1;
    INTCON.TMR0IF = 0;

    // Configuration of Port B
    ANSELB = 0b00111111; // We do not need it
   
    INTCON.RBIE = 1;
    INTCON.RBIF = 0;

    IOCB = 0b11000000;

    INTCON.GIE = 1;

    // Set port C as an output
    TRISC = 0;
    // Turn on only the last light
    LATC = 0b00000001;

    while(1){
        if (counter == n_overflows){
            counter = 0; // Don't mix = and ==
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
    if (INTCON.TMR0IF){
        counter++;
        INTCON.TMR0IF = 0;
    }
    else if (INTCON.RBIF){
        counter = 0;
        if(PORTB.RB7){
            n_overflows += 12; // +100ms
            if(n_overflows > 122) // >1000 ms
                n_overflows = 122;
        }
        else if (PORTB.RB6){
            n_overflows -= 12;  
            if(n_overflows < 12) // <100 ms
                n_overflows = 12;
        }
        INTCON.RBIF = 0;
        
    }
}