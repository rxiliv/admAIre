# AdmAIre: Wearable Assistance System for the Blind

AdmAIre is a cutting-edge wearable electronic device designed to enhance the autonomy and safety of blind or visually impaired individuals. Through the integration of artificial intelligence, computer vision, and advanced sensing, the device analyzes the surrounding environment in real-time, providing interactive and intuitive voice assistance.

## Project Overview
The goal of AdmAIre is to bridge the gap between the user and the physical environment. The system does not just detect obstacles; it "understands" the context, recognizing objects, faces, traffic signs, and text, communicating essential information through an integrated audio interface.

## System Architecture
The system is based on a low-latency circular data flow:
1. **Acquisition:** The wearable captures images (camera) and proximity data (sensors).
2. **Transmission:** Data is sent via **Bluetooth Low Energy (BLE)** or **Classic** to the smartphone.
3. **Processing:** The mobile app processes the data using AI models (Object Detection, OCR, Scene Description).
4. **Feedback:** The app sends the audio signal to the wearable, which plays it through the integrated output devices.

## Key Features
* **Object and Scene Recognition:** Real-time identification of everyday elements (means of transport, moving obstacles, furniture, signage).
* **Assisted Navigation:** Use of distance sensors (ToF/Ultrasonic) for acoustic warnings regarding the proximity of obstacles not detectable by the camera alone.
* **AI Voice Interface (NLP):** Two-way communication that allows the user to ask questions such as: *"Is there a free chair near me?"* or *"What bus number is arriving?"*.
* **Text Reading (OCR):** Ability to read labels, menus, or road signs in real-time.
* **Ergonomic Design:** Lightweight and discreet device, optimized for prolonged use.

## Technical Architecture

### Hardware
* **Camera:** Miniaturized low-power module for optimized frame streaming.
* **Sensors:** Ultrasonic or ToF (Time-of-Flight) distance sensors.
* **Communication:** Dual-mode Bluetooth module for data and audio transfer.
* **Audio Output:** Integrated audio drivers for voice assistance.
* **Power Supply:** Li-Po battery optimized to cover the entire day.

### Software
* **Computer Vision:** YOLO (You Only Look Once) or MobileNet models for real-time object detection.
* **Generative Artificial Intelligence:** Implementation of optimized LLM models to respond to user queries in natural language.
* **Text-to-Speech (TTS) & STT:** Low-latency voice engines for fluid interaction.
* **Sensor Fusion:** Proprietary algorithm to integrate camera data with proximity sensor data.
