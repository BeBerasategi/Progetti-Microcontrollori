
/*
Step 1 of Ex. LAB 5

RA0 => Start Stopwatch
RA1 => Stop Stopwatch
RA2 => Reset Stopwatch

RA3 => Increase Kitt Delay
RA4 => Reduce Kitt Delay
*/

// -----------Global Variables-----------
int kitt_delay = 500;  						//Range 50-1000 ms
unsigned int kitt_milli = 0; 				//Time Passed

unsigned int stopwatch_milli = 0;			//Time Passed
unsigned short int stopwatch_enabled = 0;	//Enable/Disable StopWatch
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
	unsigned short int stopwatch_count = 0;
	
	unsigned short int refresh_lcd = 1;
	char txtNum[7];
	
	unsigned short int button_pressed = 0;
    // ---------------------------------

    // ------Register Configuration-----

    // ------PORTA------
    TRISA = 255;
	ANSELA = 0b11100000;		//Enable input digital buffer only where needed
	// -----------------
	
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

  	Lcd_Cmd(_LCD_CLEAR);    		// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   	// Cursor OFF
	
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
			
			button_pressed = button_pressed | 0b00000001;
		}
		
		if( (PORTA & ~button_pressed) & 0b00000010 ){
			stopwatch_enabled = 0;
			
			button_pressed = button_pressed | 0b00000010;
		}
		
		if( (PORTA & ~button_pressed) & 0b00000100 ){
			stopwatch_count = 0;
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
		
		button_pressed = PORTA & button_pressed; 
		// Zergatik da azkeneko hau beharrezkoa??

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
		
		//---------StopWatch section----------
		while(stopwatch_milli >= 1000){
            
			INTCON.GIE = 0;
			
			stopwatch_milli -= 1000;
			
			INTCON.GIE = 1;
			
			stopwatch_count++;
			
			refresh_lcd = 1;
		}
		//------------------------------------
		
		INTCON.GIE = 1;
		
		//--------Refresh LCD section---------
		if(refresh_lcd){
			
			IntToStr(stopwatch_count, txtNum);

			Lcd_Out(1, 1, txtNum);
			
			refresh_lcd = 1; // Or 0???
		}
		//------------------------------------
    }
}


void interrupt(){	//ISR
	
	//---------TMR0 Routine----------
	if(INTCON.TMR0IF){
		
  		kitt_milli += 8;
		if(stopwatch_enabled)
			stopwatch_milli += 8;
		INTCON.TMR0IF = 0;
		
	}
	//-------------------------------
}