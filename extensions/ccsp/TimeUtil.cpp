//
//  Time.cpp
//  cocos2d_libs
//
//  Created by Joe on 2016/11/12.
//
//

#include "TimeUtil.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

using namespace ccsp;

static TimeUtil* s_instance=NULL;

TimeUtil* TimeUtil::getInstance(){
    if(!s_instance){
        s_instance=new TimeUtil();
    }
    return s_instance;
}

unsigned int TimeUtil::getTime(){
    return (unsigned int)time(NULL);
}

long long TimeUtil::getTimeMS(){
    timeval st={0};
    gettimeofday(&st, NULL);
    return st.tv_sec*1000+st.tv_usec/1000;
}

tm* TimeUtil::_getDate(time_t* second){
    tm* date;
    date=localtime(second);
    return date;
}

tm* TimeUtil::getDate(){
    time_t sec;
    sec=time(NULL);
    return _getDate(&sec);
}

tm* TimeUtil::getDateFromTime(unsigned int second){
    time_t sec;
    sec=(time_t) second;
    return _getDate(&sec);
}

int TimeUtil::getTimeStrLong(char* szBuf,int bufSize){
    tm* st;
    st=getDate();
    timeval st2={0};
    //get second and microseconds(1/1000000 second) in short us
    gettimeofday(&st2,NULL);
    return snprintf(szBuf, bufSize, "%04d-%02d-%02d %02d:%02d:%02d:%03d",
             st->tm_year+1900,st->tm_mon+1,st->tm_mday,st->tm_hour,st->tm_min,st->tm_sec,(unsigned int)(st2.tv_usec/1000));
};
