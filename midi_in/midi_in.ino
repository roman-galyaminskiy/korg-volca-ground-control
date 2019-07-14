int commandByte;
int noteByte;
int velocityByte;

void setup(){
  Serial.begin(9600);
  Serial2.begin(31250);
}

void checkMIDI(){
  if (Serial2.available() > 0){  

    commandByte = Serial2.read();

    if (commandByte < 240) {
      Serial.print("Command ");
    Serial.print(commandByte, DEC);
    Serial.println();
    }
    
  }
}
    

void loop(){
  checkMIDI();
}
