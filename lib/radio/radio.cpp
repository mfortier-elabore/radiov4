#include "radio.h"

// STM32WL internal radio module
// Note: STM32WL55 has an internal SubGHz radio, no external pins needed
STM32WLx radio = new STM32WLx_Module();

static RadioState currentState = RADIO_STATE_IDLE;
static int16_t lastRSSI = 0;
static float lastSNR = 0.0;

bool radio_init() {
    Serial.print(F("[Radio] Initializing... "));
    
    // Initialize the radio with LoRa settings
    int state = radio.begin(
        RADIO_FREQ,     // Frequency
        RADIO_BW,       // Bandwidth
        RADIO_SF,       // Spreading Factor
        RADIO_CR,       // Coding Rate
        RADIO_SYNC_WORD,// Sync Word
        RADIO_POWER     // Output Power
    );
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
        currentState = RADIO_STATE_IDLE;
        
        // Set additional configuration
        radio.setCRC(true);  // Enable CRC
        radio.setCurrentLimit(140);  // Set current limit to 140mA
        
        return true;
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        currentState = RADIO_STATE_ERROR;
        return false;
    }
}

bool radio_transmit(const char* data) {
    return radio_transmit((uint8_t*)data, strlen(data));
}

bool radio_transmit(uint8_t* data, size_t len) {
    currentState = RADIO_STATE_TX;
    
    Serial.print(F("[Radio] Transmitting ("));
    Serial.print(len);
    Serial.print(F(" bytes)... "));
    
    int state = radio.transmit(data, len);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
        currentState = RADIO_STATE_IDLE;
        return true;
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        currentState = RADIO_STATE_ERROR;
        return false;
    }
}

bool radio_receive(uint8_t* buffer, size_t* len) {
    currentState = RADIO_STATE_RX;
    
    int state = radio.receive(buffer, *len);
    
    if (state == RADIOLIB_ERR_NONE) {
        // Packet received successfully
        *len = radio.getPacketLength();
        lastRSSI = radio.getRSSI();
        lastSNR = radio.getSNR();
        
        Serial.print(F("[Radio] Received packet ("));
        Serial.print(*len);
        Serial.print(F(" bytes), RSSI: "));
        Serial.print(lastRSSI);
        Serial.print(F(" dBm, SNR: "));
        Serial.print(lastSNR);
        Serial.println(F(" dB"));
        
        currentState = RADIO_STATE_IDLE;
        return true;
    } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        // Timeout - no packet received
        currentState = RADIO_STATE_IDLE;
        return false;
    } else {
        // Error occurred
        Serial.print(F("[Radio] Receive failed, code "));
        Serial.println(state);
        currentState = RADIO_STATE_ERROR;
        return false;
    }
}

bool radio_available() {
    // Start listening for incoming packets (non-blocking)
    int state = radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) {
        return false;
    }
    
    // Check if a packet was received
    return radio.available();
}

int16_t radio_getRSSI() {
    return lastRSSI;
}

float radio_getSNR() {
    return lastSNR;
}

RadioState radio_getState() {
    return currentState;
}

void radio_sleep() {
    radio.sleep();
    currentState = RADIO_STATE_IDLE;
    Serial.println(F("[Radio] Entering sleep mode"));
}

void radio_wakeup() {
    radio.standby();
    currentState = RADIO_STATE_IDLE;
    Serial.println(F("[Radio] Waking up"));
}
