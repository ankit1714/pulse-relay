/*
  Pulse & Relay

  Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
  press), the output pin is toggled from LOW to HIGH or HIGH to LOW. There's a
  minimum delay between toggles to debounce the circuit (i.e. to ignore noise).

  The circuit:
  - Pulse as input to board pin --2
  - Relay Out 1 pin -- 
  - Relay Out 2 pin --

  created 21 Dev 2021
  by Ankit Maurya
  modified date
*/

// constants won't change.
const int INPUT_PULSE_PIN = 2;
const int OUTPUT_RELAY_1 = 13;
const int OUTPUT_RELAY_2 = 12;
const int DEBOUNCE_TIME_INPUT_PIN = 50;
const int DEBOUNCE_TIME_RELAY_1_FROM_INPUT_EVENT = 1000;
const int RELAY_1_ON_TIME = 2000;
const int RELAY_2_ON_TIME = 3000;
const int DELAY_BETWEEN_RELAYS = 5000;
const int INITIAL_RELAY_STATE = 0;
const int START_RELAY_WHEN_PULSE_IS = HIGH;

const int RELAY_STATE_IDLE = 0;
const int WAIT_REALY_1 = 1;
const int ENABLE_RELAY_1 = 2;
const int DISABLE_RELAY_1 = 3;
const int WAIT_RELAY_2 = 4;
const int ENABLE_RELAY_2 = 5;
const int DISABLE_RELAY_2 = 6;

const int RELAY_ON = 1;
const int RELAY_OFF = 0;

// Variables will change:
int relayState = RELAY_STATE_IDLE;
unsigned long waitRelayTime = 0;
unsigned long lastDebounceTimeInputPin = 0;  // the last time the output pin was toggled
int relay_1_State = LOW;         // the current state of the output pin
int relay_2_State = LOW;         // the current state of the output pin
int inputPinCurrentState;             // the current reading from the input pin
int inputPinLastState = LOW;
int isPulseDetected = false;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  
  pinMode(INPUT_PULSE_PIN, INPUT);
  pinMode(OUTPUT_RELAY_1, OUTPUT);
  pinMode(OUTPUT_RELAY_2, OUTPUT);

  // set initial relay state
  digitalWrite(OUTPUT_RELAY_1, INITIAL_RELAY_STATE);
  digitalWrite(OUTPUT_RELAY_2, INITIAL_RELAY_STATE);
  Serial.println("INIT...");
}

void handlerInputEvent() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(INPUT_PULSE_PIN);
//  Serial.println(reading);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the pulse changed, due to noise or pressing:
  if (reading != inputPinLastState) {
    Serial.println("Pulse Change Detected...");
    // reset the debouncing timer
    lastDebounceTimeInputPin = millis();
  }

  if ((millis() - lastDebounceTimeInputPin) > DEBOUNCE_TIME_INPUT_PIN) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != inputPinCurrentState) {
      inputPinCurrentState = reading;

      // set flag only when new input state is HIGH
      if (inputPinCurrentState == START_RELAY_WHEN_PULSE_IS) {
        Serial.println("Pulse Changed...");
        isPulseDetected = true;
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  inputPinLastState = reading;
}

void handlerRelayEnable() {

  if(isPulseDetected==true) {
    switch(relayState) {
      case RELAY_STATE_IDLE:
        Serial.println("RELAY_STATE_IDLE...");
        relayState = WAIT_REALY_1;
        waitRelayTime = millis();
        break;
      case WAIT_REALY_1:
        if((millis() - waitRelayTime) > DEBOUNCE_TIME_RELAY_1_FROM_INPUT_EVENT ) {
          Serial.println("WAIT_REALY_1...");
          relayState = ENABLE_RELAY_1;
        }
        break;
      case ENABLE_RELAY_1:
        Serial.println("ENABLE_RELAY_1...");
        digitalWrite(OUTPUT_RELAY_1, RELAY_ON);
        relayState = DISABLE_RELAY_1;
        waitRelayTime = millis();
        break;
      case DISABLE_RELAY_1:
        if((millis() - waitRelayTime) > RELAY_1_ON_TIME ) {
          Serial.println("DISABLE_RELAY_1...");
          digitalWrite(OUTPUT_RELAY_1, RELAY_OFF);
          relayState = WAIT_RELAY_2;
          waitRelayTime = millis();
        }        
        break;
      case WAIT_RELAY_2:
        if((millis() - waitRelayTime) > DELAY_BETWEEN_RELAYS ) {
          Serial.println("WAIT_RELAY_2...");
          relayState = ENABLE_RELAY_2;
        }        
        break;
      case ENABLE_RELAY_2:
        Serial.println("ENABLE_RELAY_2...");
        digitalWrite(OUTPUT_RELAY_2, RELAY_ON);
        relayState = DISABLE_RELAY_2;
        waitRelayTime = millis();
        break;
      case DISABLE_RELAY_2:
        if((millis() - waitRelayTime) > RELAY_2_ON_TIME ) {
          Serial.println("DISABLE_RELAY_2...");
          digitalWrite(OUTPUT_RELAY_2, RELAY_OFF);
          relayState = RELAY_STATE_IDLE;
          isPulseDetected = false;
          waitRelayTime = 0;
        }
        break;
    }   
  }
}

void loop() {
  handlerInputEvent();
  handlerRelayEnable();
}
