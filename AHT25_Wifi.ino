#include "AHT25_Wifi.h"

void setup() {

  /** Serial initializing */
  initialSerial();

  /** Wifi and Web Server initializing */
  // アクセスされた際に行う関数を登録する
  serveWeb();
}

void loop() {

  /** Auto Reset */
  autoReset();

  /** Wifi and Web Server */
  WifiAndWeb();
}