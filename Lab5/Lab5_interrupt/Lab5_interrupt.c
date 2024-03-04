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

volatile short print = 0;


void main() {
	long adc_value = 0;
	char str[8];

	TRISC = 0;
 	TRISD = 0;

	ADCON0 = 0b00000001;		// GO_NOT_DONE = 0
	//ADCON1 = 0bxxxxxxxx;	//Default V_DD, V_SS.
	ADCON2 = 0b00100001; // Left justified. Only 2 LSB saved in ADRESL.

	// Configure interrupts:
	PIE1.ADIE = 1;
	PIR1.ADIF = 0;
	INTCON.PEIE = 1;
	INTCON.GIE = 1;

	// -----------LCD Initialization---------
	Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)
	Lcd_Cmd(_LCD_CLEAR);    	// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
	// --------------------------------------

	//SOC
	ADCON0.GO_NOT_DONE=1;

	while (1){
		if (print){
			adc_value = ADRESH;
			adc_value = adc_value<<2 + ADRESL>>6;
			//IntToStr(adc_value,str);
			IntToStr(ADRESH<<2,str);
			Lcd_Out(1,1,str);
			IntToStr(ADRESL>>6,str);
			Lcd_Out(1,9,str);
			IntToStr((ADRESH<<2)+(ADRESL>>6),str);
			Lcd_Out(2,1,str);
			print = 0;
		}
	}
}
void interrupt(){
	if (PIR1.ADIF){
		LATC = ADRESH;
		LATD = ADRESL;

		ADCON0.GO_NOT_DONE = 1;
		PIR1.ADIF = 0;
		print = 1;
	}

}