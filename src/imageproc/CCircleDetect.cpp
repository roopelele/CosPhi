#include "CCircleDetect.h"

#include <chrono> // std::chrono::seconds
#include <thread> // std::this_thread::sleep_for

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int* CCircleDetect::buffer = NULL;
int* CCircleDetect::queue = NULL;
int* CCircleDetect::mask = NULL;
int CCircleDetect::maskNum = 0;

// Variable initialization
CCircleDetect::CCircleDetect(int wi, int he, int idi)
    : m_enableCorrections(false)
{
    m_localSearch = false;
    m_ID = idi;
    m_numberIDs = 0;
    lastTrackOK = false;
    m_debug = DEBUG_LEVEL;
    draw = DRAW_SEGMENTS;
    drawAll = true;
    m_maxFailed = 0;
    m_minSize = 15;
    m_thresholdStep = 256;
    m_maxThreshold = 3 * 256;
    m_centerDistanceToleranceRatio = 0.1;
    m_centerDistanceToleranceAbs = 15;
    m_circularTolerance = 1.5;
    m_ratioTolerance = 2.4;
    m_threshold = m_maxThreshold / 2;
    m_numFailed = m_maxFailed;
    m_track = true;
    m_circularityTolerance = 0.02;

    // initialization - fixed params
    m_width = wi;
    m_height = he;
    m_len = m_width * m_height;
    m_siz = m_len * 3;
    m_ownBuffer = false;
    if (buffer == NULL) {
        m_ownBuffer = true;
        buffer = (int*)malloc(m_len * sizeof(int));
        queue = (int*)malloc(m_len * sizeof(int));
        mask = (int*)malloc(m_len * sizeof(int));
        SSegment dummy;
        bufferCleanup(dummy);
    }
    clearCalibMask();
    m_diameterRatio = 5.0 / 14.0; // inner vs. m_outer circle diameter
    float areaRatioInner_Outer = m_diameterRatio * m_diameterRatio;
    m_outerAreaRatio = M_PI * (1.0 - areaRatioInner_Outer) / 4;
    m_innerAreaRatio = M_PI / 4.0;
    m_areasRatio = (1.0 - areaRatioInner_Outer) / areaRatioInner_Outer;
    m_tima = m_timb = m_timc = m_timd = m_sizer = m_sizerAll = 0;
    loadCircleID("../etc/m_ID.txt");
}

CCircleDetect::~CCircleDetect()
{
    //	if (m_debug > 5) printf("Timi %i %i %i %i\n",m_tima,m_timb,m_sizer,m_sizerAll);
    if (m_ownBuffer) {
        free(buffer);
        free(queue);
    }
}

bool CCircleDetect::changeThreshold()
{
    int div = 1;
    int dum = m_numFailed;
    while (dum > 1) {
        dum = dum / 2;
        div *= 2;
    }
    int step = 256 / div;
    setThreshold(3 * (step * (m_numFailed - div) + step / 2));

    if (m_debug > 5)
        fprintf(stdout, "Threshold: %i %i %i\n", div, m_numFailed, m_threshold / 3);
    return step > 8;
}

bool CCircleDetect::examineSegment(CRawImage* image, SSegment* segmen, int ii, float areaRatio)
{
    m_timer.reset();
    m_timer.start();
    int vx, vy;
    m_queueOldStart = m_queueStart;
    int position = 0;
    int pos;
    bool result = false;
    int type = buffer[ii];
    int maxx, maxy, minx, miny;

    buffer[ii] = ++m_numSegments;
    segmen->x = ii % m_width;
    segmen->y = ii / m_width;
    minx = maxx = segmen->x;
    miny = maxy = segmen->y;
    segmen->valid = false;
    segmen->round = false;
    // push segment coords to the queue
    queue[m_queueEnd++] = ii;
    // and until queue is empty
    while (m_queueEnd > m_queueStart) {
        // pull the coord from the queue
        position = queue[m_queueStart++];
        // search neighbours
        pos = position + 1;
        if (buffer[pos] == 0) {
            m_ptr = &image->data[pos * 3];
            buffer[pos] = ((m_ptr[0] + m_ptr[1] + m_ptr[2]) > m_threshold) - 2;
        }
        if (buffer[pos] == type) {
            queue[m_queueEnd++] = pos;
            maxx = max(maxx, pos % m_width);
            buffer[pos] = m_numSegments;
        }
        pos = position - 1;
        if (buffer[pos] == 0) {
            m_ptr = &image->data[pos * 3];
            buffer[pos] = ((m_ptr[0] + m_ptr[1] + m_ptr[2]) > m_threshold) - 2;
        }
        if (buffer[pos] == type) {
            queue[m_queueEnd++] = pos;
            minx = min(minx, pos % m_width);
            buffer[pos] = m_numSegments;
        }
        pos = position - m_width;
        if (buffer[pos] == 0) {
            m_ptr = &image->data[pos * 3];
            buffer[pos] = ((m_ptr[0] + m_ptr[1] + m_ptr[2]) > m_threshold) - 2;
        }
        if (buffer[pos] == type) {
            queue[m_queueEnd++] = pos;
            miny = min(miny, pos / m_width);
            buffer[pos] = m_numSegments;
        }
        pos = position + m_width;
        if (buffer[pos] == 0) {
            m_ptr = &image->data[pos * 3];
            buffer[pos] = ((m_ptr[0] + m_ptr[1] + m_ptr[2]) > m_threshold) - 2;
        }
        if (buffer[pos] == type) {
            queue[m_queueEnd++] = pos;
            maxy = max(maxy, pos / m_width);
            buffer[pos] = m_numSegments;
        }
    }

    // once the queue is empty, i.e. segment is complete, we compute its size
    segmen->size = m_queueEnd - m_queueOldStart;
    if (segmen->size > m_minSize) {
        // and if its large enough, we compute its other properties
        segmen->maxx = maxx;
        segmen->maxy = maxy;
        segmen->minx = minx;
        segmen->miny = miny;
        segmen->type = -type;
        vx = (segmen->maxx - segmen->minx + 1);
        vy = (segmen->maxy - segmen->miny + 1);
        segmen->x = (segmen->maxx + segmen->minx) / 2;
        segmen->y = (segmen->maxy + segmen->miny) / 2;
        segmen->roundness = vx * vy * areaRatio / segmen->size;
        // we check if the segment is likely to be a ring
        if (segmen->roundness - m_circularTolerance < 1.0 && segmen->roundness + m_circularTolerance > 1.0) {
            // if its round, we compute yet another properties
            segmen->round = true;
            segmen->mean = 0;
            for (int p = m_queueOldStart; p < m_queueEnd; p++) {
                pos = queue[p];
                segmen->mean += image->data[pos * 3] + image->data[pos * 3 + 1] + image->data[pos * 3 + 2];
            }
            segmen->mean = segmen->mean / segmen->size;
            result = true;
        }
    }
    if (result && m_debug > 0) {
        fprintf(stdout, "found circle in (%i, %i) \n", (maxx + minx) / 2, (maxy + miny) / 2);
        fprintf(stdout, "    mean color: %i \n", segmen->mean);
    }
    m_timb += m_timer.getTime();
    return result;
}

void CCircleDetect::bufferCleanup(SSegment init)
{
    int pos = (m_height - 1) * m_width;
    if (init.valid == false || m_track == false || lastTrackOK == false) {
        memset(buffer, 0, sizeof(int) * m_len);
        for (int i = 0; i < m_width; i++) {
            buffer[i] = -1000;
            buffer[pos + i] = -1000;
        }
        for (int i = 0; i < m_height; i++) {
            buffer[m_width * i] = -1000;
            buffer[m_width * i + m_width - 1] = -1000;
        }
    }
    else {
        int pos, ix, ax, iy, ay;
        ix = max(init.minx - 2, 1);
        ax = min(init.maxx + 2, m_width - 2);
        iy = max(init.miny - 2, 1);
        ay = min(init.maxy + 2, m_height - 2);
        for (int y = iy; y < ay; y++) {
            pos = y * m_width;
            for (int x = ix; x < ax; x++)
                buffer[pos + x] = 0;
        }
    }
}

int CCircleDetect::loadCircleID(const char* id)
{
    FILE* idFile = fopen(id, "r");
    int dummy = 0;
    m_numberIDs = 0;
    if (idFile == NULL) {
        fprintf(stderr, "m_ID file not found\n");
        return -2;
    }
    while (feof(idFile) == 0) {
        fscanf(idFile, "%i %f %f\n", &dummy, &idx[m_numberIDs], &idy[m_numberIDs]);
        if (dummy != m_numberIDs) {
            fprintf(stderr, "m_ID file corrupted\n");
            fclose(idFile);
            return -1;
        }
        m_numberIDs++;
    }
    fclose(idFile);
    return 0;
}

void CCircleDetect::identifySegment(SSegment* segment)
{
    float maxDistance = 1000;
    int index = -1;
    float dx, dy;
    for (int i = 0; i < m_numberIDs; i++) {
        dx = segment->r0 - idx[i];
        dy = segment->r1 - idy[i];
        if (dx * dx + dy * dy < maxDistance) {
            maxDistance = dx * dx + dy * dy;
            index = i;
        }
        // Debug print
        // fprintf(stderr, "dx: %f-%f=%f, dy: %f-%f=%f, distance: %f, maxdistance: %f\n", segment->r0, idx[i], dx, segment->r1, idy[i], dy, (dx * dx + dy * dy), maxDistance);
    }
    // Debug print
    if (PRINT_LOCATION) {
        fprintf(stderr, "x: %f, y: %f, distance: %f, maxdistance: %f\n", segment->r0, segment->r1, (dx * dx + dy * dy), maxDistance);
    }
    segment->ID = index;
    if (segment->m1 / segment->m0 > 0.9) {
        segment->ID = -1;
    }
}

void CCircleDetect::clearCalibMask()
{
    maskNum = 0;
}

void CCircleDetect::applyCalibMask(CRawImage* image)
{
    int pos;
    for (int p = 0; p < maskNum; p++) {
        pos = mask[p];
        image->data[3 * pos + 0] = image->data[3 * pos + 1] = image->data[3 * pos + 2] = 0;
    }
}

void CCircleDetect::addCalibMask()
{
    for (int p = m_queueOldStart; p < m_queueEnd; p++)
        mask[maskNum++] = queue[p];
}

SSegment CCircleDetect::calcSegment(SSegment segment, int size, long int x, long int y, long int cm0, long int cm1, long int cm2)
{
    float cm0f, cm1f, cm2f, fm0, fm1, fm2, f0, f1;
    SSegment result = segment;
    float sx = (float)x / size;
    float sy = (float)y / size;
    cm0f = (cm0 - sx * sx * size);
    cm1f = (cm1 - sx * sy * size);
    cm2f = (cm2 - sy * sy * size);
    fm0 = cm0f / size;
    fm1 = cm1f / size;
    fm2 = cm2f / size;
    float det = (fm0 + fm2) * (fm0 + fm2) - 4 * (fm0 * fm2 - fm1 * fm1);
    if (det > 0)
        det = sqrt(det);
    else
        det = 0;
    f0 = ((fm0 + fm2) + det) / 2;
    f1 = ((fm0 + fm2) - det) / 2;
    result.x = sx;
    result.y = sy;
    result.m0 = sqrt(f0);
    result.m1 = sqrt(f1);
    if (fm1 != 0) {
        result.v0 = -fm1 / sqrt(fm1 * fm1 + (fm0 - f0) * (fm0 - f0));
        result.v1 = (fm0 - f0) / sqrt(fm1 * fm1 + (fm0 - f0) * (fm0 - f0));
    }
    else {
        result.v0 = result.v1 = 0;
        if (fm0 > fm2)
            result.v0 = 1.0;
        else
            result.v1 = 1.0;
    }
    // printf("An: %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",det,result.v0,result.v1,fm0,fm1,fm2,f0,f1);
    return result;
}

SSegment CCircleDetect::findSegment(CRawImage* image, SSegment init)
{
    m_numSegments = 0;

    m_timer.reset();
    m_timer.start();
    m_tima += m_timer.getTime();
    int pos = 0;
    int ii = 0;
    int start = 0;
    bool cont = true;

    // bufferCleanup(init);
    if (init.valid && m_track) {
        ii = ((int)init.y) * image->width + init.x;
        start = ii;
    }
    while (cont) {
        if (buffer[ii] == 0) {
            m_ptr = &image->data[ii * 3];
            // buffer[ii]=((m_ptr[0]+m_ptr[1]+m_ptr[2]) > m_threshold)-2;
            if ((m_ptr[0] + m_ptr[1] + m_ptr[2]) < m_threshold) {
                if (m_debug > 2) {
                    fprintf(stdout, "(%i, %i) : %i + %i + %i = %i\n", (ii % m_width), (ii / m_width), m_ptr[0], m_ptr[1], m_ptr[2], m_ptr[0] + m_ptr[1] + m_ptr[2]);
                }
                buffer[ii] = -2;
            }
        }
        if (buffer[ii] == -2) {
            if (m_debug > 2) {
                fprintf(stdout, "new segment in (%i, %i)\n", (ii % m_width), (ii / m_width));
            }
            // new segment found
            m_queueEnd = 0;
            m_queueStart = 0;
            // if the segment looks like a ring, we check its inside area
            if (examineSegment(image, &m_outer, ii, m_outerAreaRatio)) {
                pos = m_outer.y * image->width + m_outer.x;
                if (buffer[pos] == 0) {
                    m_ptr = &image->data[pos * 3];
                    buffer[pos] = ((m_ptr[0] + m_ptr[1] + m_ptr[2]) >= m_threshold) - 2;
                }
                if (buffer[pos] == -1) {
                    if (examineSegment(image, &m_inner, pos, m_innerAreaRatio)) {
                        // the inside area is a circle. now what is the area ratio of the black and white ? also, are the circles concentric ?

                        if (m_debug > 5) {
                            printf("Area ratio should be %.3f, but is %.3f, that is %.0f%% off. ", m_areasRatio, (float)m_outer.size / m_inner.size, (1 - m_outer.size / m_areasRatio / m_inner.size) * 100);
                        }
                        float concentricity = (float)m_outer.size / m_areasRatio / (float)m_inner.size;
                        if (concentricity - m_ratioTolerance < 1.0 && concentricity + m_ratioTolerance > 1.0) {
                            if (m_debug > 5) {
                                fprintf(stdout, "Segment BW ratio OK.\n");
                            }
                            if (m_debug > 5) {
                                fprintf(stdout, "Concentricity %.0f %.0f %.0f %.0f.", m_inner.x, m_inner.y, m_outer.x, m_outer.y);
                            }
                            if ((abs(m_inner.x - m_outer.x) <= m_centerDistanceToleranceAbs + m_centerDistanceToleranceRatio * ((float)(m_outer.maxx - m_outer.minx))) &&
                                (abs(m_inner.y - m_outer.y) <= m_centerDistanceToleranceAbs + m_centerDistanceToleranceRatio * ((float)(m_outer.maxy - m_outer.miny)))) {
                                if (m_debug > 5) {
                                    fprintf(stdout, "Concentricity OK.\n");
                                }
                                long int six, siy, tx, ty, cm0, cm1, cm2;
                                six = siy = cm0 = cm1 = cm2 = 0;

                                for (int p = m_queueOldStart; p < m_queueEnd; p++) {
                                    pos = queue[p];
                                    tx = pos % image->width;
                                    ty = pos / image->width;
                                    six += tx;
                                    siy += ty;
                                    cm0 += tx * tx;
                                    cm1 += tx * ty;
                                    cm2 += ty * ty;
                                }
                                m_inner = calcSegment(m_inner, m_queueEnd - m_queueOldStart, six, siy, cm0, cm1, cm2);
                                // m_inner.x = (float)six/(m_queueEnd-m_queueOldStart);
                                // m_inner.y = (float)siy/(m_queueEnd-m_queueOldStart);

                                for (int p = 0; p < m_queueOldStart; p++) {
                                    pos = queue[p];
                                    tx = pos % image->width;
                                    ty = pos / image->width;
                                    six += tx;
                                    siy += ty;
                                    cm0 += tx * tx;
                                    cm1 += tx * ty;
                                    cm2 += ty * ty;
                                }
                                m_outer = calcSegment(m_outer, m_queueEnd, six, siy, cm0, cm1, cm2);
                                m_outer.bwRatio = (float)m_inner.size / m_outer.size;

                                m_sizer += m_outer.size + m_inner.size; // for debugging
                                m_sizerAll += m_len;                    // for debugging
                                float circularity = M_PI * 4 * (m_outer.m0) * (m_outer.m1) / m_queueEnd;
                                if (m_debug > 5)
                                    fprintf(stdout, "Segment circularity: %i %03f %03f \n", m_queueEnd, M_PI * 4 * (m_outer.m0) * (m_outer.m1) / m_queueEnd, M_PI * 4 * (m_outer.m0) * (m_outer.m1));
                                if (circularity - 1.0 < m_circularityTolerance && circularity - 1.0 > -m_circularityTolerance) {

                                    // chromatic aberation correction
                                    if (m_enableCorrections) {
                                        float r = m_diameterRatio * m_diameterRatio;
                                        float m0o = m_outer.m0;
                                        float m1o = m_outer.m1;
                                        float ratio = (float)m_inner.size / (m_outer.size + m_inner.size);
                                        float m0i = sqrt(ratio) * m0o;
                                        float m1i = sqrt(ratio) * m1o;
                                        float a = (1 - r);
                                        float b = -(m0i + m1i) - (m0o + m1o) * r;
                                        float c = (m0i * m1i) - (m0o * m1o) * r;
                                        float t = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
                                        // plc second version
                                        // float t0 = (-b-sqrt(b*b-4*a*c))/(2*a);
                                        // float t1 = (-b+sqrt(b*b-4*a*c))/(2*a);
                                        // if (m1i - t0 > 0 && m1i - t1 >0) printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
                                        // float t0 = (m0i-m_diameterRatio*m0o)/(1+m_diameterRatio);
                                        // float t1 = (m1i-m_diameterRatio*m1o)/(1+m_diameterRatio);
                                        m0i -= t;
                                        m1i -= t;
                                        m0o += t;
                                        m1o += t;
                                        // fprintf(stdout,"UUU: %f R: %f %f R: %f %f\n",t,m1i/m1o*0.14,m0i/m0o*0.14,(m0o*m1o-m0i*m1i)/(m0i*m1i),(0.14*0.14-0.05*0.05)/(0.05*0.05));
                                        m_inner.m0 = m0o;
                                        m_inner.m1 = m1o;
                                    }
                                    m_outer.size = m_outer.size + m_inner.size;
                                    m_outer.horizontal = m_outer.x - m_inner.x;
                                    if (fabs(m_inner.v0 * m_outer.v0 + m_inner.v1 * m_outer.v1) > 0.5) {
                                        m_outer.r0 = m_inner.m0 / m_outer.m0;
                                        m_outer.r1 = m_inner.m1 / m_outer.m1;
                                    }
                                    else {
                                        m_outer.r0 = m_inner.m1 / m_outer.m0;
                                        m_outer.r1 = m_inner.m0 / m_outer.m1;
                                    }
                                    float orient = atan2(m_outer.y - m_inner.y, m_outer.x - m_inner.x);
                                    m_outer.angle = atan2(m_outer.v1, m_outer.v0);
                                    if (m_debug > 5) {
                                        printf("Angle: %.3f %.3f \n", m_outer.angle, orient);
                                    }
                                    if (fabs(normalizeAngle(m_outer.angle - orient)) > M_PI / 2) {
                                        m_outer.angle = normalizeAngle(m_outer.angle + M_PI);
                                    }

                                    // fiducial identification - experimental only
                                    identifySegment(&m_outer);
                                    // if (lastTrackOK == false) identifySegment(&m_outer);
                                    // m_outer.m_ID =m_ID;
                                    m_outer.valid = m_inner.valid = true;
                                    setThreshold((m_outer.mean + m_inner.mean) / 2);
                                    if (m_track) {
                                        ii = start - 1;
                                    }
                                }
                                else {
                                    if (m_track && init.valid) {
                                        ii = start - 1;
                                        if (m_debug > 0) {
                                            fprintf(stdout, "Segment failed circularity test.\n");
                                        }
                                    }
                                }
                            }
                            else {
                                if (m_track && init.valid) {
                                    ii = start - 1;
                                    if (m_debug > 0) {
                                        fprintf(stdout, "Segment failed concentricity test.\n");
                                    }
                                }
                            }
                        }
                        else {
                            // tracking failed
                            if (m_track && init.valid) {
                                ii = start - 1;
                                if (m_debug > 0) {
                                    fprintf(stdout, "Segment failed BW test.\n");
                                }
                            }
                        }
                    }
                    else {
                        // tracking failed
                        if (m_track && init.valid) {
                            ii = start - 1;
                            if (m_debug > 0) {
                                printf("Inner segment not a circle\n");
                            }
                        }
                    }
                }
                else {
                    if (m_track && init.valid) {
                        ii = start - 1;
                        if (m_debug > 0) {
                            printf("Inner segment not white %i %i %i\n", m_threshold, m_ptr[0] + m_ptr[1] + m_ptr[2], m_outer.size);
                        }
                    }
                }
            }
            else {
                // tracking failed
                if (m_track && init.valid) {
                    ii = start - 1;
                    if (m_debug > 0) {
                        printf("Outer segment %.0f %.0f %i not a circle\n", m_outer.x, m_outer.y, m_outer.size);
                    }
                }
            }
        }
        ii++;
        if (ii >= m_len)
            ii = 0;
        cont = (ii != start);
    }
    if (m_debug > 5)
        printf("II: %i %i\n", ii, start);
    if (m_debug > 1)
        fprintf(stdout, "Inner %.2f %.2f Area: %i Vx: %i Vy: %i Mean: %i Thr: %i Eigen: %03f %03f %03f %03f Axes: %03f \n", m_inner.x, m_inner.y, m_inner.size, m_inner.maxx - m_inner.minx, m_inner.maxy - m_inner.miny, m_inner.mean, m_threshold, m_inner.m0, m_inner.m1, m_inner.v0, m_inner.v1, m_inner.v0 * m_outer.v0 + m_inner.v1 * m_outer.v1);
    if (m_debug > 1)
        fprintf(stdout, "Outer %.2f %.2f Area: %i Vx: %i Vy: %i Mean: %i Thr: %i Eigen: %03f %03f %03f %03f Ratios: %.3f %.3f %i\n", m_outer.x, m_outer.y, m_outer.size, m_outer.maxx - m_outer.minx, m_outer.maxy - m_outer.miny, m_outer.mean, m_threshold, m_outer.m0, m_outer.m1, m_outer.v0, m_outer.v1, m_outer.r0 * 150, m_outer.r1 * 150, m_outer.ID);
    if (m_outer.valid) {
        if (m_numSegments == 2) {
            lastTrackOK = true;
            m_localSearch = false;
        }
        else {
            lastTrackOK = false;
            if (m_localSearch) {
                m_outer.valid = false;
            }
        }
    }
    // m_threshold management
    if (m_outer.valid) {
        m_lastThreshold = m_threshold;
        drawAll = false;
        m_numFailed = 0;
    }
    else if (m_numFailed < m_maxFailed) {
        if (m_numFailed++ % 2 == 0) {
            changeThreshold();
        }
        else {
            setThreshold(m_lastThreshold);
        }
        if (m_debug > 5) {
            drawAll = true;
        }
    }
    else {
        m_numFailed++;
        if (changeThreshold() == false)
            m_numFailed = 0;
        if (m_debug > 5)
            drawAll = true;
    }

    // Drawing results
    if (m_outer.valid) {
        for (int p = m_queueOldStart; p < m_queueEnd; p++) {
            pos = queue[p];
            image->data[3 * pos + 0] = image->data[3 * pos + 1] = image->data[3 * pos + 2] = m_outer.mean / 3;
        }
    }
    if (draw) {
        if (init.valid || m_track || lastTrackOK) {
            for (int p = m_queueOldStart; p < m_queueEnd; p++) {
                pos = queue[p];
                image->data[3 * pos + 0] = 0;
                image->data[3 * pos + 1] = 255;
                image->data[3 * pos + 2] = 255;
            }
            for (int p = 0; p < m_queueOldStart; p++) {
                pos = queue[p];
                image->data[3 * pos + 0] = 255;
                image->data[3 * pos + 1] = 0;
                image->data[3 * pos + 2] = 255;
            }
        }
    }
    bufferCleanup(m_outer);
    return m_outer;
}

void CCircleDetect::setThreshold(int val)
{
    m_threshold = val;
    if (m_threshold > m_maxThreshold) {
        m_threshold = m_maxThreshold;
    }
    else if (m_threshold < 0) {
        m_threshold = 0;
    }
    if (m_debug > 1) {
        fprintf(stdout, "set threshold to %i\n", m_threshold);
    }
}
