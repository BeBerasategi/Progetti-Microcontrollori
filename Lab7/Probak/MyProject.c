// KitCar in PORTE.

void main() {

    TRISE &= 0b11110000;

    LATE = 0b1001;
    LATE.RE3 = 0;

    while (1){
        delay_ms(500);
        LATE = 0b1010;
        delay_ms(500);
        LATE = 0b1011;
        delay_ms(500);
        LATE = 0b0011;


    }

}