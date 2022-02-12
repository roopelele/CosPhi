/*
 * File name: CCircleDetect.h
 * Date:      2010
 * Author:   Tom Krajnik
 */

#ifndef __CCIRCLEDETECT_H__
#define __CCIRCLEDETECT_H__

#include "CRawImage.h"
#include "CTimer.h"
#include "cmath.h"
#include "config.h"
#include <math.h>

#define COLOR_PRECISION 32
#define COLOR_STEP      8
#define INNER           0
#define OUTER           1

typedef struct {
    float x;
    float y;
    float angle;
    float horizontal;
    int size;
    int maxy;
    int maxx;
    int miny;
    int minx;
    int mean;
    int type;
    float roundness;
    float bwRatio;
    bool round;
    bool valid;
    float m0;
    float m1;
    float v0;
    float v1;
    float r0;
    float r1;
    int ID;
} SSegment;

class CCircleDetect {

public:
    CCircleDetect(int wi, int he, int idi = -1);
    ~CCircleDetect();
    void identifySegment(SSegment* segment);
    void bufferCleanup(SSegment init);
    SSegment findSegment(CRawImage* image, SSegment init);
    bool examineSegment(CRawImage* image, SSegment* segmen, int ii, float areaRatio);
    SSegment calcSegment(SSegment segment, int size, long int x, long int y, long int cm0, long int cm1, long int cm2);

    void clearCalibMask();
    void applyCalibMask(CRawImage* image);
    void addCalibMask();

    int loadCircleID(const char* id);
    bool changeThreshold();
    bool draw;
    bool drawAll;
    bool lastTrackOK;
    int m_debug;
    bool localSearch;

private:
    bool m_track;
    int m_maxFailed;
    int m_numFailed;
    int m_threshold;

    int m_minSize;
    int m_maxSize;
    int m_lastThreshold;
    int m_thresholdBias;
    int m_maxThreshold;

    int m_thresholdStep;
    float m_circularTolerance;
    float m_circularityTolerance;
    float m_ratioTolerance;
    float m_centerDistanceToleranceRatio;
    int m_centerDistanceToleranceAbs;
    bool m_enableCorrections;

    int m_ID;
    SSegment m_inner;
    SSegment m_outer;
    float m_outerAreaRatio;
    float m_innerAreaRatio;
    float m_areasRatio;
    int m_queueStart;
    int m_queueEnd;
    int m_queueOldStart;
    int m_numSegments;
    int m_width;
    int m_height;
    int m_len;
    int m_siz;
    int m_expand[4];
    unsigned char* m_ptr;
    CTimer m_timer;
    int m_tima;
    int m_timb;
    int m_timc;
    int m_timd;
    int m_sizer;
    int m_sizerAll;
    float m_diameterRatio;
    bool m_ownBuffer;
    static int* buffer;
    static int* queue;
    static int* mask;
    static int maskNum;
    float idx[MAX_PATTERNS];
    float idy[MAX_PATTERNS];
    int m_numberIDs;
};

#endif

/* end of CCircleDetect.h */
