
#include <msp430.h> 


int numOfBytes = 0;
int byteCount = 0;
int temp = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

    //Timers Config
    P1DIR |= BIT6 + BIT1;                            // P1.6 output
    P1SEL |= BIT6;                            // P1.6 for TA0 CCR1 Output Capture
    P1SEL2 = 0x00;                               // Select default function for P1.6
    TA0CCR0 = 256;                             // PWM Freq=1000Hz
    TA0CCTL1 = OUTMOD_7;                         // CCR1 reset/set: set on at CCR0, off at CCR1 capture (see table 12-2 in specific datasheet)
    TA0CCR1 = 255;                               // CCR1 duty cycle
    TA0CTL = TASSEL_2 + MC_1 + ID_2;                  // SMCLK/4, up mode, 1MhZ

    P2DIR |= (BIT1 | BIT4);                            // P2.1,4 output
    P2SEL |= (BIT1 | BIT4);                            // P2.1,4 for TA0 CCR1 Output Capture
    P2SEL2 = 0x00;                               // Select default function
    TA1CCR0 = 256;                             // PWM Freq=1000Hz
    TA1CCTL1 = OUTMOD_7;                         // CCR1 reset/set: set on at CCR0, off at CCR1 capture (see table 12-2 in specific datasheet)
    TA1CCTL2 = OUTMOD_7;
    TA1CCR1 = 0;                               // CCR1 duty cycle
    TA1CCR2 = 0;
    TA1CTL = TASSEL_2 + MC_1 + ID_2;                  // SMCLK/4, up mode, 1MhZ

    //UART Config
    if (CALBC1_1MHZ==0xFF)                  // If calibration constant erased
      {
        while(1);                               // do not load, trap CPU!!
      }
      DCOCTL = 0;                               // Select lowest DCOx and MODx settings
      BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
      DCOCTL = CALDCO_1MHZ;
      P1SEL |= BIT1 + BIT2 + BIT5;                     // P1.1 = RXD, P1.2=TXD
      P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
      UCA0CTL1 |= UCSSEL_2;                     // SMCLK
      UCA0BR0 = 104;                            // 1MHz 9600
      UCA0BR1 = 0;                              // 1MHz 9600
      UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
      UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
      IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled

    while(1)
    {

    }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?

  temp = UCA0RXBUF;
      switch(byteCount){
      case 0:
          numOfBytes = temp;        //first byte received
          break;
      case 1:
          TA0CCR1 = temp;           //red LED value
          break;
      case 2:
          TA1CCR1 = temp;           //green LED value
          break;
      case 3:
          TA1CCR2 = temp;           //blue LED value
          UCA0TXBUF = numOfBytes-3;     //send new numBytes
          break;
      default:
          if(byteCount<numOfBytes){
              UCA0TXBUF = temp;         //send remaining bytes
          }
      }
      byteCount++;


}
