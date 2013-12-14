#include "extract_keyframe.hpp"
#include "tmem.h"
#include <math.h>
Extract_KeyFrame::Extract_KeyFrame(void* memHandle)
{
    m_sceneSpace = 3;
    m_sceneCount = 0;
    m_KFNum      = 0;
    m_threshold  = 0.9;
    m_memHandle  = memHandle;
    for (int i=0; i<SAM_MAX_KEY_FRAME_NUM; i++)
        m_index[i]=-1;
}

Extract_KeyFrame::~Extract_KeyFrame()
{}

int Extract_KeyFrame::computeHist(unsigned char *pRGB,
                                  int width, int height, int pitch,
                                  double *pR, double *pG, double *pB)
{
    if ((0==pRGB)||(0==pR)||(0==pG)||(0==pB))
        return -1;
    
    TMemSet(pR, 0, HIST_BIN*sizeof(double));
    TMemSet(pG, 0, HIST_BIN*sizeof(double));
    TMemSet(pB, 0, HIST_BIN*sizeof(double));
    
    for (int h=0; h<height; h++)
    {
        for (int w=0; w<width; w++)
        {
            int R, G, B;
            R = (int)(pRGB[3*w+0]/256.0f*HIST_BIN);
            G = (int)(pRGB[3*w+1]/256.0f*HIST_BIN);
            B = (int)(pRGB[3*w+2]/256.0f*HIST_BIN);
            pR[R] += 1;
            pG[G] += 1;
            pB[B] += 1;
        }
        pRGB += pitch;
    }
    
    // normalize
    int factor = height*width;
    for (int i=0; i<HIST_BIN; i++)
    {
        pR[i] /= factor;
        pG[i] /= factor;
        pB[i] /= factor;
    }
    return 0;
}

int Extract_KeyFrame::conputeBoundary(double *pR_pre, double *pG_pre, 
                                      double *pB_pre, double *pR_cur,
                                      double *pG_cur, double *pB_cur,
                                      int *pIsBoundary)
{    
    if ((0==pR_pre) || (0==pG_pre) || (0==pB_pre)
        ||(0==pR_cur) || (0==pG_cur) || (0==pB_cur)
        ||(0==pIsBoundary))
        return -1;

    double inner = 0;
    double leng1 = 0;
    double leng2 = 0;
    *pIsBoundary = 0;
    
    for (int i=0; i<HIST_BIN; i++)
    {
        inner += pR_pre[i]*pR_cur[i];
        inner += pG_pre[i]*pG_cur[i];
        inner += pB_pre[i]*pB_cur[i];
        
        leng1 += pR_pre[i]*pR_pre[i];
        leng1 += pG_pre[i]*pG_pre[i];
        leng1 += pB_pre[i]*pB_pre[i];

        leng2 += pR_cur[i]*pR_cur[i];
        leng2 += pG_cur[i]*pG_cur[i];
        leng2 += pB_cur[i]*pB_cur[i];        
    }

    double cos_sim = sqrt((inner*inner) / (leng1*leng2));
    m_sceneCount++;

    if (cos_sim < m_threshold && m_sceneCount > m_sceneSpace)
    {
        m_sceneCount = 0;
        *pIsBoundary = 1;
    }
    return 0;
}


int Extract_KeyFrame::extractKF(unsigned char **ppRGB, int width,
                                int height, int pitch, int imgNum)
{
    int rVal = -1;

    // can use array also.
    double *pR_pre=0, *pG_pre=0, *pB_pre=0;
    double *pR_cur=0, *pG_cur=0, *pB_cur=0;
    int isBoundary;
    int i;
    
    if (0==ppRGB)
        goto SAM_EXIT;    

    pR_pre = (double*)TMemAlloc(m_memHandle, HIST_BIN*sizeof(double));
    pG_pre = (double*)TMemAlloc(m_memHandle, HIST_BIN*sizeof(double));
    pB_pre = (double*)TMemAlloc(m_memHandle, HIST_BIN*sizeof(double));
    pR_cur = (double*)TMemAlloc(m_memHandle, HIST_BIN*sizeof(double));
    pG_cur = (double*)TMemAlloc(m_memHandle, HIST_BIN*sizeof(double));
    pB_cur = (double*)TMemAlloc(m_memHandle, HIST_BIN*sizeof(double));
    
    if ((0==pR_pre)||(0==pG_pre)||(0==pB_pre)
       ||(0==pR_cur)||(0==pG_cur)||(0==pB_cur)
       ||(imgNum<1))
        goto SAM_EXIT;
    
    m_sceneCount = 1;
    m_KFNum      = 0;  
    if (0 != computeHist(ppRGB[0], width, height, pitch, 
                         pR_cur, pG_cur, pB_cur))
        goto SAM_EXIT;
    TMemCpy(pR_pre, pR_cur, HIST_BIN*sizeof(double));
    TMemCpy(pG_pre, pG_cur, HIST_BIN*sizeof(double));
    TMemCpy(pB_pre, pB_cur, HIST_BIN*sizeof(double));
    
    for (i=1; i<imgNum; i++)
    {
        if (0 != computeHist(ppRGB[i], width, height, pitch, 
                             pR_cur, pG_cur, pB_cur))
            goto SAM_EXIT;

        if (0 != conputeBoundary(pR_pre, pG_pre, pB_pre, 
                                 pR_cur, pG_cur, pB_cur,
                                 &isBoundary))
            goto SAM_EXIT;

        TMemCpy(pR_pre, pR_cur, HIST_BIN*sizeof(double));
        TMemCpy(pG_pre, pG_cur, HIST_BIN*sizeof(double));
        TMemCpy(pB_pre, pB_cur, HIST_BIN*sizeof(double));
        if (1==isBoundary)
        {
            m_index[m_KFNum] = i-1;
            m_KFNum ++;
        }

        if(m_KFNum >= SAM_MAX_KEY_FRAME_NUM)
            break;
    }

    if ((m_KFNum < SAM_MAX_KEY_FRAME_NUM) && (m_sceneCount>0))
    {
        m_index[m_KFNum] = imgNum-1;
        m_KFNum ++;
    }
    
    rVal = 0;
 SAM_EXIT:
    if(pR_pre) TMemFree(m_memhandle, pR_pre);
    if(pG_pre) TMemFree(m_memhandle, pG_pre);
    if(pB_pre) TMemFree(m_memhandle, pB_pre);
    if(pR_cur) TMemFree(m_memhandle, pR_cur);
    if(pG_cur) TMemFree(m_memhandle, pG_cur);
    if(pB_cur) TMemFree(m_memhandle, pB_cur);
    return rVal;
}
int Extract_KeyFrame::getIndexOfKF(int **ppIndex, int *pKFNum)
{
    if ((0==ppIndex)||(0==pKFNum))
        return -1;

    *ppIndex = m_index;
    *pKFNum = m_KFNum;
    return 0;
}
