This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/FastBot2.svg?color=brightgreen)](https://github.com/GyverLibs/FastBot2/releases/latest/download/FastBot2.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/FastBot2.svg)](https://registry.platformio.org/libraries/gyverlibs/FastBot2)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/FastBot2?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# FastBot2
Telegram bot library for Arduino, ESP8266, ESP32:
- Several survey modes, including **long polling**
- Fast parsing of server responses[6 times faster](https://github.com/GyverLibs/GSON?tab=readme-ov-file#%D1%82%D0%B5%D1%81%D1%82%D1%8B)Other libraries based on ArduinoJSON, 2-3 times faster than the first version[FastBot](https://github.com/GyverLibs/FastBot))
- All the necessary tools are wrapped in comfortable classes
- In "manual mode" is available completely[Telegram Bot API](https://core.telegram.org/bots/api)
- Lightweight, efficient use of RAM
- Native support for WiFi ESP8266/ESP32
- Download and download files, OTA Update (ESP8266/ESP32)
- Works on the basis of Arduino Client, possible work on Ethernet, GSM and so on

### Compatibility
Any Arduino.

### Dependencies
- [GSON](https://github.com/GyverLibs/GSON) v1.8.0+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.5.0+
- [GyverHTTP](https://github.com/GyverLibs/GyverHTTP) v1.0.27+
- [GTL](https://github.com/GyverLibs/GTL) v1.3.1+

## Contents
- [Documentation.](#docs)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="docs"></a>

## Documentation.
Located in[file](https://github.com/GyverLibs/FastBot2/tree/main/docs/1.main.md)

### Graphic output
Use the library.[CharDisplay](https://github.com/GyverLibs/CharDisplay)To display charts and drawing in chat!

![](https://github.com/GyverLibs/CharDisplay/blob/main/docs/plots.png)

<a id="versions"></a>

## Versions
- v1.0

<a id="install"></a>

## Installation
- The library can be found under the name **FastBot2** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/FastBot2/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
