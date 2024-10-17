[![latest](https://img.shields.io/github/v/release/GyverLibs/FastBot2.svg?color=brightgreen)](https://github.com/GyverLibs/FastBot2/releases/latest/download/FastBot2.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/FastBot2.svg)](https://registry.platformio.org/libraries/gyverlibs/FastBot2)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/FastBot2?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# FastBot2
Библиотека Telegram бота для Arduino, ESP8266, ESP32:
- Несколько режимов опроса, включая **long polling**
- Быстрый парсинг ответов сервера ([в 6 раз быстрее](https://github.com/GyverLibs/GSON?tab=readme-ov-file#%D1%82%D0%B5%D1%81%D1%82%D1%8B) других библиотек на базе ArduinoJSON, в 2-3 раза быстрее первой версии [FastBot](https://github.com/GyverLibs/FastBot))
- Все самые нужные инструменты обёрнуты в удобные классы
- В "ручном режиме" доступен полностью весь [Telegram Bot API](https://core.telegram.org/bots/api)
- Лёгкий вес, эффективное использование оперативной памяти
- Нативная поддержка WiFi ESP8266/ESP32
- Загрузка и скачивание файлов, OTA обновление (ESP8266/ESP32)
- Работает на базе Arduino Client, возможна работа по Ethernet, GSM и так далее

### Совместимость
Любые Arduino

### Зависимости
- [GSON](https://github.com/GyverLibs/GSON) v1.5+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.4.14+
- [GyverHTTP](https://github.com/GyverLibs/GyverHTTP) v1.0.2+
- [GTL](https://github.com/GyverLibs/GTL) v1.1.7+

## Содержание
- [Документация](#docs)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="docs"></a>

## Документация
Находится в [папке docs](https://github.com/GyverLibs/FastBot2/tree/main/docs/1.main.md)

### Вывод графики
Используйте библиотеку [CharDisplay](https://github.com/GyverLibs/CharDisplay) для вывода графиков и рисования в чате!  

![](https://github.com/GyverLibs/CharDisplay/blob/main/docs/plots.png)

<a id="versions"></a>

## Версии
- v1.0

<a id="install"></a>

## Установка
- Библиотеку можно найти по названию **FastBot2** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/FastBot2/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код