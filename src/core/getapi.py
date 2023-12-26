# парсер API с сайта телеграм. Наполняет константами файл keys.h
import pathlib
import requests
import re

path = pathlib.Path(__file__).parent.resolve()
resp = requests.get('https://core.telegram.org/bots/api').text

tgapi = re.findall("<tbody>\n<tr>\n<td>(.+?)<\/td>", resp) + re.findall("<\/td>\n<\/tr>\n<tr>\n<td>(.+?)<\/td>", resp)
tgapi = list(set(tgapi))

tgcmd = re.findall("<\/a>([a-z][a-zA-Z]+?)<\/h4>", resp)
tgcmd = list(set(tgcmd))

keys = '''#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#define FB_MAKE_API(x) const __FlashStringHelper* x() { return F(#x); }
#define FB_MAKE_HASH(x) constexpr size_t x = sutil::SH(#x);

namespace fbapi {'''

for key in tgapi: keys += '\nFB_MAKE_API(' + key + ')'
keys += '\n}\n\nnamespace fbhash {'

for key in tgapi: keys += '\nFB_MAKE_HASH(' + key + ')'
keys += '\n}\n\nnamespace fbcmd {'

for cmd in tgcmd: keys += '\nFB_MAKE_API(' + cmd + ')'
keys += '\n}'

with open(str(path) + '/keys.h', 'w') as f: f.write(keys)

print('Done!')