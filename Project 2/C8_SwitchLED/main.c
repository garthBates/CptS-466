// There is a switch on PE0 which controlls an LED which is on PE4
//Pressig the switch will turn on the LED
// Positive Logic is used to interface the switch which measn the switch is connected to the ground by default, and pressing the switch will connect it to 3.3 volt.
// Please look at example_SwitchLED.pdf for more infromation about the circuit.

//#include "tm4c123gh6pm.h"
//#include "hardware_helpers.h"

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
//#define PE0											(*((volatile unsigned long *)0x40024004))	//switch 1
//#define PE4											(*((volatile unsigned long *)0x40024040)) //switch 2
#define PE0 0x01		//switch 1
#define PE4 0x10		//switch 2
#define PE1	0x02		//LED red
#define PE2 0x04		//LED yellow
#define PE3 0x08		//LED green
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
	
unsigned long in,out;
unsigned long pedestrian;

//	Function Prototypes
void Delay(void);
void YellowDelay(void);

int main(void){ unsigned long volatile delay;
	
	SYSCTL_RCGC2_R |= 0x10;           // Port E clock
  delay = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles
  GPIO_PORTE_DIR_R |= 0x0E;         // PE3,2,1 output
  GPIO_PORTE_DIR_R &= ~0x11;        // PE4,0 input 
  GPIO_PORTE_AFSEL_R &= ~0x17;      // not alternative
  GPIO_PORTE_AMSEL_R &= ~0x17;      // no analog
  GPIO_PORTE_PCTL_R &= ~0x000FFFFF; // bits for PE4,PE3,PE2,PE1,PE0
  GPIO_PORTE_DEN_R |= 0x1F;         // enable PE4,PE3,PE2,PE1,PE0
	
	pedestrian = 0; 									// no pedestrian
	
  while(1){
		// works
		GPIO_PORTE_DATA_R = PE3;
		
		if (GPIO_PORTE_DATA_R & PE0){			//switch 1 is pressed
			pedestrian = 1;								  //ped flag raised
			Delay();
			GPIO_PORTE_DATA_R = PE2;				//LED is yellow
			YellowDelay();									//LED is yellow for 1.5s
			while(pedestrian == 1){
				GPIO_PORTE_DATA_R = PE1;			//LED is red
				if (GPIO_PORTE_DATA_R & PE4) {
					Delay();
					pedestrian = 0;
				}
			}
		}
		
		
		/*
		if (GPIO_PORTE_DATA_R & PE0){			//switch 2 is pressed
			GPIO_PORTE_DATA_R = PE3;
		}	else if (GPIO_PORTE_DATA_R & PE4) {
			GPIO_PORTE_DATA_R = PE1;
		} else {
			GPIO_PORTE_DATA_R = PE2;
		}
		*/
		
		/*
    in = (GPIO_PORTE_DATA_R&0x01); // in 0 if not pressed, 1 if pressed (Postivice logic has been used this time which means the switch is 1 if pressed)
    out = in<<4;   // send the switch value from PE0 to PE4 (shifting by 4
    GPIO_PORTE_DATA_R = out;
		YellowDelay();
		*/
		
		/* psudo code inside while(1)
		while(ped == 0){
			green led on
			delay()
			if (switch1 pressed) {
				yellow led on
				YellowDelay()
				red led on
				ped = 1
			}
		}
		if (switch2 pressed {
			ped = 0;
		}
		
		*/
		
  }
}

void Delay(void){unsigned long volatile time;
  time = 145448;  // 0.1sec
  while(time){
		time--;
  }
}

void YellowDelay(void){unsigned long volatile time;
  time = 145448;  // 0.1sec
	time = time * 15; // 1.5sec
  while(time){
		time--;
  }
}