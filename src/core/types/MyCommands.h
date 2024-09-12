#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2Client_class.h"
#include "../api.h"
#include "../packet.h"

// https://core.telegram.org/bots/api#setmycommands
namespace fb {

class MyCommands {
    friend class ::FastBot2Client;

   public:
    MyCommands() {}
    MyCommands(const String& commands, const String& description) : commands(commands), description(description) {}

    // список команд, длина команды 1-32, разделитель ;
    String commands = "";

    // список описаний команд, длина описания 1-256, разделитель ;
    String description = "";

    // зарезервировать строки
    void reserve(uint16_t len) {
        commands.reserve(len);
        description.reserve(len);
    }

    // добавить команду
    void addCommand(const String& command, const String& description) {
        if (_first) _first = false;
        else {
            commands += ';';
            this->description += ';';
        }

        commands += command;
        this->description += description;
    }

   private:
    void makePacket(Packet& p) const {
        p.beginArr(tg_api::commands);
        su::TextParser cmd(commands, ';');
        su::TextParser desc(description, ';');
        while (cmd.parse() && desc.parse()) {
            p.beginObj().addString(tg_api::command, cmd).addStringEsc(tg_api::description, desc).endObj();
        }
        p.endArr();
    }
    bool _first = true;
};

}  // namespace fb