#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

using sutil::AnyText;

struct LocationRead {
    LocationRead(gson::Entry entry) : entry(entry) {}

    // долгота
    AnyText longitude() {
        return entry[fbhash::longitude];
    }

    // широта
    AnyText latitude() {
        return entry[fbhash::latitude];
    }

    // точность в метрах, 0-1500
    AnyText horizontal_accuracy() {
        return entry[fbhash::horizontal_accuracy];
    }

    // Время относительно даты отправки сообщения в секундах, в течение которого местоположение может быть обновлено
    AnyText live_period() {
        return entry[fbhash::live_period];
    }

    // направление в градусах, 1-360
    AnyText heading() {
        return entry[fbhash::heading];
    }

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    AnyText proximity_alert_radius() {
        return entry[fbhash::proximity_alert_radius];
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}