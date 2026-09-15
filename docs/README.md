# AdmAIre: Wearable Assistance System for the Blind

AdmAIre is a cutting-edge wearable electronic device designed to enhance the autonomy and safety of blind or visually impaired individuals. Through the integration of artificial intelligence and computer vision, the device analyzes the surrounding environment in real-time, providing interactive and intuitive voice assistance.

## Project Overview
The goal of AdmAIre is to bridge the gap between the user and the physical environment. Not only the system quickly detects the obstacles in the user's way, communicating essential information through an integrated audio interface, but it also allows the user to ask anything, as if they were talking to a person nearby, but keeping their full independence.

## System Architecture
The system is based on a low-latency circular data flow:
1. **Acquisition:** The wearable captures images (camera) and eventual voice inputs.
2. **Transmission:** Data is sent via **Bluetooth Low Energy (BLE)** or **Classic** to the smartphone.
3. **Processing:** The mobile app processes the data using AI models (Object Detection, OCR, Scene Description).
4. **Feedback:** The app sends the audio signal to the wearable, which plays it through the integrated output devices.

## Key Features

### Surroundings Analysis
* **Object and Scene Recognition:** Real-time identification of everyday elements (means of transport, moving obstacles, furniture, signage).
* **AI Voice Interface (NLP):** Two-way communication that allows the user to ask questions such as: *"Is there a free chair near me?"* or *"What bus number is arriving?"*.
* **Text Reading (OCR):** Ability to read labels, menus, or road signs in real-time.

### Advanced User Assistance
* **Voice Activation:** The user can speak with the AI assistance through the phrase "Hey AdmAIre".
* **Quick and Brief Alerts:** When near to obstacles, the device immediately detects the danger and warns the user.

### Well-thought-out Design
* **Ergonomic Glasses:** Lightweight and discreet device, optimized for prolonged use.
* **No Hearing Obstruction:** Bone conduction speakers, studied to avoid obstructing the ears and keep the hearing unimpaired.
* **Customizable Frame and Structure:** Fully editable as one's wish, as the 3D model of the glasses are free to use and modify.

### Open-Source Project
The project was made to be free for everyone to use, edit and improve, based on one's own preferences, ideas, and much more.
* **Source Code Available:** The microcontroller's C++ code is downloadable and editable.
* **Free-to-use 3D Model:** The glasses frame is also available, ready for alterations, or directly for the 3D printer.
* **Guide for the Hardware:** The electrical schematics and the documentation is thought for people to understand how the prototype works and to use as they prefer.

## Technical Architecture

### Hardware
The prototype is based on the **XIAO ESP32-S3 Sense** microcontroller.
The electronic schematics may be found within the **pcb** folder.

**Components List**
* **Microcontroller:** Seeed Studio XIAO ESP32S3 Sense (with camera and microphone).
* **Battery:** LiPo 3.7V 803450 (1500mAh).
* **Audio Amplifiers:** MAX98357A I2S 3W DAC (x2).
* **Bone Conduction Speakers:** 8 Ohm Magnetic, 1W (x2).
* **Slide Switch:** Slide Switch (SPST) for power-on/off.
* **Resistors** 100-200kΩ Resistors (x2).

The glasses frame are made through a 3D printer.
The 3D model is available in the **cad** folder.

### Software
The main code resides in the microcontroller and can be found in the **src** folder.

**Features and Technologies**
* **OpenRouter AI Model:** Implementation of optimized AI models to analyze images and to respond to user queries in natural language.
* **Text-to-Speech (TTS) & STT:** Low-latency voice engines for fluid interaction.
* **Optimization of energy consumption:** Optimized code for minimal energy waste.
