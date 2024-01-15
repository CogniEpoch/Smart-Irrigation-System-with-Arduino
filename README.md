# Smart Irrigation System with Arduino

This Arduino project implements a smart irrigation system with soil moisture sensors, water level detection, and SMS notifications using a SIM800L module. The system controls irrigation lines with valves and water pumps based on soil moisture levels.

## Components

- Arduino UNO
- Moisture sensors
- Water level system 
- SIM800L module
- Relay modules for valves and water pump
- Connecting wires

## Hardware Setup

### Moisture Sensors

1. Connect each moisture sensor to the following analog pins on the Arduino:
   - Moisture Sensor 1: A0
   - Moisture Sensor 2: A1
   - Moisture Sensor 3: A2 (if applicable)
   - Moisture Sensor 4: A3 (if applicable)

2. Connect the VCC and GND pins of each moisture sensor to the corresponding power and ground pins on the Arduino.

### Wiring Instructions:

1. Strip a small portion of insulation from both ends of each wire.
2. Insert one wire into the water container at the desired low water level, ensuring it is submerged.
3. Connect the other wire to a higher position in the container, representing the desired high water level.
4. Connect the low water level wire to the designated digital pin on the Arduino.
5. Connect the high water level wire to the ground (GND) pin on the Arduino.

### How it Works:

- When water bridges the gap between the two wires, it completes the electrical circuit.
- The digital pin connected to the low water level wire reads HIGH.
- When there's no water bridging the wires, the digital pin reads LOW.

>[!important] 
>**Considerations:**
>- This method is basic and may not be as accurate or reliable as using dedicated water level sensors.
>- The reliability depends on the conductivity of the water, which can vary based on factors like water purity.
>- Calibration and testing in your specific setup are essential to ensure accurate readings.
>- Be cautious about corrosion of the wires over time if used for prolonged periods in water.

>[!Note]
>For a more accurate and reliable water level sensing, consider using specialized water level sensors designed for this purpose.

### SIM800L Module

4. Connect the SIM800L module to the following pins on the Arduino:
   - RX: Pin 10
   - TX: Pin 11
   - VCC: 5V 
   - GND: GND
>[!Note]
> Some mosules require more power as their base please use the needed power to reduce any problems

### Relay Modules

5. Connect each relay module to the designated pins on the Arduino:
   - Valve Relay 1: Pin 12
   - Valve Relay 2: Pin 13
   - Water Pump Relay: Specify a digital pin (e.g., pin 8,9)

6. Connect the VCC and GND pins of each relay module to the corresponding power and ground pins on the Arduino.

### Relay Wiring

7. Connect each relay module to the respective irrigation components:
   - Connect each valve relay module to the corresponding irrigation valve.
   - Connect the water pump relay module to the water pump.
>[!Note]
>If you plan to use one water pump for all of the lines, then you can set the pumpPin values to the desired pin

## Software Setup

1. Adjust threshold values and pin configurations in the code as needed.
2. Upload the provided Arduino code to your Arduino board.
   

## Configuration

- Configure moisture thresholds for each flower in the `flowers` array.
- Set pump activation times based on moisture levels.
- Adjust relay pin configurations for valves and the water pump.

## Usage

1. **Configure the System:**
   - Open the Arduino IDE and load the provided Arduino code onto your Arduino board.
   - Adjust threshold values for soil moisture levels, pump activation times, and relay pin configurations in the code according to your requirements.

2. **Connect the Hardware:**
   - Follow the hardware setup instructions detailed above to connect moisture sensors, the water level system, SIM800L module, and relay modules.

3. **Power Up the System:**
   - Power up your Arduino board using an appropriate power source.

4. **Monitor Serial Output:**
   - Connect your Arduino board to your computer.
   - Open the Arduino IDE Serial Monitor.
   - Run the program and monitor the serial output for real-time system status and debug information.
  
5. **Receive SMS Notifications:**
   - Ensure the SIM800L module is connected and configured.
   - When the system detects critically low water levels, it will send an SMS notification stating "Water level is critically low. Program paused."
   - Similarly, when the water level is refilled, you'll receive a notification saying "Water level has been refilled. Program resumed."

6. **Adjust and Monitor:**
   - Observe how the system responds to changes in soil moisture levels.
   - Experiment with different threshold values and pump activation times to optimize the irrigation process for your specific setup.

7. **Enjoy Smart Irrigation:**
   - Once configured and calibrated, the smart irrigation system will efficiently manage watering based on soil moisture levels, providing an automated and optimized solution for plant care.

>[!tip]
>Remember to periodically check the system's performance and make adjustments as needed for different plants or environmental conditions.


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Dependencies

This project relies on the following Arduino library:
- [SoftwareSerial library](https://www.arduino.cc/en/Reference/SoftwareSerial)

Install the required libraries before uploading the code to your Arduino board.


