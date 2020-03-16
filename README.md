# FilmTactileSensor
FilmTactileSensor Based on ESP32

## 共通
PC側のサンプルプログラムはProcessingで作成しています。
ESP32側のサンプルプログラムはArduino IDEで作成しています。

### 使用方法
計測装置をPCにUSBケーブルで接続し、シリアルポート番号を確認してください。
Arduino IDEでESP32プログラムを書き込んでください。

### Processing側の操作
1,2,3,4を押すと計測レンジを変更します。
sを押すと計測開始、再度sを押すと計測終了で、CSVファイルが作成されます。

## ３本指での高速化（有線）
- ESP32: Measurement5x10_3FingersMultiTask8bit20200314.ino
- Processing: Measurement5x10_3Fingers8bitESP32MultiTaskHighSpeed_30300314.pde
こちらは現在安定に500Hzで3本指の計測が出来ています。
Processingプログラム中のシリアルポート番号を変更してください。

## 1本指での高速化（有線）
- ESP32: Measurement5x10_1Fingers8bit20200311.ino
- Processing: Measurement5x10_1Fingers8bitESP32HighSpeed_30300311.pde
こちらは1本指で1kHzの計測を行っています。実際にはもう少し早く出来ますが（おそらく1.5kHz程度まで）、1kHzに設定しています。
Processingプログラム中のシリアルポート番号を変更してください。
Processing側でfを押すと計測する指が変わります。

## 3本指での高速化（無線）
- ESP32: Measurement5x10_3Fingers8bitWifi20200316.ino
- Processing: Measurement5x10_3Fingers8bitESP32WiFiNoWaitHighSpeed_20200316.pde
こちらはESP32自体をアクセスポイント(AO)とした、無線接続のサンプルです。400-500Hz程度で計測できます。
無線接続の安定化のために15個分の計測データをまとめて送信しているため、30ms程度の遅延があります。
遅延が嫌な場合はプログラム中のBUFFER_NUMを変更してください。
ESP32にUSBポート経由で給電してください。PCから接続しても問題ないですし、モバイルバッテリーで接続しても結構です。
無線を使用したい状況から考えて、小型のモバイルバッテリーを使うのが良いと思われます。
ESP32はESP32WifiというSSIDのアクセスポイントになります。パスワードはpasswordです（これらはプログラムに書かれているので変更できます）
PCをこのアクセスポイントに接続してください。そのうえでProcessingのプログラムを起動してください。


