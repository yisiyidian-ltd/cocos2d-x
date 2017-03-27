//
//  jsb_cocos2dx_ccsp_manual.cpp
//  cocos2d_js_bindings
//
//  Created by Joe on 16/2/25.
//
//



#include "scripting/js-bindings/manual/extension/jsb_cocos2dx_ccsp_manual.h"
#include "extensions/cocos-ext.h"
#include "scripting/js-bindings/manual/ScriptingCore.h"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"
//#include "scripting/js-bindings/manual/jsb_cocos2dx_auto.hpp"
#include "scripting/js-bindings/auto/jsb_cocos2dx_auto.hpp"
//#include <thread>


USING_NS_CC;
USING_NS_CC_EXT;
//static FileUtil* s_FileMgr_instance=0;
//static LogMgr* s_LogMgr_instance=0;
//static HttpMgr* s_HttpMgr_instance=0;



/*
FileUtil* FileUtil::getInstance()
{
    if(!s_FileMgr_instance){
        s_FileMgr_instance=new (std::nothrow) FileUtil();
        s_FileMgr_instance->autorelease();
        s_FileMgr_instance->retain();
    }
    return s_FileMgr_instance;
}


bool FileUtil::copyFile(JSContext *cx, JS::HandleObject obj,std::string &srcFullPath,std::string &dstFullPath)
{
    unsigned char* buf=0;
    unsigned int fileSize=0;
    _cx=cx;
    _obj.construct(_cx, obj);
    auto fileData=FileUtils::getInstance()->getDataFromFile(srcFullPath);
    fileSize=(unsigned int)fileData.getSize();
    buf=(unsigned char*)malloc(fileSize);
    memcpy(buf,fileData.getBytes(),fileSize);
    
    auto hFile=fopen(dstFullPath.c_str(), "w+");
    if(!hFile){
        CCLOG("FileMgr::copyFile fopen dstfile %s failed",dstFullPath.c_str());
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
            CCLOG("FileMgr::copyFile write size %d total %d %d",(unsigned int)size,writeSize,fileSize);
        }
    }
    fclose(hFile);
    free(buf);
    CCLOG("FileMgr::copyFile copy file to %s ok",dstFullPath.c_str());
    return  true;
}


LogMgr* LogMgr::getInstance()
{
    if(!s_LogMgr_instance){
        s_LogMgr_instance=new (std::nothrow) LogMgr();
        s_LogMgr_instance->autorelease();
        s_LogMgr_instance->retain();
    }
    return s_LogMgr_instance;
}

void LogMgr::enableLogToFile(JSContext *cx, JS::HandleObject obj,bool enable)
{
    _cx=cx;
    _obj.construct(_cx, obj);
    ccsp::LogUtil::getInstance()->enableLogToFile(enable);
}

void LogMgr::setLogFileFullName(JSContext *cx, JS::HandleObject obj,std::string &fullName)
{
    _cx=cx;
    _obj.construct(_cx, obj);
    ccsp::LogUtil::getInstance()->setLogFileFullName(fullName.c_str());
}



HttpMgr* HttpMgr::getInstance()
{
    if(!s_HttpMgr_instance){
        s_HttpMgr_instance=new (std::nothrow) HttpMgr();
        s_HttpMgr_instance->autorelease();
        s_HttpMgr_instance->retain();
    }
    return s_HttpMgr_instance;
}

void HttpMgr::setConnectTimeOut(JSContext *cx, JS::HandleObject obj,int timeoutInSec)
{
    _cx=cx;
    _obj.construct(_cx, obj);
    cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(timeoutInSec);
    CCLOG("HttpMgr::setConnectTimeOut:%d",timeoutInSec);
}
void HttpMgr::setReadTimeOut(JSContext *cx, JS::HandleObject obj,int timeoutInSec)
{
    _cx=cx;
    _obj.construct(_cx, obj);
    cocos2d::network::HttpClient::getInstance()->setTimeoutForRead(timeoutInSec);
     CCLOG("HttpMgr::setReadTimeOut:%d",timeoutInSec);
}
*/

bool js_copy_file(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 2)
    {
        std::string srcFileName;
        std::string dstFileName;
        bool ok = jsval_to_std_string(cx, args.get(0), &srcFileName);
        JSB_PRECONDITION2(ok, cx, false, "js_copy_file : Error processing arguments");
        
        ok = jsval_to_std_string(cx, args.get(1), &dstFileName);
        JSB_PRECONDITION2(ok, cx, false, "js_copy_file : Error processing arguments");
        
        bool ret=ccsp::FileUtil::getInstance()->copyFile(srcFileName, dstFileName);
        jsval jsret = JSVAL_NULL;
        jsret = INT_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_copy_file : wrong number of arguments");
    return false;
}

bool js_enableLogToFile(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 1)
    {
        int bEnable;
        bool ok = jsval_to_int(cx, args.get(0), &bEnable);
        JSB_PRECONDITION2(ok, cx, false, "js_enableLogToFile : Error processing arguments");
        ccsp::LogUtil::getInstance()->enableLogToFile(bEnable);
        return true;
    }
    JS_ReportError(cx, "js_enableLogToFile : wrong number of arguments");
    return false;
}

bool js_setLogFileFullName(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 1)
    {
        std::string logFileName;
        bool ok = jsval_to_std_string(cx, args.get(0), &logFileName);
        JSB_PRECONDITION2(ok, cx, false, "js_setLogFileFullName : Error processing arguments");
        ccsp::LogUtil::getInstance()->setLogFileFullName(logFileName.c_str());
        return true;
    }
    JS_ReportError(cx, "js_setLogFileFullName : wrong number of arguments");
    return false;
}


bool js_setConnectTimeOut(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 1){
        int timeoutInSec;
        bool ok = jsval_to_int(cx, args.get(0), &timeoutInSec);
        JSB_PRECONDITION2(ok, cx, false, "js_setConnectTimeOut : Error processing arguments");
        //HttpMgr::getInstance()->setConnectTimeOut(cx, obj, timeout);
        cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(timeoutInSec);
        return  true;
    }
    JS_ReportError(cx, "js_setConnectTimeOut : wrong number of arguments");
    return  false;
}

bool js_setReadTimeOut(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 1){
        int timeoutInSec;
        bool ok = jsval_to_int(cx, args.get(0), &timeoutInSec);
        JSB_PRECONDITION2(ok, cx, false, "js_setReadTimeOut : Error processing arguments");
        //HttpMgr::getInstance()->setReadTimeOut(cx, obj, timeout);
        cocos2d::network::HttpClient::getInstance()->setTimeoutForRead(timeoutInSec);
        return true;
    }
    JS_ReportError(cx, "js_setReadTimeOut : wrong number of arguments");
    return  false;
}

void register_all_cocos2dx_ccsp_manual(JSContext* cx, JS::HandleObject global)
{
    JS::RootedObject jsbObj(cx);
    JS::RootedObject logUtilObj(cx);
    JS::RootedObject httpUtilObj(cx);
    JS::RootedObject fileUtilObj(cx);
    
    get_or_create_js_obj(cx, global, "jsb", &jsbObj);
    get_or_create_js_obj(cx, jsbObj, "fileUtil", &fileUtilObj);
    get_or_create_js_obj(cx, jsbObj, "logUtil", &logUtilObj);
    get_or_create_js_obj(cx, jsbObj, "httpUtil", &httpUtilObj);
    
    JS_DefineFunction(cx, fileUtilObj, "copyFile", js_copy_file, 2, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, logUtilObj, "enableLogToFile", js_enableLogToFile, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, logUtilObj, "setLogFileFullName", js_setLogFileFullName, 1, JSPROP_READONLY | JSPROP_PERMANENT);

    JS_DefineFunction(cx, httpUtilObj, "setConnectTimeOut", js_setConnectTimeOut, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, httpUtilObj, "setReadTimeOut", js_setReadTimeOut, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    
}
