
// bitmasks for storage and supply data tubes
byte storageData;
byte supplyData;

/**
 *  Call this in loop() to poll for new messages from the field computer.
 */
boolean readPacket() {
  boolean gotPacket = false;

  byte packet[10];    // buffer for receieved packets
  byte data[3];       // buffer for received data
  byte type;          // the received message type
  byte destination;   // the received message destination
  byte source;        // the source of the received message
  
  gotPacket = bluetooth.readPacket(packet);

  if (gotPacket) {
    // debug
    Serial.println("Got packet");
    for(int i = 0; i < 10; i++) {
      Serial.print(packet[i]); 
      Serial.print(" ");
    }
    Serial.println();

    // ignore a message if it ain't for us
    if(packet[4] != TEAM_NUMBER && packet[4] != 0x00) {
      return false;
    }
    type        = packet[2];
    source      = packet[3];
    destination = packet[4];
    
    if(type == STOP_MOVEMENT) shouldMove = false;
    if(type == START_MOVEMENT) shouldMove = true;

    if (protocol.getData(packet, data, type)) {
      switch (type) {
      case STORAGE_AVAILABILITY:
        storageData = data[0];
        break;
      case SUPPLY_AVAILABILITY:
        supplyData = data[0];
        break;
      default:
        break;
      }
      
      if(ArisGameState < 6) { // only calculate targets when we're not storing or receiving
        if(onLowSideOfField) {
          // find the lowest-numbered available supply tube
          for(int i = 0; i < 4; i++) {
            if(isSupplyTubeAvailable(i)) {
              supplyTarget = i;
              break;
            }
          }
          
          // find the lowest-numbered open storage tube
          for(int i = 0; i < 4; i++) {
            if(isStorageTubeAvailable(i)) {
              storageTarget = i;
              break;
            }
          }
        } else {
          // find the highest-numbered available supply tube
          for(int i = 3; i >= 0; i--) {
            if(isSupplyTubeAvailable(i)) {
              supplyTarget = i;
              break;
            }
          }
          
          // find the highest-numbered open storage tube
          for(int i = 3; i >= 0; i--) {
            if(isStorageTubeAvailable(i)) {
              storageTarget = i;
              break;
            }
          }        
        }
      }

      // for debugging
      Serial.print("Supply data: ");
      Serial.println(supplyData, BIN);

      Serial.print("Target: ");
      Serial.println(supplyTarget);

      Serial.print("Storage data: ");
      Serial.println(storageData, BIN);

      Serial.print("Target: ");
      Serial.println(storageTarget);
    }
  }

  return gotPacket;
}

/**
 *  Sends a radiation alert to the field.
 *  Call this no more than once per second, and only if we have a rod.
 */
void sendRadiationAlert() {
  byte packet[10];
  byte data[3];
  byte size;
  protocol.setDst(0x00);
  data[0] = radiationLevel;
  size = protocol.createPkt(RADIATION_ALERT, data, packet);
  bluetooth.sendPkt(packet, size);
}

/**
 *  Sends the robot's status to the field. Call this no more than once every 5 seconds.
 *  Are we even going to use this?
 */
void sendRobotStatus() {
  byte packet[10];
  byte data[3];
  byte size;
  protocol.setDst(0x00);
  data[0] = movementStatus;
  data[1] = gripperStatus;
  data[2] = operationStatus;
  size = protocol.createPkt(ROBOT_STATUS, data, packet);
  bluetooth.sendPkt(packet, size);
}

/**
 *  Generate and send the heartbeat message. Call this every 1 to 3 seconds
 */
void sendHeartbeat() {
  byte data[0];    
  byte packet[10];
  byte size;
  protocol.setDst(0x00);
  size = protocol.createPkt(HEARTBEAT, data, packet);
  bluetooth.sendPkt(packet, size); 
}

/**
 * Checks if the given storage tube has a rod (four tubes, zero-indexed)
 */
boolean isStorageTubeAvailable(int tubeNumber) {
  return bitRead(storageData, tubeNumber) == 1;
}

/**
 * Checks if the given supply tube is empty (four tubes, zero-indexed)
 */
boolean isSupplyTubeAvailable(int tubeNumber) {
  return bitRead(supplyData, tubeNumber) == 0;
}

/**
 *  Sends any and all messages that need to be sent to the field controller.
 *  Also resets the flags for sending each type of message
 */
void sendMessages() {
  if(sendHB) {
    sendHeartbeat();
    sendHB = false;       // reset flag
  }
  if(sendRadAlert && radiationLevel != NO_RAD) {
    delay(20);            // delay to make sure the field gets the message
    sendRadiationAlert();
    sendRadAlert = false; // reset flag
  }
  if(sendStatus) {
    delay(20);            // delay to make sure the field gets the message
    sendRobotStatus();
    sendStatus = false;   // reset flag
  }
}