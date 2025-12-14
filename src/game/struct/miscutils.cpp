#include "miscutils.hpp"
#include <string>

std::string IntToTimeSeconds(uint32_t seconds, bool bTextFormat, bool showDays)
{
    uint32_t originalSeconds = seconds;

    uint32_t minutes = seconds / 60;
    seconds -= minutes * 60;

    uint32_t hours = minutes / 60;
    minutes -= hours * 60;

    uint32_t days = hours / 24;
    hours -= days * 24;

    std::string r;

    if (!bTextFormat)
    {
        char temp[24];

        if (hours > 0 || days > 0)
        {
            if (showDays)
                sprintf(temp, "%d:%02d:%02d", days, hours, minutes);
            else
                sprintf(temp, "%d:%02d:%02d", hours + (days * 24), minutes, seconds);
        }
        else
        {
            sprintf(temp, "%d:%02d", minutes, seconds);
        }

        return std::string(temp);
    }

    if (originalSeconds <= 0)
	{
		return "None";
	}

	if (seconds > 0)
	{
		r += std::to_string(seconds)+" secs";
	}

	if (minutes > 0)
	{
		if (!r.empty())
		{
			r = ", "+r;
		}
		std::string mins = " mins";

		if (minutes == 1)
		{
			mins = " min";
		}

		r = std::to_string(minutes) +mins+r;

	}

	if (hours > 0)
	{
		if (!r.empty())
		{
			r = ", "+r;
		}

		std::string stHours = " hours";

		if (hours == 1)
		{
			stHours = " hour";
		}

		r = std::to_string(hours) +stHours+r;
	}

	if (days > 0)
	{
		if (!r.empty())
		{
			r = ", "+r;
		}

		std::string stDays = " days";

		if (days == 1)
		{
			stDays = " day";
		}

		r = std::to_string(days) +stDays+r;
	}

	if (r.empty()) return "Now!";
	return r;
}