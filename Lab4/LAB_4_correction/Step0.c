
/*
Step 0 of Ex. LAB 5
*/

// -----------Global Variables-----------
unsigned int kitt_delay = 500;  //Range 1-1000 ms
unsigned int kitt_milli = 0; 	//Time Passed

unsigned int counter_milli = 0;	//Time Passed

// Beharrezkoa da volatile moduan deklaratzea??

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
	unsigned short int counter = 0;
	char txtNum[7];
    // ---------------------------------

    // ------Register Configuration-----

    // ------PORTD------
	LATD = 0;
	TRISD = 0;
    // -----------------

	// ----Interrupt----
 	T0CON = 0b11000111;
 	INTCON = 0b10100000;
	// -----------------

    // ---------------------------------

   	
    // ---------Initialization----------
	Lcd_Init();						// Init the display library (This will set also the PORTB Configuration)

  	Lcd_Cmd(_LCD_CLEAR);    		// Clear displaya
	Lcd_Cmd(_LCD_CURSOR_OFF);   	// Cursor OFF
	
	LATD = 0;
    // ---------------------------------

	// ONGI IKUSI NOLA ITZALI/PIZTEN DUTEN
	// Global Interrupt Enable-a!!

    while(1){
				
		INTCON.GIE = 0;
		
		//-------------Kitt section-----------
		while(kitt_milli >= kitt_delay){
            
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
		
		//----------Counter section-----------
		while(counter_milli >= 1000){
            
			INTCON.GIE = 0;
			
			counter_milli -= 1000;
			
			INTCON.GIE = 1;
			
			counter++;
			
			IntToStr(counter, txtNum);

			Lcd_Out(1, 1, txtNum);
			
		}
		//------------------------------------
		
		INTCON.GIE = 1;
		
    }
}


void interrupt(){	//ISR

	if(INTCON.TMR0IF){
		
  		kitt_milli += 8;
		counter_milli += 8;
		INTCON.TMR0IF = 0;
		
	}
	
}