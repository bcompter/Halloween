/**
 * LedCtrl
 * 
 * Target: 16F690
 * 
 * 
 */

#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#include <xc.h>

/**
 * Pin configuration
 */
#define LED_1 RC7
#define LED_2 RC6
#define LED_3 RC3
#define LED_4 RC1
#define LED_5 RC0
#define LED_6 RA2
#define LED_7 RB6
#define LED_8 RB4
#define LED_9 RC2

/**
 * Timer reset value
 */
#define TMR_RESET 65385     // 65380

/**
 * True
 */
#define TRUE 1

/**
 * This ID number is statically set when programmed and identifies this 
 * controller
 */
#define ID 1

/**
 * PWM values 
 */
unsigned char Pwm_R;				// PWM Values
unsigned char Pwm_G;				// ...
unsigned char Pwm_B;				// ...
unsigned char Pwm_clk;				// PWM Master clock

/**
 * Communications and operations state variables
 */
unsigned char ttl;                  // Command time to live
#define TTL_MAX             9
unsigned char RecvState;            // Receive state
unsigned char RecvData[5];          // Receive Data
#define COMMAND_COMPLETE    5
#define COMMAND             0
#define PAYLOAD_1           1
#define PAYLOAD_2           2
#define PAYLOAD_3           3
#define CHECKSUM            4
#define COMMAND_RED         1
#define COMMAND_GREEN       2
#define COMMAND_BLUE        3
#define COMMAND_STATE       0b11000000
#define COMMAND_COLOR       0b00110000
unsigned char CommandRed;
unsigned char CommandGreen;
unsigned char CommandBlue;
unsigned char CommandState;
#define STATE_OFF           0
#define STATE_LOW           1
#define STATE_HIGH          2
#define STATE_OSC           3
#define RED                 0
#define GREEN               1
#define BLUE                2
unsigned char OscHigh[3];
unsigned char OscLow[3];
unsigned char OscStep[3];
unsigned char OscValue[3];
unsigned char OscDirection;
unsigned char OddEven;
unsigned char LastOddEven;

/**
 * Initialize and setup the PIC for operation
 */
void Initialize(void);

/**
 * Reset the timer value for the next interrupt
 */
void ResetTimer(void);

/**
 * Get a character from the serial port
 */
char getch(void);

/**
 * Process a received message
 */
void ProcessMessage(void);

/**
 * Main program entry point
 */
void main(void) 
{
    Initialize();

    // Loop forever
    while(1 == 1)
    {
        // Check for a new byte
        if (RCIF == TRUE)
        {
            ttl = 0;
            RecvData[RecvState] = getch();
            RecvState++;
            
            // Check for a complete command
            if (RecvState == COMMAND_COMPLETE)
            {
                ProcessMessage();
            }
        }
        
        // Check the TTL for a timeout and reset comms state
        if (ttl > TTL_MAX)
        {
            RecvState = 0;
            ttl = 0;
        }
        
        // Process the current commanded state
        unsigned char i;
        switch(CommandState)
        {
            case STATE_OFF:
                Pwm_R = 0;
                Pwm_G = 0;
                Pwm_B = 0;
                break;
            case STATE_LOW:
                Pwm_R = (unsigned char)(CommandRed >> 2);
                Pwm_G = (unsigned char)(CommandGreen >> 2);
                Pwm_B = (unsigned char)(CommandBlue >> 2);
                break;
            case STATE_HIGH:
                Pwm_R = CommandRed;
                Pwm_G = CommandGreen;
                Pwm_B = CommandBlue;
                break;
            case STATE_OSC:                
                if (!OscDirection)
                {
                    // Going UP
                    for (i = 0; i < 3; i++)
                    {
                        OscValue[i] += OscStep[i];
                        if (OscValue[i] > OscHigh[i])
                        {
                            OscValue[i] = OscHigh[i];
                            OscDirection = 1;
                        }
                    }
                }
                else
                {
                    // Going DOWN
                    for (i = 0; i < 3; i++)
                    {
                        OscValue[i] -= OscStep[i];
                        if (OscValue[i] < OscLow[i])
                        {
                            OscValue[i] = OscLow[i];
                            OscDirection = 0;
                        }
                    }
                }
                
                Pwm_R = OscValue[RED];
                Pwm_G = OscValue[GREEN];
                Pwm_B = OscValue[BLUE];
                
                break;
        }
        
    }  // end while
    
    return;
}

/**
 * Initialize and setup the PIC for operation
 */
void Initialize(void)
{
    OSCCON = 0b01110001;		// 8 MHz INT OSC
	CM1CON0 = 7;         		// Turn off Comparators
	T1CON = 0b00000001;			// Prescaler 1:1
	INTCON = 0;					// Clear Interrupts
	PEIE = 1;					// Enable Interrupts
	TMR1IF = 0;					// Clear TMR1 Interrupt Flag
	TMR1H = 0;					// Clear Timer1
	TMR1L = 0;					// ...
	TMR1IE = 1;					// Timer1 Interrupts Enabled

	TRISA = 0;					// Enable Digital Outputs
	TRISB = 0b00100000;			// Enable RX pin of UART
	TRISC = 0;					// ...
	ANSEL = 0;					// No analog
	ANSELH = 0;

	// Init UART
	BRGH = 1;		// High Speed
	BRG16 = 0;
	//SPBRG = 51	// Baud == 9600
    //SPBRG = 47    // Baud == 10417
	SPBRG = 25;		// Baud == 19200
	SYNC = 0;		// Asynch
	SPEN = 1;
	TXEN = 1;		// TX Enabled
	CREN = 1;		// RX Enabled
    
    // Init ttl to zero
    ttl = 0;
    
    // Start at high state
    CommandState = STATE_HIGH;
    
    // Init receive state
    RecvState = 0;
    
    // Init osc direction
    OscDirection = 0;

    // Turn everything off
    LED_1 = 0;
    LED_2 = 0;
    LED_3 = 0;
    LED_4 = 0;
    LED_5 = 0;
    LED_6 = 0;
    LED_7 = 0;
    LED_8 = 0;
    LED_9 = 0;

	GIE = 1;		// Enable interrupts and GO
}

/**
 * Reset the timer value for the next interrupt
 */
void ResetTimer(void)
{
	unsigned short reset = TMR_RESET;
	TMR1H = reset >> 8;
	TMR1L = reset;
}

/**
 * Get a character from the serial port
 */
char getch(void)
{
	RCIF = 0;
	return RCREG;
}

/**
 * Process a received message
 */
void ProcessMessage(void)
{
    // Reset state for next message
    RecvState = 0;
    
    // Calculate and verify the checksum
    unsigned char calcChecksum;
    calcChecksum = (unsigned char)(RecvData[0] + RecvData[1] + RecvData[2] + RecvData[3]);
    if (calcChecksum != RecvData[CHECKSUM])
        return;
    
    // Check the command
    unsigned char command = (unsigned char)(RecvData[COMMAND] & 0b11110000);     // Mask command data 
    unsigned char cmdID;
    unsigned int cmdState;
    
    // Check the ID
    cmdID = (unsigned char)(RecvData[COMMAND] & 0b00001111);
    if (cmdID != ID)
        return;         // Not for me, goodbye
    
    switch(command)
    {
        case COMMAND_STATE:            
            CommandState = (unsigned char)(RecvData[PAYLOAD_1]);             
            break;
            
        case COMMAND_COLOR:            
            CommandRed      = RecvData[COMMAND_RED];
            CommandGreen    = RecvData[COMMAND_GREEN];
            CommandBlue     = RecvData[COMMAND_BLUE];
            
            // Setup Osc values
            OscHigh[RED] = CommandRed >> 1;
            OscLow[RED] = CommandRed >> 4;
            OscStep[RED] = CommandRed >> 6;
            OscValue[RED] = OscLow[RED];
            
            OscHigh[GREEN] = CommandGreen >> 1;
            OscLow[GREEN] = CommandGreen >> 4;
            OscStep[GREEN] = CommandGreen >> 6;
            OscValue[GREEN] = OscLow[GREEN];
            
            OscHigh[BLUE] = CommandBlue >> 1;
            OscLow[BLUE] = CommandBlue >> 4;
            OscStep[BLUE] = CommandBlue >> 6;
            OscValue[BLUE] = OscLow[BLUE];

            OscDirection = 0;
            
            break;
            
    }  //end switch
    
}

/**
 * Timer interrupt
 * Execute PWM actions and TTL actions
 */
void interrupt timer()
{
	// Reset Timer for next interrupt
	TMR1IF = 0;
	ResetTimer();

	// Increment PWM clock
	Pwm_clk = (unsigned char)(Pwm_clk + 1);
    
	// Increment comms time to live
	if ( RecvState > 0 )
		ttl++;

	// If CLK = 0, end of period
		// Turn ON all channels > 0
	if ( Pwm_clk == 0 )
	{
		if ( Pwm_R > 0 )
        {
			LED_1 = 1;
            LED_4 = 1;
            LED_7 = 1;
        }
		if ( Pwm_G > 0 )
        {
            LED_3 = 1;
            LED_6 = 1;
            LED_9 = 1;
        }
		if ( Pwm_B > 0 )
        {
			LED_2 = 1;
            LED_5 = 1;
            LED_8 = 1;
        }
	}

	// Turn OFF channels if Pwm_clk > channel value
	if ( Pwm_clk > Pwm_R )
    {
		LED_1 = 0;
        LED_4 = 0;
        LED_7 = 0;
    }
	if ( Pwm_clk > Pwm_G )
    {
        LED_3 = 0;
        LED_6 = 0;
        LED_9 = 0;
    }
	if ( Pwm_clk > Pwm_B )
    {
        LED_2 = 0;
        LED_5 = 0;
        LED_8 = 0;
    }
}