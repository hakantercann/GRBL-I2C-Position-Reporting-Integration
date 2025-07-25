# 📡 GRBL I2C Position Reporting Integration

This project extends the GRBL firmware with the ability to send real-time position data over **I2C** to an external device (such as a display, secondary microcontroller, or logger). Communication is implemented using software (bit-banged) I2C on pins **A4 (SDA)** and **A5 (SCL)**.

---

## 🚀 Features

- Sends stepper motor position data from GRBL via I2C  
- Software-based I2C protocol (bit-banged)
- Transfers each axis as a 4-byte signed integer (little endian)
- Sends data periodically when the system is in `IDLE` or `CYCLE` state
- Safe pin usage check: avoids interference with GRBL probe functionality
- Lightweight, non-blocking integration with GRBL runtime

---

## ⚙️ Hardware Requirements

- Arduino Uno (or any GRBL-compatible ATmega328p board)
- External I2C-compatible device (receiver)
- Free **A4 (SDA)** and **A5 (SCL)** pins  
  > ⚠️ Note: If the probe feature is enabled in GRBL, **A5** cannot be used for I2C.  
  > 💡 You may modify the code to use other available digital pins if A4/A5 are not suitable for your setup.

---

## 🔧 Installation

1. Add `i2c_comm_integrated.h` to your GRBL project directory.
2. Include it in `protocol.c`:
   ```c
   #include "i2c_comm_integrated.h"
   ```
3. Call `init_i2c_safe();` at the start of `protocol_main_loop()`.
4. Call `update_i2c_data_safe();` inside the main loop (non-time-critical section).
5. (Optional) Use `report_i2c_status();` to include I2C status in GRBL reports.

---

## 🧪 I2C Transmission Format

Each axis position is sent as a 4-byte **signed integer** (`int32_t`), sent in **little-endian** format (LSB to MSB). The number of axes is defined by `N_AXIS`.

Example for a 3-axis system:
```
[Slave Address (0x08)] [X0][X1][X2][X3][Y0][Y1][Y2][Y3][Z0][Z1][Z2][Z3]
```

---

## ⏱ Timing & Performance

- Default send interval: **500ms** (`I2C_REPORT_INTERVAL_MS`)
- Only sends data if GRBL is in a **safe state**: `IDLE` or `CYCLE`
- Max retry attempts: **3** (`I2C_MAX_RETRIES`)
- Non-blocking logic minimizes impact on GRBL's real-time control

---

## 📁 File Structure

```
├── i2c_comm_integrated.h   → Bit-banged I2C driver and data send logic
├── protocol.c              → GRBL main loop integration
```
---

## 🤝 Contributing

Contributions and feedback are welcome! Feel free to fork the repo, improve the code, and open a pull request.
