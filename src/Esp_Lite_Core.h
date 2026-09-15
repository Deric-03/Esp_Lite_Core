#pragma once

/*
  Socle commun aux bibliotheques Qt_Lite_Esp et Control_Led_Bus.

      #include <Esp_Lite_Core.h>

  Regroupe ce que les deux ont besoin de partager. Ces classes vivent ici
  et nulle part ailleurs : deux copies dans deux bibliotheques donneraient
  deux classes Debug distinctes, incompatibles entre elles des qu'un
  pointeur traverse la frontiere d'une lib a l'autre.

  Contenu :
    MutexLock -- verrou RAII sur un SemaphoreHandle_t FreeRTOS
    Debug     -- journal circulaire, avec sortie serie optionnelle
*/

#include "mutex.h"
#include "Debug.h"
