#include "radio.h"

// STM32WL internal radio module
// Note: STM32WL55 has an internal SubGHz radio, no external pins needed
STM32WLx radio = new STM32WLx_Module();

static RadioState currentState = RADIO_STATE_IDLE;
static int16_t lastRSSI = 0;
static float lastSNR = 0.0;

// RF switch configuration for Nucleo WL55JC
static const uint32_t rfswitch_pins[] = {PC3, PC4, PC5, RADIOLIB_NC, RADIOLIB_NC};
static const Module::RfSwitchMode_t rfswitch_table[] = {
    {STM32WLx::MODE_IDLE,  {LOW,  LOW,  LOW}},
    {STM32WLx::MODE_RX,    {HIGH, HIGH, LOW}},
    {STM32WLx::MODE_TX_LP, {HIGH, HIGH, HIGH}},
    {STM32WLx::MODE_TX_HP, {HIGH, LOW,  HIGH}},
    END_OF_MODE_TABLE,
};

bool radio_init() {
    Serial.print(F("[Radio] Initializing... "));
    
    // Set RF switch configuration BEFORE calling begin()
    radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
    
    // Initialize the radio with LoRa settings
    int state = radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_POWER);
    
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print(F("failed, code "));
        Serial.println(state);
        currentState = RADIO_STATE_ERROR;
        return false;
    }
    
    // Set TCXO voltage AFTER begin()
    state = radio.setTCXO(RADIO_TCXO_VOLTAGE);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print(F("TCXO failed, code "));
        Serial.println(state);
        currentState = RADIO_STATE_ERROR;
        return false;
    }
    
    // Set additional configuration
    radio.setCRC(true);
    radio.setCurrentLimit(140);
    
    currentState = RADIO_STATE_IDLE;
    Serial.println(F("success!"));
    return true;
}

bool radio_transmit(const char* data) {
    return radio_transmit((uint8_t*)data, strlen(data));
}

bool radio_transmit(uint8_t* data, size_t len) {
    currentState = RADIO_STATE_TX;
    
    int state = radio.transmit(data, len);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.print(F("[Radio] TX OK ("));
        Serial.print(len);
        Serial.println(F(" bytes)"));
        currentState = RADIO_STATE_IDLE;
        return true;
    } else {
        Serial.print(F("[Radio] TX failed, code "));
        Serial.println(state);
        currentState = RADIO_STATE_ERROR;
        return false;
    }
}

bool radio_receive(uint8_t* buffer, size_t* len) {
    int state = radio.readData(buffer, *len);
    
    if (state == RADIOLIB_ERR_NONE) {
        *len = radio.getPacketLength();
        lastRSSI = radio.getRSSI();
        lastSNR = radio.getSNR();
        
        Serial.print(F("[Radio] RX OK ("));
        Serial.print(*len);
        Serial.print(F(" bytes) RSSI: "));
        Serial.print(lastRSSI);
        Serial.print(F(" dBm, SNR: "));
        Serial.print(lastSNR);
        Serial.println(F(" dB"));
        
        return true;
    } else {
        Serial.print(F("[Radio] RX failed, code "));
        Serial.println(state);
        return false;
    }
}

bool radio_available() {
    // Start listening if not already in RX mode
    if (currentState != RADIO_STATE_RX) {
        int state = radio.startReceive();
        if (state != RADIOLIB_ERR_NONE) {
            Serial.print(F("[Radio] Start RX failed, code "));
            Serial.println(state);
            return false;
        }
        currentState = RADIO_STATE_RX;
        Serial.println(F("[Radio] RX mode started"));
    }
    
    // Check if packet was received
    uint16_t irqStatus = radio.getIrqStatus();
    return (irqStatus & RADIOLIB_SX126X_IRQ_RX_DONE);
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
