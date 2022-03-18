#define LIGHT                   (*((volatile unsigned long *)0x400050FC)) //bits 5-0
#define greenNorth 	0x01
#define yellowNorth 0x02
#define redNorth		0x04
#define greenEast 	0x08
#define yellowEast	0x10
#define redEast			0x20
#define GPIO_PORTB_OUT          (*((volatile unsigned long *)0x400050FC)) // bits 5-0
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002401C)) // bits 2-0
#define EastSwitch  0x01
#define NorthSwitch	0x02
#define PedSwitch   0x04

#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_DATA_R				(*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define LED							        (*((volatile unsigned long *)0x40025038))	//all LED colors
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))


unsigned long eastCar,northCar,ped;

void PortF_Init(void);
//Initialize PLL and configure the clock frequency using SYSDIV2
void PLL_Init(void);
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);
// Time delay using busy wait. This assumes 80 MHz system clock.
void SysTick_Wait10ms(unsigned long delay);

void GoN_f(void){	
GPIO_PORTB_OUT = greenNorth +redEast;
LED = 0x02;
SysTick_Wait10ms(50);	
};
void waitN_f(void){	
unsigned short i = 0;	
	GPIO_PORTB_OUT = yellowNorth + redEast;
	LED = 0x02;
while(i<10){
	SysTick_Wait10ms(50);
	i++;
}	
}
void stopN_f(void){
GPIO_PORTB_OUT = redNorth + greenEast;
	LED = 0x02;
SysTick_Wait10ms(100);
}

void goE_f(void) {
GPIO_PORTB_OUT = greenEast + redNorth;
	LED = 0x02;
SysTick_Wait10ms(50);
};

void waitE_f(void) {
unsigned short i = 0;
	//LIGHT = 0x04;
	GPIO_PORTB_OUT = yellowEast + redNorth;
	LED = 0x02;
while(i<10) {
	SysTick_Wait10ms(50);
	i++;
}
}

void stopE_f(void) {
//LIGHT = 0x0e;
GPIO_PORTB_OUT = redEast + greenNorth;
LED = 0x02;
SysTick_Wait10ms(100);
}

void goP_f(void){
GPIO_PORTB_OUT = redEast + redNorth;
LED = 0x08;
SysTick_Wait10ms(500);
}

void slowP_f(void){
unsigned short i = 0;	
	GPIO_PORTB_OUT = redEast + redNorth;	
while(i<5){
	LED = 0x02;
	SysTick_Wait10ms(20);
	LED = 0x00;
	SysTick_Wait10ms(20);
	i++;
}
}

void startN_f(void){
unsigned short i = 0;	
	LIGHT = 0x10;	
while(i<10){
	SysTick_Wait10ms(50);
	LIGHT ^= 0x02;
	i++;
}
}
// Linked data structure
struct State {
	void (*CmdPt)(void);   // output function 
  unsigned long Next[8];}; 
typedef const struct State STyp;
#define goN 	  0
#define waitN 	1
#define goP 		2
#define slowP 	3	
#define goE			4
#define waitE   5
STyp FSM[6]={
	{&GoN_f,{goN,waitN,goN,waitN,waitN,waitN,goP,waitN}},
	{&waitN_f,{goE,goE,goE,goE,goP,goP,goP,goE}},
	{&goP_f,{goP,slowP,slowP,waitN,goP,slowP,slowP,slowP}},
	{&slowP_f,{slowP,goE,goN,waitN,goP,goE,goN,goN}},
	{&goE_f,{goE,goE,waitE,waitE,waitE,waitE,waitE,waitE}},
	{&waitE_f,{goN,goN,goN,goN,goP,goP,goP,goP}}
};

unsigned long S;  // index to the current state 
unsigned long Input; 
int main(void){ volatile unsigned long delay;
  PLL_Init();       // 80 MHz, Program 10.1
	PortF_Init();
  SysTick_Init();   // Program 10.2
  SYSCTL_RCGC2_R |= 0x12;      // 1) B E
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE1-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE1-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
  S = goN;  
	northCar = 1;
	eastCar = 0;
	ped = 0;
  while(1){
		(FSM[S].CmdPt)(); // output + delay
		Input = GPIO_PORTE_IN;
    S = FSM[S].Next[Input]; // next states		
  }
}

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  //GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  //GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}

