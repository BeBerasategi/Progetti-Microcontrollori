/*  ATTEMP1
void main() {
     TRISC = 0;
     LATC = 1;
     
     while(1){
              // Shift left
              while (LATC != 128){
                    delay_ms(250);
                    LATC = LATC << 1;
                    }
              // Shift right
              while (LATC != 1){
                    delay_ms(250);
                    LATC = LATC >> 1;
                    }
              }
}*/


/* ATTEMP 2
void main() {
        
    int short direction = 0; //direction = 0-> left, direction = 1 -> right
    TRISC = 0;
    LATC = 1;

    while(1){
                
                delay_ms(250);
                
                        if (direction == 0){ // LEFT
                                LATC = LATC << 1; 
                        }
                        else{               // RIGHT
                                LATC = LATC >> 1;
                        }
                        if (LATC == 128){
                                direction = 1;
                        }
                        else if (LATC == 1){
                                direction = 0;
                        }
        }
              
}     */

      
// Declaration of global arguments. It does not work. That's why I made the function return
// the current value after each execution... I do not know why global variables don't work well.
int short direction_a, direction_b, direction_c, direction_d;

// Function prototype.
int short move(volatile unsigned short *LATX, int short direction_x);
void initialize();

void main() {

    initialize();

    while(1){
             delay_ms(250);
             direction_a=move(&LATA, direction_a);
             direction_b=move(&LATB, direction_b);
             direction_c=move(&LATC, direction_c);
             direction_d=move(&LATD, direction_d);
             
    }

}  

// Initialization:

void initialize()
{
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    
    LATA = 1;
    LATB = 1;
    LATC = 1;
    LATD = 1; 

    direction_a = 0;
    direction_b = 0;
    direction_c = 0;
    direction_d = 0;
}

// I'm not sure why, but if we use pointers it works. Global variable
// direction_x is not working.
int short move(volatile unsigned short *LATX, int short direction_x){
    if (direction_x == 0){ // LEFT
        *LATX = *LATX << 1;
    }
    else{               // RIGHT
        *LATX = *LATX >> 1;
    }
    if (*LATX == 128){
        direction_x = 1;
    }
    else if (*LATX == 1){
        direction_x = 0;
    }
    
    return direction_x;
}
