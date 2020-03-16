# FilmTactileSensor
FilmTactileSensor Based on ESP32

フィルム触覚センサのサンプルプログラムの説明です。状況に応じた複数のサンプルを用意しています。

## 共通
PC側のサンプルプログラムはProcessingで作成しています。
ESP32側のサンプルプログラムはArduino IDEで作成しています。

### 準備
センサのESP32-DevkitCをPCにUSBケーブルで接続し、シリアルポート番号を確認してください。
Arduino IDEでESP32プログラムを書き込んでください。

### Processing側の共通操作
1,2,3,4を押すと計測レンジを変更します。
sを押すと計測開始、再度sを押すと計測終了で、CSVファイルが作成されます。

## 有線接続の３本指触覚計測
- ESP32: Measurement5x10_3FingersESP32.ino
- Processing: Measurement5x10_3Fingers8bit.pde

このサンプルは、３本指の触覚センサをUSB接続で利用するものです。500Hzで計測出来ます。
動作させる前に、Processingプログラム中のシリアルポート番号を変更してください。

## 有線接続の１本指触覚計測
- ESP32: Measurement5x10_1Fingers8bitESP32.ino
- Processing: Measurement5x10_1Fingers8bitESP32.pde

このサンプルは、1本の指のみ計測することで高速なセンシングを実現するものです。1kHzの計測を行っています。
動作させる前に、Processingプログラム中のシリアルポート番号を変更してください。
Processing側でfを押すと計測する指が変わります。

## 3本指での高速化（無線）
- ESP32: Measurement5x10_3Fingers8bitWifiESP32.ino
- Processing: Measurement5x10_3Fingers8bitWiFi.pde

このサンプルはESP32自体をアクセスポイント(AP)とした、無線接続のサンプルです。400-500Hz程度で計測できます。
無線接続の安定化のために15個分の計測データをまとめて送信しているため、30ms程度の遅延があります。
遅延が嫌な場合はプログラム中のBUFFER_NUMを変更してください。

ESP32にUSBポート経由で給電してください。PCから接続しても問題ないですし、モバイルバッテリーで接続しても結構です。
無線を使用したい状況から考えて、小型のモバイルバッテリーを使う場合が多いと思われます。

ESP32はESP32WifiというSSIDのアクセスポイントになります。パスワードはpasswordです（これらはプログラムに書かれているので変更できます）
PCをこのアクセスポイントに接続してください。そのうえでProcessingのプログラムを起動してください。
この説明から分かる通り、このサンプルプログラムを動作させる際にはPCはESP32に直接無線接続されるので、外部ネットワークに接続できなくなります。
これが不都合な場合は外部ルータをアクセスポイントとして用いるプログラムに変更する必要があります。

