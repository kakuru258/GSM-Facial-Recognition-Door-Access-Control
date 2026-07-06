/*
 The actual project code being used is in the projectCodeSketch file
// codeSketch_v1.0 
#include "gsm.h"
#include<Servo.h>
Servo servoMotor;
int servoPin = 5; 
int servoTrans = 11;

int red = 7;
int blue = 6;
int Duration = 5000;

int buzzer = 8;
int sirenDelay = 300;

String Name, Date, Time, command, extracted, input, message;

void setup() {
     Serial.begin(9600);
     gsm_initialize();
          
     servoMotor.attach(servoPin);
     pinMode(servoTrans, OUTPUT);

     digitalWrite(servoTrans, HIGH);                   
     servoMotor.write(0);
     delay(800);
     digitalWrite(servoTrans, LOW);
     delay(300);
     
     pinMode(red, OUTPUT);
     digitalWrite(red, HIGH); // LED indicator for closed Door at bootup
     
     pinMode(blue, OUTPUT);
     pinMode(buzzer, OUTPUT);
}

void loop() { 
     // Check if there is data sent to the GSM receive buffer 
     if (gsmSerial.available() > 0 and !offline) {    
        String sendToSerial = gsmSerial.readStringUntil('\n');
        sendToSerial.trim(); // remove any whitespaces acompanying the above

        Serial.print("\nGSM response: ");
        Serial.println(sendToSerial); // sent to serial monitor
        delay(300);
     }     
     int switchValue = digitalRead(offlineSwitch);
     if (!switchValue and offlineAlert){        
        offline = true;        
        if (offlineAlert){ // this condition ensures the offline alert sms is sent only once
//            sendSMS(phoneNumber, "System offline !!!");
            offlineAlert = false;           
            delay(100); 
        }
     }
     else if (switchValue){ // this condition ensures the offline alert sms is sent only once      
        offlineAlert = true;
        offline = false;
     }
     delay(300);
     
     if (Serial.available() > 0 and !offline){
        input = Serial.readString();
        input.trim(); // remove any white spaces

        Serial.print("\n\nSerial Monitor Input: ");
        Serial.print(input);

//      extract the message contents
        if (input.startsWith("sent:")) {
           command = extractContentInBetween('(',')');
           String extractedCommand = "\n\ncommand: " + command;
           Serial.print(extractedCommand);

           if (command == "open"){
              Name = extractContentInBetween(':',',');
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

        if (command == "open"){
            digitalWrite(red, LOW);
            digitalWrite(blue, HIGH);
            
            digitalWrite(servoTrans, HIGH);                   
            servoMotor.write(90);
            delay(Duration); 
            digitalWrite(servoTrans, LOW);
            delay(300);
        }
        else if (command == "unknown"){
            uint8_t count = 0;
            do{
              alarmSiren(LOW, HIGH, 800); // Higher tone (around 700–900 Hz works well)
              alarmSiren(HIGH, LOW, 500); // Lower tone (around 400–600 Hz)                           
              count += 1;
            }
            while(count < 15); // number of times to trigger the above Led siren alarm
        } 
        noTone(buzzer);  // Stop the tone
        digitalWrite(red, HIGH);
        digitalWrite(blue, LOW);
        
        digitalWrite(servoTrans, HIGH);                   
        servoMotor.write(0);
        delay(800);
        digitalWrite(servoTrans, LOW);
        delay(300);      
     }
     else if (offline){ // offline mode => GSM active
        wait_for_sms();
     }
     command = "";
}

void alarmSiren(bool firstLed, bool secondLed, int toneDuration){
      digitalWrite(red, firstLed);
      digitalWrite(blue, secondLed);
      tone(buzzer, toneDuration);   
      delay(sirenDelay);           
}

void wait_for_sms(){
    // Check if there is data sent to the GSM receive buffer 
    if (gsmSerial.available() > 0) {    
        String sendToSerial = gsmSerial.readStringUntil('\n');
        sendToSerial.trim(); // remove any whitespaces acompanying the above 
        
        // execute the following below if GSM recieves an SMS text message from a phone
        if (sendToSerial.startsWith("+CMT:")) {              
            int firstQuote  = sendToSerial.indexOf('"');  
            int comma = sendToSerial.indexOf(','); 
        
            phoneNumber = sendToSerial.substring(firstQuote+1, comma-1); // extracts number in between the first quote " and comma , but excludes the last quote next to the comma e.g. // Example: +CMT: "+254779653248","","26/03/19,22:37:58+12" between "+254779653248",
            Serial.print("\n\nSender's phone number: ");
            Serial.println(phoneNumber);
            delay(300);
                
            if (gsmSerial.available()) {    
                message = gsmSerial.readStringUntil('\n');
                message.trim(); // remove any whitespaces acompanying the above 
                        
                Serial.print("\nExtracted message: ");
                Serial.println(message); // sent to serial monitor
                delay(200); 
                
                if (message == "hello" or message == "Hello" or message == "HELLO"){
                    String msg = "phone number saved\nGSM offline mode active";         
                    sendSMS(phoneNumber, msg); // Notification sms sent to admin about saving there contact number for sms alerts
                    message = "";
                    delay(500);
                }
                
                if (message == "open"){                 
                   digitalWrite(red, LOW);
                   digitalWrite(blue, HIGH);
                                      
                   digitalWrite(servoTrans, HIGH);                  
                   servoMotor.write(90);
                   delay(Duration); 
                   digitalWrite(servoTrans, LOW); 
                   delay(300);                  
                }
                noTone(buzzer);  // Stop the tone
                digitalWrite(red, HIGH);
                digitalWrite(blue, LOW);
                
                digitalWrite(servoTrans, HIGH);                   
                servoMotor.write(0);
                delay(800);
                digitalWrite(servoTrans, LOW);
                delay(300);                  
            }  
        }
        else{
            Serial.print("\nGSM response: ");
            Serial.println(sendToSerial); // sent to serial monitor
            delay(300); 
        }
    }
    
    if (Serial.available() > 0){
        input = Serial.readString();
        input.trim(); // remove any white spaces

        Serial.print("\n\nSerial Monitor Input: ");
        Serial.print(input); 
        
//      extract the message contents
        if (input.startsWith("sent:")) {
           command = extractContentInBetween('(',')');
           String extractedCommand = "\n\ncommand: " + command;
           Serial.print(extractedCommand);

           if (command == "open"){
              Name = extractContentInBetween(':',',');
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

        if (command == "unknown"){
            String messageContent = "Unknown person at door !!\n";
            String messageToSend = messageContent + "\nDate: " + Date + ", Time: " + Time;
            sendSMS(phoneNumber, messageToSend);
            
            uint8_t count = 0;
            do{
              alarmSiren(LOW, HIGH, 800); // Higher tone (around 700–900 Hz works well)
              alarmSiren(HIGH, LOW, 500); // Lower tone (around 400–600 Hz)                           
              count += 1;
            }
            while(count < 15); // number of times to trigger the above Led siren alarm
        }
        else if (command == "open"){
            String messageContent = Name + " at door waiting for authorization\n";
            String messageToSend = messageContent + "\nDate: " + Date + ", Time: " + Time;
            sendSMS(phoneNumber, messageToSend);
        }                                   
        noTone(buzzer);  // Stop the tone
        digitalWrite(red, HIGH);
        digitalWrite(blue, LOW);
    }
}

String extractContentInBetween(char firstIndex, char lastIndex){                         
     extracted = input.substring(input.indexOf(firstIndex)+1, input.indexOf(lastIndex)); 
     return extracted;
}

*/
