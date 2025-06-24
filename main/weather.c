#include "weather.h"
#include <stddef.h>
#include <stdint.h>

weather_t openweather[] = {
    {200, "Thunderstorm", "thunderstorm with light rain",    WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {201, "Thunderstorm", "thunderstorm with rain",          WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {202, "Thunderstorm", "thunderstorm with heavy rain",    WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {210, "Thunderstorm", "light thunderstorm",              WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {211, "Thunderstorm", "thunderstorm",                    WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {212, "Thunderstorm", "heavy thunderstorm",              WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {221, "Thunderstorm", "ragged thunderstorm",             WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {230, "Thunderstorm", "thunderstorm with light drizzle", WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {231, "Thunderstorm", "thunderstorm with drizzle",       WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {232, "Thunderstorm", "thunderstorm with heavy drizzle", WEATHER_ICON_LIGHTNING_WITH_RAIN, WEATHER_ICON_LIGHTNING_WITH_RAIN}, //
    {300, "Drizzle",      "light intensity drizzle",         WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {301, "Drizzle",      "drizzle",                         WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {302, "Drizzle",      "heavy intensity drizzle",         WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {310, "Drizzle",      "light intensity drizzle rain",    WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {311, "Drizzle",      "drizzle rain",                    WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {312, "Drizzle",      "heavy intensity drizzle rain",    WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {313, "Drizzle",      "shower rain and drizzle",         WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {314, "Drizzle",      "heavy shower rain and drizzle",   WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {321, "Drizzle",      "shower drizzle",                  WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {500, "Rain",         "light rain",                      WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {501, "Rain",         "moderate rain",                   WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {502, "Rain",         "heavy intensity rain",            WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {503, "Rain",         "very heavy rain",                 WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {504, "Rain",         "extreme rain",                    WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {511, "Rain",         "freezing rain",                   WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {520, "Rain",         "light intensity shower rain",     WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {521, "Rain",         "shower rain",                     WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {522, "Rain",         "heavy intensity shower rain",     WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {531, "Rain",         "ragged shower rain",              WEATHER_ICON_DRIZZLE,             WEATHER_ICON_DRIZZLE            }, //
    {600, "Snow",         "light snow",                      WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {601, "Snow",         "Snow",                            WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {602, "Snow",         "Heavy snow",                      WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {611, "Snow",         "Sleet",                           WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {612, "Snow",         "Light shower sleet",              WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {613, "Snow",         "Shower sleet",                    WEATHER_ICON_CLOUD_WITH_RAIN,     WEATHER_ICON_CLOUD_WITH_RAIN    }, //
    {615, "Snow",         "Light rain and snow",             WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {616, "Snow",         "Rain and snow",                   WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {620, "Snow",         "Light shower snow",               WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {621, "Snow",         "Shower snow",                     WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {622, "Snow",         "Heavy shower snow",               WEATHER_ICON_SNOW,                WEATHER_ICON_SNOW               }, //
    {701, "Mist",         "mist",                            WEATHER_ICON_MIST,                WEATHER_ICON_MIST               }, //
    {711, "Smoke",        "Smoke",                           WEATHER_ICON_SMOKE,               WEATHER_ICON_SMOKE              }, //
    {721, "Haze",         "Haze",                            WEATHER_ICON_HAZE,                WEATHER_ICON_HAZE               }, //
    {731, "Dust",         "sand/ dust whirls",               WEATHER_ICON_DUST_WHIRLS,         WEATHER_ICON_DUST_WHIRLS        }, //
    {741, "Fog",          "fog",                             WEATHER_ICON_FOG,                 WEATHER_ICON_FOG                }, //
    {751, "Sand",         "sand",                            WEATHER_ICON_SAND,                WEATHER_ICON_SAND               }, //
    {761, "Dust",         "dust",                            WEATHER_ICON_DUST,                WEATHER_ICON_DUST               }, //
    {762, "Ash",          "volcanic ash",                    WEATHER_ICON_ASH,                 WEATHER_ICON_ASH                }, //
    {771, "Squall",       "squalls",                         WEATHER_ICON_SQUALL,              WEATHER_ICON_SQUALL             }, //
    {781, "Tornado",      "tornado",                         WEATHER_ICON_TORNADO,             WEATHER_ICON_TORNADO            }, //
    {800, "Clear",        "clear sky",                       WEATHER_ICON_SUN,                 WEATHER_ICON_MOON               }, //
    {801, "Clouds",       "few clouds",                      WEATHER_ICON_SUN_WITH_CLOUDS,     WEATHER_ICON_MOON_WITH_CLOUDS   }, //
    {802, "Clouds",       "scattered clouds",                WEATHER_ICON_SUN_WITH_CLOUDS,     WEATHER_ICON_MOON_WITH_CLOUDS   }, //
    {803, "Clouds",       "broken clouds",                   WEATHER_ICON_BROKEN_CLOUDS,       WEATHER_ICON_BROKEN_CLOUDS      }, //
    {804, "Clouds",       "overcast clouds",                 WEATHER_ICON_BROKEN_CLOUDS,       WEATHER_ICON_BROKEN_CLOUDS      }, //
};
// clang-format off
const uint8_t weather_icon[][20] = {
    {
     // lightning with rain icon
        0B00001100, 0B00000000, //
        0B00011000, 0B00010010, //
        0B00110000, 0B00000100, //
        0B01111100, 0B01000001, //
        0B00011100, 0B10010000, //
        0B00011000, 0B00100100, //
        0B00110010, 0B00001000, //
        0B00100100, 0B10000000, //
        0B01000001, 0B00100000, //
        0B00000000, 0B00000000, //
    },
    {
     // drizzle icon
        0B00000001, 0B11100000, //
        0B00000010, 0B00010000, //
        0B00000100, 0B00001000, //
        0B00111000, 0B00001000, //
        0B01000000, 0B00001000, //
        0B01000000, 0B00001000, //
        0B00111111, 0B11110000, //
        0B00000000, 0B00000000, //
        0B00100100, 0B10000000, //
        0B01001001, 0B00000000, //
    },

    {
     // cloud with rain icon
        0B00011000, 0B00001100, //
        0B01100100, 0B01110010, //
        0B10000010, 0B10000001, //
        0B10000010, 0B10000001, //
        0B01111100, 0B01111110, //
        0B00000000, 0B00000000, //
        0B00100100, 0B10010010, //
        0B01001001, 0B00100100, //
        0B10010010, 0B01001000, //
        0B00000000, 0B00000000, //
    },

    {
     // snow icon
        0B01001001, 0B00000000, //
        0B11001001, 0B10000000, //
        0B00101010, 0B00000000, //
        0B00011100, 0B00000000, //
        0B11111111, 0B10000000, //
        0B00011100, 0B00000000, //
        0B00101010, 0B00000000, //
        0B11001001, 0B10000000, //
        0B01001001, 0B00000000, //
        0B00000000, 0B00000000, //
    },
    {
     // mist icon
        0B00011100, 0B00000111, //
        0B01100011, 0B00011000, //
        0B00000000, 0B11100000, //
        0B00011100, 0B00000111, //
        0B01100011, 0B00011000, //
        0B00000000, 0B11100000, //
        0B00011100, 0B00000111, //
        0B01100011, 0B00011000, //
        0B00000000, 0B11100000, //
        0B00000000, 0B00000000, //
    },
    {
     // smoke icon
        0B01001001, 0B00100000, //
        0B00100100, 0B10010000, //
        0B00100100, 0B10010000, //
        0B01000100, 0B10010000, //
        0B00001001, 0B00100100, //
        0B00010010, 0B01001000, //
        0B00010010, 0B01001000, //
        0B00001001, 0B00100100, //
        0B00000000, 0B00000000, //
        0B00000000, 0B00000000, //
    },
    {
     // haze icon
        0B00000111, 0B00000000, //
        0B00001000, 0B10000000, //
        0B01110000, 0B01000000, //
        0B10000000, 0B00100000, //
        0B10000000, 0B00100000, //
        0B01111111, 0B11000000, //
        0B00000000, 0B00000000, //
        0B11101110, 0B11101110, //
        0B00000000, 0B00000000, //
        0B00111011, 0B10111000, //
    },
    {
     // dust icon
        0B00000001, 0B10000000, //
        0B00011000, 0B01001000, //
        0B00000100, 0B01000000, //
        0B00000100, 0B01000100, //
        0B11111000, 0B01000000, //
        0B00000000, 0B10001000, //
        0B11111111, 0B00000000, //
        0B00000000, 0B00000000, //
        0B00000000, 0B00000000, //
        0B00000000, 0B00000000, //
    },
    {
     // fog icon
        0B00000011, 0B10000000, //
        0B00000100, 0B01000000, //
        0B00011000, 0B00100000, //
        0B00100000, 0B00010000, //
        0B00111111, 0B11110000, //
        0B00000000, 0B00000000, //
        0B11101110, 0B11101110, //
        0B00000000, 0B00000000, //
        0B00111011, 0B10111000, //
        0B00000000, 0B00000000, //
    },
    {
     // sand icon
        0B00000011, 0B10000000, //
        0B00000101, 0B01000000, //
        0B00011010, 0B10100000, //
        0B00100101, 0B01010000, //
        0B01000000, 0B00001000, //
        0B01000000, 0B00001000, //
        0B00111111, 0B11110000, //
        0B00000000, 0B00000000, //
        0B00000000, 0B00000000, //
        0B00000000, 0B00000000, //
    },
    {
     // dust icon
        0B00000011, 0B10000000, //
        0B00000100, 0B01000000, //
        0B00011000, 0B00100000, //
        0B00100010, 0B10010000, //
        0B01000000, 0B00001000, //
        0B01000101, 0B01001000, //
        0B00100000, 0B00001000, //
        0B00010001, 0B00010000, //
        0B00001100, 0B01100000, //
        0B00000011, 0B10000000, //
    },
    {
     // ash icon
        0B00000010, 0B01000000, //
        0B01001001, 0B10010010, //
        0B00000000, 0B00000000, //
        0B00100011, 0B11000100, //
        0B00000100, 0B00100000, //
        0B00001000, 0B00010000, //
        0B00010000, 0B00001000, //
        0B00100000, 0B00000100, //
        0B01000000, 0B00000010, //
        0B01111111, 0B11111110, //
    },
    {
     // squall icon
        0B00001001, 0B00000000, //
        0B00010010, 0B00000000, //
        0B00000000, 0B10000000, //
        0B00111001, 0B00010000, //
        0B01000100, 0B00100000, //
        0B00000110, 0B00000000, //
        0B00001001, 0B00010000, //
        0B00010001, 0B00100000, //
        0B00100001, 0B00000000, //
        0B00000010, 0B00000000, //
    },
    {
     // tornado icon
        0B00100000, 0B10000000, //
        0B00111011, 0B10000000, //
        0B00011111, 0B10000000, //
        0B00001111, 0B10000000, //
        0B00000111, 0B00000000, //
        0B00000011, 0B00000000, //
        0B00000011, 0B00000000, //
        0B00000010, 0B00000000, //
        0B00000100, 0B00000000, //
        0B00001000, 0B00000000, //
    },
    {
     // sun icon
        0B11110000, 0B00000000, //
        0B11110001, 0B00000000, //
        0B11100000, 0B10000000, //
        0B11000100, 0B01100000, //
        0B00000010, 0B00010000, //
        0B00010001, 0B00001000, //
        0B10001000, 0B10000000, //
        0B01000100, 0B01000000, //
        0B01000010, 0B00000000, //
        0B00100000, 0B00000000, //
    },
    {
     // moon icon
        0B11000000, 0B00000000, //
        0B00110000, 0B00000000, //
        0B00011000, 0B00000000, //
        0B00011100, 0B00000000, //
        0B00011100, 0B00000000, //
        0B00011100, 0B00000000, //
        0B00011000, 0B00000000, //
        0B00110000, 0B00000000, //
        0B11000000, 0B00000000, //
        0B00000000, 0B00000000, //
    },
    {
     // sun with clouds icon
        0B11100001, 0B11100000, //
        0B11100010, 0B00010000, //
        0B11000001, 0B11100000, //
        0B00001000, 0B00001100, //
        0B00100100, 0B00010010, //
        0B10010000, 0B00010010, //
        0B01001000, 0B11100001, //
        0B01000001, 0B00000001, //
        0B00100001, 0B00000001, //
        0B00000000, 0B11111110, //
    },
    {
     // moon with clouds icon
        0B11000001, 0B11100000, //
        0B00110010, 0B00010000, //
        0B00011001, 0B11100000, //
        0B00011100, 0B00001100, //
        0B00011100, 0B00010010, //
        0B00011100, 0B00010010, //
        0B00011000, 0B11100001, //
        0B00110001, 0B00000001, //
        0B11000001, 0B00000001, //
        0B00000000, 0B11111110, //
    },
    {
     // broken clouds icon
        0B01110001, 0B11100000, //
        0B10001010, 0B00010000, //
        0B01110001, 0B11100000, //
        0B00001100, 0B00000100, //
        0B00010010, 0B00001010, //
        0B01100010, 0B00110010, //
        0B10000001, 0B01000001, //
        0B10000001, 0B01000001, //
        0B01111110, 0B00111110, //
        0B00000000, 0B00000000, //
    }
};
// clang-format on

const weather_t *findWeather(int id)
{
    for (size_t i = 0; i < sizeof(openweather) / sizeof(openweather[0]); i++) {
        if (openweather[i].idCond == id) {
            return &openweather[i];
        }
    }
    return NULL;
}

const uint8_t *getWeatherIcon16x10(weather_icon_id_t id)
{
    if (id < 0 || id >= WEATHER_ICON_MAX) {
        return NULL; // Invalid icon ID
    }
    return weather_icon[id];
}