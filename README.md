# FastBot2 [beta]
Библиотека Telegram бота для Arduino, ESP8266, ESP32:
- Несколько режимов опроса (long polling, short polling, async short)
- Быстрый парсинг ответов сервера ([в 6 раз быстрее](https://github.com/GyverLibs/GSON?tab=readme-ov-file#%D1%82%D0%B5%D1%81%D1%82%D1%8B) других библиотек на базе ArduinoJSON, в 2-3 раза быстрее первой версии [FastBot](https://github.com/GyverLibs/FastBot))
- Все самые нужные инструменты обёрнуты в удобные классы
- В "ручном режиме" доступен полностью весь [Telegram Bot API](https://core.telegram.org/bots/api)
- Лёгкий вес, эффективное использование оперативной памяти
- Нативная поддержка WiFi ESP8266/ESP32
- Загрузка и скачивание файлов, OTA обновление (ESP8266/ESP32)
- Работает на базе Arduino Client, возможна работа по Ethernet, GSM и так далее
- Виртуальная библиотека, можно добавить связь абсолютно любым способом

> Библиотека в разработке! Руководствуйтесь примером и подсказками IDE

## Установка
Библиотека не в релизе, качайте репозиторий zip архивом (кнопка Code справа вверху) или по [прямой ссылке](https://github.com/GyverLibs/FastBot2/archive/refs/heads/main.zip) на загрузку.

Для работы FastBot2 нужно установить библиотеки:
- [GSON](https://github.com/GyverLibs/GSON) v1.4.10+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.4.8+

## Документация
Находится в [папке docs](https://github.com/GyverLibs/FastBot2/tree/main/docs/1.main.md)