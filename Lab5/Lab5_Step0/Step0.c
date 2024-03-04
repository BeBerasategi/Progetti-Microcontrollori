

void main() {

    // ----- Output ------
    TRISC = 0;
    // -------------------



    // ---- AN0 = RA0 ----
    // Default is analog input; i.e., digital input buffer off
    // ANSELA.RA0 = 1;
    // -------------------


    // -- Set CHS (AN0) --
    ADCON0.CHS0 = 0;
    ADCON0.CHS1 = 0;
    ADCON0.CHS2 = 0;
    ADCON0.CHS3 = 0;
    ADCON0.CHS4 = 0;
    // -------------------

    // ----- Set TAD -----
    // TAD (1;25) us
    // set TAD min = 1us


    //Fosc = 8MHz TAD
    // Fosc/8
    // ADCS = 001
    ADCON2.ADCS0 = 1;
    ADCON2.ADCS1 = 0;
    ADCON2.ADCS2 = 0;
    // -------------------

    // -- Set +/- Vref ---
    //ADCON1 = 0;
    // -------------------

    // --- Set ACQT -----
    // TACQ = 7.45 us
    // TACQTmin = 8 TAD
    ADCON2.ACQT0 = 0;
    ADCON2.ACQT1 = 0;
    ADCON2.ACQT2 = 1;
    // -------------------

    // --- Just. Left ---
    ADCON2.ADFM = 0;
    // -------------------


    // ----- ADC ON -------
    ADCON0.ADON = 1;
    // -------------------


    /*
    // Fast ADC Settings

    Fosc = 8MHz (PLL OFF)
    ADCON2 = 0b00100001;
    //ADCON1 = 0;
    ADCON0=0b00000001;
    
    */


    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;

    while (1)
    {

        // Polling
        if (ADCON0.GO_NOT_DONE == 0)
        {
            // Print 8 MSBs on PORTC
            LATC = ADRESH;

            ADCON0.GO_NOT_DONE = 1;
        }


    }

}
