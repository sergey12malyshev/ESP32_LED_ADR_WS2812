# ESP32_LED_ADR
The project of controlling the addressable LED strip type WS2812 through the Telegramm chat bot

Based on FastBot and FastLed libraries

Проект управления адресной светодиодной LED-лентой типа WS2812 через чат-бот Telegramm

Основана на библиотеках FastBot и FastLed

## Tools (Инструментарий)
*PlatformIO (Core 6.1.11·Home 3.4.4)* + *VS Code*

## Connect to WI-FI
Create a pass.h file and add to the project:
```
 #define WIFI_SSID "XXXX"
 #define WIFI_PASS "XXXXXXXXX"
 #define BOT_TOKEN "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
 #define CHAT_ID  "XXXXXXXXXXXXXXX"
```
## Board
- ESP32_DevKitc_V4
- WS2812 address led (5 m, 5 V, 60 LED/m WS2812B60)
- Wi-Fi antenna
- Power Sourse: 5V 2A
<img src="https://github.com/sergey12malyshev/ESP32_LED_ADR_WS2812/blob/master/image/hard2.jpg" width=20% height=20%>

PinOut: https://www.electronshik.ru/news/show/12935?from=terraelectronica

## Management commands
<img src="https://github.com/sergey12malyshev/ESP32_LED_ADR_WS2812/blob/master/image/screen.jpg" width=20% height=20%>

Для удалённого обновления ПО скинуть в чат команду update с прошивкой в формате .bin

## Helpful information (Полезная информация)
Исходник FastLed: https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino

Библиотека FastBot: https://kit.alexgyver.ru/tutorials/fastbot/

Создание бота Telegramm: https://kit.alexgyver.ru/tutorials/telegram-basic/  https://habr.com/ru/articles/729246/

Настройка VS Code + PlatformIO: https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/
