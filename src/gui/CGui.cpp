#include "CGui.h"
#include "config.h"

#define THICK_CROSS

CGui::CGui(int wi, int he, int sc)
{
    m_averageTime = 0;
    m_maxTime = 0;
    m_numStats = 0;
    m_height = he / sc;
    m_width = wi / sc;
    m_scale = sc;
    SDL_Init(SDL_INIT_VIDEO | SDL_HWSURFACE | SDL_HWACCEL);
    if (TTF_Init() == -1) {
        printf("Unable to initialize SDL_ttf: %s\n", TTF_GetError());
    }
    m_screen = NULL;
    m_screen = SDL_SetVideoMode(wi / sc, he / sc, 24, SDL_SWSURFACE);
    if (m_screen == NULL) {
        fprintf(stderr, "Couldn't set SDL video mode: %s\r\n", SDL_GetError());
    }
    SDL_WM_SetCaption("SWARMCON", "Swarm Robot Localization");
    m_smallFont = TTF_OpenFont("../etc/font.ttf", 16);
    if (!m_smallFont) {
        printf("Unable to open font: %s\n", TTF_GetError());
    }
    TTF_SetFontStyle(m_smallFont, TTF_STYLE_NORMAL);
    m_num = 0;
}

void CGui::clearStats()
{
    m_averageTime = 0;
    m_maxTime = 0;
    m_numStats = 0;
}

CGui::~CGui()
{
    SDL_FreeSurface(m_screen);
}

void CGui::drawImage(CRawImage* image)
{
    CRawImage* imageSrc = image;

    if (m_scale != 1) {
        int wi = m_width;
        int he = m_height;
        imageSrc = new CRawImage(wi, he);
        for (int j = 0; j < he; j++) {
            int srp = (j * m_scale) * wi * m_scale * 3;
            int dep = j * wi * 3;
            for (int i = 0; i < wi; i++) {
                int dp = dep + i * 3;
                int sp = srp + m_scale * i * 3;
                imageSrc->data[dp] = image->data[sp];
                imageSrc->data[dp + 1] = image->data[sp + 1];
                imageSrc->data[dp + 2] = image->data[sp + 2];
            }
        }
    }
    SDL_Surface* imageSDL = SDL_CreateRGBSurfaceFrom(imageSrc->data, imageSrc->width, imageSrc->height, imageSrc->bpp * 8, imageSrc->bpp * imageSrc->width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000);
    if (imageSDL != NULL) {
        SDL_BlitSurface(imageSDL, NULL, m_screen, NULL);
    }
    SDL_FreeSurface(imageSDL);
    if (m_scale != 1) {
        delete imageSrc;
    }
}

void CGui::drawTimeStats(int evalTime, int numBots)
{
    char info[1000];
    SDL_Rect rect{0, 0, 0, 0};
    SDL_Surface* text;

    SDL_FillRect(m_screen, &rect, SDL_MapRGB(m_screen->format, 0, 0, 0));
    SDL_Color ok_col = {255, 0, 0, 0};

    if (evalTime > m_maxTime) {
        m_maxTime = evalTime;
    }
    m_averageTime += evalTime;
    m_numStats++;
    sprintf(info, "             Tracking of %i robots took %.3f ms.", numBots, evalTime / 1000.0);
    text = TTF_RenderUTF8_Blended(m_smallFont, info, ok_col);
    rect.x = 0;
    rect.y = 0;
    SDL_BlitSurface(text, NULL, m_screen, &rect);
    SDL_FreeSurface(text);
}

void CGui::guideCalibration(int calibNum, float dimX, float dimY)
{
    if (calibNum >= 0 && calibNum <= 4) {
        char ttt[100];
        const char* texty[] = {
            " Click the circle at the [0.000,0.000].",
            " Click the circle at the [%.3f,0.000].",
            " Click the circle at the [0.000,%.3f].",
            " Click the circle at the [%.3f,%.3f].",
        };

        SDL_Rect rect;
        SDL_Surface* text;
        rect.x = m_width / 2 - 130;
        rect.y = m_height / 2;
        rect.w = 260;
        rect.h = 14;
        SDL_FillRect(m_screen, &rect, SDL_MapRGB(m_screen->format, 0, 0, 0));
        SDL_Color ok_col = {0, 255, 0, 0};
        sprintf(ttt, texty[calibNum], dimX, dimY);
        if (calibNum == 2) {
            sprintf(ttt, texty[calibNum], dimY);
        }
        text = TTF_RenderUTF8_Blended(m_smallFont, ttt, ok_col);
        SDL_BlitSurface(text, NULL, m_screen, &rect);
        SDL_FreeSurface(text);
    }
}

void CGui::displayHelp(bool displayHelp)
{
    const char* helpText[] = {
        "[h] - to display/hide help,",
        "[l] - draw/hide coordinates,",
        "[s] - save current image,",
        "[d] - draw/hide segmentation outcome,",
        "[-] - decrease the number of tracked patterns,",
        "[+] - increase the number of tracked patterns,",
        "[3] - switch to 3D coordinage system,",
        "[2] - switch to planar coordinage system,",
        "[1] - switch to camera coordinage system,",
        "[r] - recalibrate,",
    };

    SDL_Rect rect;
    SDL_Surface* text;
    int numStrings = 10;
    if (displayHelp == false) {
        numStrings = 1;
    }
    rect.x = 0;
    rect.y = m_height - 22 * numStrings;
    rect.w = 350;
    rect.h = 22 * numStrings;
    SDL_FillRect(m_screen, &rect, SDL_MapRGB(m_screen->format, 0, 0, 0));
    SDL_Color ok_col = {0, 255, 0, 0};
    for (int i = 0; i < numStrings; i++) {
        rect.y = m_height - (i + 1) * 22;
        text = TTF_RenderUTF8_Blended(m_smallFont, helpText[i], ok_col);
        SDL_BlitSurface(text, NULL, m_screen, &rect);
        SDL_FreeSurface(text);
    }
}

void CGui::saveScreen(int a)
{
    CRawImage image((unsigned char*)m_screen->pixels, m_width, m_height);
    image.swapRGB();
    char name[1000];
    if (a == -1) {
        m_num++;
    }
    else {
        m_num = a;
    }
    sprintf(name, "output/%09i.bmp", m_num);
    image.saveBmp(name);
}

void CGui::drawStats(int x, int y, STrackedObject o, bool D2)
{
    SDL_Rect rect;     // Pozice textu na obrazovce
    SDL_Surface* text; // Surface s vykreslenym textem

    rect.x = x / m_scale;
    rect.y = y / m_scale;
    rect.x = x / m_scale;
    rect.w = 0;
    rect.h = 0;
    SDL_FillRect(m_screen, &rect, SDL_MapRGB(m_screen->format, 0, 0, 0));
    SDL_Color ok_col = {255, 0, 0, 0};
    char info[1000];

    if (D2) {
        sprintf(info, "%02i %03i", o.ID, (int)(o.yaw / M_PI * 180));
        text = TTF_RenderUTF8_Blended(m_smallFont, info, ok_col);
        rect.y = y / m_scale - 14;
        SDL_BlitSurface(text, NULL, m_screen, &rect);
    }

    if (D2) {
        sprintf(info, "%03.0f %03.0f", 1000 * o.x, 1000 * o.y);
    }
    else {
        sprintf(info, "%.3f %.3f %.3f", o.x, o.y, o.z);
    }
    text = TTF_RenderUTF8_Blended(m_smallFont, info, ok_col);
    rect.y = y / m_scale;
    SDL_BlitSurface(text, NULL, m_screen, &rect);

    if (D2 == false) {
        sprintf(info, "%.3f %.3f %.3f", o.pitch, o.roll, o.yaw);
        text = TTF_RenderUTF8_Blended(m_smallFont, info, ok_col);
        rect.y = y / m_scale + 12;
        SDL_BlitSurface(text, NULL, m_screen, &rect);
    }
    SDL_FreeSurface(text);
}

void CGui::drawLine(float sx1, float sx2, float sy1, float sy2)
{
    float r = (sy1 - sy2) / (sx1 - sx2);
    float d;
    if (fabs(r) < 1) {
        if (sx1 > sx2) {
            d = sx1;
            sx1 = sx2;
            sx2 = d;
            d = sy1;
            sy1 = sy2;
            sy2 = d;
        }
        for (float x = sx1; x < sx2; x++) {
            Uint8* bufp;
            bufp = (Uint8*)m_screen->pixels + (int)((x - sx1) * r + sy1 + 0.5) * m_screen->pitch + (int)(x + 0.5) * 3;
            *(bufp + m_screen->format->Rshift / 8) = 255;
            *(bufp + m_screen->format->Gshift / 8) = 0;
            *(bufp + m_screen->format->Bshift / 8) = 0;
        }
    }
    else {
        if (sy1 > sy2) {
            d = sx1;
            sx1 = sx2;
            sx2 = d;
            d = sy1;
            sy1 = sy2;
            sy2 = d;
        }
        for (float y = sy1; y < sy2; y++) {
            Uint8* bufp;
            bufp = (Uint8*)m_screen->pixels + (int)(y + 0.5) * m_screen->pitch + (int)((y - sy1) / r + sx1 + 0.5) * 3;
            *(bufp + m_screen->format->Rshift / 8) = 255;
            *(bufp + m_screen->format->Gshift / 8) = 0;
            *(bufp + m_screen->format->Bshift / 8) = 0;
        }
    }
}

void CGui::drawEllipse(SSegment s, STrackedObject t)
{
    float sx1 = s.x + s.v0 * s.m0 * 2;
    float sx2 = s.x - s.v0 * s.m0 * 2;
    float sy1 = s.y + s.v1 * s.m0 * 2;
    float sy2 = s.y - s.v1 * s.m0 * 2;
    drawLine(sx1, sx2, sy1, sy2);

    sx1 = s.x + s.v1 * s.m1 * 2;
    sx2 = s.x - s.v1 * s.m1 * 2;
    sy1 = s.y - s.v0 * s.m1 * 2;
    sy2 = s.y + s.v0 * s.m1 * 2;
    drawLine(sx1, sx2, sy1, sy2);

    int x;
    int y;
    float fx;
    float fy;
    for (float a = 0; a < 6.28; a += 0.01) {
        fx = s.x + cos(a) * s.v0 * s.m0 * 2 + s.v1 * s.m1 * 2 * sin(a);
        fy = s.y + s.v1 * s.m0 * 2 * cos(a) - s.v0 * s.m1 * 2 * sin(a);
        x = (int)(fx + 0.5);
        y = (int)(fy + 0.5);
        //printf("GGG: %.3f %.3f\n",fx,fy);
        Uint8* bufp;
        if (x > 0 && y > 0 && x < m_screen->w && y < m_screen->h) {
            bufp = (Uint8*)m_screen->pixels + y * m_screen->pitch + x * 3;
            *(bufp + m_screen->format->Rshift / 8) = 0;
            *(bufp + m_screen->format->Gshift / 8) = 255;
            *(bufp + m_screen->format->Bshift / 8) = 0;
        }
    }
}

void CGui::update()
{
    SDL_UpdateRect(m_screen, 0, 0, 0, 0);
}
