#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lcd.h"
uint16_t PWM=4;
uint8_t state=0;
///PWM

///LCD BUFFER
unsigned char s[81];
///TIMER SECOND
uint16_t pb=0;
uint16_t tick=0;
///SEGMENT
char offset = 4;
char seg=0;
char sev_seg[8] = {8,7,6,5,4,3,2,1};
unsigned char seg_on=1;
///BUTTON CONTROLL
unsigned char m_button=0;
char m_offset=7;
unsigned char row = 0x08;
///FGV DECLARATION
void LCD_Puts(char *s);
void button();
void matrix();
void put_seg(char *s);
void init();

int main(void)
{
    LCD_init();
    init();

    ///timer0 setup
    TCCR0 =  (1<<CS01)| (1<<CS00);
    TIMSK=(1<<TOIE0);
    sei();

    LCD_goto(0,0);
	sprintf(s,"SEGMENSEK:");
	LCD_Puts(s);
	put_seg(sev_seg);

    while(1)
    {
        button();
        matrix();
        if(m_button != 12){
            switch(m_button){
                case 10:m_offset = (m_offset-1); if(m_offset<0) m_offset = 7;break;
                case 11:m_offset = (m_offset+1)%8; break;
                default:sev_seg[m_offset] = m_button; put_seg(sev_seg);
            }
        }
    }

    return 0;
}

ISR(TIMER0_OVF_vect) {

    if(!(tick%PWM)){
            PORTC |= 1<<PORT7;
            PORTE |= 1<<PORT3;
            PORTE |= 1<<PORT2;
    }
    else {
            PORTC &= ~(1<<PORT7);
            PORTE &= ~(1<<PORT3);
            PORTE &= ~(1<<PORT2);
    }

    tick=(tick+1) % 976;

    if(tick==975){
        pb++;
        PORTB = pb<<4;
    }

    seg_on=1;
    seg=(seg+1)%4;
    if((((seg + offset)%8)==m_offset)&&(tick<488)) seg_on=0;
    PORTA = (seg_on<<7)+(0<<6)+(seg<<4)+sev_seg[(seg + offset)%8];

}

void button(){

    switch (PING){
        case 1:{offset++; offset=offset%8; while(PING); break;}
        case 2:{ offset--;if(offset<0) offset=8;while(PING); break;}
        case 4:{PWM++; while(PING);break;}
        case 8:{PWM--; while(PING);break;}
    }
}

void matrix()
{
const unsigned char billtomb[12] = { 69, 14, 13, 11, 22, 21, 19, 38, 37, 35, 67, 70 };
unsigned char num, bill;

    	PORTC = row;
		_delay_ms( 5 );
    	bill  = PINC & 0x7f;
   	 	num = 0;

    		while( num<12 )
    		{
       		if( bill == billtomb[num] )
       			  {
					m_button = num;
					while(PINC == billtomb[num] );
	  				break;
      			  }
        	else
        		{
                    m_button = 12;
                    num++;
        		}
   			}

			if( row == 0x40 )
            {
     				 row = 0x08;
            }
    		else
            {
     				 row = row << 1;
            }
}

void init(){

    DDRG=0x00;
    DDRC = 0x78;
    PORTG=0x00;
    DDRB=0xF0;
    DDRD=0xF0;
    PORTB=0x00;
    PORTD=0;
    DDRA=0xFF;
}

void LCD_Puts(char *s)
{
	while(*s)
	{
		LCD_data(*s);
		s++;
	}

}

put_seg(char *s){

    int i=0;
    LCD_goto(1,0);
    for(i=7;i>=0;i--){
	LCD_data(s[i]+'0');
	}
}
