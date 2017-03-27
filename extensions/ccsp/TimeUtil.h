//
//  Time.hpp
//  cocos2d_libs
//
//  Created by Joe on 2016/11/12.
//
//

#ifndef Time_hpp
#define Time_hpp
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

namespace  ccsp{
    class TimeUtil{
    public:
        static TimeUtil* getInstance();
        unsigned int getTime();
        long long getTimeMS();
        tm* _getDate(time_t* second);
        tm* getDate();
        tm* getDateFromTime(unsigned int second);
        int getTimeStrLong(char* szBuf,int bufSize);
    };
}


#endif /* Time_hpp */
