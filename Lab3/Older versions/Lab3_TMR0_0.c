void main() {

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
    INTCON.GIE = 1;

    // Set port D as an output
    TRISD = 0;

    // Turn off the lights
    LATD = 0;
}

void interrupt(){
    if (INTCON.TMR0IF){
        LATD++;
    INTCON.TMR0IF = 0;
    }

}