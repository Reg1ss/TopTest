#include <opencv2/opencv.hpp>
#include <iostream>
#include <zconf.h>
#include <mutex>
#include <glog/logging.h>
#include <sys/time.h>
#include <syscall.h>
#include <string>
#include "DLStream/DataTypes.h"
#include "DLStream/DLStream.h"
#include "DLStream/FrameDataRef.h"
#include <chrono>
#include "CPUCore/FPSCount.h"
#include <chrono>
#include <ctime>
#include <time.h>

#include "thread"

using namespace tpv;
using namespace std;

IDrawCmdListPtr testVideo(FrameDataRef frameData);
char *itoa(int src);

int main(int argc, char* argv[])
{

    FPSCount fpsCount;
    LOG(INFO) << TOPDLSTREAM_LIBRARY_VERSION;
    //LOG(INFO) << FLAGS_topplus;
    LOG(ERROR) << "TestLog";

    //std::shared_ptr<DLStream> dlStream = DLStreamFactory::Create(TOPDLSTREAM_LIBRARY_VERSION);
    std::shared_ptr<DLStream> dlStream = DLStreamFactory::Create();
    DLStreamInitParams initParams;
    initParams.streamID = 1;
    initParams.limitVideoFileFPS = true;
    initParams.algorithmConfig.vehicleClassify.enable = false;
    initParams.algorithmConfig.detect.enable = true;
    initParams.algorithmConfig.detect.detectType = DetectType::DT_VEHICLE_AND_HUMAN;
    initParams.algorithmConfig.abandonedObject.enable = false;
    initParams.algorithmConfig.abandonedObject.detailConfig.iStep = 1;
    initParams.algorithmConfig.abandonedObject.detailConfig.iMinDripTime = 20;   //单位为s
    initParams.algorithmConfig.abandonedObject.detailConfig.iMinDripWidth = 10;  //最小遗留物宽度 单位像素 与最小高度关系为与
    initParams.algorithmConfig.abandonedObject.detailConfig.iMinDripHeight = 10; //最小遗留高度 单位像素 与最小宽度关系为与
    initParams.algorithmConfig.abandonedObject.detailConfig.iDripImgThval = 45;  //取值0~255
    initParams.algorithmConfig.abandonedObject.detailConfig.tDectRoi = {
            cv::Point(0,0),
            cv::Point(1920,0),
            cv::Point(1920,1080),
            cv::Point(0,1080),
    };
    //initParams.inputVideoPath = "/home/yang/yangyong/video/test.avi";
    initParams.inputVideoPath = "/home/kuangrx/TopTest/Data/天府五街流量监控.mp4";

    //initParams.inputVideoPath = "/home/yang/yangyong/video/10.152.175.101_01_20180917150651268_3.mp4";
    initParams.outputRtmpURL = "rtmp://192.168.0.243:9000/live/test_5";
    dlStream->Init(initParams);
    dlStream->Start();

//    std::shared_ptr<TopEVEInfoIntergration> m_pEventAnalyzer = std::make_shared<TopEVEInfoIntergration>();
//
//    m_pEventAnalyzer->Init(initParam.eventInitParams);

    chrono::system_clock::time_point start = chrono::system_clock::now();
    int count = 1;
    while(start + chrono::seconds(3600) > chrono::system_clock::now())
    {
        IDrawCmdListPtr drawCmdList = IDrawCmdList::Create();
        FrameDataRef frameData = dlStream->GetFrameData().first;
        if( !frameData ) break;
        fpsCount.Update();
        LOG_EVERY_N(INFO,512) << fpsCount.FormatStrFPS(initParams.streamID, "DLStream");
        drawCmdList = testVideo(frameData);
        dlStream->RenderAndPush(frameData, drawCmdList);
        cv::Mat img = dlStream->RenderAndGet(frameData,drawCmdList);
        std::string outputPath = "/home/kuangrx/TopTest/Data/TF5th_Result/";
        std::string path = outputPath + itoa(count) + ".jpg";
        cv::imwrite(path, img);
        count++;
        std::cout << "Count : " << count << std::endl;
//        std::string frameID = itoa(frameData.FrameID());
//        cv::Mat img = frameData.GetCPUMat();
//        for(auto& item : frameData.Objects())
//        {
//            drawCmdList->rectangle(
//                    item.targetRect.tl().y,
//                    item.targetRect.tl().x,
//                    item.targetRect.br().x,
//                    item.targetRect.br().y,
//                    5,cv::Scalar(255,0,0,255)
//            );
//            std::string cartype = cv::format(" type: %d ",item.detail.vehicleDetail.type);
//            if(item.type == ObjectType ::OT_Vehicle && (int)item.detail.vehicleDetail.type!=1000)
//                drawCmdList->putText(item.targetRect.tl().y-20,  item.targetRect.tl().x, cartype, 30, cv::Scalar(255,255,0,255));
//            else
//                drawCmdList->putText(item.targetRect.tl().y-20,  item.targetRect.tl().x, cv::format(" type: %d ",item.type), 30, cv::Scalar(0,0,255,255));
//        }
        dlStream->RenderAndPush(frameData, drawCmdList);
    }

    dlStream->Stop();
    dlStream.reset();
   // google::ShutdownGoogleLogging();
   // gflags::ShutDownCommandLineFlags();
}

IDrawCmdListPtr testVideo(FrameDataRef frameData){
    //cv::Mat img = frameData.GetCPUMat();
    IDrawCmdListPtr drawCmdList = IDrawCmdList::Create();
    for(auto& item : frameData.Objects())
    {

//        std::string cartype = cv::format(" type: %d ",item.detail.vehicleDetail.type);
//        if(item.type == ObjectType ::OT_Vehicle && (int)item.detail.vehicleDetail.type!=1000)
//            drawCmdList->putText(item.targetRect.tl().y-20,  item.targetRect.tl().x, cartype, 30, cv::Scalar(255,255,0,255));
//        else
        if(item.type == ObjectType::OT_Vehicle)
        {
            drawCmdList->putText(item.targetRect.tl().y-20,  item.targetRect.tl().x, cv::format(" type: %d ",item.type), 30, cv::Scalar(0,0,255,255));
            drawCmdList->rectangle(
                    item.targetRect.tl().y,
                    item.targetRect.tl().x,
                    item.targetRect.br().x,
                    item.targetRect.br().y,
                    5,cv::Scalar(255,0,0,255)
            );
        }
        if(item.type == ObjectType::OT_Human)
        {
            drawCmdList->putText(item.targetRect.tl().y-20,  item.targetRect.tl().x, cv::format(" type: %d ",item.type), 30, cv::Scalar(0,0,255,255));
            drawCmdList->rectangle(
                    item.targetRect.tl().y,
                    item.targetRect.tl().x,
                    item.targetRect.br().x,
                    item.targetRect.br().y,
                    5,cv::Scalar(255,255,0,255)
            );
        }
    }
    return drawCmdList;
}

char *itoa(int src) {
    int temp = -1;
    int tv = src > 0 ? src : -src;
    int length = 0;
    while ((tv = tv / 10) > 0) {
        length++;
    }
    length++;
    tv = src > 0 ? src : -src;
    char *des = (char *) malloc(sizeof(char) * (length + 1));
    memset(des, 0, length + 1);
    for (int i = 0; i < length; i++) {
        int v = 1;
        for (int j = length - i; j > 1; j--) {
            v = v * 10;
        }
        temp = tv / (v);
        des[i] = (temp + 48);//accii码0对应48
        if (temp == 0) {
            temp = 1;
        }
        tv = tv % (temp * v);
    }
    des[length] = '\0';
    if (src < 0) {
        char *nSrc = (char *) malloc((strlen(des) + 2) * sizeof(char));
        sprintf(nSrc, "-%s", des);
        free(des);
        des = nSrc;
    }
    return des;
}