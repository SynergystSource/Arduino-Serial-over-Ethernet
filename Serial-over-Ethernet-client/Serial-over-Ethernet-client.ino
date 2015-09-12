#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EEPROM.h>

File root;

// Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 13, 38, 178 };
byte gateway[] = { 10, 13, 38, 1 }; // Also used as DNS
byte subnet[] = { 255, 255, 255, 0 };
//IPAddress ip(10, 13, 38, 178);

// Serial connection
int serialBaud = 19200;
int serialCfg = SERIAL_8N1; // Default settings controller has 8 data, no parity and 1 stop

// Socket parameters
int serverPort = 8888;

// Start TCP servers
EthernetClient client;

int rebootUnitPin = 31;
int hardwareRebootPin = 30;
int setupConf = 28;
int remAddr0 = 0;
int remAddr1 = 1;
int remAddr2 = 2;
int remAddr3 = 3;

// More Ethernet..
int addOct0 = EEPROM.read(remAddr0);
int addOct1 = EEPROM.read(remAddr1);
int addOct2 = EEPROM.read(remAddr2);
int addOct3 = EEPROM.read(remAddr3);
IPAddress server(addOct0, addOct1, addOct2, addOct3);

// Shift register(output)
int dataPin = 32;
int latchPin = 34;
int clockPin = 36;
// More shift register stuffz
//int numberToDisplay = 0;

//byte dec_digits[] = {0b11000000,0b11111001,0b10100100,0b10110000,0b10011001,0b10010010,0b10000011,0b11111000,0b10000000,0b10011000 };
byte dec_digits[] = { 63, 6, 91, 79, 102, 109, 125, 7, 127, 111 };

void setup() {
  delay(250);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(setupConf, INPUT);
  digitalWrite(rebootUnitPin, HIGH);
  delay(250);
  pinMode(rebootUnitPin, OUTPUT);
  pinMode(hardwareRebootPin, INPUT);
  Serial2.begin(serialBaud, serialCfg); // Open Serial1 communications
  Serial1.begin(serialBaud, serialCfg); // Open Serial1 communications
  Serial.begin(serialBaud, serialCfg); // Open Serial communications
  Serial.println();
  Serial1.println();
  Serial.println();
  Serial1.println();
  Serial.println("Initializing system..");
  Serial1.println("Initializing system..");
  setupEEPROM();
  //Ethernet.begin(mac, ip, gateway, gateway, subnet);
  Ethernet.begin(mac); // Start the Ethernet connection
  delay(250);
  String serverIP = "";
  String daftCode = "";
  delay(250);
  printLocalAddress();
  getServerIP(serverIP, server);
  initEthernetLink(serverIP);
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
  initSDCard();
  Serial.println("Booted system successfully!");
  Serial1.println("Booted system successfully!");
}

void initEthernetLink(String serverIP) {
  // give the Ethernet shield a second to initialize:
  Serial.println("Initializing Ethernet link..");
  Serial1.println("Initializing Ethernet link..");
  delay(250);
  Serial.print("Connecting to ");
  Serial1.print("Connecting to ");
  Serial.println(serverIP);
  Serial1.println(serverIP);
}

void setupEEPROM() {
  if (digitalRead(setupConf) == LOW) {
    Serial.println();
    delay(275);
    Serial.println();
    Serial.println("Entered EEPROM setup mode..");
    Serial1.println("Entered EEPROM setup mode..");
    EEPROM.write(remAddr0, 10);
    EEPROM.write(remAddr1, 13);
    EEPROM.write(remAddr2, 38);
    EEPROM.write(remAddr3, 1);
    Serial.print("Remote IP(EEPROM): ");
    Serial1.print("Remote IP(EEPROM): ");
    printRemAddr();
    Serial.println();
    Serial1.println();
    displayNumOnLED(1);
  } else {
    Serial.println();
    delay(275);
    Serial.println();
    Serial.println("Skipping EEPROM setup mode..");
    Serial1.println("Skipping EEPROM setup mode..");
    Serial.print("Remote IP(EEPROM): ");
    Serial1.print("Remote IP(EEPROM): ");
    printRemAddr();
    displayNumOnLED(0);
  }
}

void getServerIP(String &serverIP, IPAddress server) {
  for (uint8_t m = 0; m < 4; m++) {
    serverIP += byte(server[m]);
    if (m < 3) {
      serverIP += ".";
    } else {
      // Do nothing
    }
  }
}

void displayNumOnLED(int numberToDisplay) {
  // take the latchPin low so
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, dec_digits[numberToDisplay] + 128);
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}

void printLocalAddress() {
  Serial.print("IP address: ");
  Serial1.print("IP address: ");
  Serial.println(Ethernet.localIP());
  Serial1.println(Ethernet.localIP());
}

void printRemAddr() {
  Serial.print(EEPROM.read(remAddr0));
  Serial1.print(EEPROM.read(remAddr0));
  Serial.print(".");
  Serial1.print(".");
  Serial.print(EEPROM.read(remAddr1));
  Serial1.print(EEPROM.read(remAddr1));
  Serial.print(".");
  Serial1.print(".");
  Serial.print(EEPROM.read(remAddr2));
  Serial1.print(EEPROM.read(remAddr2));
  Serial.print(".");
  Serial1.print(".");
  Serial.print(EEPROM.read(remAddr3));
  Serial1.print(EEPROM.read(remAddr3));
}

void daftPunk(String &daftCode) {
  String daftName = "daft";
  root = SD.open(daftName);
  if (root) {
    Serial.println("Found and loading: " + daftName);
    while (root.available()) {
      char daftChar = root.read();
      daftCode += daftChar;
    }
    //daftString = daftCode;
    root.close();
  } else {
    Serial.println("Error opening: " + daftName);
  }
  Serial1.flush();
  Serial.flush();
}

void pseudoFunc(String &clientMsg, EthernetClient &client, String daftCode) {
  clientMsg.replace("#daft#", daftCode);
}

void rebootUnit(String &clientMsg, EthernetClient &client) {
  if (clientMsg == "#reboot-backdoor#\n") {
    Serial.println("#Initiating client reboot..#");
    Serial1.println("#Initiating client reboot..#");
    client.println("#Initiating client reboot..#");
    for (uint8_t i = 5; i > 0; i--) {
      displayNumOnLED(i);
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

void hardwareReboot(EthernetClient &client) {
  Serial.println("#Initiating hardware pin client reboot..#");
  Serial1.println("#Initiating hardware pin client reboot..#");
  client.println("#Initiating hardware pin client reboot..#");
  for (uint8_t i = 5; i > 0; i--) {
    displayNumOnLED(i);
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
  Serial.println("#Client hardware pin reboot failed!#");
  Serial1.println("#Client hardware pin reboot failed!#");
  Serial.println("#Unit is soft rebooting!#");
  Serial1.println("#Unit is soft rebooting!#");
  Serial.flush();
  Serial1.flush();
  client.flush();
}

void initSDCard() {
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
    // Was a while loop
    if (client.connected()) {

      // Reboot via hardware input pin
      if (digitalRead(hardwareRebootPin) == HIGH) {
        Serial.println("#Caught reboot signal!#");
        Serial1.println("#Caught reboot signal!#");
        hardwareReboot(client);
      }

      // Receive
      int incomingClientByte = 0;
      if (client.available() > 0) {
        incomingClientByte = client.read();
        Serial.write(char(incomingClientByte));
        Serial1.write(char(incomingClientByte));
        Serial.flush();
        Serial1.flush();
      } // End of Receive

      // Transmit Serial2
      int incomingByte2 = 0; // For incoming serial data
      while (Serial2.available() > 0) { // If data has been received from the serial connection
        incomingByte2 = Serial2.read();
        client.write(char(incomingByte2));
        clientMsg += char(incomingByte2);
        if (char(incomingByte2) == '\n') {
          //rebootUnit(clientMsg, client);
          //client.print(clientMsg); // Then send the message through the Ethernet shield
          Serial.print(clientMsg);
          Serial1.print(clientMsg);
          clientMsg = "";
          client.flush();
          Serial.flush();
          Serial1.flush();
          Serial2.flush();
        }
      } // End of Transmit Serial2

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
      Ethernet.begin(mac);
      Serial.println();
      String serverIP = "";
      delay(250);
      printLocalAddress();
      initEthernetLink(serverIP);
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
