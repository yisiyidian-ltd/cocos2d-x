//
//  LogUtil.cpp
//  cocos2d_libs
//
//  Created by Joe on 2016/11/12.
//
//

#include "LogUtil.h"
#include "TimeUtil.h"
#include "platform/CCFileUtils.h"

#define  CCSP_MAX_LOG_LENGTH 16*1024

using namespace ccsp;

static LogUtil* s_instance=NULL;

LogUtil* LogUtil::getInstance(){
    if(!s_instance){
        s_instance=new LogUtil();
    }
    return s_instance;
}

void LogUtil::logToFileWithTime(const char *szFmt, ...){
    va_list args;
    va_start(args, szFmt);
    //char szTimeBuf[128]={0};
    char* szLog;
    if(!_bEnable  || _strLogFileName.length()<=0)
        return;
    szLog=(char*)malloc(CCSP_MAX_LOG_LENGTH);
    memset(szLog,0,CCSP_MAX_LOG_LENGTH);
    
    TimeUtil::getInstance()->getTimeStrLong(szLog, CCSP_MAX_LOG_LENGTH);
    unsigned long len=strlen(szLog);
    szLog[len++]=0x20;
    vsnprintf(&szLog[len], CCSP_MAX_LOG_LENGTH-len, szFmt, args);
    cocos2d::FileUtils::getInstance()->appendStringToFile(szLog, _strLogFileName);
    free(szLog);
    va_end(args);
    return;
}
