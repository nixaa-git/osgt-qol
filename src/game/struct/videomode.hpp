#pragma once
#include <string>
#include <map>

struct VideoMode
{
  public:
    void* vftable;
    std::string m_name;
    int m_platformID;
    int m_orientation;
    float m_unkFloat;
    int m_screenW;
    int m_screenH;

    void Print()
    {
        printf("VIDEOMODE: %s - m_platformID=%d m_orientation=%d m_unkFloat=%.2f m_screenW=%d "
               "m_screenH=%d\n",
               m_name.c_str(), m_platformID, m_orientation, m_unkFloat, m_screenW, m_screenH);
    }
};
static_assert(sizeof(VideoMode) == 64, "VideoMode struct size mismatch.");
class VideoModeManager
{
  public:
    int m_screenW;
    int m_screenH;
    std::map<std::string, VideoMode*> m_videoModes;
    VideoMode* m_pActiveVidMode;
};
static_assert(sizeof(VideoModeManager) == 32, "VideoModeManager class size mismatch.");
