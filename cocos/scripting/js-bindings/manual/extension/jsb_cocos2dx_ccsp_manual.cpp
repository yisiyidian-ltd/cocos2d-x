//
//  jsb_cocos2dx_ccsp_manual.cpp
//  cocos2d_js_bindings
//
//  Created by Joe on 16/2/25.
//
//



#include "jsb_cocos2dx_ccsp_manual.h"
#include "extensions/cocos-ext.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"
#include "jsb_cocos2dx_auto.hpp"
#include <thread>


USING_NS_CC;
USING_NS_CC_EXT;

static FileMgr* s_FileMgr_instance=0;
FileMgr* FileMgr::getInstance()
{
    if(!s_FileMgr_instance)
        s_FileMgr_instance=new (std::nothrow) FileMgr();
    s_FileMgr_instance->autorelease();
    s_FileMgr_instance->retain();
    return s_FileMgr_instance;
}


bool FileMgr::copyFile(JSContext *cx, JS::HandleObject obj,std::string &srcFullPath,std::string &dstFullPath)
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
        
        bool ret=FileMgr::getInstance()->copyFile(cx, obj, srcFileName, dstFileName);
        
        jsval jsret = JSVAL_NULL;
        jsret = INT_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_copy_file : wrong number of arguments");
    return false;
}


void register_all_cocos2dx_ccsp_manual(JSContext* cx, JS::HandleObject global)
{
    JS::RootedObject jsbObj(cx);
    JS::RootedObject fileMgrObj(cx);

    get_or_create_js_obj(cx, global, "jsb", &jsbObj);
    get_or_create_js_obj(cx, jsbObj, "fileMgr", &fileMgrObj);
    JS_DefineFunction(cx, fileMgrObj, "copyFile", js_copy_file, 2, JSPROP_READONLY | JSPROP_PERMANENT);
    
}