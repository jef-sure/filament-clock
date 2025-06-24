#include <stdint.h>
#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    WEATHER_ICON_LIGHTNING_WITH_RAIN = 0,
    WEATHER_ICON_DRIZZLE,
    WEATHER_ICON_CLOUD_WITH_RAIN,
    WEATHER_ICON_SNOW,
    WEATHER_ICON_MIST,
    WEATHER_ICON_SMOKE,
    WEATHER_ICON_HAZE,
    WEATHER_ICON_DUST_WHIRLS,
    WEATHER_ICON_FOG,
    WEATHER_ICON_SAND,
    WEATHER_ICON_DUST,
    WEATHER_ICON_ASH,
    WEATHER_ICON_SQUALL,
    WEATHER_ICON_TORNADO,
    WEATHER_ICON_SUN,
    WEATHER_ICON_MOON,
    WEATHER_ICON_SUN_WITH_CLOUDS,
    WEATHER_ICON_MOON_WITH_CLOUDS,
    WEATHER_ICON_BROKEN_CLOUDS,
    WEATHER_ICON_MAX
} weather_icon_id_t;

typedef struct weather_
{
    int               idCond;
    const char       *mainGroup;
    const char       *description;
    weather_icon_id_t dayIconId;
    weather_icon_id_t nightIconId;
} weather_t;

const weather_t *findWeather(int id);
const uint8_t   *getWeatherIcon16x10(weather_icon_id_t id);

#ifdef __cplusplus
}
#endif