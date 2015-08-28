#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

File root;

// ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 13, 38, 178 };
byte gateway[] = { 10, 13, 38, 1 }; // Also used as DNS
byte subnet[] = { 255, 255, 255, 0 };
//IPAddress ip(10, 13, 38, 178);
IPAddress server(10, 13, 38, 177);

// serial connection
int serialBaud = 19200;
int serialCfg = SERIAL_8N1; // Default settings controller has 8 data, 1 stop and no parity.

// socket parameters
int serverPort = 8888;

// start TCP servers
EthernetClient client;

int rebootUnitPin = 31;

void setup() {
  delay(1250);
  digitalWrite(rebootUnitPin, HIGH);
  delay(2500);
  pinMode(rebootUnitPin, OUTPUT);
  Serial1.begin(serialBaud, serialCfg); // Open Serial1 communications
  Serial.begin(serialBaud, serialCfg); // Open Serial communications
  Ethernet.begin(mac, ip, gateway, gateway, subnet); // Start the Ethernet connection
  Serial.println();
  delay(250);
  Serial.println("Initializing system..");
  delay(2250);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println();
  // give the Ethernet shield a second to initialize:
  Serial.println("Initializing Ethernet");
  delay(1000);
  Serial.println("Connecting to ");
  // if you get a connection, report back via serial:
  if (client.connect(server, serverPort)) {
    Serial.println("Connected to: ");
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("Connection failed");
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");
  root = SD.open("/");
  printDirectory(root, 0);
  Serial.println("Booted system successfully!");
}

/*void daftPunk(String &clientMsg, String &daftCode) {
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
}
}*/

void rebootUnit(String &clientMsg, EthernetClient &client) {
  if (clientMsg == "#reboot-backdoor#\n") {
    Serial.println("#Initiating reboot..#");
    Serial1.println("#Initiating reboot..#");
    client.println("#Initiating reboot..#");
    for (uint8_t i = 5; i > 0; i--) {
      String r = String(i);
      Serial.println("#Rebooting in " + r + " seconds..#");
      Serial1.println("#Rebooting in " + r + " seconds..#");
      client.println("#Rebooting in " + r + " seconds..#");
      delay(1000);
    }
    delay(500);
    client.stop();
    delay(500);
    digitalWrite(rebootUnitPin, LOW);
    Serial.println("#Reboot failed!#");
    Serial1.println("#Reboot failed!#");
    Serial.println("#Unit is soft rebooting!#");
    Serial1.println("#Unit is soft rebooting!#");
    clientMsg = "";
    Serial.flush();
    Serial1.flush();
    client.flush();
  }
}

void printDirectory(File dir, int numTabs) {
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
}

void loop(void) {
  // listen for incoming clients
  //EthernetClient client = server.available();
  //String clientMsg = "";
  if (client) {
    String clientMsg = "";
    while (client.connected()) {

      // Receive
      int incomingByte2 = 0;
      while (client.available()) {
        incomingByte2 = client.read();
        clientMsg+=char(incomingByte2);
        if (char(incomingByte2) == '\n') {
          Serial.println();
          Serial.print(clientMsg); // Then send the message through Serial
          clientMsg = "";
          Serial.flush();
        }
      } // End of Receive

      // Transmit Serial1
      /*int incomingByte1 = 0; // For incoming serial data
      while (Serial1.available() > 0) { // If data has been received from the serial connection
        incomingByte1 = Serial1.read();
        clientMsg += char(incomingByte1);
        if (char(incomingByte1) == '\n') {
          client.print(clientMsg); // Then send the message through the Ethernet shield
          clientMsg = "";
          client.flush();
        }
      }*/ // End of Transmit Serial1

      // Arduino console
      int incomingByte = 0; // For incoming serial data
      while (Serial.available() > 0) { // If data has been received from the serial connection
        incomingByte = Serial.read();
        clientMsg += char(incomingByte);
        delay(1);
        if (char(incomingByte) == '\n') {
          rebootUnit(clientMsg, client);
          //Serial.println("Sent: " + clientMsg);
          client.print(clientMsg); // Then send the message through the Ethernet shield
          clientMsg = "";
          client.flush();
        }
      } // End of Arduino console

    } // End of if (client.connected())

  } else {

    if (!client.connected()) {
      Serial.println("Disconnecting..");
      Serial1.println("Disconnecting..");
      Ethernet.begin(mac, ip, gateway, gateway, subnet); // Start the Ethernet connection
      Serial.println();
      delay(250);
      Serial.println("Initializing system..");
      delay(2250);
      Serial.print("IP address: ");
      Serial.println(Ethernet.localIP());
      Serial.println();
      // give the Ethernet shield a second to initialize:
      Serial.println("Initializing Ethernet");
      delay(1000);
      Serial.println("Connecting to 10.13.38.177");
      // if you get a connection, report back via serial:
      if (client.connect(server, serverPort)) {
        Serial.println("Connected to: 10.13.38.177");
      }
      else {
        // if you didn't get a connection to the server:
        Serial.println("Connection failed");
      }
      //client.stop();
    }

  }
}