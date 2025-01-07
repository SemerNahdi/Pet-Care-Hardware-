# 🐾 Pet Care System: Automated Food and Water Dispensing

## 📋 Overview
The **Pet Care System** is an automated solution for feeding and monitoring the water level of your pets. Using **ESP32**, **ultrasonic sensors**, and **servo motors**, it ensures your pets stay well-fed and hydrated — all controlled remotely through **Firebase**.

---

## 🐕 Key Features

### 🍽️ Automated Food Dispensing:
- Dispenses the requested amount of food directly into your pet's bowl.
- **Ultrasonic sensor** measures food in the container.
- **Servo motor** controls the dispensing action.

### 💧 Water Level Monitoring:
- **Ultrasonic sensor** detects the water level.
- **LED indicator** alerts when the water level is low.
- **Firebase updates** keep you informed about the water level.

---

## 🛠️ Hardware Components

- **ESP32/ESP8266** (the brain of the system)
- **HC-SR04 Ultrasonic Sensors** (for measuring food and water levels)
- **Servo Motor** (for dispensing food)
- **LED** (for water level indication)
- **Wi-Fi** connection to interact with Firebase.

---

## 📚 Software Libraries Used

- **ESP32Servo**: Controls the servo motor.
- **WiFi**: Manages the Wi-Fi connection.
- **Firebase_ESP_Client**: Integrates Firebase for real-time data communication.

---

## 🚀 Setup Instructions

1. **Connect the hardware**:
   - **Food Dispensing**: Connect ultrasonic sensor (TRIG_PIN 5, ECHO_PIN 4), and servo (SERVO_PIN 9).
   - **Water Dispensing**: Connect ultrasonic sensor (TRIG_PIN 5, ECHO_PIN 18), and LED (PIN 33).

2. **Configure Firebase**:
   - Create a Firebase project, enable **Realtime Database**, and get your **API_KEY** and **DATABASE_URL**.
   - Replace those values in the code to link the ESP32 with Firebase.

3. **Upload the code**:
   - Install required libraries: **ESP32Servo**, **WiFi**, **Firebase_ESP_Client**.
   - Upload both the **Food Dispensing** and **Water Monitoring** code to your ESP32.

4. **Monitor through Serial Monitor**:
   - See live updates on feeding and water status.

---

## 📱 Firebase Data Structure

### Food Dispensing
```json
/feedingRequests/
  requestedFood: (int, grams)

/feedingStatus/
  actionMessage: (string, dispensing action message)
```

### Water Monitoring
```json
/waterLevel: (string, "low" or "ok")
/distance: (float, water level distance in cm)
```

---

## 💡 Notes & Troubleshooting

- **Wi-Fi issues?** Check credentials and ensure your ESP32 is within range.
- **Servo not working?** Verify wiring and test it with a simple sketch.
- **Firebase errors?** Ensure correct API key and database URL.

---

## 🚀 Future Improvements
- Add **automated feeding schedules**.
- Integrate **automatic water dispensing** when low.
- Create a **mobile app** to manage the system.

---

## 📜 License
This project is open-source under the [MIT License](LICENSE).
