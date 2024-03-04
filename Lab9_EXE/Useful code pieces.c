// To reverse PORTC:

unsigned char reverse(unsigned char b) {  // [b7 b6 b5 b4 b3 b2 b1 b0]
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; // [b3 b2 b1 b0 b7 b6 b5 b4]
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2; // [b1 b0 b3 b2 b5 b4 b7 b6]
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1; // [b0 b1 b2 b3 b4 b5 b6 b7]
   return b;
}

// To correctly set the oscillator at 8MHz:

OSCCON = 0b01100010;

