
#define THRUST PD3

unsigned char thrust;

int pair(int pin){
  delay(2000);
  digitalWrite(pin, LOW);
  delay(2000);
  digitalWrite(pin, HIGH);
  delay(2000);
  digitalWrite(pin, LOW);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(THRUST, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  thrust = 0;
  //Pair
  pair(THRUST);
}

void loop() {

  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  /*
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
  */

  unsigned char new_thrust;
  new_thrust = Serial.parseInt();
  if (new_thrust == 1)
    thrust = 0;
  if (new_thrust > 1)
    thrust = new_thrust;
    
  analogWrite(THRUST, thrust);
  Serial.println(thrust); 
}


 
