#ifndef __EXTRACT_KEYFRAME_HPP__
#define __EXTRACT_KEYFRAME_HPP__

#include "base-object.hpp"

#define SAM_MAX_KEY_FRAME_NUM (100)
#define HIST_BIN              (100)

/*
  Extract keyframes from a video using color histogram based 
  shot boundary detection algorithm
*/
class Extract_KeyFrame: public Object
{
    
public:
    Extract_KeyFrame(void *memHandle);
    ~Extract_KeyFrame();
    int extractKF(unsigned char **ppRGB, int width,
                  int height, int pitch, int imgNum);
    int getIndexOfKF(int **ppIndex, int *keyFrameNum);
    
private:
    int computeHist(unsigned char *pRGB,
                    int width, int height, int pitch,
                    double *pR, double *pG, double *pB);

    int conputeBoundary(double *pR_pre, double *pG_pre, double *pB_pre,
                        double *pR_cur, double *pG_cur, double *pB_cur,
                        int *pIsBoundary);
private:
    int    m_sceneSpace;
    int    m_sceneCount;
    int    m_KFNum;    
    int    m_index[SAM_MAX_KEY_FRAME_NUM];
    double m_threshold;  
    void*  m_memHandle;
};

#endif
