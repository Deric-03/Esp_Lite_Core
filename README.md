# Esp_Lite_Core

Socle commun pour ESP32 : un journal de debug circulaire et un verrou RAII FreeRTOS.

Partagé par les bibliothèques Control_Led_Bus, Qt_Lite_Esp et Network_Lite_Esp. Ces classes vivent ici et nulle part ailleurs : deux copies dans deux bibliothèques donneraient deux classes `Debug` distinctes, incompatibles dès qu'un pointeur passe de l'une à l'autre.

## Contenu

### `Debug` — journal de debug

- Journal circulaire en RAM des 100 dernières lignes (64 caractères au plus chacune), sans allocation dynamique.
- Sortie série activable, et **mémorisée** en NVS : le réglage survit au redémarrage.
- Consultable ligne par ligne avec `getNbDebug()` et `getLine()`, pour l'afficher sur un écran.
- Au démarrage à froid, attend l'ouverture du moniteur série (3 s au plus) pour ne perdre aucune ligne.
- Écriture non bloquante en USB CDC quand aucun moniteur n'écoute.
- Protégé par un mutex : utilisable depuis plusieurs tâches.

### `MutexLock` — verrou RAII

Prend un `SemaphoreHandle_t` FreeRTOS à la construction et le relâche à la destruction, y compris sur un `return` anticipé. `lock()` et `unlock()` restent disponibles pour le relâcher plus tôt.

## Exemple

```cpp
#include <Esp_Lite_Core.h>

Debug debug;
SemaphoreHandle_t mtx;
int compteur = 0;

void setup() {
  // true : sortie serie active par defaut. Le choix enregistre ensuite
  // par SetSerialBus() prime sur cette valeur.
  debug.Init(true);

  mtx = xSemaphoreCreateMutex();
  debug.Print("Demarrage");
}

void loop() {
  {
    MutexLock lock(mtx);          // relache en sortie de bloc
    compteur++;
  }

  // Les dernieres lignes, pour un affichage a l'ecran.
  for (int i = 0; i < debug.getNbDebug(); i++) {
    String ligne = debug.getLine(i);   // 0 = la plus recente
  }

  delay(1000);
}
```

## Compatibilité

ESP32 et ses variantes, avec le core arduino-esp32.

## Licence

MIT — voir [LICENSE](LICENSE).
