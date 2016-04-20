/*
 * Created by James Chen on 3/11/13.
 * Copyright (c) 2013-2014 Chukong Technologies Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __jsb_cocos2dx_extension_manual__
#define __jsb_cocos2dx_extension_manual__

#include "jsapi.h"
#include "jsfriendapi.h"
#include "network/CCDownloader.h"
#include "renderer/CCTexture2D.h"

class __JSDownloaderDelegator : cocos2d::Ref
{
public:
    void downloadAsync();
    void download();
    
    void downloadToFileAsync(std::string &savePath);
    void downloadToFile(std::string &savePath);
   
    
    static __JSDownloaderDelegator *create(JSContext *cx, JS::HandleObject obj, const std::string &url, JS::HandleObject callback,JS::HandleObject callbackOnProgress);

protected:
    __JSDownloaderDelegator(JSContext *cx, JS::HandleObject obj, const std::string &url, JS::HandleObject callback,JS::HandleObject callbackOnProgress);
    ~__JSDownloaderDelegator();
    
    void startDownload();
    void startDownloadToFile();
    
private:
    void onSuccess(cocos2d::Texture2D *tex);
    void onSuccessToFile();
    void onError();
    void onProgress( int64_t bytesReceived,int64_t totalBytesReceived,int64_t totalBytesExpected);
    std::shared_ptr<cocos2d::network::Downloader> _downloader;
    std::string _url;
    std::string _saveFileName;
    std::string _password;
    JSContext *_cx;
    JS::Heap<JSObject*> _jsCallback;
	JS::Heap<JSObject*> _jsCallbackOnProgress;
    JS::Heap<JSObject*> _obj;
};


class ZipMgr : cocos2d::Ref
{
public:
    static ZipMgr* getInstance();
    int unzip(JSContext *cx, JS::HandleObject obj,std::string &zipFileName,std::string &unzipPath,std::string &password,JS::HandleObject callback);
    int unzipAsync(JSContext *cx, JS::HandleObject obj,std::string &zipFileName,std::string &unzipPath,std::string &password);
    int getCurrentNum(){return _currentNum;};
    int getTotalNum(){return _totalNum;};
    
protected:
    ZipMgr();
    void onProgress();
    int _unzip();
    
private:
    JSContext *_cx;
    JS::Heap<JSObject*> _obj;
    JS::Heap<JSObject*> _jsCallback;

    std::string _fileName;
    std::string _unzipPath;
    std::string _password;
    int _totalNum;
    int _currentNum;
    bool _async;
};

void register_all_cocos2dx_extension_manual(JSContext* cx, JS::HandleObject global);

#endif /* defined(__jsb_cocos2dx_extension_manual__) */
