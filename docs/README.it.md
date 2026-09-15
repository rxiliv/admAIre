# AdmAIre: Sistema di Assistenza Indossabile per i Non Vedenti

AdmAIre è un dispositivo elettronico indossabile all'avanguardia studiato per accrescere l'autonomia e la sicurezza delle persone non vedenti o ipovedenti. Con l'integrazione dell'intelligenza artificiale, il dispositivo analizza l'ambiente circostante in tempo reale, fornendo un'assistenza vocale interattiva e intuitiva.

## Panoramica del Progetto
L'obiettivo principale di AdmAIre è quello di creare un ponte solido tra l'utente e l'ambiente fisico. Questo sistema non solo rileva gli ostacoli di fronte all'utente, comunicando le informazioni essenziali attraverso un'interfaccia audio integrata, ma permette anche all'utente di chiedere qualunque cosa, come se stesse parlando con una persona accanto a sé, ma mantenendo la loro totale indipendenza.

## Architettura di Sistema
Il sistema è basato su un flusso di dati circolare a bassa latenza:
1. **Acquisizione:** Gli occhiali catturano immagini, attraverso la telecamera integrata, ed eventuali input vocali.
2. **Trasmissione:** I dati vengono trasmessi tramite **Connessione Wi-Fi** al modello AI di OpenRouter (tramite l'API).
3. **Elaborazione:** I modelli AI elaborano i dati (Object Detection, OCR, Scene Description).
4. **Feedback:** Il modello manda indietro la risposta in formato testo agli occhiali, che la riproducono tramite i dispositivi di output integrati.

## Funzionalità Chiave

### Funzionalità Attualmente Implementate

**Analisi delle Circostanze**
* **Riconoscimento di Oggetti e Scene:** Identificazione in tempo reale di elementi quotidiani che possono costituire un pericolo.

**Design Ben Pensato**
* **Occhiali Ergonomici:** Dispositivo leggero e discreto, ottimizzato per l'uso prolungato.
* **Nessuna Ostruzione all'Udito:** Altoparlanti a conduzione ossera, scelti per evitare di ostruire le orecchie e per mantenere l'udito efficiente.
* **Montatura e Struttura Personalizzabile:** Completamente modificabile in base alla propria volontà, in quanto il modello 3D degli occhiali sono utilizzabili gratuitamente (vedi `license` per maggiori informazioni).

**Progetto con Codice Sorgente Disponibile**

Il progetto è stato creato con l'intenzione di essere utilizzato, modificato e migliorato da tutti, in base alle proprie preferenze, idee, e tanto altro, affinché i termini e condizioni della licenza del progetto, presenti nella `root` della repository, siano rispettate.

* **Codice Sorgente Disponibile:** Il codice in C++ del microcontrollore è scaricabile e modificabile.
* **Modello 3D Utilizzabile Gratuitamente:** Anche la montatura degli occhiali è disponibile come modello 3D, pronto per alterazioni, o direttamente disponibile per la stampante 3D.
* **Guida per l'Hardware:** Lo schema elettrico e la documentazione sono pensati perché si possa capire come funzioni il prototipo e utilizzare a proprio piacimento.

### Funzionalità Previste in Futuro
**Analisi Avanzata delle Circostanze**
* **Interfaccia Vocale AI (NLP):** Comunicazione bidirezionale che permette all'utente di fare domande come: *"C'è una sedia libera vicino a me?"* or *"Che pullman sta arrivando?"*.
* **Lettura di Testi (OCR):** Abilità di leggere etichette, menù o segnali stradali in tempo reale.

**Assistenza per Utenti Avanzata**
* **Attivazione Vocale:** L'utente può parlare con l'assistente AI attraverso la dicitura "Hey AdmAIre", individuata dal microfono integrato.
* **Avvisi Veloci e Brevi:** Quando l'utente si trova vicino ad ostacoli, il dispositivo rileva immediatamente il pericolo e avvisa l'utente.

## Architettura Tecnica

### Hardware
Il prototipo è basato sul microcontrollore **XIAO ESP32-S3 Sense**.
Lo schema elettrico si può trovare nella cartella `pcb`.

**Lista dei Componenti**
* **Microcontrollore:** Seeed Studio XIAO ESP32S3 Sense (con modulo camera e microfono).
* **Batteria:** LiPo 3.7V 803450 (1500mAh).
* **Ampplificatori Audio:** MAX98357A I2S 3W DAC (x2).
* **Altoparlanti a Conduzione Ossea:** 8 Ohm Magnetic, 1W (x2).
* **Slide Switch:** Slide Switch (SPST) per accensione/spegnimento.
* **Resistori:** Resistori da 100-200kΩ (x2).

Il prototipo della montatura degli occhiali è pensato per essere stampato in 3D (PLA).
Il modello 3D è disponibile nella cartella `cad`, sia come modello a pezzo unico, sia suddiviso in moduli in formato `.stl`, rendendolo adatto anche a stampanti 3D di dimensioni ridotte.

### Software
Il codice principale risiede nel microcontrollor e può essere trovato nella cartella `firmware`.

**Contenuto del Codice**
* **Codice del Microcontrollore:** Codice che permette all'ESP32-S3 di inviare e ricevere segnali radio, trasmettere le risposte dell'AI, e tutte le funzionalità attaualmente presenti.
* **Pagina HTML Semplice:** Pagine semplice per testare il dispositivo e fare debugging.

**Tecnologie e Funzionalità**
* **API di OpenRouter:** Implementazione dell'API di OpenRouter per usufruire di modelli AI ottimizzati per analizzare immagini e rispondere alle necessità e domande dell'utente in linguaggio naturale.
* **Text-to-Speech (TTS) & STT:** Meccanismi di interazione vocale a bassa latenza.
* **Ottimizzazione del Consumo di Energia:** Codice ottimizzato per minimizzare lo spreco energetico.