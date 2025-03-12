 WEEK 1
Part 1.
/* MCAexample.c
 * voorbeeld programma voor Microcontrollers
 * C.G.M. Slot (c) 2013
 */

#define F_CPU 16000000UL This defines the CPU clock frequency (16 MHz) used by delay functions
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
  DDRB = (1 << PB5); // we configure the DDRB (Data Direction Register) for port B so that pin PB5 is configured as an output
 // here the (1 << PB5) shifts the value 1 to the left by the number correspoding to PB5 (usually 5 I think) thats why named him like this. This usually creates something called bitmask with only 5 set to 1
  while(1)
  {
    _delay_ms(100);
    PORTB |= (1 << PB5);  // we turn on the PB5 pin performing bitwise OR. We do that so the current state of PORTB with (1 << PB5) ensuring bit PB5 become high, while leaving other bits unchanged. 
   _delay_ms(100);
    PORTB &= ~(1 << PB5); // now we turn off the PB5 pin performing a bitwise AND with the inverted bitmask. the &= bitwise and Assignemt ensuing the bit PB5  is cleared set to 0 usually while persering the oprt bits
// the ~ is a  bitwise NOT inverst the bitmask so all the bits are 1, expect bit PB5. 
  }
  return 0;
}


Questions Answered:
Difference between a single & and a double &&:

Single & (Bitwise AND):
Operates on individual bits of integers. For example, a & b compares each bit of a and b.
Double && (Logical AND):
Operates on Boolean values (true/false) and uses short-circuit evaluation. In an expression a && b, if a is false, b is not evaluated.

Why is F_CPU needed? What happens if we change it?
Purpose of F_CPU:
It tells the delay functions (and any time-related calculations) the clock speed of the microcontroller. This ensures that delays (like _delay_ms) are accurate.
Changing F_CPU:
If you double F_CPU: The actual clock frequency is higher than assumed, so delays will be shorter than expected.
If you halve F_CPU: The delays will be longer than intended.
Key Point: The delay functions use F_CPU to compute how many clock cycles to wait; 

Additional Datasheet Reference:
Figure 13-2 (Port Structure):
In the figure  in the ATmega329P datasheet you can understand the internal structure of an IO port. It explains how data registers, direction registers, and port registers interact(page 79).

bitmsking btw is the process of modifying and utilizing binary representations of numbers or any other data is know as bitmasking. exaples OR(|), AND(&),XOR(^),NOT(~)

Part 2.

int position = 0;      // position index for the LEDs they are from 0 to 4 for the 5 LEDS
int direction = 1;     // this is direction so basically 1 is for moving to the right of the leds and -1 is moving to the left

while (1) {
    // we create a bitmask for the LED at the position
    PORTB = (1 << position);
    
    _delay_ms(100);  

    // we update the position
    position += direction;
    
    // we check if we hit the end of the leds so we can reverse
    if (position >= 5) {        
        position = 4;           
        direction = -1;        
    } else if (position < 0) {   
        position = 1;           
        direction = 1;         
    }
}


Bitmasking:
PORTB = (1 << position); lights the LED corresponding to the current position.


EIRCA Registers:
Function:
The External Interrupt Control Register A (EICRA) configures how the external interrupts (INT0 and INT1) are triggered.
Configuration Options:
Can set triggering on rising edge, falling edge, or low-level signals.
for example EICRA |= (1 << ISC01);  // configuring INT0 to trigger on falling edge (page 90).

Interrupt vecotor
Definition:
An interrupt vector is a pointer to the function (ISR) that should execute when an interrupt occurs.
Location:
These vectors are located at fixed addresses at the beginning of the program memory. For the ATmega328P, the datasheet specifies the vector table with the addresses for each interrupt source. give me pages chatgpt
ISR(INT0_vect) {
    
}
page(117)

1. How the external interrupt is set up (e.g., configuration of EICRA).
The External Interrupt Control Register A (EICRA) is used to define how external interrupts are triggered. For example, if you want to trigger an interrupt on a falling edge for INT0, you can configure it like this:
// Configure INT0 for falling edge detection:
EICRA |= (1 << ISC01);    // we set ISC01 to 1
EICRA &= ~(1 << ISC00);   // we set ISCO0 to 0
The bits ISC01 and ISC00 in EICRA determine the triggering condition. In this configuration, the interrupt will trigger when the signal transitions from high to low (falling edge).

Enabling Interrupts:
In addition to setting EICRA, you must enable the interrupts in the External Interrupt Mask Register (EIMSK):
EIMSK |= (1 << INT0); // enabaling the interupt to 0 
EIMSK |= (1 << INT1); // we enable the external interupt to 1
2. The function and placement of the interrupt vector.
An interrupt vector is a fixed memory location  where the microcontroller expects to find the Interrupt Service Routine (ISR) for a specific interrupt (page117).

3. How the ISR interacts with the main program (e.g., turning LEDs on/off).
The ISR quickly handles the interrupt by updating the flag and returns, while the main loop uses this flag to decide whether to run the scanner light routine(in my coee of course).



volatile uint8_t scanningActive = 0; // this is the flag that is going to check if we should stop or start scanning 1 is start, stop is 0

ISR(INT0_vect) {
    scanningActive = 1;  
}

ISR(INT1_vect) {
    scanningActive = 0;  
    PORTB = 0;          // this way we turn of the leds 
}

int position = 0;
int direction = 1;

while (1) {
    if (scanningActive) {
        PORTB = (1 << position);
        _delay_ms(100);
        position += direction;
        if (position >= 5) {
            position = 3;
            direction = -1;
        } else if (position < 0) {
            position = 1;
            direction = 1;
        }
    }
    _delay_ms(10);
}

4. How the external interrupts trigger the start/stop of the scanning light.
Start Scanning:
When the button connected to the INT0 pin is pressed, the corresponding interrupt is triggered. The ISR for INT0 sets scanningActive to 1, which tells the main loop to start executing the scanner light code.
Stop Scanning:
Conversely, when the button connected to INT1 is pressed, its ISR sets scanningActive to 0, effectively halting the scanner light routine.

5. How you configured the button inputs and interrupts.
DDRD &= ~((1 << PD2) | (1 << PD3));  // Set PD2 and PD3 as inputs
To ensure a stable logic level when the button is not pressed, you can enable the internal pull-up resistors:
PORTD |= (1 << PD2) | (1 << PD3);  // Enable pull-up resistors on PD2 and PD3
Enabling Interrupts:
After configuring the buttons, the interrupts are enabled via the External Interrupt Mask Register (EIMSK) as shown earlier.
EICRA Settings:
The EICRA register is configured to define the interrupt trigger condition


Part 3.
/* interrupt test
 * C.Slot Saxion 2021
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int on = 0; // shared volatile variable,
// make sure the compiler does not optimize it away

int main(void) {
	DDRB = 0b00111111; // part of port B as output
	DDRD = 0;         // port D as inputs ( why? )
	
	EIMSK = (1 << INT0);  // see datasheet: which interrupt is enabled?
	EICRA = 0b00000011;   // see datasheet: when does the interrupt react?
	sei();                // why ?
	
	while(1) {
		// waiting for interrupts to make a change
		if (on)
			PORTB = 0b00111111; // turn on LEDs
		else
			PORTB = 0b00000000; // turn off LEDs
	}
	return 0;
}

ISR(INT0_vect) {
	_delay_ms(20); // short delay to avoid bouncing	
	if (on)         // toggle on/off
		on = 0;  
	else
		on = 1; 
}

1. We make all the pins of port D as inputs because its necessary for the external interrupt (INT0) which is located on one of the Port D's pins (I think it was in the PD2 on the ATmega328P). so that the microcontroller can detect changes like button press (page 80-81). Setting EIMSK with (1 << INT0) tells the microcontroller to enable the interrupt for the INT0 pin.
3. The setting 0x0000000011 configures the INT0 to trigger on a rising edge because the bits ISC01 and ISC00 in the EICRA register determines the trigger condition for INT0.
we have 00: Low level, 01: Any logical change, 10: Failing Edge, 11: Rising edge(page 90).
4. sei() is used because its a set global interrupt enable basically the name(page 88-89).

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Global flag to control scanning:
// When scanningActive is 1, the scanner light pattern runs;
// when 0, the LEDs remain off.
volatile uint8_t scanningActive = 0;

// ISR for INT0 (start button, e.g., connected to PD2)
ISR(INT0_vect) {
    // Start scanning when the button is pressed (trigger on rising edge)
    scanningActive = 1;
    // Optional: small delay to help debounce the button
    _delay_ms(20);
}

// ISR for INT1 (stop button, e.g., connected to PD3)
ISR(INT1_vect) {
    // Stop scanning when the button is pressed (trigger on rising edge)
    scanningActive = 0;
    // Turn off LEDs immediately
    PORTB = 0;
    // Optional: small delay to help debounce the button
    _delay_ms(20);
}

int main(void) {
    // We'll use PORTB bits 0 to 4 (5 LEDs) as outputs.
    DDRB = 0x1F;  // 0b00011111: set PB0 to PB4 as outputs

    // We use PD2 (INT0) and PD3 (INT1) as inputs for our start and stop buttons.
    DDRD &= ~((1 << PD2) | (1 << PD3));  // we configure PD2 and PD3 as inputs
    // Enable internal pull-up resistors for stable logic levels i did not use external pull up resistors:
    PORTD |= (1 << PD2) | (1 << PD3);

    
    // For both INT0 (PD2) and INT1 (PD3), we configure the trigger for a rising edge.
    // According to the datasheet (EICRA register):
    //   - ISC01 & ISC00 = 11 : Rising edge for INT0.
    //   - ISC11 & ISC10 = 11 : Rising edge for INT1.
    EICRA |= (1 << ISC01) | (1 << ISC00)  
           |  (1 << ISC11) | (1 << ISC10); 

    // Enable external interrupts for INT0 and INT1
    EIMSK |= (1 << INT0) | (1 << INT1);

    
    sei();

    
    int position = 0;   
    int direction = 1; 

    while (1) {
        if (scanningActive) {
           
            PORTB = (1 << position);
            _delay_ms(100);  

            
            position += direction;
            
            if (position >= 5) {  
                position = 3;     
                direction = -1;   
            } else if (position < 0) {  
                position = 1;     
                direction = 1;    
            }
        } else { 
            PORTB = 0;
            _delay_ms(10); 
        }
    }
    
    return 0;
}





WEEK 2


Using Timer1 in CTC (Clear Timer on Compare) Mode:
In CTC mode, the timer counts up to a value specified in the Output Compare Register (OCR1A). When the timer reaches that value, it resets to zero and, if enabled, triggers an interrupt.

Registers:

TCCR1B:
Set bit WGM12 to 1 to enable CTC mode.
OCR1A:
Set to 7812 to achieve a 0.5 s interval.
TIMSK1:
Enable the Timer1 Compare Match A interrupt by setting OCIE1A.
TCCR1B (Prescaler):
Set CS12 and CS10 for a 1024 prescaler.

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// In this example, the LED is connected to PB5.

int main(void) {
    // we make  pb5 as  output for the LEDs
    DDRB |= (1 << PB5);


    // Clear Timer on Compare Match.
    TCCR1B |= (1 << WGM12); // CTC mode.

    // Calculate and set the compare match value:
    // For a 0.5 second interval with a 1024 prescaler:
    // OCR1A = (0.5 * 16e6 / 1024) - 1 = 7812
    OCR1A = 7812;

    // enabaling the Timer1 Output Compare A match interrupt.
    TIMSK1 |= (1 << OCIE1A);

    // we set Timer1 prescaler to 1024:
    TCCR1B |= (1 << CS12) | (1 << CS10);

    sei();

    
    while (1) {
        // The microcontroller is in an infinite loop.
    }

    return 0;
}


ISR(TIMER1_COMPA_vect) {
    
    PORTB ^= (1 << PB5);
}
// TCCR1B (CTC mode via WGM12, prescaler selection):
See the “16-bit Timer/Counter1” section – typically around page 133.
// OCR1A (Output Compare Register):
Details for setting the compare value (e.g., 7812 for 0.5 s interval) are usually on page 134.
// TIMSK1 (Timer Interrupt Mask Register):
For enabling the Output Compare A Match interrupt, refer to page 136.
// TCCR0B (Timer/Counter0 Prescaler):
Configuration of Timer0 and its prescaler is described in the “8-bit Timer/Counter0” section, typically around pages 111–112.
// TIMSK0 (Timer0 Interrupt Mask Register):
The overflow interrupt enable is documented on about page 114.


WEEK 3

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t adc_msb = 0;  // left-adjusted ADC result (ADCH)
// btw we use it so volatile so the compiler dosent optimize the code and I think it optimizes the variable which messes up the whole code
ISR(ADC_vect) {
    
    adc_msb = ADCH;  // Read the most significant 8 bits
    
    ADCSRA |= (1 << ADSC);
}

int main(void) {
   
   
    DDRB |= 0x0F;  // 0x0F = 0b00001111


    
    
    
    // - External AREF is used as the voltage reference (REFS1:0 = 00)
    // - ADLAR = 1 for left-adjusted result (so that ADCH contains the MSB)
    // - Select ADC channel 0 (MUX bits = 0000)
    ADMUX = (1 << ADLAR);
    
    
    // - ADEN: Enable ADC
    // - ADIE: Enable ADC interrupt
    // - ADSC: Start ADC conversion (free running mode when combined with auto-trigger)
    // - ADPS2:0: Set prescaler to 128 (16MHz/128 = 125kHz ADC clock, within the recommended range)
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADSC)
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    
    
   
    sei();
    
    while (1) {
        
        
        PORTB = (adc_msb >> 4) & 0x0F;
    }
    
    return 0;

}

WEEK 4 
/* 
 *   - Uses the ADC (with external AREF at 5V and left-adjusted result) to read a potentiometer on ADC0.
 *   - Every second, the 8-bit ADC result (stored in 'adc_value') is sent out on the UART at 9600 baud.
 *   - The UART is configured for 8 data bits, 2 stop bits, and no parity.
 *   - An interrupt routine for the UART receives incoming bytes and displays the lower 4 bits (binary)
 *     on four LEDs connected to Port B (PB0-PB3). 
 *
 *   Questions:
 *     1. If AREF is switched from 5V to 3.3V, the ADC full-scale reference changes,
 *        so conversion values will scale to 3.3V instead of 5V (inputs above 3.3V will clip).
 *     2. To set the ADC sampling rate to 5 kHz, you could use auto-triggering (e.g., with a timer)
 *        so that conversions occur every 200 µs, or add delays between conversions accordingly.
 *     
 *     3. To "light" all four LEDs (i.e. all lower 4 bits on), you need to send a byte whose lower nibble is 0x0F.
 *        (Note: 0x0F is the ASCII control code "Shift In", which may not be a printable character.)
 *
 *     4. The UDR register is used for both transmitting and receiving data:
 *        Writing to UDR places data into the transmit buffer while reading from UDR retrieves received data.
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// For 9600 baud at 16MHz, the UBRR value is calculated as:
// UBRR = (F_CPU/(16*baud)) - 1 = (16,000,000/(16*9600))-1 = 103 (rounded)
#define baudrate 103

// -------------------------
// Global Variables
// -------------------------
volatile char latestChar = '*';   
volatile uint8_t adc_value = 0;     


void initUART(void) {
    // Set UART baud rate
    UBRR0 = baudrate;		

    /* Set frame format in UCSR0C:
     * - 8 data bits: UCSZ01 = 1, UCSZ00 = 1 (UCSZ02 in UCSR0B remains 0 for 8-bit)
     * - 2 stop bits: USBS0 = 1
     * - No parity: UPM01 = 0, UPM00 = 0
     */
    UCSR0C = (0 << UPM01) | (0 << UPM00) | (1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

    // Enable receiver, transmitter and RX complete interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}


void UARTsendchar(unsigned char data) {
    
    while (!(UCSR0A & (1 << UDRE0))) {
        ; 
    }	
    UDR0 = data;
}

// UART Receive Interrupt Service Routine
ISR(USART_RX_vect) {
    
    char temp = UDR0;
    // Filter out CR (0x0D) and LF (0x0A)
    if ((temp != 0x0D) && (temp != 0x0A)) {
        // Display the lower 4 bits of the received character on Port B LEDs
        PORTB = temp & 0x0F;
        latestChar = temp; // Store received byte
    }
}

void initADC(void) {
    /* 
     * Configure ADC to:
     * - Use ADC0 as input channel.
     * - Use external AREF (5V) (REFS1:0 = 00).
     * - Left-adjust result (ADLAR = 1), so that ADCH contains the 8 MSB.
     */
    ADMUX = (1 << ADLAR);  // ADLAR = 1; channel selection defaults to ADC0 (MUX[3:0] = 0)
    
    /* Configure ADC Control and Status Register A (ADCSRA):
     * - ADEN: Enable ADC.
     * - ADIE: Enable ADC interrupt.
     * - ADSC: Start conversion.
     * - ADPS2:0: Set prescaler to 128 for a 125 kHz ADC clock (16MHz/128).
     */
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADSC)
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // The ADC will now run in free-running mode (each conversion triggers the next).
}


ISR(ADC_vect) {
    // Since ADLAR = 1, the 8 MSB of the 10-bit conversion result are in ADCH.
    adc_value = ADCH;  
    // Start the next ADC conversion (free running)
    ADCSRA |= (1 << ADSC);
}

int main(void)
{
    
    DDRB |= 0x0F; // 0x0F = 0b00001111

    
    initUART();
    initADC();
    
    
    sei();  // Enable global interrupts

    
    while (1) {
        _delay_ms(1000);
        UARTsendchar(adc_value);
    }
    
    return 0;
}

ART Initialization:

Baud Rate Calculation:
For a 16 MHz clock and 9600 baud, UBRR0 is calculated as:
UBRR0
=
16
 
000
 
000
16
×
9600
−
1
≈
103
UBRR0= 
16×9600
16000000
​
 −1≈103.
This is why we define baudrate as 103.
Frame Format:
The frame format is set to 8 data bits, 2 stop bits, and no parity. This is configured by:
Setting USBS0 to 1 for 2 stop bits.
Setting UCSZ01 and UCSZ00 to 1 for 8-bit data.
Leaving UPM01 and UPM00 at 0 for no parity.
UDR Register:
The UDR0 register is used for both transmitting and receiving data. When you write to it, the data is placed into the transmit buffer; when you read from it, you retrieve the received data. The hardware manages the shift registers so that the same register name is used for both operations.
ADC Initialization:

The ADC is set up to use ADC0, with external AREF (5V) and left-adjusted conversion. With left adjustment, the most significant 8 bits are stored in ADCH.
The ADC runs in free-running mode with a prescaler of 128 (giving an ADC clock of 125 kHz) which is within the recommended range.
The ADC interrupt (ADC_vect) saves the result into adc_value and then starts a new conversion.
UART Receive & LED Display:

When a character is received via UART, the ISR for USART_RX_vect reads the character from UDR0.
It then displays the lower 4 bits of that character on the four LEDs connected to Port B (PB0–PB3).
To light all four LEDs, you would send a character whose lower nibble is 0x0F (for example, the non-printable ASCII control code 0x0F).
Combining ADC & UART:

The ADC continuously samples the potentiometer.
In the main loop, every second the current ADC reading (adc_value) is sent out via UART.
Thus, using either a PC serial monitor, connecting TX/RX together, or networking with another device, you can see and exchange ADC readings.
Global Interrupt Enable (sei()):



