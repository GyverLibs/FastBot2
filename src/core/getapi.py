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

apiver = re.search("<p><strong>Bot API (.*)<\/strong><\/p>", resp).group(1)

################ h
text = '''#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#define FB_BOT_API_VERSION '''
text += '\"' + apiver + '\"'
text += '''

#define FB_MAKE_HASH(x) constexpr size_t x = su::SH(#x);
#define FB_MAKE_EXTERN(x) extern const __FlashStringHelper* x;

'''
# fstr
text += 'namespace fb {\n\n'

text += 'namespace api {'
for key in tgapi: text += '\nFB_MAKE_EXTERN(' + key + ')'
text += '\n}\n\n'

text += 'namespace cmd {'
for cmd in tgcmd: text += '\nFB_MAKE_EXTERN(' + cmd + ')'
text += '\n}\n\n'

text += '}\n\n'

# hash
text += 'namespace fbh {\n\n'

text += 'namespace api {'
for key in tgapi: text += '\nFB_MAKE_HASH(' + key + ')'
text += '\n}\n\n'

text += 'namespace cmd {'
for cmd in tgcmd: text += '\nFB_MAKE_HASH(' + cmd + ')'
text += '\n}'

text += '\n}'

with open(str(path) + '/api.h', 'w') as f: f.write(text)

############## cpp
text = '''#include "api.h"\n

#define FB_MAKE_FSTR(x) static const char __##x[] PROGMEM = #x; const __FlashStringHelper* x = (const __FlashStringHelper*)__##x;

'''
text += 'namespace fb {\n\n'

text += 'namespace api {'
for key in tgapi: text += '\nFB_MAKE_FSTR(' + key + ')'
text += '\n}\n\n'

text += 'namespace cmd {'
for cmd in tgcmd: text += '\nFB_MAKE_FSTR(' + cmd + ')'
text += '\n}'

text += '\n}'

with open(str(path) + '/api.cpp', 'w') as f: f.write(text)

print('Done!')