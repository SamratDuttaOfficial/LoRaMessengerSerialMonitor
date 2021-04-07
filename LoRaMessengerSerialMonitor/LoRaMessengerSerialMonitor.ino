//Arduino.cc references: https://www.arduino.cc/reference/en/
//Read more about strings on https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/
//Read about LoRa packet size on: https://www.thethingsnetwork.org/forum/t/limitations-data-rate-packet-size-30-seconds-uplink-and-10-messages-downlink-per-day-fair-access-policy-guidelines/1300

#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

byte groupAddress = 0x00;     // address to send a message to every LoRa in range. Used for SOS. 
byte localAddress = 0xBB;     // address of this device
byte destinationAddress;      // destination to send to
//Ranges from 0-0xFF
//0x means the address is represented in Hexadecimal. 0xFF is 255 in byte format. In hex, FF is  15*(16^1)+ 15*(16^0) = 255.
//Capitalization doesn't matter here.

int packetSize;
String destinationAddressString;
String incomingString;
String outgoingString;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setTxPower(17, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  // We are using different addresses for sender and receiver. The sync word can be used to separate zones. 
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK! SyncWord is 0xF3. Your address is 0xBB(HEX)/187(BYTE).");
  Serial.println("Message format: <receiver_address><space><message>");
  Serial.println("Message example: 0xFF Hello World.");
  Serial.println("Send to address 0x00 to send a message to every LoRa in range (Group Message). Used for SOS.");
  Serial.println("Maximum message length is 35 characters.");
  Serial.println();
}

byte HEXStringtoByte(String inputString){
  int a = (int) strtol( &inputString[2], NULL, 16);
  int b = (int) strtol( &inputString[3], NULL, 16);
  int c = (a*16) + b;
  return c;
}

void loop() {
  //Serial.println("Type your message to send...");
  if (Serial.available()){ //you have to use delay to keep this logic running. otherwise just remove it (might be unsafe).
    //Serial.println("Serial available.");
      incomingString = Serial.readString();
      
    //The format of the incomingString will be like "0xF3 Hey, how are you?" The first four characters will be the receiver's address. 
    //The substring of the address will be substr(0, 4), signifying from 0th character to 3rd character; total first 4 characters.
    //The substring of the outgoingString will be substr(5). It signifies we're taking a substring begining from 5th character to the end.
    //It cuts the space after the address (0xFF), on position 4.
    
    if(incomingString.length()>5){ //First 5 characters will just be the address I'mm sending it to, followed by a space.
      destinationAddressString = incomingString.substring(0, 4); //We can't use substr in Arduino. We gotta use substring.
      destinationAddress = HEXStringtoByte(destinationAddressString); //Arduino does not support stoi() function, so we made our own.
      outgoingString = incomingString.substring(5);
      outgoingString.trim(); //It trims extra whitespaces and extra new lines.
      packetSize = outgoingString.length() + 1; //one character is 1 byte, so we're just counting the string length.
                                                //In C, there's an extra NULL character in the end. So we're adding an extra 1 byte.
      if(packetSize < 36){  //Max Packetzise of LoRa is 51 bytes for the slowest data rates, SF10, SF11 and SF12 on 125kHz. 
                            //13 to 28 byte is reserved for header. Here we're using lesser or equal to 13 bytes (probably).                                  
        sendMessage(outgoingString);
      }
      else{
        Serial.println("The packetsize is too big!");
      }
    }
    delay(100); //this delay is important. otherwise it won't check for Serial.available after some time and the whole thing stops.
    //Serial.println("Listening...");
  }
  //other ways you can use the delay to keep this runnning.
  //delay(100);
  //else delay(1);
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  String messStr = outgoing;
  Serial.println("[To: " + String(destinationAddress) + "]: " + String(messStr) + " [Packet Size: " + String(packetSize) + "]");
  Serial.print("[Sending...");  
  LoRa.beginPacket();
  LoRa.write(destinationAddress);        // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(messStr.length()); 
  LoRa.print(messStr);
  LoRa.endPacket();
  Serial.println("Sent!]");
}

void onReceive(int packetSize) {
  //Serial.println("IncomingAVAILABLE");
  if (packetSize == 0) return; // if there's no packet, return

  // read packet header bytes:
  byte recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingLength = LoRa.read();   // incoming msg length

  // if the recipient isn't this device or broadcast,
  if ((recipient != localAddress)  && (recipient != groupAddress)) {
    //Serial.println("This message is not for me.");
    return; // skip rest of function
  }

  String incoming = "";

  while (LoRa.available()) {
    //Serial.println("Receiving...");
    incoming += (char)LoRa.read();
  }
  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("[From: " + String(sender) + "]: error: message length does not match length");
    return; // skip rest of function
  }
  // if message is for this device, or broadcast, print details:
  //Serial.println("Message length: " + String(incomingLength));
  if (recipient == groupAddress)
    Serial.print("[Group Message]");
  Serial.println("[From: " + String(sender) + "]: " + incoming);
  Serial.print("[RSSI: " + String(LoRa.packetRssi()));
  Serial.print("; SNR: " + String(LoRa.packetSnr()));
  Serial.println("; PFE: " + String(long(LoRa.packetFrequencyError())) + "]");
}
