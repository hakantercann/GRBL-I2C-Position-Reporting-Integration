#ifndef I2C_COMM_INTEGRATED_H
#define I2C_COMM_INTEGRATED_H

#include <stdint.h>

// Pin definitions for bit-banged I2C
#define I2C_SDA_PIN A4  // Use A4 if mist coolant disabled
#define I2C_SCL_PIN A5  // Use A5 (will disable probe functionality)

// Port and bit definitions for fast I/O
#define I2C_SDA_PORT PORTC
#define I2C_SDA_DDR  DDRC  
#define I2C_SDA_PIN_REG PINC
#define I2C_SDA_BIT  4     // A4 = PC4

#define I2C_SCL_PORT PORTC
#define I2C_SCL_DDR  DDRC
#define I2C_SCL_PIN_REG PINC  
#define I2C_SCL_BIT  5     // A5 = PC5

// I2C timing delay (adjust as needed)
#define I2C_DELAY_US 5

// Bit manipulation macros for fast I/O
#define SDA_HIGH() (I2C_SDA_DDR &= ~(1 << I2C_SDA_BIT))  // Set as input (pulled high)
#define SDA_LOW()  (I2C_SDA_DDR |= (1 << I2C_SDA_BIT), I2C_SDA_PORT &= ~(1 << I2C_SDA_BIT))
#define SCL_HIGH() (I2C_SCL_DDR &= ~(1 << I2C_SCL_BIT))  // Set as input (pulled high)  
#define SCL_LOW()  (I2C_SCL_DDR |= (1 << I2C_SCL_BIT), I2C_SCL_PORT &= ~(1 << I2C_SCL_BIT))
#define SDA_READ() (I2C_SDA_PIN_REG & (1 << I2C_SDA_BIT))

// Simple delay function
static inline void i2c_delay() {
  // Simple NOP-based delay instead of delayMicroseconds
  for(volatile uint8_t i = 0; i < 20; i++) {
    asm("nop");
  }
}
// I2C Start condition
static inline void i2c_start() {
  SDA_HIGH();
  SCL_HIGH();
  i2c_delay();
  SDA_LOW();
  i2c_delay();
  SCL_LOW();
  i2c_delay();
}

// I2C Stop condition  
static inline void i2c_stop() {
  SDA_LOW();
  i2c_delay();
  SCL_HIGH();
  i2c_delay();
  SDA_HIGH();
  i2c_delay();
}

// Write a single bit
static inline void i2c_write_bit(uint8_t bit) {
  if (bit) {
    SDA_HIGH();
  } else {
    SDA_LOW();
  }
  i2c_delay();
  SCL_HIGH();
  i2c_delay();
  SCL_LOW();
  i2c_delay();
}

// Read acknowledge bit
static inline uint8_t i2c_read_ack() {
  uint8_t ack;
  SDA_HIGH();  // Release SDA
  i2c_delay();
  SCL_HIGH();
  i2c_delay();
  ack = SDA_READ() ? 0 : 1;  // ACK is low
  SCL_LOW();
  i2c_delay();
  return ack;
}

// Write a byte
static inline uint8_t i2c_write_byte(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    i2c_write_bit(data & 0x80);
    data <<= 1;
  }
  return i2c_read_ack();
}

// Initialize I2C (bit-banged)
static inline void i2c_init() {
  // Set initial states
  I2C_SDA_PORT |= (1 << I2C_SDA_BIT);  // Enable internal pull-up
  I2C_SCL_PORT |= (1 << I2C_SCL_BIT);  // Enable internal pull-up
  SDA_HIGH();
  SCL_HIGH();
}

// Send steps via I2C (complete function)
static inline void send_steps_via_i2c(int32_t *steps, uint8_t length) {

  i2c_start();
  
  // Send slave address with write bit
  if (!i2c_write_byte(0x08 << 1)) {  // 0x08 shifted left + write bit (0)
    i2c_stop();
    return; // No ACK received
  }
  
  // Send step data
  for (uint8_t i = 0; i < length; i++) {
    i2c_write_byte((uint8_t)((steps[i] >> 0) & 0xFF));   // LSB
    i2c_write_byte((uint8_t)((steps[i] >> 8) & 0xFF));   // 
    i2c_write_byte((uint8_t)((steps[i] >> 16) & 0xFF));  // 
    i2c_write_byte((uint8_t)((steps[i] >> 24) & 0xFF));  // MSB
  }
  
  i2c_stop();
}

#endif // I2C_COMM_INTEGRATED_H