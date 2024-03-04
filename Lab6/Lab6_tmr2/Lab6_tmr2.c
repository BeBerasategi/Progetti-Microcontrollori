/*
    CCP5 = RE2.
    TIMER2 -> PR2

    Duty cycle: \delta = CCPR5 / (PR2+1)

    Period:
    T = (PR2+1)*TMR2PS*4/F_osc



*/


volatile unsigned int time_mikros = 0; // 0-65535

void main() {
    // - Disaple CCP Out -
    TRISE.RE2 = 1;
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
    CCPR5L = 0;
	// -------------------


    // ---- Set TMR2 -----
    // TMR2 ON, Max Prescaler
    T2CON = 0b00000111;
	// -------------------

    // Interrupt configuration for TMR2.
    PIE1.TMR2IE = 1;
    PIR1.TMR2IF = 0;
    INTCON.PEIE = 1;
    INTCON.GIE = 1;

    // TMR2 will yield an interrupt every 512 micro second.

    // - Output Enable --
    TRISE.RE2 = 0;
	// -------------------

    while(1){
        while (time_mikros>10000){ // 10ms have passed
            time_mikros -= 10000;
            CCPR5L++;
        }
    }

}

void interrupt(){
    if (PIR1.TMR2IF){
        PIR1.TMR2IF = 0;
        time_mikros += 512; // We can count up to 127 ms.
    }
}