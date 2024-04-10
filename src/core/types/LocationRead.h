#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#location
struct LocationRead : public gson::Entry {
    LocationRead(gson::Entry entry) : gson::Entry(entry) {}

    // широта
    su::Text latitude() {
        return (*this)[fbh::api::latitude];
    }

    // долгота
    su::Text longitude() {
        return (*this)[fbh::api::longitude];
    }

    // точность в метрах, 0-1500
    su::Text horizontalAccuracy() {
        return (*this)[fbh::api::horizontal_accuracy];
    }

    // Время относительно даты отправки сообщения в секундах, в течение которого местоположение может быть обновлено
    su::Text livePeriod() {
        return (*this)[fbh::api::live_period];
    }

    // направление в градусах, 1-360
    su::Text heading() {
        return (*this)[fbh::api::heading];
    }

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    su::Text proximityAlertRadius() {
        return (*this)[fbh::api::proximity_alert_radius];
    }
};

}  // namespace fb