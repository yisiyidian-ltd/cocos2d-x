//
//  jsb_cocos2dx_ccsp_manual.h
//  cocos2d_js_bindings
//
//  Created by Joe on 16/2/25.
//
//

#ifndef jsb_cocos2dx_ccsp_manual_h
#define jsb_cocos2dx_ccsp_manual_h


#include "jsapi.h"
#include "jsfriendapi.h"
#include "mozilla/Maybe.h"
#include "cocos/network/HttpClient.h"
#include "extensions/ccsp/LogUtil.h"
#include "extensions/ccsp/FileUtil.h"

//#include "cocos/network/CCDownloader.h"
//#include "renderer/CCTexture2D.h"
//#include "external/unzip/unzip.h"
/*
class LogMgr : cocos2d::Ref
{
public:
    static LogMgr* getInstance();
    void enableLogToFile(JSContext *cx, JS::HandleObject obj,bool enable);
    void setLogFileFullName(JSContext *cx, JS::HandleObject obj,std::string &logFileFullPath);
private:
    JSContext *_cx;
    mozilla::Maybe<JS::PersistentRootedObject> _obj;
};

 class HttpMgr : cocos2d::Ref
 {
 public:
 static HttpMgr* getInstance();
 void setConnectTimeOut(JSContext *cx, JS::HandleObject obj,int timeoutInSec);
 void setReadTimeOut(JSContext *cx, JS::HandleObject obj,int timeoutInSec);
 
 private:
 JSContext *_cx;
 mozilla::Maybe<JS::PersistentRootedObject> _obj;
 };

 class FileUtil : cocos2d::Ref
 {
 public:
 static FileUtil* getInstance();
 bool copyFile(JSContext *cx, JS::HandleObject obj,std::string &srcFullPath,std::string &dstFullPath);
 private:
 JSContext *_cx;
 mozilla::Maybe<JS::PersistentRootedObject> _obj;
 };
 
*/



void register_all_cocos2dx_ccsp_manual(JSContext* cx, JS::HandleObject global);



#endif /* jsb_cocos2dx_ccsp_manual_h */






