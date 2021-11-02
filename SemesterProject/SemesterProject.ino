//Settings
float tempSet = 0;
int angleSet = 0;

// Variables in system
float temp = 0, humidity = 0;
int lastState = 0, state = 0, waterLevel = 0, vAngle;


void setup() {
  temp = temp(0)
}

void loop() {
  //Setup Led indication.
  LEDS();
  Vent();

  //Check for state change and save info if changed.
  if(lastState != state){
    Save();
  }
    
  //Idle State
  if(state == 1){
    //Check for state changes due to these conditions
    Temp();
    Display();
    WaterLevel();
  }
  
  //Running State
  else if(state == 2){
    //Check for state changes due to these conditions
    Temp();
    Motor();
    Display();
    WaterLevel();
  }
  
  //Error State
  else if(state == 3){
    //Check for state changes due to these conditions
    Motor();
    Display();
    WaterLevel();
  }
  
  //Default: Disabled State
  else{
    //Check for on/off switch change
    Motor();
    Display();
  }
}

void Temp(){
  // Check temp and change state accordingly
  temp = //some reading we will need to get.
  if(state == 1){
    if(tempSet < temp){
      state = 2; 
    }
  }
  else{
    if(tempSet >= temp){
      state = 1;
    }
  }
  // Check humidity
  humidity = //some reading we will need to get.
}

void WaterLevel(){
  // Check water level
  waterLevel = //some reading we will need to get.
  float minLevel = //some number we will need to decide on.
  if(waterLevel < minLevel){
    state = 3;
  }
}

void Motor(){
  if(lastState != state){
    if(state == 3){
      // turn motor off
      
    }
    else{
      // turn motor on
      
    }
  }
}

void Display(){
  if(state == 3){
    // Print Error msg and water level to screen.
    
  }
  else{
    // Print Temp and humidity to screen.
    
  }
}

void LEDS(){
  if(lastState != state){
    if(state == 1){
      // Light the Green LED.
      
    }
    else if(state == 2){
      // Light the Blue LED.
      
    }
    else if(state == 3){
      // Light the Red LED.
      
    }
    else{
      // Light the Yellow LED.
      
    }
  }
}

void Vent(){
  //check for change in address to see if angle has changed vAngle and angleSet might need to be hex values.
  // Set vent angle
  if(vAngle != angleSet){
    //Change vent angle somehow.
  }
  angleSet = vAngle;
}

void Save(){
  // Save information on the state change with file writing.
  lastState = state;
}
