//23.04.2024 by Sercan Kurt - TA3KRT

#include <16F628A.h>
#fuses INTRC_IO, NOPROTECT, NOBROWNOUT, NOMCLR
#use delay(clock = 4MHZ)
//#use rs232(baud = 9600, xmit = PIN_B1)

#define BLUE  PIN_B6
#define GREEN PIN_B5
#define RED  PIN_B4
 

#define servoPin PIN_B3
#define trigger PIN_A2
#define echo PIN_A3
 
#define uzaklik 0.55  //ultrasonik sensör otomatik tetikleme değeri cm/m cinsinden
#define manuel_uzaklik 0.10  //ultrasonik sensör el ile manuel tetikleme değeri cm/m cinsinden
#define tetik_sure 12
#define final_tetik_sure 6

float seshiz, distance, echotime;
float timer1cycle = 0.2487;  // 16300/65536;ms
long timer;

int bayrak = 0;
int sonbayrak = 0;
int durum = 0;

#INT_TIMER1
void TIMER1_isr(void) {
  set_timer1(0);
}

void servo(long int servoAngle, long int servoDelay) {  // 500 - 2500 arası 180 derece
  output_high(servoPin);
  delay_us(servoAngle);
  output_low(servoPin);
  delay_us(20000 - servoAngle);
  delay_ms(servoDelay);
}

void sifon_hareket() {
 
  output_high(BLUE); output_high(RED); output_high(GREEN);
   
  for(int j = 0; j <= 2 ; j++)
                     {
                      output_low(BLUE); delay_ms(100);
                      output_high(BLUE);  delay_ms(100);
                     } 
                     
  output_high(BLUE); output_high(RED); output_high(GREEN);
  
  servo(500, 1000);
  servo(1250, 3000);
  servo(500, 0);
  
   delay_ms(20000);
  
}


void ultrasonik() {
    output_high(trigger);  // sensörü tetikle
    delay_us(10);
    output_low(trigger);
    while (!input(echo));
    set_timer1(0);
    while (input(echo));
    timer = get_timer1();
    seshiz = 0.0002730;  // sesin, 25 derecedeki hava sıcaklığındaki hızı mm/sn
    echotime = (timer * timer1cycle) / 1000;
    distance = seshiz * (timer * timer1cycle) * 10 / 2;
}

void main() {
  setup_timer_1(T1_INTERNAL | T1_DIV_BY_2);
  set_timer1(0);
  enable_interrupts(INT_TIMER1);
  enable_interrupts(GLOBAL);
  
  set_tris_a(0x04);
  set_tris_b(0x00);
  

 output_high(RED);
 output_high(GREEN);
 output_high(BLUE);
 
 servo(700,0);   servo(500, 500);  servo(550,0); // Servo init. başlangıç
  
  for(int j=0; j <= 3 ; j++)
{  
}

 
  while (TRUE) {
  
    ultrasonik();
    
    if(distance <= manuel_uzaklik) {   sifon_hareket();  }  //eli yakşaltırarak anında sifon.
    
    else if ( durum == 0 && distance < uzaklik) {
         
        bayrak++; output_low(RED);  delay_ms(75); output_high(RED);
        delay_ms(675);
        
        if (bayrak >= tetik_sure) { 
        
          for(int j = 0; j <= 2 ; j++)
                     {
                      output_low(RED); delay_ms(100);
                      output_high(RED);  delay_ms(100);
                     } 
                     
        durum = 1; 
        output_low(GREEN); output_high(RED); output_high(BLUE); }  //sifon tetiklenmesi için durum 1 oldu (kişi kalkınca)
        
      }
    
     else if (durum == 1 && distance > uzaklik ) { //kişi kalktı, sifon tetiklendi.
   
        sonbayrak++; 


 output_high(GREEN); delay_ms(125); output_low(GREEN);  delay_ms(150);
 output_low(GREEN);  delay_ms(250);

           
           if (sonbayrak >= final_tetik_sure)
               {       
               
                sifon_hareket(); 
                
               sonbayrak = 0; bayrak=0; durum=0; set_timer1(0);
                delay_ms(50);
        
               }
 
      } 
      else { 
      bayrak = 0; sonbayrak = 0;  //tetik_sure değerine erişmezse sıfırla
      }
      
   // printf("uzaklik:   %2.2fm\nEcho=%1.3fms\n", distance, echotime);
   delay_ms(250);
  }

}




