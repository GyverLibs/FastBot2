# парсер API с сайта телеграм. Наполняет константами файл api.h
import pathlib
import requests
import re

path = pathlib.Path(__file__).parent.resolve()
resp = requests.get('https://core.telegram.org/bots/api').text

tgapi = re.findall("<tbody>\n<tr>\n<td>(.+?)<\/td>", resp) + re.findall("<\/td>\n<\/tr>\n<tr>\n<td>(.+?)<\/td>", resp)
tgapi = list(set(tgapi))

tgcmd = re.findall("<\/a>([a-z][a-zA-Z]+?)<\/h4>", resp)
tgcmd = list(set(tgcmd))

# make .h single
text = '''#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#define FB_BOT_API_VERSION "7.1"

#define FB_MAKE_API(x) static inline const __FlashStringHelper* x() { return F(#x); }
#define FB_MAKE_HASH(x) constexpr size_t x = su::SH(#x);

'''

text += 'namespace fbapi {'
for key in tgapi: text += '\nFB_MAKE_API(' + key + ')'
text += '\n}\n\n'

text += 'namespace fbh {'
for key in tgapi: text += '\nFB_MAKE_HASH(' + key + ')'
text += '\n}\n\n'

text += 'namespace fbcmd {'
for cmd in tgcmd: text += '\nFB_MAKE_API(' + cmd + ')'
text += '\n}\n\n'

text += 'namespace fbcmdh {'
for cmd in tgcmd: text += '\nFB_MAKE_HASH(' + cmd + ')'
text += '\n}'

with open(str(path) + '/api.h', 'w') as f: f.write(text)

print('Done!')