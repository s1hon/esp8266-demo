#define DEBUG true // turn debug message on or off in serial
#include <SoftwareSerial.h>
SoftwareSerial Serial1(4,5);

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CWMODE=3\r\n", 1000, DEBUG); // configure as access point
  sendData("AT+CWJAP=\"YOUR_WIFI_NAME\",\"YOUR_WIFI_PASSWD\"\r\n", 3000, DEBUG); // connect wifi AP
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address //192.168.4.1
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,8888\r\n", 1000, DEBUG); // turn on server on port 80
}

void loop() {
  if (Serial1.available()) { // check if the esp is sending a message

    if (Serial1.find("+IPD,")) {
      delay(1000); // wait for the serial buffer to fill up (read all the serial data)
      int connectionId = Serial1.read() - 48;

      // on your browser type http://YOURIP/?pin=13
      Serial1.find("pin="); // advance cursor to "pin="

      // get 3 digits
      int pinNumber = (Serial1.read() - 48) * 10;
      pinNumber += (Serial1.read() - 48);

      // toggle pin
      digitalWrite(pinNumber, !digitalRead(pinNumber));

      // generate web page
      String qstring;

      if( Serial1.find("status=1") ){
        // You can write any codes to get the sensor status
        // just need to save result to r variable
        // for now, we set 100 for r to test json response
        int r = 100;
        qstring = "{ \"pin\": " + String(pinNumber) + ", \"status\": " + String(r) + " }\n";
      }else{
        qstring = "{ \"pin\": " + String(pinNumber) + " }\n";
      }

      String webpage[] = {
        "HTTP/1.1 200 OK\n",
        "Content-Type: application/json\n",
        "Access-Control-Allow-Origin: *\n",
        "Connection: close\n",
        "\n",
        qstring };

      for (int i=0; i<6; i++ ){
        String cipSend = "AT+CIPSEND=";
        cipSend += connectionId;
        cipSend += ",";
        cipSend += webpage[i].length();
        cipSend += "\r\n";
        sendData(cipSend, 500, DEBUG);
        sendData(webpage[i], 500, DEBUG);
      }


      // make close command
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";

      sendData(closeCommand, 1000, DEBUG); // close connection
    }
  }
}

/*
 * Name: sendData
 * Description: Function used to send data to ESP8266.
 * Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
 * Returns: The response from the esp8266 (if there is a reponse)
 */
String sendData(String command, const int timeout, boolean debug) {
  String response = "";

  Serial1.print(command); // send the read character to the esp8266

  long int time = millis();

  while ((time + timeout) > millis()) {
    while (Serial1.available()) {

      // The esp has data so display its output to the serial window
      char c = Serial1.read(); // read the next character.
      response += c;
    }
  }

  if (debug) {
    Serial.print(response);
  }

  return response;
}
