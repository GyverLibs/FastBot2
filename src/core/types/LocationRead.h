#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/EntryAccess.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#location
struct LocationRead : public EntryAccess {
    LocationRead(gson::Entry entry) : EntryAccess(entry) {}

    // широта
    su::Text latitude() {
        return entry[tg_apih::latitude];
    }

    // долгота
    su::Text longitude() {
        return entry[tg_apih::longitude];
    }

    // точность в метрах, 0-1500
    su::Text horizontalAccuracy() {
        return entry[tg_apih::horizontal_accuracy];
    }

    // Время относительно даты отправки сообщения в секундах, в течение которого местоположение может быть обновлено
    su::Text livePeriod() {
        return entry[tg_apih::live_period];
    }

    // направление в градусах, 1-360
    su::Text heading() {
        return entry[tg_apih::heading];
    }

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    su::Text proximityAlertRadius() {
        return entry[tg_apih::proximity_alert_radius];
    }
};

}  // namespace fb