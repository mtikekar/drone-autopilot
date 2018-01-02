
#define THROTTLE_PIN 11
#define AILERON_PIN 3
#define ELEVATOR_PIN 9
#define RUDDER_PIN 10
#define REMOTE_POWER_PIN 2

#define SERIAL_FREQ 115200
#define END_OF_COMMAND '\n'

unsigned char throttle;
unsigned char aileron;
unsigned char elevator;
unsigned char rudder;

unsigned char new_throttle;
unsigned char new_aileron;
unsigned char new_elevator;
unsigned char new_rudder;

static uint16_t ppm[4] = {0,0,0,0};

// PPM stream settings
#define CHANNELS 12 // number of channels in ppm stream, 12 ideally
enum chan_order{
    THROTTLE,
    AILERON,
    ELEVATOR,
    RUDDER,
    AUX1,  // (CH5)  led light, or 3 pos. rate on CX-10, H7, or inverted flight on H101
    AUX2,  // (CH6)  flip control
    AUX3,  // (CH7)  still camera (snapshot)
    AUX4,  // (CH8)  video camera
    AUX5,  // (CH9)  headless
    AUX6,  // (CH10) calibrate Y (V2x2), pitch trim (H7), RTH (Bayang, H20), 360deg flip mode (H8-3D, H22)
    AUX7,  // (CH11) calibrate X (V2x2), roll trim (H7)
    AUX8,  // (CH12) Reset / Rebind
};

#define PPM_MIN 1000
#define PPM_SAFE_THROTTLE 1050 
#define PPM_MID 1500
#define PPM_MAX 2000
#define PPM_MIN_COMMAND 1300
#define PPM_MAX_COMMAND 1700
#define GET_FLAG(ch, mask) (ppm[ch] > PPM_MAX_COMMAND ? mask : 0)

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char *p, *i;
char* c = new char[200 + 1]; // match 200 characters reserved for inputString later
char* errpt;
uint8_t ppm_cnt;



int pair(int pin){
  delay(500);
  //digitalWrite(pin, LOW);
  analogWrite(pin, 0);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  //digitalWrite(pin, HIGH);
  analogWrite(pin, 200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  //digitalWrite(pin, LOW);
  analogWrite(pin, 0);
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  // Setup control pins

  pinMode(THROTTLE_PIN, OUTPUT);
  pinMode(AILERON_PIN, OUTPUT);
  pinMode(ELEVATOR_PIN, OUTPUT);
  pinMode(RUDDER_PIN, OUTPUT);
  pinMode(REMOTE_POWER_PIN, OUTPUT);

  // Change PWM frequency to maximum possible
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 (D9 and D10) divisor to     1 for PWM frequency of 31372.55 Hz
  TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 (D3 and D11) divisor to     1 for PWM frequency of 31372.55 Hz

  analogWrite(THROTTLE_PIN, 0);
  //analogWrite(AILERON_PIN, 150);
  //analogWrite(ELEVATOR_PIN, 150);
  //analogWrite(RUDDER_PIN, 250);
  digitalWrite(AILERON_PIN, HIGH);
  digitalWrite(ELEVATOR_PIN, HIGH);
  digitalWrite(RUDDER_PIN, HIGH);
  
  // Setup led pin
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Setup Serial port
  Serial.begin(SERIAL_FREQ);

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  //Pair
  digitalWrite(REMOTE_POWER_PIN, HIGH);
  pair(THROTTLE_PIN);
  Serial.println("Pairing complete!");


  // initialize control inputs
  throttle = 0;
  aileron = 0;
  elevator = 0;
  rudder = 0;
  
  analogWrite(THROTTLE_PIN, 0);
  analogWrite(AILERON_PIN, 0);
  analogWrite(ELEVATOR_PIN, 0);
  analogWrite(RUDDER_PIN, 0);
  
}

void loop() {

  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);

  
  if (stringComplete) {
    Serial.println("Input received."); delay(10);
    // process string
    char* pt;
    strcpy(c, inputString.c_str());
    pt = strtok (c,",");
    ppm_cnt = 0;
    while (pt != NULL) {
        int a = atoi(pt);
        ppm[ppm_cnt] = a;
        //Serial.println(a); delay(20);
        pt = strtok (NULL, ",");
        ppm_cnt += 1;
    }
    
    if (ppm_cnt==4){
      Serial.println("."); delay(10);// prints "." at end of command
      new_throttle = ppm[0];
      new_aileron = ppm[1];
      new_elevator = ppm[2];
      new_rudder = ppm[3];
      
      if (new_throttle == 1)
        throttle = 0;
      if (new_throttle > 1)
        throttle = new_throttle;  
      analogWrite(THROTTLE_PIN, throttle);

      if (new_aileron == 1)
        aileron = 0;
      if (new_aileron > 1)
        aileron = new_aileron;  
      analogWrite(AILERON_PIN, aileron);
      
      if (new_elevator == 1)
        elevator = 0;
      if (new_elevator > 1)
        elevator = new_elevator;  
      analogWrite(ELEVATOR_PIN, elevator);

      if (new_rudder == 1)
        rudder = 0;
      if (new_rudder > 1)
        rudder = new_rudder;  
      analogWrite(RUDDER_PIN, rudder);
      
      Serial.println("Throttle:"); delay(10);  
      Serial.println(throttle); delay(10);
      Serial.println("Aileron:"); delay(10);
      Serial.println(aileron); delay(10);
      Serial.println("Elevator:"); delay(10);
      Serial.println(elevator); delay(10);
      Serial.println("Rudder"); delay(10);
      Serial.println(rudder); delay(10);
    } else {
      Serial.println("Command ignored.");
      inputString = "";
      ppm_cnt=0;
      stringComplete = false;
    }
        
   // clear the string:
   inputString = "";
   stringComplete = false;
   ppm_cnt=0;
  }
    
   // Read the string from the serial buffer
   while (Serial.available()) {
    // get the new byte:
    //Serial.println("Reading Byte.");
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == END_OF_COMMAND) {
      stringComplete = true;
      delay(20);
      Serial.println("Read Command");
      delay(20);
      Serial.println(inputString);
      delay(20);
    }
    else {      
    // add it to the inputString:
      inputString += inChar;
    }
  }

  
}


 
