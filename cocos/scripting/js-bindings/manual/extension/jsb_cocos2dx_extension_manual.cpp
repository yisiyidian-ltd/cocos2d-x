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

#include "jsb_cocos2dx_extension_manual.h"
#include "extensions/cocos-ext.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"
#include "jsb_cocos2dx_auto.hpp"
#include <thread>

USING_NS_CC;
USING_NS_CC_EXT;


class JSB_ScrollViewDelegate
: public Ref
, public ScrollViewDelegate
{
public:
    JSB_ScrollViewDelegate()
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        _JSDelegate.construct(cx);
    }
    
    virtual ~JSB_ScrollViewDelegate()
    {
        _JSDelegate.destroyIfConstructed();
    }
    
    virtual void scrollViewDidScroll(ScrollView* view) override
    {
        js_proxy_t * p = jsb_get_native_proxy(view);
        if (!p) return;
        
        jsval arg = OBJECT_TO_JSVAL(p->obj);
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "scrollViewDidScroll", 1, &arg);
    }
    
    virtual void scrollViewDidZoom(ScrollView* view) override
    {
        js_proxy_t * p = jsb_get_native_proxy(view);
        if (!p) return;
        
        jsval arg = OBJECT_TO_JSVAL(p->obj);
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "scrollViewDidZoom", 1, &arg);
    }
    
    void setJSDelegate(JS::HandleObject pJSDelegate)
    {
        _JSDelegate.ref() = pJSDelegate;
    }
private:
    mozilla::Maybe<JS::PersistentRootedObject> _JSDelegate;
};

static bool js_cocos2dx_CCScrollView_setDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::ScrollView* cobj = (cocos2d::extension::ScrollView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    
    if (argc == 1)
    {
        // save the delegate
        JS::RootedObject jsDelegate(cx, args.get(0).toObjectOrNull());
        JSB_ScrollViewDelegate* nativeDelegate = new JSB_ScrollViewDelegate();
        nativeDelegate->setJSDelegate(jsDelegate);
        
        cobj->setUserObject(nativeDelegate);
        cobj->setDelegate(nativeDelegate);
        
        nativeDelegate->release();
        
        args.rval().setUndefined();
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}


#define KEY_TABLEVIEW_DATA_SOURCE  "TableViewDataSource"
#define KEY_TABLEVIEW_DELEGATE     "TableViewDelegate"

class JSB_TableViewDelegate
: public Ref
, public TableViewDelegate
{
public:
    JSB_TableViewDelegate()
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        _JSDelegate.construct(cx);
    }
    
    virtual ~JSB_TableViewDelegate()
    {
        _JSDelegate.destroyIfConstructed();
    }
    
    virtual void scrollViewDidScroll(ScrollView* view) override
    {
        callJSDelegate(view, "scrollViewDidScroll");
    }
    
    virtual void scrollViewDidZoom(ScrollView* view) override
    {
        callJSDelegate(view, "scrollViewDidZoom");
    }
    
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override
    {
        callJSDelegate(table, cell, "tableCellTouched");
    }
    
    virtual void tableCellHighlight(TableView* table, TableViewCell* cell) override
    {
        callJSDelegate(table, cell, "tableCellHighlight");
    }
    
    virtual void tableCellUnhighlight(TableView* table, TableViewCell* cell) override
    {
        callJSDelegate(table, cell, "tableCellUnhighlight");
    }
    
    virtual void tableCellWillRecycle(TableView* table, TableViewCell* cell) override
    {
        callJSDelegate(table, cell, "tableCellWillRecycle");
    }
    
    void setJSDelegate(JS::HandleObject pJSDelegate)
    {
        _JSDelegate.ref() = pJSDelegate;
    }
    
private:
    void callJSDelegate(ScrollView* view, std::string jsFunctionName)
    {
        js_proxy_t * p = jsb_get_native_proxy(view);
        if (!p) return;
        
        jsval arg = OBJECT_TO_JSVAL(p->obj);
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), jsFunctionName.c_str(), 1, &arg);
    }
    
    void callJSDelegate(TableView* table, TableViewCell* cell, std::string jsFunctionName)
    {
        js_proxy_t * p = jsb_get_native_proxy(table);
        if (!p) return;
        
        js_proxy_t * pCellProxy = jsb_get_native_proxy(cell);
        if (!pCellProxy) return;
        
        jsval args[2];
        args[0] = OBJECT_TO_JSVAL(p->obj);
        args[1] = OBJECT_TO_JSVAL(pCellProxy->obj);
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), jsFunctionName.c_str(), 2, args);
    }
    
    mozilla::Maybe<JS::PersistentRootedObject> _JSDelegate;
};

static bool js_cocos2dx_CCTableView_setDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::TableView* cobj = (cocos2d::extension::TableView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    
    if (argc == 1)
    {
        // save the delegate
        JS::RootedObject jsDelegate(cx, args.get(0).toObjectOrNull());
        JSB_TableViewDelegate* nativeDelegate = new JSB_TableViewDelegate();
        nativeDelegate->setJSDelegate(jsDelegate);
        
        __Dictionary* userDict = static_cast<__Dictionary*>(cobj->getUserObject());
        if (NULL == userDict)
        {
            userDict = new __Dictionary();
            cobj->setUserObject(userDict);
            userDict->release();
        }
        
        userDict->setObject(nativeDelegate, KEY_TABLEVIEW_DELEGATE);
        
        cobj->setDelegate(nativeDelegate);
        
        nativeDelegate->release();
        
        args.rval().setUndefined();
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

class JSB_TableViewDataSource
: public Ref
, public TableViewDataSource
{
public:
    JSB_TableViewDataSource()
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        _JSTableViewDataSource.construct(cx);
    }
    
    virtual ~JSB_TableViewDataSource()
    {
        _JSTableViewDataSource.destroyIfConstructed();
    }
    
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::RootedValue ret(cx);
        bool ok = callJSDelegate(table, idx, "tableCellSizeForIndex", &ret);
        if (!ok) 
        {
            ok = callJSDelegate(table, "cellSizeForTable", &ret);
        }
        if (ok) 
        {
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            
            Size size;
            bool isSucceed = jsval_to_ccsize(cx, ret, &size);
            if (isSucceed) return size;
        }
        return Size::ZERO;
        
    }
    
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::RootedValue ret(cx);
        bool ok = callJSDelegate(table, idx, "tableCellAtIndex", &ret);
        if (ok)
        {
            cocos2d::extension::TableViewCell* arg0;
            do {
                js_proxy_t *proxy;
                JS::RootedObject tmpObj(cx, ret.toObjectOrNull());
                proxy = jsb_get_js_proxy(tmpObj);
                arg0 = (cocos2d::extension::TableViewCell*)(proxy ? proxy->ptr : NULL);
                JSB_PRECONDITION2( arg0, cx, NULL, "Invalid Native Object");
            } while (0);
            return arg0;
        }
        return NULL;
    }
    
    virtual ssize_t numberOfCellsInTableView(TableView *table) override
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::RootedValue ret(cx);
        bool ok = callJSDelegate(table, "numberOfCellsInTableView", &ret);
        if (ok)
        {
            ssize_t count = 0;
            bool isSucceed = jsval_to_ssize(cx, ret, &count);
            if (isSucceed) return count;
        }
        return 0;
    }
    
    void setTableViewDataSource(JS::HandleObject pJSSource)
    {
        _JSTableViewDataSource.ref() = pJSSource;
    }
    
private:
    bool callJSDelegate(TableView* table, std::string jsFunctionName, JS::MutableHandleValue retVal)
    {
        js_proxy_t * p = jsb_get_native_proxy(table);
        if (!p) return false;
        
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        
        bool hasAction;
        JS::RootedValue temp_retval(cx);
        jsval dataVal = OBJECT_TO_JSVAL(p->obj);
        
        JS::RootedObject obj(cx, _JSTableViewDataSource.ref());
        JSAutoCompartment ac(cx, obj);
        
        if (JS_HasProperty(cx, obj, jsFunctionName.c_str(), &hasAction) && hasAction)
        {
            if(!JS_GetProperty(cx, obj, jsFunctionName.c_str(), &temp_retval))
            {
                return false;
            }
            if(temp_retval == JSVAL_VOID)
            {
                return false;
            }

            JS_CallFunctionName(cx, obj, jsFunctionName.c_str(),
                                JS::HandleValueArray::fromMarkedLocation(1, &dataVal), retVal);
            return true;
        }
        return false;
    }
    
    bool callJSDelegate(TableView* table, ssize_t idx, std::string jsFunctionName, JS::MutableHandleValue retVal)
    {
        js_proxy_t * p = jsb_get_native_proxy(table);
        if (!p) return false;
        
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        bool hasAction;
        JS::RootedValue temp_retval(cx);
        jsval dataVal[2];
        dataVal[0] = OBJECT_TO_JSVAL(p->obj);
        dataVal[1] = ssize_to_jsval(cx,idx);
        
        JS::RootedObject obj(cx, _JSTableViewDataSource.ref());
        JSAutoCompartment ac(cx, obj);
        
        if (JS_HasProperty(cx, obj, jsFunctionName.c_str(), &hasAction) && hasAction)
        {
            if(!JS_GetProperty(cx, obj, jsFunctionName.c_str(), &temp_retval))
            {
                return false;
            }
            
            if(temp_retval == JSVAL_VOID)
            {
                return false;
            }

            bool ret = JS_CallFunctionName(cx, obj, jsFunctionName.c_str(),
                JS::HandleValueArray::fromMarkedLocation(2, dataVal), retVal);
            return ret == true ? true : false;
        }
        return false;
    }
    
private:
    mozilla::Maybe<JS::PersistentRootedObject> _JSTableViewDataSource;
};

static bool js_cocos2dx_CCTableView_setDataSource(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::TableView* cobj = (cocos2d::extension::TableView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 1)
    {
        JSB_TableViewDataSource* pNativeSource = new JSB_TableViewDataSource();
        JS::RootedObject jsdata(cx, args.get(0).toObjectOrNull());
        pNativeSource->setTableViewDataSource(jsdata);
    
        __Dictionary* userDict = static_cast<__Dictionary*>(cobj->getUserObject());
        if (NULL == userDict)
        {
            userDict = new __Dictionary();
            cobj->setUserObject(userDict);
            userDict->release();
        }

        userDict->setObject(pNativeSource, KEY_TABLEVIEW_DATA_SOURCE);

        cobj->setDataSource(pNativeSource);
        
        pNativeSource->release();
        
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments");
    return false;
}

static bool js_cocos2dx_CCTableView_create(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    if (argc == 3 || argc == 2)
    {
        
        JSB_TableViewDataSource* pNativeSource = new JSB_TableViewDataSource();
        JS::RootedObject jsdata(cx, args.get(0).toObjectOrNull());
        pNativeSource->setTableViewDataSource(jsdata);
        
        cocos2d::Size arg1;
        ok &= jsval_to_ccsize(cx, args.get(1), &arg1);
        cocos2d::extension::TableView* ret = NULL;
        ret = new TableView();
        ret->autorelease();
        
        ret->setDataSource(pNativeSource);
        
        jsval jsret;
        do {
            if (ret)
            {
                js_proxy_t *proxy = js_get_or_create_proxy<cocos2d::extension::TableView>(cx, ret);
                jsret = OBJECT_TO_JSVAL(proxy->obj);
            } 
            else
            {
                jsret = JSVAL_NULL;
            }
        } while (0);
        
        if (argc == 2)
        {
            ret->initWithViewSize(arg1);
        }
        else
        {
            cocos2d::Node* arg2;
            do 
            {
                js_proxy_t *proxy;
                JS::RootedObject tmpObj(cx, args.get(2).toObjectOrNull());
                proxy = jsb_get_js_proxy(tmpObj);
                arg2 = (cocos2d::Node*)(proxy ? proxy->ptr : NULL);
                JSB_PRECONDITION2( arg2, cx, false, "Invalid Native Object");
            } while (0);
            JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
            ret->initWithViewSize(arg1, arg2);
        }
        ret->reloadData();
        
        __Dictionary* userDict = new __Dictionary();
        userDict->setObject(pNativeSource, KEY_TABLEVIEW_DATA_SOURCE);
        ret->setUserObject(userDict);
        userDict->release();
        
        pNativeSource->release();
        
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments");
    return false;
}

static bool js_cocos2dx_CCTableView_init(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::TableView* cobj = (cocos2d::extension::TableView *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_TableView_dequeueCell : Invalid Native Object");
    bool ok = true;
    if (argc == 3 || argc == 2)
    {
        
        JSB_TableViewDataSource* pNativeSource = new JSB_TableViewDataSource();
        JS::RootedObject jsdata(cx, args.get(0).toObjectOrNull());
        pNativeSource->setTableViewDataSource(jsdata);
        cobj->setDataSource(pNativeSource);

        cocos2d::Size arg1;
        ok &= jsval_to_ccsize(cx, args.get(1), &arg1);

        if (argc == 2)
        {
            cobj->initWithViewSize(arg1);
        }
        else
        {
            cocos2d::Node* arg2;
            do 
            {
                JS::RootedObject tmpObj(cx, args.get(2).toObjectOrNull());
                proxy = jsb_get_js_proxy(tmpObj);
                arg2 = (cocos2d::Node*)(proxy ? proxy->ptr : NULL);
                JSB_PRECONDITION2( arg2, cx, false, "Invalid Native Object");
            } while (0);
            JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
            cobj->initWithViewSize(arg1, arg2);
        }
        cobj->reloadData();
        
        __Dictionary* userDict = new __Dictionary();
        userDict->setObject(pNativeSource, KEY_TABLEVIEW_DATA_SOURCE);
        cobj->setUserObject(userDict);
        userDict->release();
        
        pNativeSource->release();
        
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments");
    return false;
}



class JSB_ControlButtonTarget : public Ref
{
public:
    JSB_ControlButtonTarget()
    : _callback(nullptr),
      _type(Control::EventType::TOUCH_DOWN)
    {
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        _jsFunc.construct(cx);
    }
    
    virtual ~JSB_ControlButtonTarget()
    {
        CCLOGINFO("In the destruction of JSB_ControlButtonTarget ...");
        _jsFunc.destroyIfConstructed();
        
        if (_callback != nullptr)
        {
            CC_SAFE_DELETE(_callback);
        }

        for (auto iter = _jsNativeTargetMap.begin(); iter != _jsNativeTargetMap.end(); ++iter)
        {
            if (this == iter->second)
            {
                _jsNativeTargetMap.erase(iter);
                break;
            }
        }
    }
    
    virtual void onEvent(Ref *controlButton, Control::EventType event)
    {
        js_proxy_t * p;
        JS_GET_PROXY(p, controlButton);
        if (!p)
        {
            log("Failed to get proxy for control button");
            return;
        }
        
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        
        jsval dataVal[2];
        dataVal[0] = OBJECT_TO_JSVAL(p->obj);
        int arg1 = (int)event;
        dataVal[1] = INT_TO_JSVAL(arg1);
        JS::RootedValue jsRet(cx);
        
        _callback->invoke(2, dataVal, &jsRet);
    }
    
    void setJSCallback(JS::HandleValue jsFunc, JS::HandleObject jsTarget)
    {
        if (_callback != nullptr)
        {
            CC_SAFE_DELETE(_callback);
        }
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        _callback = new JSFunctionWrapper(cx, jsTarget, jsFunc);
        _jsFunc.ref() = jsFunc.toObjectOrNull();
    }
    
    void setEventType(Control::EventType type)
    {
        _type = type;
    }
public:
    
    static std::multimap<JSObject*, JSB_ControlButtonTarget*> _jsNativeTargetMap;
    JSFunctionWrapper *_callback;
    Control::EventType _type;
    mozilla::Maybe<JS::PersistentRootedObject> _jsFunc;
};

std::multimap<JSObject*, JSB_ControlButtonTarget*> JSB_ControlButtonTarget::_jsNativeTargetMap;

static bool js_cocos2dx_CCControl_addTargetWithActionForControlEvents(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::Control* cobj = (cocos2d::extension::Control *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    
    bool ok = true;
    if (argc == 3)
    {
        JSObject* jsDelegate = args.get(0).toObjectOrNull();
        JSObject* jsFunc = args.get(1).toObjectOrNull();
        Control::EventType arg2;
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "Error processing control event");
        
        // Check whether the target already exists.
        auto range = JSB_ControlButtonTarget::_jsNativeTargetMap.equal_range(jsDelegate);
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second->_jsFunc.ref().get() == jsFunc && arg2 == it->second->_type)
            {
                // Return true directly.
                args.rval().setUndefined();
                return true;
            }
        }
        
        // save the delegate
        JSB_ControlButtonTarget* nativeDelegate = new JSB_ControlButtonTarget();
        
        JS::RootedObject jscb(cx, jsDelegate);
        nativeDelegate->setJSCallback(args.get(1), jscb);
        nativeDelegate->setEventType(arg2);

        __Array* nativeDelegateArray = static_cast<__Array*>(cobj->getUserObject());
        if (nullptr == nativeDelegateArray)
        {
            nativeDelegateArray = new __Array();
            nativeDelegateArray->init();
            cobj->setUserObject(nativeDelegateArray);  // The reference of nativeDelegateArray is added to 2
            nativeDelegateArray->release(); // Release nativeDelegateArray to make the reference to 1
        }
        
        nativeDelegateArray->addObject(nativeDelegate); // The reference of nativeDelegate is added to 2
        nativeDelegate->release(); // Release nativeDelegate to make the reference to 1
        
        cobj->addTargetWithActionForControlEvents(nativeDelegate, cccontrol_selector(JSB_ControlButtonTarget::onEvent), arg2);
        
        JSB_ControlButtonTarget::_jsNativeTargetMap.insert(std::make_pair(jsDelegate, nativeDelegate));
        
        args.rval().setUndefined();
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}

static bool js_cocos2dx_CCControl_removeTargetWithActionForControlEvents(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::Control* cobj = (cocos2d::extension::Control *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    
    bool ok = true;
    if (argc == 3)
    {
        Control::EventType arg2;
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "Error processing control event");
        
        obj = args.get(0).toObjectOrNull();
        JSObject* jsFunc = args.get(1).toObjectOrNull();
        
        JSB_ControlButtonTarget* nativeTargetToRemoved = nullptr;
        
        auto range = JSB_ControlButtonTarget::_jsNativeTargetMap.equal_range(obj);
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second->_jsFunc.ref().get() == jsFunc && arg2 == it->second->_type)
            {
                nativeTargetToRemoved = it->second;
                JSB_ControlButtonTarget::_jsNativeTargetMap.erase(it);
                break;
            }
        }
        
        cobj->removeTargetWithActionForControlEvents(nativeTargetToRemoved, cccontrol_selector(JSB_ControlButtonTarget::onEvent), arg2);

        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}

/*
static bool js_cocos2dx_ext_AssetsManager_updateAssets(JSContext *cx, uint32_t argc, jsval *vp)
{
    jsval *argv = JS_ARGV(cx, vp);
    bool ok = true;
    JS::RootedObject obj(cx, JS_THIS_OBJECT(cx, vp));
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::AssetsManager* cobj = (cocos2d::extension::AssetsManager *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_AssetsManager_updateAssets : Invalid Native Object");
    if (argc == 1) {
        std::unordered_map<std::string, Downloader::DownloadUnit> dict;
        do {
            if (!argv[0].isObject()) { ok = false; break; }
            JS::RootedObject tmpObj(cx, JSVAL_TO_OBJECT(argv[0]));
            
            if (!tmpObj) {
                CCLOG("%s", "jsval_to_ccvaluemap: the jsval is not an object.");
                return false;
            }
            
            JSObject* it = JS_NewPropertyIterator(cx, tmpObj);
            while (true)
            {
                jsid idp;
                jsval key;
                if (! JS_NextProperty(cx, it, &idp) || ! JS_IdToValue(cx, idp, &key)) {
                    return false; // error
                }
                
                if (key == JSVAL_VOID) {
                    break; // end of iteration
                }
                
                if (!JSVAL_IS_STRING(key)) {
                    continue; // ignore integer properties
                }
                
                JSStringWrapper keyWrapper(JSVAL_TO_STRING(key), cx);
                std::string keystr = keyWrapper.get();
                
                JS::RootedValue value(cx);
                JS_GetPropertyById(cx, tmpObj, idp, &value);
                
                JS::RootedObject tmp(cx);
                JS::RootedValue jsSrcUrl(cx);
                JS::RootedValue jsStoragePath(cx);
                JS::RootedValue jsCustomId(cx);
                ok = value.isObject() &&
                JS_ValueToObject(cx, JS::RootedValue(cx, value), &tmp) &&
                JS_GetProperty(cx, tmp, "srcUrl", &jsSrcUrl) &&
                JS_GetProperty(cx, tmp, "storagePath", &jsStoragePath) &&
                JS_GetProperty(cx, tmp, "customId", &jsCustomId);
                JSB_PRECONDITION3(ok, cx, false, "Error parsing map entry");
                
                Downloader::DownloadUnit unit;
                
                JSString *jsstr = JS::ToString(cx, jsSrcUrl);
                JSB_PRECONDITION3(jsstr, cx, false, "Error processing srcUrl value of entry: %s", keystr);
                JSStringWrapper srcUrlStr(jsstr);
                unit.srcUrl = srcUrlStr.get();
                
                jsstr = JS::ToString(cx, jsStoragePath);
                JSB_PRECONDITION3(jsstr, cx, false, "Error processing storagePath value of entry: %s", keystr);
                JSStringWrapper storagePathStr(jsstr);
                unit.storagePath = storagePathStr.get();
                
                jsstr = JS::ToString(cx, jsCustomId);
                JSB_PRECONDITION3(jsstr, cx, false, "Error processing customId value of entry: %s", keystr);
                JSStringWrapper customIdStr(jsstr);
                unit.customId = customIdStr.get();
                
                dict[keystr] = unit;
            }
        } while (0);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_AssetsManager_updateAssets : Error processing arguments");
        cobj->updateAssets(dict);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_AssetsManager_updateAssets : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_ext_AssetsManager_getFailedAssets(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::RootedObject obj(cx, JS_THIS_OBJECT(cx, vp));
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::extension::AssetsManager* cobj = (cocos2d::extension::AssetsManager *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_AssetsManager_getFailedAssets : Invalid Native Object");
    if (argc == 0) {
        const std::unordered_map<std::string, Downloader::DownloadUnit> &ret = cobj->getFailedAssets();
        jsval jsret = JSVAL_NULL;
        do {
            JSObject* jsRet = JS_NewObject(cx, NULL, NULL, NULL);
            
            for (auto it = ret.cbegin(); it != ret.cend(); ++it) {
                std::string key = it->first;
                const Downloader::DownloadUnit& unit = it->second;
                
                JS::RootedObject elem(cx, JS_NewObject(cx, NULL, NULL, NULL));
                if (!elem)
                {
                    JS_ReportError(cx, "js_cocos2dx_extension_AssetsManager_getFailedAssets : can not create js object");
                    break;
                }
                bool ok = JS_DefineProperty(cx, elem, "srcUrl", std_string_to_jsval(cx, unit.srcUrl), NULL, NULL, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, elem, "storagePath", std_string_to_jsval(cx, unit.storagePath), NULL, NULL, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, elem, "customId", std_string_to_jsval(cx, unit.customId), NULL, NULL, JSPROP_ENUMERATE | JSPROP_PERMANENT);
                JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_AssetsManager_getFailedAssets : Error processing DownloadUnit struct");
                
                if (!key.empty())
                {
                    JS::RootedValue dictElement(cx);
                    dictElement = OBJECT_TO_JSVAL(elem);
                    JS_SetProperty(cx, jsRet, key.c_str(), dictElement);
                }
            }
        } while (0);
        JS_SET_RVAL(cx, vp, jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_AssetsManager_getFailedAssets : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
*/

static ZipMgr* s_ZipMgr_instance=0;
ZipMgr* ZipMgr::getInstance()
{
    if(!s_ZipMgr_instance)
        s_ZipMgr_instance=new (std::nothrow) ZipMgr();
    s_ZipMgr_instance->autorelease();
    s_ZipMgr_instance->retain();
    return s_ZipMgr_instance;
}


ZipMgr::ZipMgr()
{
}

int ZipMgr::unzip(JSContext *cx, JS::HandleObject obj,std::string &zipFileName,std::string &unzipPath,std::string &password,JS::HandleObject callback){
    _cx=cx;
    _obj.construct(_cx, obj);
    _jsCallback.construct(_cx, callback);
    
    _fileName=zipFileName;
    _unzipPath=unzipPath;
    _password=password;
    _totalNum=0;
    _currentNum=0;
    _async=false;
    
    return _unzip();
}

int ZipMgr::unzipAsync(JSContext *cx, JS::HandleObject obj,std::string &zipFileName,std::string &unzipPath,std::string &password){
    _cx=cx;
    _obj.construct(_cx, obj);
    
    _fileName=zipFileName;
    _unzipPath=unzipPath;
    _password=password;
    _totalNum=0;
    _currentNum=0;
    _async=true;
    
    unz_global_info globalInfo = {0};
    std::string fullFileName=FileUtils::getInstance()->fullPathForFilename(_fileName);
    unzFile _unzFile = cocos2d::unzOpen(fullFileName.c_str());
    if (_unzFile){
        unzGetGlobalInfo(_unzFile, &globalInfo);
        unzCloseCurrentFile(_unzFile);
    }
    
    auto t = std::thread(&ZipMgr::_unzip, this);
    t.detach();
    
    _totalNum=globalInfo.number_entry;
    return globalInfo.number_entry;
}

int ZipMgr::_unzip(){
    bool success = true;
    unsigned char buffer[4096] = {0};
    char fileNameBuf[1024]={0};
    int ret=0;
    unz_global_info globalInfo = {0};
    
    std::string fullFileName=FileUtils::getInstance()->fullPathForFilename(_fileName);
    unzFile _unzFile = cocos2d::unzOpen(fullFileName.c_str());
    if (!_unzFile){
        CCLOG("startUnzipToPath:unzOpen %s failed",fullFileName.c_str());
        success=false;
        return 0;
    }
    
    if (unzGetGlobalInfo(_unzFile, &globalInfo) != UNZ_OK) {
        CCLOG("startUnzipToPath:unzGetGlobalInfo %s failed",fullFileName.c_str());
        success=false;
        return 0;
    }
    
    CCLOG("zipfile %s have entry %lu, unzip to %s",_fileName.c_str(),globalInfo.number_entry,_unzipPath.c_str());
    _totalNum=globalInfo.number_entry;
    
    unzGoToFirstFile(_unzFile);
    do {
        if (_password.size() == 0)
            ret = unzOpenCurrentFile(_unzFile);
        else
            ret = unzOpenCurrentFilePassword(_unzFile, _password.c_str());
        if (ret != UNZ_OK) {
            if(!_password.size())
                CCLOG("startUnzipToPath:unzOpenCurrentFile failed");
            else
                CCLOG("startUnzipToPath:unzOpenCurrentFilePassword failed");
            success = false;
            break;
        }
        // reading data and write to file
        int read ;
        unz_file_info fileInfo = {0};
        
        memset(fileNameBuf, 0, sizeof(fileNameBuf));
        unzGetCurrentFileInfo(_unzFile, &fileInfo, fileNameBuf, sizeof(fileNameBuf), nullptr, 0, nullptr, 0);
        
        std::string fullPath = _unzipPath + StringUtils::format("%s",fileNameBuf);
        
        int fileLen=strlen(fileNameBuf);
        if (fileNameBuf[fileLen - 1] == '/' || fileNameBuf[fileLen - 1] == '\\')
            FileUtils::getInstance()->createDirectory(fullPath.c_str());
        else
        {
            FILE* fp = fopen(fullPath.c_str(), "wb");
            if(!fp){
                success=false;
                break;
            }
            while (true) {
                read = unzReadCurrentFile(_unzFile, buffer, 4096);
                if (read > 0) {
                    fwrite(buffer, read, 1, fp);
                }
                else
                    break;
            }
            fclose(fp);
        }
        unzCloseCurrentFile(_unzFile);
        ret = unzGoToNextFile(_unzFile);
        _currentNum++;
        if(!_async){
            onProgress();
        }
    } while (ret == UNZ_OK);
    return  _totalNum;
}


void ZipMgr::onProgress(){
    jsval valArr[2];
    valArr[0]=INT_TO_JSVAL(_currentNum);
    valArr[1]=INT_TO_JSVAL(_totalNum);
    
    JS::RootedObject global(_cx, ScriptingCore::getInstance()->getGlobalObject());
    JSAutoCompartment ac(_cx, global);
    
    JS::RootedValue callback(_cx, OBJECT_TO_JSVAL(_jsCallback.ref()));
    if (!callback.isNull())
    {
        JS::RootedValue retval(_cx);
        JS_CallFunctionValue(_cx, global, callback, JS::HandleValueArray::fromMarkedLocation(2, valArr), &retval);
    }
}




__JSDownloaderDelegator::__JSDownloaderDelegator(JSContext *cx, JS::HandleObject obj, const std::string &url, JS::HandleObject callback,JS::HandleObject callbackOnProgress)
: _cx(cx)
, _url(url)
{
    _obj.construct(_cx, obj);
    _jsCallback.construct(_cx, callback);
    _jsCallbackOnProgress.construct(_cx,callbackOnProgress);
}

__JSDownloaderDelegator::~__JSDownloaderDelegator()
{
    _obj.destroyIfConstructed();
    _jsCallback.destroyIfConstructed();
    if(_downloader){
        _downloader->onTaskError = (nullptr);
        _downloader->onDataTaskSuccess = (nullptr);
        _downloader->onFileTaskSuccess = (nullptr);
        _downloader->onTaskProgress=(nullptr);
    }
}

__JSDownloaderDelegator *__JSDownloaderDelegator::create(JSContext *cx, JS::HandleObject obj, const std::string &url, JS::HandleObject callback,JS::HandleObject callbackOnProgress)
{
    __JSDownloaderDelegator *delegate = new (std::nothrow) __JSDownloaderDelegator(cx, obj, url, callback,callbackOnProgress);
    delegate->autorelease();
    return delegate;
}

void __JSDownloaderDelegator::startDownload()
{
    if (auto texture = Director::getInstance()->getTextureCache()->getTextureForKey(_url))
    {
        onSuccess(texture);
    }
    else
    {
        _downloader = std::make_shared<cocos2d::network::Downloader>();
//        _downloader->setConnectionTimeout(8);
        _downloader->onTaskError = [this](const cocos2d::network::DownloadTask& task,
                                          int errorCode,
                                          int errorCodeInternal,
                                          const std::string& errorStr)
        {
            this->onError();
        };
        
        _downloader->onDataTaskSuccess = [this](const cocos2d::network::DownloadTask& task,
                                                std::vector<unsigned char>& data)
        {
            Image img;
            Texture2D *tex = nullptr;
            do
            {
                if (false == img.initWithImageData(data.data(), data.size()))
                {
                    break;
                }
                tex = Director::getInstance()->getTextureCache()->addImage(&img, _url);
            } while (0);
            if (tex)
            {
                this->onSuccess(tex);
            }
            else
            {
                this->onError();
            }
        };
        
        _downloader->createDownloadDataTask(_url);
    }
}


void __JSDownloaderDelegator::startDownloadToFile()
{
    _downloader = std::make_shared<cocos2d::network::Downloader>();
    //        _downloader->setConnectionTimeout(8);
    
    _downloader->onTaskProgress = [this](const cocos2d::network::DownloadTask& task,
                                         int64_t bytesReceived,
                                         int64_t totalBytesReceived,
                                         int64_t totalBytesExpected)
    {
        //CCLOG("__JSDownloaderDelegator:download file %s error",_url.c_str());
        this->onProgress(bytesReceived,totalBytesReceived,totalBytesExpected);
    };
    
    _downloader->onTaskError = [this](const cocos2d::network::DownloadTask& task,
                                      int errorCode,
                                      int errorCodeInternal,
                                      const std::string& errorStr)
    {
        CCLOG("__JSDownloaderDelegator:download file %s error",_url.c_str());
        this->onError();
    };
    
    _downloader->onFileTaskSuccess = [this](const cocos2d::network::DownloadTask& task)
    {
        CCLOG("__JSDownloaderDelegator:download file %s ok",_url.c_str());
        this->onSuccessToFile();
    };

    _downloader->createDownloadFileTask(_url,_saveFileName);
}


void __JSDownloaderDelegator::download()
{
    retain();
    startDownload();
}

void __JSDownloaderDelegator::downloadAsync()
{
    retain();
    auto t = std::thread(&__JSDownloaderDelegator::startDownload, this);
    t.detach();
}

void __JSDownloaderDelegator::downloadToFile(std::string &saveFileName)
{
    retain();
    _saveFileName=saveFileName;
    startDownloadToFile();
}

void __JSDownloaderDelegator::downloadToFileAsync(std::string &saveFileName)
{
    retain();
    _saveFileName=saveFileName;
    auto t = std::thread(&__JSDownloaderDelegator::startDownloadToFile, this);
    t.detach();
}

void __JSDownloaderDelegator::onError()
{
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([this]
    {
        JS::RootedValue callback(_cx, OBJECT_TO_JSVAL(_jsCallback.ref()));
        if (!callback.isNull()) {
            JS::RootedObject global(_cx, ScriptingCore::getInstance()->getGlobalObject());
            JSAutoCompartment ac(_cx, global);
            
            jsval succeed = BOOLEAN_TO_JSVAL(false);
            JS::RootedValue retval(_cx);
            JS_CallFunctionValue(_cx, global, callback, JS::HandleValueArray::fromMarkedLocation(1, &succeed), &retval);
        }
        release();
    });
}

void __JSDownloaderDelegator::onSuccess(Texture2D *tex)
{
    CCASSERT(tex, "__JSDownloaderDelegator::onSuccess must make sure tex not null!");
    //Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, tex]
    {
        JS::RootedObject global(_cx, ScriptingCore::getInstance()->getGlobalObject());
        JSAutoCompartment ac(_cx, global);
        
        jsval valArr[2];
        if (tex)
        {
            valArr[0] = BOOLEAN_TO_JSVAL(true);
            js_proxy_t* p = jsb_get_native_proxy(tex);
            if (!p)
            {
                JS::RootedObject texProto(_cx, jsb_cocos2d_Texture2D_prototype);
                JS::RootedObject obj(_cx, JS_NewObject(_cx, jsb_cocos2d_Texture2D_class, texProto, global));
                // link the native object with the javascript object
                p = jsb_new_proxy(tex, obj);
                JS::AddNamedObjectRoot(_cx, &p->obj, "cocos2d::Texture2D");
            }
            valArr[1] = OBJECT_TO_JSVAL(p->obj);
        }
        else
        {
            valArr[0] = BOOLEAN_TO_JSVAL(false);
            valArr[1] = JSVAL_NULL;
        }
    
        JS::RootedValue callback(_cx, OBJECT_TO_JSVAL(_jsCallback.ref()));
        if (!callback.isNull())
        {
            JS::RootedValue retval(_cx);
            JS_CallFunctionValue(_cx, global, callback, JS::HandleValueArray::fromMarkedLocation(2, valArr), &retval);
        }
        release();
    }//);
}

void __JSDownloaderDelegator::onProgress(int64_t bytesReceived,int64_t totalBytesReceived,int64_t totalBytesExpected)
{
    JS::RootedObject global(_cx, ScriptingCore::getInstance()->getGlobalObject());
    JSAutoCompartment ac(_cx, global);
    
    JS::RootedValue callbackOnProgress(_cx, OBJECT_TO_JSVAL(_jsCallbackOnProgress.ref()));
    if (!callbackOnProgress.isNull())
    {
        jsval retArray[3];
        retArray[0]=long_long_to_jsval(_cx, bytesReceived);
        retArray[1]=long_long_to_jsval(_cx, totalBytesReceived);
        retArray[2]=long_long_to_jsval(_cx, totalBytesExpected);
        JS::RootedValue retval(_cx);
        JS_CallFunctionValue(_cx, global, callbackOnProgress, JS::HandleValueArray::fromMarkedLocation(3, retArray), &retval);
    }
}

void __JSDownloaderDelegator::onSuccessToFile()
{
        JS::RootedObject global(_cx, ScriptingCore::getInstance()->getGlobalObject());
        JSAutoCompartment ac(_cx, global);
        
        JS::RootedValue callback(_cx, OBJECT_TO_JSVAL(_jsCallback.ref()));
        if (!callback.isNull())
        {
            jsval success=BOOLEAN_TO_JSVAL(true);
            JS::RootedValue retval(_cx);
            JS_CallFunctionValue(_cx, global, callback, JS::HandleValueArray::fromMarkedLocation(1, &success), &retval);
        }
        release();
}

// jsb.loadRemoteImg(url, function(succeed, result) {})
bool js_load_remote_image(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc >= 2)
    {
        std::string url;
        bool ok = jsval_to_std_string(cx, args.get(0), &url);
        JSB_PRECONDITION2(ok, cx, false, "js_load_remote_image : Error processing arguments");
        JS::RootedObject callback(cx, args.get(1).toObjectOrNull());
        JS::RootedObject callbackOnProgress(cx, args.get(2).toObjectOrNull());
        
        __JSDownloaderDelegator *delegate = __JSDownloaderDelegator::create(cx, obj, url, callback,callbackOnProgress);
        delegate->downloadAsync();
        
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "js_load_remote_image : wrong number of arguments");
    return false;
}

//add by flyingkisser
// jsb.downloadToFileAsync(url,savePath,function(success){},function(recv,totalRecv,total) {})
bool js_download_to_file_async(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc >= 3)
    {
        std::string url;
        std::string savePath;
        bool ok = jsval_to_std_string(cx, args.get(0), &url);
        JSB_PRECONDITION2(ok, cx, false, "js_download_to_file_async : Error processing arguments");
        
        ok = jsval_to_std_string(cx, args.get(1), &savePath);
        JSB_PRECONDITION2(ok, cx, false, "js_download_to_file_async : Error processing arguments");

        
        JS::RootedObject callback(cx, args.get(2).toObjectOrNull());
        JS::RootedObject callbackOnProgress(cx, args.get(3).toObjectOrNull());
        
        __JSDownloaderDelegator *delegate = __JSDownloaderDelegator::create(cx, obj, url, callback,callbackOnProgress);
        delegate->downloadToFileAsync(savePath);
        
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "js_download_to_file_async : wrong number of arguments");
    return false;
}


//jsb.unzipToPath(zipFileName,pathName,password,function(totalUnzipped,total){});
bool js_unzip_to_path(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 4)
    {
        std::string zipFileName;
        std::string unzipPath;
        std::string password;
        bool ok = jsval_to_std_string(cx, args.get(0), &zipFileName);
        JSB_PRECONDITION2(ok, cx, false, "js_unzip_to_path : Error processing arguments");
        
        ok = jsval_to_std_string(cx, args.get(1), &unzipPath);
        JSB_PRECONDITION2(ok, cx, false, "js_unzip_to_path : Error processing arguments");
        
        ok = jsval_to_std_string(cx, args.get(2), &password);
        JSB_PRECONDITION2(ok, cx, false, "js_unzip_to_path : Error processing arguments");
        
        JS::RootedObject callback(cx, args.get(3).toObjectOrNull());
        
        int ret=ZipMgr::getInstance()->unzip(cx,obj,zipFileName,unzipPath,password,callback);
        jsval jsret = JSVAL_NULL;
        jsret = INT_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_unzip_to_path : wrong number of arguments");
    return false;
}

//jsb.unzipToPathAsync(zipFileName,pathName,"");
bool js_unzip_to_path_async(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    if (argc == 3)
    {
        std::string zipFileName;
        std::string unzipPath;
        std::string password;
        bool ok = jsval_to_std_string(cx, args.get(0), &zipFileName);
        JSB_PRECONDITION2(ok, cx, false, "js_unzip_to_path : Error processing arguments");
        
        ok = jsval_to_std_string(cx, args.get(1), &unzipPath);
        JSB_PRECONDITION2(ok, cx, false, "js_unzip_to_path : Error processing arguments");
        
        ok = jsval_to_std_string(cx, args.get(2), &password);
        JSB_PRECONDITION2(ok, cx, false, "js_unzip_to_path : Error processing arguments");
        
        int ret=ZipMgr::getInstance()->unzipAsync(cx,obj,zipFileName,unzipPath,password);
        
        jsval jsret = JSVAL_NULL;
        jsret = INT_TO_JSVAL(ret);
        args.rval().set(jsret);
        
        return true;
    }
    
    JS_ReportError(cx, "js_unzip_to_path : wrong number of arguments");
    return false;
}

bool js_unzip_get_progress(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    //JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    int ret=ZipMgr::getInstance()->getCurrentNum();
    jsval jsret = JSVAL_NULL;
    jsret = INT_TO_JSVAL(ret);
    args.rval().set(jsret);
    return  true;
}
bool js_unzip_get_total(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    //JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    int ret=ZipMgr::getInstance()->getTotalNum();
    jsval jsret = JSVAL_NULL;
    jsret = INT_TO_JSVAL(ret);
    args.rval().set(jsret);
    return  true;
}


extern JSObject* jsb_cocos2d_extension_ScrollView_prototype;
extern JSObject* jsb_cocos2d_extension_TableView_prototype;
extern JSObject* jsb_cocos2d_extension_Control_prototype;
extern JSObject* jsb_cocos2d_extension_AssetsManagerEx_prototype;
extern JSObject* jsb_cocos2d_extension_Manifest_prototype;

void register_all_cocos2dx_extension_manual(JSContext* cx, JS::HandleObject global)
{
    JS::RootedObject ccObj(cx);
    JS::RootedValue tmpVal(cx);
    JS::RootedObject tmpObj(cx);
    get_or_create_js_obj(cx, global, "cc", &ccObj);
    
    tmpObj.set(jsb_cocos2d_extension_AssetsManagerEx_prototype);
    JS_DefineFunction(cx, tmpObj, "retain", js_cocos2dx_retain, 0, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, tmpObj, "release", js_cocos2dx_release, 0, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    tmpObj.set(jsb_cocos2d_extension_Manifest_prototype);
    JS_DefineFunction(cx, tmpObj, "retain", js_cocos2dx_retain, 0, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, tmpObj, "release", js_cocos2dx_release, 0, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    //JS_DefineFunction(cx, jsb_cocos2d_extension_AssetsManager_prototype, "updateAssets", js_cocos2dx_ext_AssetsManager_updateAssets, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    //JS_DefineFunction(cx, jsb_cocos2d_extension_AssetsManager_prototype, "getFailedAssets", js_cocos2dx_ext_AssetsManager_getFailedAssets, 0, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    tmpObj.set(jsb_cocos2d_extension_ScrollView_prototype);
    JS_DefineFunction(cx, tmpObj, "setDelegate", js_cocos2dx_CCScrollView_setDelegate, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS::RootedObject tableview(cx, jsb_cocos2d_extension_TableView_prototype);
    JS_DefineFunction(cx, tableview, "setDelegate", js_cocos2dx_CCTableView_setDelegate, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, tableview, "setDataSource", js_cocos2dx_CCTableView_setDataSource, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, tableview, "_init", js_cocos2dx_CCTableView_init, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS::RootedObject control(cx, jsb_cocos2d_extension_Control_prototype);
    JS_DefineFunction(cx, control, "addTargetWithActionForControlEvents", js_cocos2dx_CCControl_addTargetWithActionForControlEvents, 3, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, control, "removeTargetWithActionForControlEvents", js_cocos2dx_CCControl_removeTargetWithActionForControlEvents, 3, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    JS_GetProperty(cx, ccObj, "TableView", &tmpVal);
    tmpObj.set(tmpVal.toObjectOrNull());
    JS_DefineFunction(cx, tmpObj, "create", js_cocos2dx_CCTableView_create, 3, JSPROP_READONLY | JSPROP_PERMANENT);
    
    JS::RootedObject jsbObj(cx);
    get_or_create_js_obj(cx, global, "jsb", &jsbObj);
    
    JS_DefineFunction(cx, jsbObj, "loadRemoteImg", js_load_remote_image, 2, JSPROP_READONLY | JSPROP_PERMANENT);
    
    //add by flyingkisser
    JS_DefineFunction(cx, jsbObj, "downloadToFileAsync", js_download_to_file_async, 3, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, jsbObj, "unzipToPath", js_unzip_to_path, 4, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, jsbObj, "unzipToPathAsync", js_unzip_to_path_async, 3, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, jsbObj, "unzipGetProgress", js_unzip_get_progress, 0, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, jsbObj, "unzipGetTotal", js_unzip_get_total, 0, JSPROP_READONLY | JSPROP_PERMANENT);
}