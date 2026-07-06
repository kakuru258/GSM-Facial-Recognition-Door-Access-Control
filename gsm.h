#ifndef GSM_H
#define GSM_H

bool offline, offlineAlert;
String atCommand, phoneNumber, contact; 

#include <SoftwareSerial.h>
// pins to be used for the SoftwareSerial UART: Rx pin is 3, Tx pin is 2 on Arduino
#define SIM800_TX_PIN 3
#define SIM800_RX_PIN 2
auto gsmSerial = SoftwareSerial(SIM800_TX_PIN, SIM800_RX_PIN); 

void gsm_initialize(){
//     pinMode(offlineSwitch, INPUT_PULLUP); // At logic HIGH by Default

     gsmSerial.begin(9600);  
     gsmSerial.println("AT");          
     delay(300);  
     
     gsmSerial.println("AT+CMGF=1");   
     delay(300);    
     
     gsmSerial.println("AT+CNMI=1,2,0,0,0"); 
     delay(300);
}

// function to send SMS text messages
void sendSMS(String recipient, String smsContent){ 
      gsmSerial.println("AT+CMGF=1"); 
      gsmSerial.println("AT+CSMP=17,167,0,0");   // Set SMS parameters
      delay(300); 
    
      gsmSerial.print("AT+CMGS=\"");
      gsmSerial.print(recipient); // Specify the recipient phone number
      gsmSerial.println("\"");
      delay(300); 
    
      gsmSerial.print(smsContent); // Specify the SMS content
      delay(300);
    
      gsmSerial.write(26); // Send the SMS by writing the ASCII code 26  
      delay(2000);
}
#endif
