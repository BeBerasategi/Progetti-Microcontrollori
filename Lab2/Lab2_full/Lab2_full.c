unsigned int tempo = 250; //ms

void main() {
    short dir = 1; //dir=1 direzione è SX=>DX
    unsigned short int i;
    TRISC = 0;
    LATC = 1;
    LATC = 0b00000001;
    LATC = 0b1;
    LATC = 0x01;
    LATC = 0x1;

    ANSELB = 0b00111111;
    TRISB = 0b11000000; // Set input 0b00000001

    IOCB = 0b11000000;
    INTCON.RBIE = 1;
    INTCON.RBIF = 0;
    INTCON.GIE = 1;

    while(1){

        if (dir==1)
            LATC <<= 1;
        else
            LATC >>= 1;
        
        if (LATC == 1)
            dir = 1;
        else if (LATC == 128)
            dir = -1;

        vdelay_ms(tempo); // To use a variable argument    
        
    }

}

// TRY RE-DOING THE EXERCISE USING POLLING.

void interrupt(){
    if(INTCON.RBIF){
        if(PORTB.RB7){
            tempo += 100;
            if(tempo > 1000)
                tempo = 1000;
        }
        else if (PORTB.RB6){
            tempo -= 100;  
            if(tempo <100)
                tempo = 100;
        }
           
        INTCON.RBIF = 0;
    }

}