[![latest](https://img.shields.io/github/v/release/GyverLibs/FastBot2.svg?color=brightgreen)](https://github.com/GyverLibs/FastBot2/releases/latest/download/FastBot2.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/FastBot2.svg)](https://registry.platformio.org/libraries/gyverlibs/FastBot2)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/FastBot2?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# FastBot2 [beta]
Библиотека Telegram бота для Arduino, ESP8266, ESP32:
- Несколько режимов опроса, включая **long polling**
- Быстрый парсинг ответов сервера ([в 6 раз быстрее](https://github.com/GyverLibs/GSON?tab=readme-ov-file#%D1%82%D0%B5%D1%81%D1%82%D1%8B) других библиотек на базе ArduinoJSON, в 2-3 раза быстрее первой версии [FastBot](https://github.com/GyverLibs/FastBot))
- Все самые нужные инструменты обёрнуты в удобные классы
- В "ручном режиме" доступен полностью весь [Telegram Bot API](https://core.telegram.org/bots/api)
- Лёгкий вес, эффективное использование оперативной памяти
- Нативная поддержка WiFi ESP8266/ESP32
- Загрузка и скачивание файлов, OTA обновление (ESP8266/ESP32)
- Работает на базе Arduino Client, возможна работа по Ethernet, GSM и так далее

> Библиотека в разработке! Руководствуйтесь примером и подсказками IDE

### Совместимость
Любые Arduino

### Зависимости
- [GSON](https://github.com/GyverLibs/GSON) v1.5+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.4.14+
- [GyverHTTP](https://github.com/GyverLibs/GyverHTTP) v1.0.2+

## Установка
Библиотека не в релизе, качайте репозиторий zip архивом (кнопка Code справа вверху) или по [прямой ссылке](https://github.com/GyverLibs/FastBot2/archive/refs/heads/main.zip) на загрузку.

## Вывод графики
Используйте библиотеку [CharDisplay](https://github.com/GyverLibs/CharDisplay) для вывода графиков и рисования в чате!  

![](https://github.com/GyverLibs/CharDisplay/blob/main/docs/plots.png)

## Документация
Находится в [папке docs](https://github.com/GyverLibs/FastBot2/tree/main/docs/1.main.md)
