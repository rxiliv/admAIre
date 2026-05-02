# AdmAIre: Sistema Indossabile di Assistenza per Non Vedenti

AdmAIre è un dispositivo elettronico indossabile all'avanguardia progettato per migliorare l'autonomia e la sicurezza delle persone non vedenti o ipovedenti. Attraverso l'integrazione di intelligenza artificiale, visione artificiale e sensoristica avanzata, il dispositivo analizza l'ambiente circostante in tempo reale, fornendo un'assistenza vocale interattiva e intuitiva.

## Panoramica del Progetto
L'obiettivo di AdmAIre è colmare il divario tra l'utente e l'ambiente fisico. Il sistema non si limita a rilevare ostacoli, ma "comprende" il contesto, riconoscendo oggetti, volti, segnali stradali e testo, comunicando le informazioni essenziali tramite un'interfaccia audio integrata.

## Architettura del Sistema
Il sistema si basa su un flusso di dati circolare a bassa latenza:
1. **Acquisizione:** Il wearable cattura immagini (camera) e dati di prossimità (sensori).
2. **Trasmissione:** I dati vengono inviati via **Bluetooth Low Energy (BLE)** o **Classic** allo smartphone.
3. **Elaborazione:** L'App mobile processa i dati tramite modelli di AI (Object Detection, OCR, Scene Description).
4. **Feedback:** L'App invia il segnale audio al wearable, che lo riproduce tramite i dispositivi di output integrati.

## Caratteristiche Principali
* **Riconoscimento Oggetti e Scenari:** Identificazione in tempo reale di elementi quotidiani (mezzi di trasporto, ostacoli mobili, arredi, segnaletica).
* **Navigazione Assistita:** Utilizzo di sensori di distanza (ToF/Ultrasuoni) per avvisi acustici sulla prossimità di ostacoli non rilevabili dalla sola telecamera.
* **Interfaccia Vocale AI (NLP):** Comunicazione bidirezionale che permette all'utente di porre domande del tipo: *"C'è una sedia libera vicino a me?"* o *"Che numero di autobus sta arrivando?"*.
* **Lettura Testo (OCR):** Capacità di leggere etichette, menu o cartelli stradali in tempo reale.
* **Design Ergonomico:** Dispositivo leggero e discreto, ottimizzato per un utilizzo prolungato.

## Architettura Tecnica

### Hardware
* **Camera:** Modulo miniaturizzato a basso consumo per lo streaming di frame ottimizzati.
* **Sensori:** Sensori di distanza a ultrasuoni o ToF (Time-of-Flight).
* **Comunicazione:** Modulo Bluetooth dual-mode per il trasferimento dati e audio.
* **Audio Output:** Driver audio integrati per l'assistenza vocale.
* **Alimentazione:** Batteria Li-Po ottimizzata per coprire l'intera giornata.

### Software
* **Computer Vision:** Modelli YOLO (You Only Look Once) o MobileNet per l'object detection in tempo reale.
* **Intelligenza Artificiale Generativa:** Implementazione di modelli LLM ottimizzati per rispondere alle query dell'utente in linguaggio naturale.
* **Text-to-Speech (TTS) & STT:** Motori vocali a bassa latenza per un'interazione fluida.
* **Sensor Fusion:** Algoritmo proprietario per integrare i dati della telecamera con i dati dei sensori di prossimità.
