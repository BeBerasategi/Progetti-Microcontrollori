
/*
Step 3 of Ex. LAB 5

RA0 => Start Stopwatch
RA1 => Stop Stopwatch
RA2 => Reset Stopwatch

RA3 => Increase Kitt Delay
RA4 => Reduce Kitt Delay

RA5 => Enable the Ultrasonic Module and Print Measured Distance
*/

// -----------Global Variables-----------
int kitt_delay = 500;  						// Range 50-1000 ms
unsigned int kitt_milli = 0; 				// Time Passed

unsigned int stopwatch_milli = 0, stopwatch_micro = 0;							// Time Passed
unsigned short int stopwatch_sec = 0, stopwatch_min = 0, stopwatch_hours = 0;	// Time Passed

unsigned short int stopwatch_enabled = 0;	// Enable/Disable StopWatch

unsigned short int refresh_lcd_enabled = 0;	// Enable/Disable automatic lcd refresh
unsigned short int refresh_lcd_delay = 66;	// Refresh lcd after x millisecond
unsigned short int refresh_lcd_milli = 0;	// how many millisecond passed after last refresh

unsigned int ta, tb, width = 0;				// Variable used in CCP interrupt

int print_distance_delay = 1000;			// Millisecont to show the distance after button release
int print_distance_milli = 0;				// When 0 don't print distance
// --------------------------------------

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

    // ----------Variables--------------
	unsigned short int kitt_dir = 0, kitt_state = 0;
	
	unsigned short int refresh_lcd = 1;
	char txt[17];	   	// String for storing a whole lcd row
	char txtNum[7];		// String for storing the CCP measured distance
	
	unsigned short int button_pressed = 0;
	
	unsigned int meas_distance = 0;
	
	short int i;
    // ---------------------------------

    // ------Register Configuration-----

    // ------PORTA------
    TRISA = 255;
	ANSELA = 0b11000000;		//Enable input digital buffer only where needed
	// -----------------
	
	// ------PORTC------
	TRISC = 0b10111111;
	ANSELC = 0b00001000;
	// -----------------
	
    // ------PORTD------
	LATD = 0;
	TRISD = 0;
    // -----------------
	
	// ------Timer 1-----
	T1CON = 0b00000011;
	// ------------------

	// -------CCP1-------
	CCP1CON = 0b00000101;
	CCPTMRS0 = 0;
	// ------------------
	
	// ----Interrupt----
	PIR1 = 0;
	PIE1.CCP1IE = 1;
 	T0CON = 0b11000111;
 	INTCON = 0b11100000;
	// -----------------

    // ---------------------------------

   	
    // ---------Initialization----------
	Lcd_Init();						// Init the display library (This will set also the PORTB Configuration)

  	Lcd_Cmd(_LCD_CLEAR);    		// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   	// Cursor OFF
	
	LATC.RC6 = 0;					// Shutdown the Ultrasonic Module
	
	LATD = 0;
    // ---------------------------------


    while(1){
		
		//-----------Button polling-----------
		/*
		In button_pressed it is stored if the corresponding button was already pressed and elaborated.
		If the button is released automatically the corresponding bit is cleared
		*/
		
		if( (PORTA & ~button_pressed) & 0b00000001 ){
			
			stopwatch_enabled = 1;
			refresh_lcd_enabled = 1; // Re-enable the automatic refresh
			
			button_pressed = button_pressed | 0b00000001;
		}
		
		if( (PORTA & ~button_pressed) & 0b00000010 ){
			
			stopwatch_enabled = 0;
			refresh_lcd_enabled = 0;	// Disable also the automatic refresh because it is not needed
			
			button_pressed = button_pressed | 0b00000010;
		}
		
		if( (PORTA & ~button_pressed) & 0b00000100 ){
			
			INTCON.GIE = 0;
			
			stopwatch_micro = 0;
			stopwatch_milli = 0;
			
			INTCON.GIE = 1;
			
			stopwatch_sec = 0;
			stopwatch_min = 0;
			stopwatch_hours = 0;
			
			refresh_lcd = 1;
			
			button_pressed = button_pressed | 0b00000100;
		}
		
		if( (PORTA & ~button_pressed) & 0b00001000 ){
			
			kitt_delay += 50;
			
			if(kitt_delay > 1000)
				kitt_delay = 1000;
			
			button_pressed = button_pressed | 0b00001000;
		}
		
		if( (PORTA & ~button_pressed) & 0b00010000 ){
			
			kitt_delay -= 50;
			
			if(kitt_delay < 50)
				kitt_delay = 50;
			
			button_pressed = button_pressed | 0b00010000;
		}
		
		if( PORTA & 0b00100000 ){
			
			INTCON.GIE = 0;
			print_distance_milli = print_distance_delay;
			INTCON.GIE = 1;
			refresh_lcd = 1;
			LATC.RC6 = 1;
		}
		
		button_pressed = PORTA & button_pressed;
		//------------------------------------
		
		INTCON.GIE = 0;
		
		//-------------Kitt section-----------
		while(kitt_milli >= kitt_delay){
            
			INTCON.GIE = 0;
			
			kitt_milli -= kitt_delay;
			
			INTCON.GIE = 1;
			
			switch(kitt_state){
				
				case 0:
					LATD = 0;
					kitt_dir = 1;
					kitt_state = 1;
					break;
				
				case 1:
					LATD = 0b10000000 | 0b00000001;
					if(kitt_dir)
						kitt_state++;
					else
						kitt_state--;
					break;
				
				case 2:
					LATD = 0b11000000 | 0b00000011;
					if(kitt_dir)
						kitt_state++;
					else
						kitt_state--;
					break;
				
				case 3:
					LATD = 0b11100000 | 0b00000111;
					if(kitt_dir)
						kitt_state++;
					else
						kitt_state--;
					break;
				
				case 4:
					LATD = 0b11110000 | 0b00001111;
					kitt_dir = 0;
					kitt_state--;
					break;
				
				default:
					kitt_state = 0;
					kitt_dir = 0;
					
			}
			
		}
		//------------------------------------
		
		INTCON.GIE = 0;
		
		//---------Distance section-----------
		
		if(width != 0){
			
			meas_distance = width >> 3;
			
			width = 0;

			refresh_lcd = 1;
			
		}
		//------------------------------------
		
		INTCON.GIE = 1;
		
		
		INTCON.GIE = 0;
		
		//--------Refresh LCD section---------
		if(refresh_lcd || refresh_lcd_milli > refresh_lcd_delay){
			
			refresh_lcd_milli = 0;	// Reset the automatic refresh timer of the lcd because will be upgraded now
			
			INTCON.GIE = 0;
			
			// Perchè while e non if? Prova a risponderti da solo
			while(stopwatch_micro >= 1000){
				stopwatch_micro -= 1000;
				stopwatch_milli++;
			}
			
			while(stopwatch_milli >= 1000){
				stopwatch_milli -= 1000;
				stopwatch_sec++;
			}
			
			INTCON.GIE = 1; // Zergatik aktibatzen da berriro hemen?
			
			while(stopwatch_sec >= 60){
				stopwatch_sec -= 60;
				stopwatch_min++;
			}
			
			while(stopwatch_min >= 60){
				stopwatch_min -= 60;
				stopwatch_hours++;
			}
			
			while(stopwatch_hours >= 24){
				stopwatch_hours = 0;
			}
			
			
			// -----------------Print stopwatch section----------------------
			IntToStr(stopwatch_hours, txtNum);	// Convert the number in a string
			for(i = 0; i < 7 - 4; i++)
				txtNum[i] = txtNum[i + 4];		// Shift string characters for reducing the size
			if(txtNum[0] == ' ')				// If there is a blank character
				txtNum[0] = '0';				// Replace with 0
			strcpy(txt, txtNum);				// Copy in txt (only the first, the others will be concat)
			strcat(txt, ":");					// Concat the :
			
			IntToStr(stopwatch_min, txtNum);
			for(i = 0; i < 7 - 4; i++)
				txtNum[i] = txtNum[i + 4];	// Shift string characters 
			if(txtNum[0] == ' ')
				txtNum[0] = '0';
			strcat(txt, txtNum);
			strcat(txt, ":");
			
			IntToStr(stopwatch_sec, txtNum);
			for(i = 0; i < 7 - 4; i++)
				txtNum[i] = txtNum[i + 4];	// Shift string characters 
			if(txtNum[0] == ' ')
				txtNum[0] = '0';
			strcat(txt, txtNum);
			strcat(txt, ":");
			
			
			INTCON.GIE = 0;	// Disable global interrupt for ensure atomicity
			
			IntToStr(stopwatch_milli, txtNum);
			
			INTCON.GIE = 1;
			
			for(i = 0; i < 7 - 3; i++)
				txtNum[i] = txtNum[i + 3];	// Shift string characters 
			if(txtNum[0] == ' ')
				txtNum[0] = '0';
			if(txtNum[1] == ' ')
				txtNum[1] = '0';
			strcat(txt, txtNum);
			
			Lcd_Out(1,1,txt);
			// --------------------------------------------------------------
			
			// ------------------Print distance section----------------------
			INTCON.GIE = 0;
			
			if(print_distance_milli){
				
				INTCON.GIE = 1;
				
				if(meas_distance){
					
					IntToStr(meas_distance, txtNum);
					strcpy(txt, "CCP[mm] = ");
					strcat(txt, txtNum);

					Lcd_Out(2, 1, txt);
					
				}
				else
					Lcd_Out(2,1,"Waiting CCP ...");	// Print default first line (CCP)
				
			}
			else{
				
				INTCON.GIE = 1;
				
				meas_distance = 0;			// Reset value for the next measure session
				LATC.RC6 = 0;
				Lcd_Out(2,1,"Press RA5       ");	// Print default first line (CCP)
				
			}
			// --------------------------------------------------------------
			
			INTCON.GIE = 1;
			
			refresh_lcd = 0;
		}
		//------------------------------------
		
		INTCON.GIE = 1;
		
    }
}


void interrupt(){	//ISR
	
	//---------TMR0 Routine----------
	if(INTCON.TMR0IF){
		
  		kitt_milli += 8;
		
		if(refresh_lcd_enabled)
			refresh_lcd_milli += 8;
		
		if(stopwatch_enabled){
			stopwatch_milli += 8;
			stopwatch_micro += 192;
				
		}
			
		if(print_distance_milli){
			print_distance_milli -= 8;
			if(print_distance_milli < 0)
				print_distance_milli = 0;
		}
		
		INTCON.TMR0IF = 0;
		
	}
	//-------------------------------
	//----------CCP Routnie----------
	else if(PIR1.CCP1IF){

		if(CCP1CON.CCP1M0){	// If we are sensing the Rising-Edge

			ta = CCPR1H << 8;
			ta += CCPR1L;

            CCP1CON.CCP1M0 = 0; // Set Sense to Falling
		}
		else{	// If we are sensing the Falling-Edge

			tb = CCPR1H << 8;
			tb += CCPR1L;

   			width = tb - ta;

            CCP1CON.CCP1M0 = 1; // Set Sense to Rising
		}

		PIR1.CCP1IF = 0;
	}
	//-------------------------------
}