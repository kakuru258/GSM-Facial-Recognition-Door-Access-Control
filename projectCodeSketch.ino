// codeSketch_v1.1
#include "gsm.h"
#include "door_automation.h"
#include "mainCodeSketch.h"

void setup() {
     Serial.begin(9600);
     gsm_initialize();
     servoSetUp();
     mainSketchSetUp();
}

void loop() { 
  // Check if there is data sent to the GSM receive buffer 
     if (gsmSerial.available() > 0) { 
        String sendToSerial = gsmSerial.readStringUntil('\n');
        sendToSerial.trim(); // remove any whitespaces acompanying the above

        Serial.print("\nGSM response: ");
        Serial.println(sendToSerial); // sent to serial monitor
               
        processTextMessage(sendToSerial); // inside mainCodeSketch.h file
     }     
   
     if (Serial.available() > 0){
        input = Serial.readString();
        input.trim(); // remove any white spaces

        Serial.print("\n\nSerial Monitor Input: ");
        Serial.print(input);

        extract_target_messageData(); // inside mainCodeSketch.h file
        
        if (command == "open"){
            executeTask(LOW, HIGH, 50, Duration);
            
            String messageContent = "Room access given to " + Name + "\n";
            String messageToSend = messageContent + "\nOn " + Date + ", at " + Time;
            sendSMS(phoneNumber, messageToSend);
                        
        }
        else if (command == "unknown"){
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
        noTone(buzzer);  // Stop the tone
        executeTask(HIGH, LOW, 0, 300); 
        command = "";    
     }
}
