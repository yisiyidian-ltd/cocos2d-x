//
//  FileUtil.cpp
//  cocos2d_libs
//
//  Created by Joe on 2016/11/12.
//
//

#include "FileUtil.h"
#include "platform/CCFileUtils.h"

using namespace ccsp;

static FileUtil* s_instance=NULL;

FileUtil* FileUtil::getInstance(){
    if(!s_instance){
        s_instance=new FileUtil();
    }
    return s_instance;
}

bool FileUtil::copyFile(std::string &srcFullPath,std::string &dstFullPath)
{
    unsigned char* buf=0;
    unsigned int fileSize=0;
    auto fileData=cocos2d::FileUtils::getInstance()->getDataFromFile(srcFullPath);
    fileSize=(unsigned int)fileData.getSize();
    buf=(unsigned char*)malloc(fileSize);
    memcpy(buf,fileData.getBytes(),fileSize);
    
    auto hFile=fopen(dstFullPath.c_str(), "w+");
    if(!hFile){
        CCLOG("ccsp::FileUtil::copyFile fopen dstfile %s failed",dstFullPath.c_str());
        return false;
    }
    
    int writeSize=0;
    int onceWrite=1024*1000;
    if(fileSize<=onceWrite)
        fwrite(buf,1,fileSize,hFile);
    else{
        auto wsize=onceWrite;
        while (writeSize<fileSize) {
            wsize= wsize>fileSize-writeSize ? fileSize-writeSize : onceWrite;
            auto size=fwrite(&buf[writeSize],1,wsize,hFile);
            writeSize+=size;
            CCLOG("ccsp::FileUtil::copyFile write size %d total %d %d",(unsigned int)size,writeSize,fileSize);
        }
    }
    fclose(hFile);
    free(buf);
    CCLOG("ccsp::FileUtil::copyFile copy file to %s ok",dstFullPath.c_str());
    return  true;
}
