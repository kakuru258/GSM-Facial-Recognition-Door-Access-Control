#ifndef MAINCODESKETCH_H
#define MAINCODESKETCH_H
bool saveContact = true;

int red = 7;
int blue = 6;
int Duration = 5000;

int buzzer = 8;
int sirenDelay = 300;

String Name, Date, Time, command, extracted, input, message;

void mainSketchSetUp(){
     pinMode(red, OUTPUT);
     digitalWrite(red, HIGH); // LED indicator for closed Door at bootup
     
     pinMode(blue, OUTPUT);
     pinMode(buzzer, OUTPUT);
}

// functions used .........................................................................
void alarmSiren(bool firstLed, bool secondLed, int toneDuration){
      digitalWrite(red, firstLed);
      digitalWrite(blue, secondLed);
      tone(buzzer, toneDuration);   
      delay(sirenDelay);           
}

String extractContentInBetween(char firstIndex, char lastIndex){                         
     extracted = input.substring(input.indexOf(firstIndex)+1, input.indexOf(lastIndex)); 
     return extracted;
}

void extract_target_messageData(){
        if (input.startsWith("sent:")) {
           command = extractContentInBetween('(',')');
           String extractedCommand = "\n\ncommand: " + command;
           Serial.print(extractedCommand);

           if (command == "open"){
              Name = extractContentInBetween(':',',');
              Name[0] = toupper(Name[0]); // make the first letter capital
              
              String extractedName = "\n\nperson's name: " + Name;
              Serial.print(extractedName);
           }
           Date = extractContentInBetween(',','|');
           String extractedDate = "\n\nthe date: " + Date;
           Serial.print(extractedDate);

           Time = extractContentInBetween('|','(');
           String extractedTime = "\n\nthe time: " + Time;
           Serial.print(extractedTime);
           delay(300);
        }
}

void executeTask(bool redVal, bool blueVal, int angle, long timeDuration){
     digitalWrite(red, redVal);
     digitalWrite(blue, blueVal);
                  
     digitalWrite(servoTrans, HIGH);                   
     servoMotor.write(angle);
     delay(timeDuration); 
                   
     digitalWrite(servoTrans, LOW);
     delay(300);
}

void processTextMessage(String serial_input){
// execute the following below if GSM recieves an SMS text message from a phone
      if (serial_input.startsWith("+CMT:")) {              
         int firstQuote  = serial_input.indexOf('"');  
         int comma = serial_input.indexOf(','); 

         phoneNumber = serial_input.substring(firstQuote+1, comma-1); // extracts number in between the first quote " and comma , but excludes the last quote next to the comma e.g. // Example: +CMT: "+254779653248","","26/03/19,22:37:58+12" between "+254779653248",

         if(saveContact){
            contact = phoneNumber;
            saveContact = false;
         }
         else{
            phoneNumber = contact;
         }

         Serial.print("\n\nSender's phone number: ");
         Serial.println(contact);
         delay(300);

         // Wait for actual SMS text
         while (!gsmSerial.available());
         
         message = gsmSerial.readStringUntil('\n');
         message.trim(); // remove any whitespaces acompanying the above 
                        
         Serial.print("\nExtracted message: ");
         Serial.println(message); // sent to serial monitor
         delay(200); 
                
         if (message == "hello" or message == "Hello" or message == "HELLO"){
             String msg = "Admin phone number saved :)";         
             sendSMS(phoneNumber, msg); // Notification sms sent to admin about saving there contact number for sms alerts
             message = "";
             delay(300);
         }              
         if (message == "open" or message == "Open"){ // used by admin to grant access to person detected at the door
             executeTask(LOW, HIGH, 50, Duration);                 
         }
         noTone(buzzer);  // Stop the tone
         executeTask(HIGH, LOW, 0, 300);  
      }
}
#endif
