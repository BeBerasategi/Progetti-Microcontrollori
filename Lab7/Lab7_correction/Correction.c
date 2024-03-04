// -------Global Variables--------
unsigned int amplitude = 0;
unsigned short int sensor_flag_ADC, sensor_flag_CCP = 0;
unsigned int Tb, Ta = 0;

// -------------------------------

// Lcd module connections
sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End Lcd module connections




void main() {

	// -------------Variables---------------
	char txt[17];	   	// String for storing the CCP/ADC "LCD Text"
	char txtNum[7];		// String for storing the CCP/ADC "Measure text"
	// -------------------------------------

	// ----------Port Configuration---------
	TRISC = 0b10111111;		//RC6 output
	ANSELC = 0b11111011;	//RC3 ananlog (ADC AN15), RC2 digital (CCP1)
	// -------------------------------------

	// --------ADC-------
	ADCON2 = 0b10100001;
    ADCON1 = 0b00000000;
	ADCON0 = 0b00111101;
	// ------------------
	
	// -----CCP1----
	CCP1CON = 0b00000101;
	CCPTMRS0 = 0;
	// -------------

	// -----TMR1----
	// Fosc = 8MHz Tclk = 1us (Fosc/4 with PRE=2)
	T1CON = 0b00010011;
	// -------------
	
	// ----Interrupt-----
	PIR1 = 0;
	PIE1.CCP1IE = 1;
	PIE1.ADIE = 1;
	INTCON = 0b11000000;
	// ------------------
	
	// -------------------------------------


	LATC.RC6 = 1;					// Setting this always high meas work in free-runnig mode

	Lcd_Init();						// Init the display library (This will set also the PORTB Configuration)

  	Lcd_Cmd(_LCD_CLEAR);    		// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   	// Cursor OFF

 	Lcd_Out(1,1,"Waiting CCP ...");	// Print default first line (CCP)
	Lcd_Out(2,1,"Waiting ADC ...");	// Print default second line (ADC)


	ADCON0.GO_NOT_DONE = 1;			// Start ADC Acquisition

	while(1){

		// First Line ADC
		if(sensor_flag_CCP==1){

			IntToStr((unsigned int)Tb-Ta, txtNum);	// time*1 = Distance [mm] => 1 LSB = 1 us = 1 mm
			
			strcpy(txt, "CCP[mm] = ");
			strcat(txt, txtNum);

			Lcd_Out(1, 1, txt);
			sensor_flag_CCP = 0;


		}

		// Second Line ADC
		if(sensor_flag_ADC==1){

			IntToStr(amplitude*5, txtNum);	// Amplitude*5 = Distance [mm] => 1 LSB = 5 mV = 5 mm
		
			strcpy(txt, "ADC[mm] = ");
			strcat(txt, txtNum);

			Lcd_Out(2, 1, txt);
			sensor_flag_ADC = 0;

			ADCON0.GO_NOT_DONE = 1;

		}

	}

}

void interrupt(){	//ISR

	if(PIR1.ADIF){

		amplitude = ADRESL;
		amplitude += ADRESH << 8;

		
		sensor_flag_ADC = 1;
		PIR1.ADIF = 0;
	}
	
	if (PIR1.CCP1IF){
		PIR1.CCP1IF = 0;
		if(CCP1CON.CCP1M0){ // Se vero => rising
			Ta = (CCPR1H << 8) | CCPR1L;
			CCP1CON.CCP1M0 = 0;
		}
		else{ // Altrimenti => Falling
			Tb = (CCPR1H << 8) | CCPR1L; // Using | or + is equivalent (see assembly!)
			CCP1CON.CCP1M0 = 1;
			sensor_flag_CCP = 1;
		}

	}

}