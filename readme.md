# Arduino_iMFD
## 概要
ArduinoでPLX iMFDシリーズの製品と通信をするためのライブラリ。
ArduinoからiMFDシリーズへの情報送信、iMFDシリーズからArduinoへの情報受信が可能。

このライブラリは十分なテスト・デバッグを行えていません。想定しない動作を行う可能性があります。

## 使い方
既存のArduinoプロジェクトにArduino_iMFD.cppとArduino.hをコピーし、inoファイル内に`"#include "iMFD.h"`を追記。  
詳細は[reference.md](/reference/reference.md)を参照。

## ハードウェア構成
PLX iMFDシリーズとの通信を行うため
- Serial RXジャック
- Serial TXジャック
- TERMピン

の3つを実装済み。他デバイスとの接続はPLX iMFDシリーズの製品と同様。

### Serial RXジャック
ArduinoのRXピンと3極Φ3.5のLch(3極のうち先端側の接点)に接続する。
シリアルポートはどれでも良い。(iMFD.cppの先頭で定義み。変更可能。)

![シリアル受信の回路](/img/serial_RX_circuit.png)

### Serial TXジャック
ArduinoのTXピンを50Ω/200Ωで分圧し、3極Φ3.5のLch(3極のうち先端側の接点)に接続する。
分圧抵抗が無いと正常に動作しないので注意。
シリアルポートはどれでも良い。(iMFD.cppの先頭で定義み。変更可能。)

![シリアル送信の回路](/img/serial_TX_circuit.png)

### TERMピン
TERMピンの切り替え機能はソフトに実装済みなので、任意のデジタル入力で良い。[リファレンス参照。](/reference/reference.md#beginbool-isedgedeviceunsignedintinterval_msunsigned-int-baud)

## 動作を確認した環境
### マイコンボード
- Arduino Uno
- Arduino Leonardo
- Arduino Mega 2560

### IDE
- Arduino IDE 1.8.9
