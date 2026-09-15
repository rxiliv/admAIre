# AdmAIre: Wearable Assistance System for the Blind

AdmAIre is a cutting-edge wearable electronic device, in the form of a pair of simple glasses, designed to enhance the autonomy and safety of blind or visually impaired individuals. Through the integration of artificial intelligence and computer vision, the device analyzes the surrounding environment in real-time, providing interactive and intuitive voice assistance.

## Project Overview
The goal of AdmAIre is to bridge the gap between the user and the physical environment. Not only the system quickly detects the obstacles in the user's way, communicating essential information through an integrated audio interface, but it also allows the user to ask anything, as if they were talking to a person nearby, but keeping their full independence.

## System Architecture
The system is based on a low-latency circular data flow:
1. **Acquisition:** The wearable captures images (camera) and eventual voice inputs.
2. **Transmission:** Data is sent via **Wi-Fi connection** to the OpenRouter AI model (through the API).
3. **Processing:** The AI models process the data (Object Detection, OCR, Scene Description).
4. **Feedback:** The AI sends the text response back to the glasses, which plays it through the integrated output devices.

## Key Features

### Implemented Features

**Surroundings Analysis**
* **Object and Scene Recognition:** Real-time identification of potentially dangerous everyday elements.
* **Quick and Brief Alerts:** When near to obstacles, the device immediately detects the danger and warns the user.
* **Text Reading (OCR):** Ability to read labels, menus, or road signs in real-time.

**Well-thought-out Design**
* **Ergonomic Glasses:** Lightweight and discreet device, optimized for prolonged use.
* **No Hearing Obstruction:** Bone conduction speakers, studied to avoid obstructing the ears and keep the hearing unimpaired.
* **Customizable Frame and Structure:** Fully editable as one's wish, as the 3D model of the glasses are free to use and modify (check `license` for further information).

**Source-Available Project**

The project was made to be free for everyone to use, edit and improve, based on one's own preferences, ideas, and much more, as long as the project terms and conditions of the license, available in the `root` folder, are respected.

* **Source Code Available:** The microcontroller's C++ code is downloadable and editable.
* **Free-to-use 3D Model:** The eyeglasses frame is also available, ready for alterations, or directly for the 3D printer.
* **Guide for the Hardware:** The electrical schematics and the documentation is thought for people to understand how the prototype works and to use as they prefer.

### Planned Features
**Advanced Surroundings Analysis**
* **AI Voice Interface (NLP):** Two-way communication that allows the user to ask questions such as: *"Is there a free chair near me?"* or *"What bus number is arriving?"*.

**Advanced User Assistance**
* **Voice Activation:** The user can speak with the AI assistance through the phrase "Hey AdmAIre", captured by the integrated microphone.

## Technical Architecture

### Hardware
The prototype is based on the **XIAO ESP32-S3 Sense** microcontroller.
The electronic schematics may be found within the `pcb` folder.

**Components List**
* **Microcontroller:** Seeed Studio XIAO ESP32S3 Sense (with camera and microphone).
* **Battery:** LiPo 3.7V 803450 (1500mAh).
* **Audio Amplifiers:** MAX98357A I2S 3W DAC (x2).
* **Bone Conduction Speakers:** 8 Ohm Magnetic, 1W (x2).
* **Slide Switch:** Slide Switch (SPST) for power-on/off.
* **Resistors:** 100-200kΩ Resistors (x2).

The eyeglasses frame prototype is made through a 3D printer (PLA).
The 3D model is available in the `cad` folder, both as a one-piece and subdivided in `.stl` format, making it suitable for small 3D printers as well.

### Software
The main code resides in the microcontroller and can be found in the `firmware` folder.
The software is made through Arduino IDE.

**Code Content**
* **Microcontroller Code:** Code that allows the ESP32-S3 to send and receive radio signals, transmit the AI responses, and all the features mentioned above.
* **Simple HTML page:** Simple page for testing the device and debugging.

**Features and Technologies**
* **OpenRouter API:** Implementation of the OpenRouter API to use optimized AI models to analyze images and to respond to user queries in natural language.
* **Text-to-Speech (TTS) & STT:** Low-latency voice engines for fluid interaction.
* **Optimization of energy consumption:** Optimized code for minimal energy waste.
