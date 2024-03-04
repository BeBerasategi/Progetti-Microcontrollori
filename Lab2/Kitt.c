void main() {
    short dir = 1; //dir=1 direzione è SX=>DX
    TRISC = 0;
    LATC = 1;
    LATC = 0b00000001;
    LATC = 0b1;
    LATC = 0x01;
    LATC = 0x1;

    while(1){

        if (dir==1)
            LATC <<= 1;
        else
            LATC >>= 1;
        
        if (LATC == 1)
            dir = 1;
        else if (LATC == 128)
            dir = -1;

        delay_ms(250);
        
    }



}