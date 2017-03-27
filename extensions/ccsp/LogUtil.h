//
//  LogUtil.hpp
//  cocos2d_libs
//
//  Created by Joe on 2016/11/12.
//
//

#ifndef LogUtil_hpp
#define LogUtil_hpp

#include <stdio.h>
#include <string>

namespace ccsp {
class LogUtil {
    public :
        
    static LogUtil* getInstance();
    void enableLogToFile(bool bEnable){_bEnable=bEnable;};
    bool isEnable(){return _bEnable;};
    void setLogFileFullName(const char* name){
        _strLogFileName=name;
    };
    //std::string getLogFileName(){return _strLogFileName;};
    void logToFileWithTime(const char* szFmt,...);
    
    
    private:
        bool _bEnable;
        std::string _strLogFileName;
};
    
}



#endif /* LogUtil_hpp */
