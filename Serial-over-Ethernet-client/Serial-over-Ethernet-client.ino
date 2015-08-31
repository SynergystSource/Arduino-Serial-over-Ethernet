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
  delay(250);
  digitalWrite(rebootUnitPin, HIGH);
  delay(250);
  pinMode(rebootUnitPin, OUTPUT);
  Serial1.begin(serialBaud, serialCfg); // Open Serial1 communications
  Serial.begin(serialBaud, serialCfg); // Open Serial communications
  Ethernet.begin(mac, ip, gateway, gateway, subnet); // Start the Ethernet connection
  Serial.println();
  Serial1.println();
  delay(250);
  Serial.println("Initializing system..");
  Serial1.println("Initializing system..");
  String serverIP = "";
  delay(250);
  Serial.print("IP address: ");
  Serial1.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial1.println(Ethernet.localIP());
  Serial.println();
  Serial1.println();
  // give the Ethernet shield a second to initialize:
  Serial.println("Initializing Ethernet");
  Serial1.println("Initializing Ethernet");
  getServerIP(serverIP, server);
  delay(250);
  Serial.print("Connecting to ");
  Serial1.print("Connecting to ");
  Serial.println(serverIP);
  Serial1.println(serverIP);
  // if you get a connection, report back via serial:
  if (client.connect(server, serverPort)) {
    Serial.print("Connected to: ");
    Serial1.print("Connected to: ");
    Serial.println(serverIP);
    Serial1.println(serverIP);
  } else {
    // if you didn't get a connection to the server:
    Serial.println("Connection failed");
    Serial1.println("Connection failed");
  }
  Serial.println("Initializing SD card...");
  Serial1.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("SD card initialization failed!");
    Serial1.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");
  Serial1.println("SD card initialization done.");
  root = SD.open("/");
  printDirectory(root, 0);
  Serial.println("Booted system successfully!");
  Serial1.println("Booted system successfully!");
}

void getServerIP(String &serverIP, IPAddress server) {
  for (uint8_t m = 0; m < 4; m++) {
    serverIP += byte(server[m]);
    //Serial.print(byte(server[m]));
    if (m < 3) {
      serverIP += ".";
      //Serial.print(".");
    } else {
      // Do nothing
      //Serial.println();
    }
  }
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
    Serial.println("#Initiating client reboot..#");
    Serial1.println("#Initiating client reboot..#");
    client.println("#Initiating client reboot..#");
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
    Serial.println("#Client reboot failed!#");
    Serial1.println("#Client reboot failed!#");
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
      Serial1.print('\t');
    }
    Serial.print(entry.name());
    Serial1.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      Serial1.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial1.print("\t\t");
      Serial.println(entry.size(), DEC);
      Serial1.println(entry.size(), DEC);
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
    // FIXME?
    while (client.connected()) {

      // Receive
      int incomingByte2 = 0;
      if (client.available() > 0) {
        incomingByte2 = client.read();
        clientMsg += char(incomingByte2);
        if (char(incomingByte2) == '\n') {
          // FIXME!!!
          //Serial.println();
          Serial.print(clientMsg);
          Serial1.print(clientMsg);
          clientMsg = "";
          Serial.flush();
          Serial1.flush();
        }
      } // End of Receive

      // Transmit Serial1
      int incomingByte1 = 0; // For incoming serial data
      while (Serial1.available() > 0) { // If data has been received from the serial connection
        incomingByte1 = Serial1.read();
        client.write(char(incomingByte1));
        clientMsg += char(incomingByte1);
        if (char(incomingByte1) == '\n') {
          rebootUnit(clientMsg, client);
          //client.print(clientMsg); // Then send the message through the Ethernet shield
          clientMsg = "";
          client.flush();
          Serial.flush();
          Serial1.flush();
        }
      } // End of Transmit Serial1

      // Arduino console
      int incomingByte = 0; // For incoming serial data
      while (Serial.available() > 0) { // If data has been received from the serial connection
        incomingByte = Serial.read();
        client.write(char(incomingByte));
        clientMsg += char(incomingByte);
        //delay(1);
        if (char(incomingByte) == '\n') {
          rebootUnit(clientMsg, client);
          //Serial.println("Sent: " + clientMsg);
          //client.print(clientMsg); // Then send the message through the Ethernet shield
          clientMsg = "";
          client.flush();
          Serial.flush();
          Serial1.flush();
        }
      } // End of Arduino console

    } // End of if (client.connected())

  } else {
    // Lost connection, attempt to reconnect..
    if (!client.connected()) {
      Serial.println("Disconnecting..");
      Serial1.println("Disconnecting..");
      Ethernet.begin(mac, ip, gateway, gateway, subnet); // Start the Ethernet connection
      Serial.println();
      delay(250);
      Serial.println("Initializing system..");
      Serial1.println("Initializing system..");
      String serverIP = "";
      delay(250);
      Serial.print("IP address: ");
      Serial.println(Ethernet.localIP());
      Serial.println();
      // give the Ethernet shield a second to initialize:
      Serial.println("Initializing Ethernet");
      Serial1.println("Initializing Ethernet");
      getServerIP(serverIP, server);
      delay(250);
      Serial.print("Connecting to ");
      Serial1.print("Connecting to ");
      Serial.println(serverIP);
      Serial1.println(serverIP);
      // if you get a connection, report back via serial:
      if (client.connect(server, serverPort)) {
        Serial.print("Connected to: ");
        Serial1.print("Connected to: ");
        Serial.println(serverIP);
        Serial1.println(serverIP);
      } else {
        // if you didn't get a connection to the server:
        Serial.println("Connection failed");
      }
      //client.stop();
    } // End of attempted reconnection..
  }
}
