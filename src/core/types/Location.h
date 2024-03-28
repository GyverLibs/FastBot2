#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "core/api.h"

// https://core.telegram.org/bots/api#sendlocation
namespace fb {

class Location : private Message {
    friend class ::VirtualFastBot2;

   public:
    Location();
    Location(float latitude, float longitude, const su::Value& chatID) : latitude(latitude), longitude(longitude) {
        this->chatID = chatID;
    }

    using Message::chatID;
    using Message::notification;
    using Message::protect;
    using Message::removeMenu;
    using Message::reply;
    using Message::setInlineMenu;
    using Message::setMenu;
    using Message::threadID;

    // широта
    float latitude;

    // долгота
    float longitude;

    // точность в метрах, 0-1500
    float horizontalAccuracy = NAN;

    // период обновления локации в секундах 60.. 86400
    uint32_t livePeriod = 0;

    // направление в градусах, 1-360
    uint16_t heading = 0;

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    uint32_t proximityAlertRadius = 0;

   protected:
    void makePacket(fb::Packet& p) const {
        Message::makePacket(p);
        p.addFloat(fb::api::latitude, latitude, 6);
        p.addFloat(fb::api::longitude, longitude, 6);
        if (!isnan(horizontalAccuracy)) p.addFloat(fb::api::horizontal_accuracy, horizontalAccuracy, 1);
        if (livePeriod) p[fb::api::live_period] = livePeriod;
        if (heading) p[fb::api::heading] = heading;
        if (proximityAlertRadius) p[fb::api::proximity_alert_radius] = proximityAlertRadius;
    }
};

}  // namespace fb