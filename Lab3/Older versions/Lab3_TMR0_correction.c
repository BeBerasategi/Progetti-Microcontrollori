// Global variables can be ONLY USED FOR THE INTERRUPT

/*
Always use the smallest possible variables.
Learn this well, it is important for the exam.
*/

unsigned int tempo = 0;
int delay = 500;

void main() {
    short dir = 1;

    /*
    In this project we are NOT using the PLL.
    This gives T_int ~ 32 ms
    LATD 0->255: 33*255 ~ 8s
    */

    // Port D configuration
    TRISD = 0;
    LATD = 0;

    // Timer0 
    T0CON = 0b11000111; // PR = 256
    IOCB = 0b11000000;
    INTCON = 0b10101000;

    // Port C
    TRISC = 0;

    while(1){
        if (tempo > delay){
            //tempo = 0; 
            // To consider the error, use:
            tempo -= delay

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
    // We should first order it in this order

    if (INTCON.TMR0IF){
        INTCON.TMR0IF = 0;
        tempo += 33;
        LATD++;
    }
    else if (INTCON.RBIF){ // Use the ELSE IF
        if (PORTB.RB7)
            delay += 100;
        if (PORTB.RB6)
            delay -= 100;

        if (delay < 100)
            delay = 100;
        if (delay > 1000)
            delay = 1000;
        INTCON.RBIF = 0;
    }
}