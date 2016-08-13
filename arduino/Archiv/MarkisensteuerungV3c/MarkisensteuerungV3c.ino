//HW Arduino Pro Mini M328 w. 8MHz



// Input Pins for Switch Markise Up/Down
const int SwMarkUp = 8;
const int SwMarkDown = 9;
// Input Pins for Switch Jalosie Up/Down
const int SwJalUp = 7;
const int SwJalDown = 6;
//Notfall
const int SwEmergency = 5;


// Output Pins
const int MarkOn = 12;
const int MarkDown = 13;
const int JalOn = 10;
const int JalDown = 11;
int MarkUpState = 0;
int MarkDownState = 0;
int JalUpState = 0;
int JalDownState = 0;
int EmergencyState = 0;

//States
const int MOTOR_POSITION_UNKNOWN = 0;
const int MOTOR_POSITION_UP = 1;
const int MOTOR_POSITION_DOWN = 2;
const int MOTOR_MOVING_UP = 3;
const int MOTOR_MOVING_DOWN = 4;


//mark
int MarkState = 0;
long mark_time_for_one_task = 55000;
long mark_time_until_finish = 0;
long mark_time_while_mute = 0;

//jal
int JalState = 0;
long jal_time_for_one_task = 50000;
long jal_time_until_finish = 0;
long jal_time_while_mute = 0;


void setup() {
  // Initialize In-/Outputs
  pinMode(SwMarkUp, INPUT_PULLUP);
  pinMode(SwMarkDown, INPUT_PULLUP);
  pinMode(SwJalUp, INPUT_PULLUP);
  pinMode(SwJalDown, INPUT_PULLUP);
  pinMode(SwEmergency, INPUT_PULLUP);
  pinMode(MarkOn, OUTPUT);
  pinMode(MarkDown, OUTPUT);
  pinMode(JalOn, OUTPUT);
  pinMode(JalDown, OUTPUT);
  digitalWrite(MarkOn, HIGH);
  digitalWrite(MarkDown, HIGH);
  digitalWrite(JalOn, HIGH);
  digitalWrite(JalDown, HIGH);
  
  Serial.begin(9600);


}

void loop() {
  // Read Input
  MarkUpState = digitalRead(SwMarkUp);
  MarkDownState = digitalRead(SwMarkDown);
  JalUpState = digitalRead(SwJalUp);
  JalDownState = digitalRead(SwJalDown);
  EmergencyState = digitalRead(SwEmergency);


  if (mark_time_while_mute > millis()) {
    debug("Is muted");
    return;
  }

  if(EmergencyState == LOW) {
    debug("Emergency!");
      if (MarkState != MOTOR_MOVING_UP) {
    debug("Move motor up");
        startMarkMovement(MOTOR_MOVING_UP);
      }
    return;
  }
    debug("No Emergency");

  //Markise
  if (MarkUpState == LOW)  { //Pressing "Move Up"
    if (MarkState == MOTOR_MOVING_DOWN) {
      stopMarkMovement(MOTOR_POSITION_UNKNOWN);
      mark_time_while_mute = millis() + 400;
      return;
    }
    if (canMarkMove(MOTOR_POSITION_UP)) {
      if (MarkState != MOTOR_MOVING_UP) {
        startMarkMovement(MOTOR_MOVING_UP);
      }
    }

  } else if (MarkDownState == LOW) { //Pressing "Move Down"
    if (MarkState == MOTOR_MOVING_UP) {
      stopMarkMovement(MOTOR_POSITION_UNKNOWN);
      mark_time_while_mute = millis() + 400;
      return;
    }
    if (canMarkMove(MOTOR_POSITION_DOWN)) {
      if (MarkState != MOTOR_MOVING_DOWN) {
        startMarkMovement(MOTOR_MOVING_DOWN);
      }
    }
  }

  if (mark_time_until_finish <= millis() && mark_time_until_finish > 0) {
    switch (MarkState) {
      case MOTOR_MOVING_UP:
        stopMarkMovement(MOTOR_POSITION_UP);
        break;
      case MOTOR_MOVING_DOWN:
        stopMarkMovement(MOTOR_POSITION_DOWN);
        break;
    }
  }

  //Jalosie
  if (JalUpState == LOW)  { //Pressing "Move Up"
    if (JalState == MOTOR_MOVING_DOWN) {
      stopJalMovement(MOTOR_POSITION_UNKNOWN);
      jal_time_while_mute = millis() + 400;
      return;
    }
    if (canJalMove(MOTOR_POSITION_UP)) {
      if (JalState != MOTOR_MOVING_UP) {
        startJalMovement(MOTOR_MOVING_UP);
      }
    }

  } else if (JalDownState == LOW) { //Pressing "Move Down"
    if (JalState == MOTOR_MOVING_UP) {
      stopJalMovement(MOTOR_POSITION_UNKNOWN);
      jal_time_while_mute = millis() + 400;
      return;
    }
    if (canJalMove(MOTOR_POSITION_DOWN)) {
      if (JalState != MOTOR_MOVING_DOWN) {
        startJalMovement(MOTOR_MOVING_DOWN);
      }
    }
  }


  if (jal_time_until_finish <= millis() && jal_time_until_finish > 0) {
    switch (JalState) {
      case MOTOR_MOVING_UP:
        stopJalMovement(MOTOR_POSITION_UP);
        break;
      case MOTOR_MOVING_DOWN:
        stopJalMovement(MOTOR_POSITION_DOWN);
        break;
    }
  }

  
}

  //mark
  boolean canMarkMove(int requested) {
    switch (requested) {
      case MOTOR_POSITION_UNKNOWN:
        return false;
      case MOTOR_POSITION_UP:
        return MarkState != MOTOR_POSITION_UP;
      case MOTOR_POSITION_DOWN:
        return MarkState != MOTOR_POSITION_DOWN;
    }
    return true;
  }

  void stopMarkMovement(int destination) {
    mark_time_until_finish = 0; //Set destination time to 0 -> it's not active anymore
    digitalWrite(MarkOn, HIGH);
    delay(150);
    digitalWrite(MarkDown, HIGH);
    MarkState = destination;
  }

  void startMarkMovement(int motor_state) {
    MarkState = motor_state; //Set current motor state. Either "MOTOR_MOVING_DOWN" or "MOTOR_MOVING_UP"
    int mark_down_type = (motor_state == MOTOR_MOVING_UP) ? HIGH : LOW; //Set the relais depending on the movement direction; UP -> LOW
    digitalWrite(MarkDown, mark_down_type); //Activate relais
    delay(150);
    digitalWrite(MarkOn, LOW); //Activate motor
    mark_time_until_finish = millis() + mark_time_for_one_task; //Set destination time
  }

  //jal
  boolean canJalMove(int requested) {
    switch (requested) {
      case MOTOR_POSITION_UNKNOWN:
        return false;
      case MOTOR_POSITION_UP:
        return JalState != MOTOR_POSITION_UP;
      case MOTOR_POSITION_DOWN:
        return JalState != MOTOR_POSITION_DOWN;
    }
    return true;
  }

  void stopJalMovement(int destination) {
    jal_time_until_finish = 0; //Set destination time to 0 -> it's not active anymore
    digitalWrite(JalOn, HIGH);
    delay(150);
    digitalWrite(JalDown, HIGH);
    JalState = destination;
  }

  void startJalMovement(int motor_state) {
    JalState = motor_state; //Set current motor state. Either "MOTOR_MOVING_DOWN" or "MOTOR_MOVING_UP"
    int jal_down_type = (motor_state == MOTOR_MOVING_UP) ? HIGH: LOW; //Set the relais depending on the movement direction; UP -> LOW
    digitalWrite(JalDown, jal_down_type); //Activate relais
    delay(150);
    digitalWrite(JalOn, LOW); //Activate motor
    jal_time_until_finish = millis() + jal_time_for_one_task; //Set destination time
  }







  void debug(String text) {
    Serial.println(text);
  }
