//LED's should be plugged into digital ports 8-12
//Bug exists where pine 12 LED won't light up, looking into it

const int numberOfLeds = 5;

int leds[numberOfLeds] = {8, 9, 10, 11, 12};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < numberOfLeds; i++) 
  {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  static char input[2];
  static uint8_t i;
  
  if (Serial.available() > 0) {
    char value = Serial.read();
    
    if ( value != '\r' && i < 1 ) {
    input[i++] = value;
    
    }else {
      input[i] = '\0';
      i = 0;
      
      int engineRPM = atoi(input);
      
      for(i = 0; i < numberOfLeds; i++)
 {
 	int ledNum = i + 1;

 	bool pinStatus = (engineRPM & ledNum == ledNum);

 	digitalWrite(ledNum, pinStatus);
 }
      
      Serial.println(value);
    }
  }
  
  /*LED TEST
  
  int led;
  for(led = 8; led < 13; led++) {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
  }*/
}
