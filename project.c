// LCD module connections
sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_D4 at RB0_bit;
sbit LCD_D5 at RB1_bit;
sbit LCD_D6 at RB2_bit;
sbit LCD_D7 at RB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;

#LCD_instructions.
#define MAX_HEIGHT 200  // Maximum height of the tank in cm (empty tank = 0% at 200 cm)
#define MIN_HEIGHT 5    // Minimum height of the tank in cm (full tank = 100% at 5 cm)
void main() {
    float D, t;
    int distance;
    ansel = 0;
    anselh = 0;

    Lcd_Init();  // Initialize LCD
    Lcd_Cmd(_LCD_CLEAR);  // Clear LCD screen
    Lcd_Out(1, 5, "Water Level:");  // Center "Water Level:" on line 1 (start at position 5)
    Lcd_Out(2, 5, "Level: ?");      // Center "Level: ?" on line 2 (start at position 5)

    delay_ms(2500);  // Delay to show the initial messages

    TRISD = 0b00000010;  // RD0 (Trigger) as output, RD1 (Echo) as input

    OPTION_REG = 0b00000110;  // Prescaler 1:128
    TMR0 = 68;                // Initial count from 68

    while(1) {
        PORTD.F0 = 1;    // Set RD0 high (Trigger)
        Delay_us(10);     // Wait for 10 microseconds
        PORTD.F0 = 0;    // Set RD0 low (Trigger)

        while(PORTD.F1 == 0);  // Wait for echo to go high

        TMR0 = 68;   // Clear Timer0 register before starting to measure time

        while(PORTD.F1 == 1);  // Wait for echo to go low

        t = (128.0 * (TMR0 - 68)) / 1000000.0;  // Time in microseconds (TMR0 is 8-bit timer)

        D = (t * 39000.0) / 2.0;  // Distance (speed of sound = 39000 cm/s)

        // Check for valid distance and calculate the water level as a percentage
        if (D <= MAX_HEIGHT && D >= MIN_HEIGHT) {
            distance = ((MAX_HEIGHT - D) * 100) / (MAX_HEIGHT - MIN_HEIGHT);  // Valid distance
        } else if (D > MAX_HEIGHT) {
            distance = 0;  // Tank is empty or distance is out of range
        } else if (D < MIN_HEIGHT) {
            distance = 100;  // Tank is full
        } else {
            distance = -1;  // Invalid distance, set to -1 to indicate no valid reading
        }

        Lcd_Cmd(_LCD_CLEAR);  // Clear the LCD before updating the display
        Lcd_Out(1, 5, "Water Level");  // Center "Water Level" on line 1
        if (distance == -1) {
            Lcd_Out(2, 5, "Level: ?");  // Center "Level: ?" on line 2
        } else {
            Lcd_Out(2, 5, "Level: ");  // Display "Level: " on line 2
            Lcd_Chr(2, 12, (distance / 10) + '0');  // Tens digit of the distance (percentage)
            Lcd_Chr(2, 13, (distance % 10) + '0');  // Ones digit of the distance (percentage)
            Lcd_Chr(2, 14, '%');  // Display percentage symbol
        }

        Delay_ms(1000);  // Delay before the next measurement
    }
}
