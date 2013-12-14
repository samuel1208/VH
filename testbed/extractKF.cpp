
#include "extract_keyframe.hpp"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
using namespace cv;

int getData(char *pathList, int imgNum,
                unsigned char **ppImgList, int *pWidth, int *pHeight, int *pPitch)
{
    FILE *pathFile =  fopen(pathList, "r");
    char path[1024] = {0};
    int  width, height, pitch;
    
    for (int i=0; i<imgNum; i++)
    {
        Mat mat;
        memset(path, 0, 1024);
        char *s=fgets(path, 1024,pathFile);       
        if (strlen(path)<4)
            continue;
        //remove the Enter character
        path[strlen(path)-1] = 0;
        
        mat = imread(path);
        if(!mat.data)
        {
            printf("ERROR:: Open %s failed\n", path);
            return -1;
        }
        
        width = mat.cols;
        height = mat.rows;
        pitch = mat.step1()*mat.elemSize1();
        ppImgList[i] = (unsigned char *)TMemAlloc(0, pitch*height);
        TMemCpy(ppImgList[i], mat.data, pitch*height);
    }
    fclose(pathFile);
    
    *pWidth=width;
    *pHeight=height;
    *pPitch=pitch;
    return 0;
}


int main(int argc, char **argv)
{
    char *pathList = 0;
    int  imgNum    = 0;
    unsigned char **ppImgList=0;
    int width=0, height=0, pitch=0;
    Extract_KeyFrame *extractKF=0;
    int *pIndex, KFNum, i;

    if (argc < 3)
    {
        printf("./extractKF imgPathList imgNum\n");
        goto SAM_EXIT; 
    }

    pathList = argv[1];
    imgNum   = atoi(argv[2]);
    
    ppImgList = (unsigned char **)malloc(imgNum*sizeof(unsigned char *));
    if(0 == ppImgList)
        goto SAM_EXIT;
    

    if (0!=getData(pathList, imgNum, ppImgList, &width, &height, &pitch))
        goto SAM_EXIT;

    extractKF = new Extract_KeyFrame(0);
    if (0 == extractKF)
        return -1;

    if (0 != extractKF->extractKF(ppImgList, width, height, pitch, imgNum))
        goto SAM_EXIT;

    if (0 != extractKF->getIndexOfKF(&pIndex, &KFNum))
        goto SAM_EXIT;

    for(i =0; i<KFNum; i++)
        printf("select index: %d\n", pIndex[i]);

 SAM_EXIT:
    if (extractKF) delete extractKF;
    if (ppImgList)
    {
        for(int i=0; i<imgNum; i++)
            free(ppImgList[i]);
        free(ppImgList);
    }
    return 0;
}
