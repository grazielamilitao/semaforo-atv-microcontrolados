#include <msp430.h> 

#define LD1_Green       BIT6
#define LD1_Yellow      BIT5
#define LD1_Red         BIT4
//#define LD1_Green       BIT0
//#define LD1_Yellow      BIT1
//#define LD1_Red         BIT2

//#define LD2_Green       BIT3
//#define LD2_Yellow      BIT4
//#define LD2_Red         BIT5
#define LD2_Green       BIT0
#define LD2_Yellow      BIT1
#define LD2_Red         BIT2

void start_p1_p2(void);

unsigned char traffic_light_state = 0;
unsigned long i = 0;
unsigned int switch1_green_time = 8;
unsigned int switch2_green_time = 8;

void main(void)
{
    unsigned int t = 0;

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    _enable_interrupt();

    // Iniciar as portas do MSP430
    start_p1_p2();

    // Máquina de estados para o semáforo
    do {
        switch(traffic_light_state){
        case 0:
            P2OUT = LD1_Green + LD2_Red;

            if(t >= switch1_green_time) {
                t = 0;
                traffic_light_state = 1;
                switch1_green_time = 8;
            }
            ini_Timer1();

            break;
        case 1:
            P2OUT = LD1_Yellow + LD2_Red;

            if(t >= 2){
                t = 0;
                traffic_light_state = 2;
            }
            ini_Timer1();

            break;
        case 2:
            if(t >= switch2_green_time){
                t = 0;
                traffic_light_state = 3;
                switch2_green_time = 8;
            }
            ini_Timer1();

            break;
        case 3:
            P2OUT = LD1_Red + LD2_Yellow;

            if(t >= 2){
                t = 0;
                traffic_light_state = 0;
            }

            ini_Timer1();

            break;
        }

        // Incrementar o tempo
        for(i = 0; i < 8000000; i++)
        t++;
    } while(1);
}

// Interrupção do botão de pedestre
#pragma vector = PORT1_VECTOR
__interrupt void Port1_RTI(void) {
    switch(P1IFG & (BIT5 + BIT4)){
    case BIT5:
        P1IFG &= ~BIT5;

        if(traffic_light_state == 0){
            switch1_green_time = 2;
        }

        ini_Timer1();

        break;
    case BIT4:
        P1IFG &= ~BIT4;

        if(traffic_light_state == 2){
            switch2_green_time = 2;
        }

        ini_Timer1();
        break;
    default:
        P1IFG &= BIT6;
        break;
    }
}

void start_p1_p2(void){

    P1DIR = ~(BIT5 + BIT4);     // ou P1DIR = 0xF9
    P1OUT = BIT5 + BIT4;        // ou P1OUT = 0x06
    P1REN = BIT5 + BIT4;        // ou P1REN = 0x06
    P1IES = BIT5 + BIT4;        // ou P1IES = 0x06
    P1IFG = 0;
    P1IE = BIT5 + BIT4;         // ou P1IE = 0x06

    P2SEL = 0;                  // Limpar bits
    P2SEL2 = 0;
    P2DIR = 0xFF;
    P2OUT = 0x00;
}

void ini_Timer1(void) { // base de 100ms
    for(i=0; i<100000; i++);
}
