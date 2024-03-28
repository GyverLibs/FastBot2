#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#location
struct LocationRead {
    LocationRead(gson::Entry entry) : entry(entry) {}

    // широта
    su::Text latitude() {
        return entry[fbh::api::latitude];
    }

    // долгота
    su::Text longitude() {
        return entry[fbh::api::longitude];
    }

    // точность в метрах, 0-1500
    su::Text horizontalAccuracy() {
        return entry[fbh::api::horizontal_accuracy];
    }

    // Время относительно даты отправки сообщения в секундах, в течение которого местоположение может быть обновлено
    su::Text livePeriod() {
        return entry[fbh::api::live_period];
    }

    // направление в градусах, 1-360
    su::Text heading() {
        return entry[fbh::api::heading];
    }

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    su::Text proximityAlertRadius() {
        return entry[fbh::api::proximity_alert_radius];
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb