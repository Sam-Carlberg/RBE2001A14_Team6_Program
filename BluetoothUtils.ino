/**
 * BLUETOOTHUTILS.INO
 *
 * Contains all the functionality to read, parse, and send messages to and from the field controller.
 */


BluetoothMaster bluetooth;
ReactorProtocol protocol(TEAM_NUMBER);

// variables to hold various bits from an incoming packet
byte data[3];
byte type;
byte destination;
byte storageData;
byte supplyData;

/*
 *  Try to read and parse a packet. Returns true if a packet was read and was for us, otherwise returns false.
 *  This method sets several global variables for use by line counting and movement methods
 */
boolean readPacket() {
  byte packet[10];

  if(bluetooth.readPacket(packet)) {
    if(packet[4] != TEAM_NUMBER && packet[4] != ALL_ROBOTS) {
      return false;
    }

    type        = packet[2]; // set the message type
    destination = packet[4]; // set teh message destination

    if(type == STOP_MOVEMENT)  shouldMove = false;
    if(type == START_MOVEMENT) shouldMove = true;

    if(protocol.getData(packet, data, type)) { // attempt to parse storage and supply data
      switch(type) {
      case STORAGE_DATA:
        storageData = data[0];
        break;
      case SUPPLY_DATA:
        supplyData = data[0];        
        break;
      }

      if(currentState < DRIVE_TO_STORAGE_LANE) { // only check new rods up until we start driving to storage and supply
        for(int i = 0; i < 4; i++) { // loop over first four LSBs until we hit a zero
          if(bitRead(storageData, i) == 0) {
            storageTarget = i + 1; // add one to go from 0..3 to 1..4 (lane number)
            break;
          }
        }

        for(int i = 0; i < 4; i++) { // loop over first four LSBs until we hit a one
          if(bitRead(supplyData, i) == 1) {
            supplyTarget = i + 1; // add one to go from 0..3 to 1..4 (lane number)
            break;
          }
        }
      }
    }
    return true;
  }
  return false;
}

/* 
 *  Sends a radiation alert to the field.
 *  Call this no more than once per second, and only if we have a rod.
 */
void sendRadiationAlert() {
  byte packet[10];
  byte data[3];
  byte size;
  protocol.setDst(ALL_ROBOTS);
  data[0] = radiationLevel;
  size = protocol.createPkt(RADIATION_ALERT, data, packet);
  bluetooth.sendPkt(packet, size);
}

/*   
 *  Generate and send the heartbeat message. Call this every 1 to 3 seconds
 */
void sendHeartbeat() {
  byte data[0];    
  byte packet[10];
  byte size;
  protocol.setDst(ALL_ROBOTS);
  size = protocol.createPkt(HEARTBEAT, data, packet);
  bluetooth.sendPkt(packet, size); 
}

/* 
 *  Sends any and all messages that need to be sent to the field controller.
 *  Also resets the flags for sending each type of message
 */
void sendMessages() {
  if(sendHB) {
    sendHeartbeat();
    sendHB = false;       // reset flag
  }
  if(sendRadAlert && radiationLevel != NO_RADIATION) {
    delay(20);            // delay to make sure the field gets the message
    sendRadiationAlert();
    sendRadAlert = false; // reset flag
  }
}

