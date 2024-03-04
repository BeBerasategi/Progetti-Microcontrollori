/*
    CCP5 = RE2.
    TIMER2 -> PR2

    Duty cycle: \delta = CCPR5 / (PR2+1)

    Period:
    T = (PR2+1)*TMR2PS*4/F_osc



*/


// FROM PIAZZA


void main() {

    // ---- AN0 = RA0 ----
    // Default is analog input; i.e., digital input buffer off
    // ANSELA.RA0 = 1;
    // ANSELA.RA1 = 1;
    // -------------------


    // - Disaple CCP Out -
    TRISE.RE2 = 1;  // During the configuration of the CCP, we need to DISABLE THE OUTPUT.
                    // This could be dangerous if we think in real systems...
	// -------------------


    // --- TMR2 - CCP5 ---
    CCPTMRS1.C5TSEL1 = 0;
    CCPTMRS1.C5TSEL0 = 0;
	// -------------------


    // --- Set Period ----
    PR2 = 255;
	// -------------------

    // --- PWM on CCP5 ---
    CCP5CON = 0b00001100;
    //CCP5CON.CCP5M3 = 1;
    //CCP5CON.CCP5M2 = 1;
	// -------------------

    // ---- Set Ton ------
	// d = CCPR5L /(PR2 +1)
    CCPR5L = 0; // It will go from 0/256 to 255/256. Then it restarts.
	// -------------------

    // Set the ADC
    // Fast ADC Settings: Fosc = 32MHz (PLLL on)
    ADCON2 = 0b00100010;
    //ADCON1 = 0;
    ADCON0=0b00000001; // Default An0

    // Interrupt
    PIE1.ADIE = 1;
    PIR1.ADIF = 0;
    INTCON.PEIE = 1;


    // ---- Set TMR2 -----
    // TMR2 ON, Max Prescaler
    T2CON = 0b00000111;
	// -------------------

    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;



    // - Output Enable --
    TRISE.RE2 = 0;
	// -------------------

    INTCON.GIE = 1;

    while(1){

       // ...

    }


}


void interrupt(){
    if (PIR1.ADIF){
        PIR1.ADIF = 0;   // Prima possibile
        CCPR5L = ADRESH;
        ADCON0.GO_NOT_DONE = 1;
    }
}

/*

void main() {
    // Disable the CCPx pin output driver:
    TRISE.RE2 = 1;

    // Select TMR2
    CCPTMRS1.C5TSEL0 = 0;
    CCPTMRS1.C5TSEL1 = 0;

    // Load the PR2 register with the value of T:
    // PR2 =

    // Set the CCP module for PWM:
    CCP5CON |= 0B00001100; // CCP5M<3:0> bits.

    // Duty cycle, 8 bits:
    // CCPR5L = ;

    // 10 bits extension (we will not use it):
    // CCP5CON.DC5B1 = ;
    // CCP5CON.DC5B0 = ;

    // Start TMR2:
    PIR1.TMR2IF = 0; // Clear the interrupt flag.
    PIE1.TMR2IE = 0; // Disable interrupt enable.

    // Set the prescaler:
    // T2CON.T2CKPS0 = 0;
    // T2CON.T2CKPS1 = 0;

    T2CON.TMR2ON = 1; // Enable the timer:

    // Enable PWM output pin:

    while(!PIR1.TMR2IF){
        // Wait until the flag is set. ????????????
    }

    TRISE.RE2 = 0; // Enable the output driver of CCP5.

    while(1){


    }


}

/*

/*
  T2CON = 0b00000100;
  PR2 = 0xFF;
  CCP5CON = 0b00001100;
  CCPTMRS1 = 0;
  CCPR5L = 20;
*/