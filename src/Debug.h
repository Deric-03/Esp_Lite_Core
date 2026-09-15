#pragma once
#include "mutex.h"
#include <Preferences.h>
#include <atomic>

class Debug {
  private:
  SemaphoreHandle_t DebugMutex = NULL;
  static constexpr int Size = 100;
  static constexpr int LineLen = 64;   // longueur max d'une ligne (troncature au-dela)
  char buffer[Size][LineLen];
  int head = 0;
  int count = 0; 
  bool SerialDebug = true;
  std::atomic<bool> init = false;
  Preferences prefs;
  
  void Print_NoLock(const String& Ni) {
    if (SerialDebug) Serial.println(Ni);
    Ni.toCharArray(buffer[head], LineLen);   // copie bornee + null-terminee, pas d'alloc
    head = (head + 1) % Size;
    if (count < Size) count++;
  }

  public:
  
  bool state() {
    return init;
  }

  void Print(const String& Ni) {
    if (!init) return;
    MutexLock lock(DebugMutex);
    Print_NoLock(Ni);
  }

  void Init(bool base) {
    DebugMutex = xSemaphoreCreateMutex();
    esp_reset_reason_t reason = esp_reset_reason();
    prefs.begin("config", false);
    SerialDebug = prefs.getBool("Debug", base);
    prefs.end();
    if (SerialDebug == true) {
      Serial.begin(115200);
    #if ARDUINO_USB_CDC_ON_BOOT
      Serial.setTxTimeoutMs(1);   // USB CDC : ecriture non bloquante si aucun moniteur ecoute
    #endif
      if (reason == ESP_RST_POWERON) {
        unsigned long t = millis();
        while (!Serial && millis() - t < 3000) {
          Print("Wait Terminal");
          delay(10);  // rend la main des que le moniteur est ouvert
        }
      }
    }
    init = true;
    Print("New Start------------------------------");
    if (reason != ESP_RST_POWERON) Print("Reset Code");
    if (SerialDebug) Print("Int BusSerial Ok");
  }
  
  int getNbDebug() {
    if (!init) return 0;
    MutexLock lock(DebugMutex);
    return count;
  }

  String getLine(int i) {
    if (!init) return "";
    MutexLock lock(DebugMutex);
    if (i < 0 || i >= count) return String();
    return String(buffer[(head - 1 - i + Size) % Size]);   // copie en String a la lecture
  }

  void SetSerialBus (bool Active) {
    if (!init) return;
    if (Active == SerialDebug) return;

    MutexLock lock(DebugMutex);
    
    if (!SerialDebug && Active) { 
      Serial.begin(115200);
      Print_NoLock("Int BusSerial Ok");
    } else if (SerialDebug && !Active) {
      Serial.end();
      Print_NoLock("BusSerial Close");
    }
    
    SerialDebug = Active;
    prefs.begin("config", false);
    prefs.putBool("Debug", SerialDebug);
    prefs.end();
  }
    
};