#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include <RadioLib.h>

// Radio configuration
#define RADIO_FREQ 868.0        // Frequency in MHz (EU868)
#define RADIO_BW 125.0          // Bandwidth in kHz
#define RADIO_SF 9              // Spreading Factor (7-12)
#define RADIO_CR 7              // Coding Rate (5-8)
#define RADIO_SYNC_WORD 0x12    // Sync word
#define RADIO_POWER 14          // Output power in dBm

// Radio states
enum RadioState {
    RADIO_STATE_IDLE,
    RADIO_STATE_TX,
    RADIO_STATE_RX,
    RADIO_STATE_ERROR
};

// Initialize the radio
bool radio_init();

// Transmit data
bool radio_transmit(const char* data);
bool radio_transmit(uint8_t* data, size_t len);

// Receive data (non-blocking)
bool radio_receive(uint8_t* buffer, size_t* len);

// Check if data is available
bool radio_available();

// Get RSSI of last packet
int16_t radio_getRSSI();

// Get SNR of last packet
float radio_getSNR();

// Get current radio state
RadioState radio_getState();

// Set radio to sleep mode
void radio_sleep();

// Wake up radio
void radio_wakeup();

#endif // RADIO_H
