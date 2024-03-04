void main() {
    ANSELB = 0b00111111;
    //TRISB = 1; // Set input
    TRISD = 0;
    LATD = 0b1;

    IOCB = 0b11000000;
    INTCON.RBIE = 1;
    INTCON.RBIF = 0;
    INTCON.GIE = 1;
}

void interrupt(){
    if(INTCON.RBIF){
        // INTCON.RBIF = 0; //Not here! -> Step of 2
        // Flag reseted without reading port B, mismatch condition.
        // The flag will not be reseted until reading port B.

        if(PORTB.RB7)
            LATD++;
        else if (PORTB.RB6) // Careful when pressing both RB6 and RB7
            LATD--;
        /* 
        With this structure, if I press RB6 while mantaining
        RB7 pressed, we will only see LATD++.
        */
        INTCON.RBIF = 0;
    }

}