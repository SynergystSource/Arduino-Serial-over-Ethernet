#include <SPI.h>
#include <Ethernet.h>
//#include <SD.h>

//File root;

// ethernet
byte mac[] = { 0xD3, 0x4D, 0xB3, 0x3F, 0xF3, 0x3D };
byte ip[] = { 10, 13, 38, 177 };
byte gateway[] = { 10, 13, 38, 1 };
byte subnet[] = { 255, 255, 255, 0 };

// serial connection
int serialBaud = 19200;
int serialCfg = SERIAL_8N1; // Default settings controller has 8 data, 1 stop and no parity.

// socket parameters
int serverPort = 8888;

// start TCP servers
EthernetServer server(serverPort);

int rebootUnitPin = 31;

void setup() {
  delay(1250);
  digitalWrite(rebootUnitPin, HIGH);
  delay(2500);
  pinMode(rebootUnitPin, OUTPUT);
  Serial1.begin(serialBaud, serialCfg); // Open Serial1 communications
  Serial.begin(serialBaud, serialCfg); // Open Serial communications
  Ethernet.begin(mac, ip, gateway, subnet); // Start the Ethernet connection
  server.begin(); // Begin listening for TCP connections
  Serial.println();
  delay(250);
  Serial.println("Initializing..");
  delay(2250);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println();
  /*Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");
  root = SD.open("/");
  printDirectory(root, 0);*/
  Serial.println("Booted system successfully!");
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    String clientMsg = "";
    //String daftCode = "";
    while (client.connected()) {

      // Transmit
      while (client.available()) {
        char c = client.read();
        /*Serial.write(c);
        Serial1.write(c);*/
        clientMsg += c; // Store received chars up to newline
        if (c == '\n') {
          //daftPunk(clientMsg, daftCode);
          rebootUnit(clientMsg, client);
          // REMOVED
          Serial1.print(clientMsg); // Then send the message through Serial1
          Serial.print(clientMsg); // Then send the message through Serial
          clientMsg = "";
          Serial1.flush();
          Serial.flush();
        }
      }

      // Receive
      int incomingByte1 = 0; // For incoming serial data
      while (Serial1.available() > 0) { // If data has been received from the serial connection
        incomingByte1 = Serial1.read();
        client.print(char(incomingByte1)); // Print the char data back to the client
        if (char(incomingByte1) == '\n')
          client.flush();
      }

      // Arduino console
      int incomingByte = 0; // For incoming serial data
      while (Serial.available() > 0) { // If data has been received from the serial connection
        incomingByte = Serial.read();
        client.print(char(incomingByte)); // Print the char data back to the client
        if (char(incomingByte) == '\n')
          client.flush();
      }
    }
  }
}

/*void daftPunk(String clientMsg, String daftCode) {
  if (clientMsg == "#daft#\n") {
    String daftName = "daft";
    Serial.println("Initializing SD card..");
    root = SD.open(daftName);
    if (root) {
      Serial.println("Found: " + daftName); // FIXME
      Serial.println("Loading: " + daftName); // FIXME
      while (root.available()) {
        char daftChar = root.read();
        daftCode += daftChar;
      }
      Serial.println("Playing Daft Punk - Areodynamic");
      //Serial.print(daftCode);
      Serial1.print(daftCode);
      root.close();
    } else {
      Serial.println("Error opening: " + daftName); // FIXME
    }
    clientMsg = "";
    daftCode = "";
    Serial1.flush();
    Serial.flush();
  }
}*/

void rebootUnit(String &clientMsg, EthernetClient &client) {
  if (clientMsg == "#reboot-remote#\n") {
    Serial.println("#Initiating local reboot..#");
    Serial1.println("#Initiating local reboot..#");
    client.println("#Initiating remote reboot..#");
    for (uint8_t i = 5; i > 0; i--) {
      String r = String(i);
      Serial.println("#Rebooting in " + r + " seconds..#");
      Serial1.println("#Rebooting in " + r + " seconds..#");
      client.println("#Rebooting in " + r + " seconds..#");
      delay(1000);
    }
    delay(500);
    digitalWrite(rebootUnitPin, LOW);
    Serial.println("#Local reboot failed!#");
    Serial1.println("#Local reboot failed!#");
    client.println("#Remote reboot failed!#");
    clientMsg = "";
    Serial.flush();
    Serial1.flush();
    client.flush();
  }
}

/*void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}*/
