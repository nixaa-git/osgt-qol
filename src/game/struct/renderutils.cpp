#include "renderutils.hpp"
#include <cmath>

float SinPulseByCustomTimerMS(int ms, unsigned int timerMS)
{
    int tick = timerMS % ms;
    return (float)(sin((float(tick) / float(ms)) * 3.141592f * 2));
}