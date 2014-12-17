//Arduino PWM Speed Control：
//int E1 = 5;  
//int M1 = 4;                     

int M1=7;
int E1=6;
 
void setup() 
{ 
    pinMode(M1, OUTPUT);  
    pinMode(E1, OUTPUT);  
} 
 
void loop() 
{ 
  int value;
  for(value = 0 ; value <= 255; value+=1) 
  { 
    digitalWrite(M1,HIGH);        
    analogWrite(E1, value);   //PWM Speed Control
    delay(30); 
  }
  
  for(value = 0 ; value <= 255; value+=1) 
  { 
    digitalWrite(M1,LOW);        
    analogWrite(E1, value);   //PWM Speed Control
    delay(30); 
  }  
}
