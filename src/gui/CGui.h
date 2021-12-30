#ifndef __CGUI_H__
#define __CGUI_H__

#include "CRawImage.h"
#include "CTransformation.h"
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

class CGui
{
public:
    CGui(int wi, int he, int scale);
    ~CGui();

    void drawImage(CRawImage* image);
    void drawStats(int x, int y, STrackedObject o);
    void drawTimeStats(int evalTime, int numBots);
    void guideCalibration(int calibNum, float a, float b);
    void clearStats();
    void saveScreen(int a = -1);
    void update();
    void drawEllipse(SSegment s, STrackedObject t);
    void drawLine(float sx1, float sx2, float sy1, float sy2);
    void displayHelp(bool extended);
    void drawStats(int x, int y, STrackedObject o, bool D2);

private:
    SDL_Surface* m_screen;
    int m_width;
    int m_height;
    int m_scale;
    TTF_Font* m_smallFont;
    int m_averageTime;
    int m_maxTime;
    int m_numStats;
    int m_num;
};

#endif

/* end of CGui.h */
