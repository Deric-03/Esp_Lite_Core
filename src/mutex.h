#pragma once
class MutexLock {
  SemaphoreHandle_t &_m;
  bool _locked = false;
  public:
  
  MutexLock(SemaphoreHandle_t &m, bool autoLock = true) : _m(m) {
    if (autoLock) lock();
  }
  
  ~MutexLock() {
    if (_locked) unlock();
  }

  void lock() {
    if (!_locked) {
      xSemaphoreTake(_m, portMAX_DELAY);
      _locked = true;
    }
  }

  void unlock() {
    if (_locked) {
      xSemaphoreGive(_m);
      _locked = false;
    }
  }

};