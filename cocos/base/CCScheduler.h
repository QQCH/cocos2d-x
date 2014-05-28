/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCSCHEDULER_H__
#define __CCSCHEDULER_H__

#include <functional>
#include <mutex>
#include <set>

#include "base/CCRef.h"
#include "base/CCVector.h"
#include "2d/uthash.h"

NS_CC_BEGIN

/**
 * @addtogroup global
 * @{
 */

class Scheduler;

typedef std::function<void(float)> ccSchedulerFunc;
//
// Timer
//
/** @brief Light-weight timer */
//
class CC_DLL Timer : public Ref
{
protected:
    Timer();
public:
    /** get interval in seconds */
    /** ������������ʱ���� */
    inline float getInterval() const { return _interval; };
	/** set interval in seconds */
    /** �������������ʱ���� */
    inline void setInterval(float interval) { _interval = interval; };
    
    void setupTimerWithInterval(float seconds, unsigned int repeat, float delay);
    
    virtual void trigger() = 0;
    virtual void cancel() = 0;
    
    /** triggers the timer */
    /** ������ʱ�� */
    void update(float dt);
    
protected:
    
    Scheduler* _scheduler; // weak ref
    float _elapsed;
    bool _runForever;
    bool _useDelay;
    unsigned int _timesExecuted;
    unsigned int _repeat; //0 = once, 1 is 2 x executed
    float _delay;
    float _interval;
};


class CC_DLL TimerTargetSelector : public Timer
{
public:
    TimerTargetSelector();

    /** Initializes a timer with a target, a selector and an interval in seconds, repeat in number of times to repeat, delay in seconds. */
    /** ͨ��������һ��Ŀ�꣬һ��ѡ������һ������������ظ������Լ��ӳٵ���������ʼ����ʱ�� */
    bool initWithSelector(Scheduler* scheduler, SEL_SCHEDULE selector, Ref* target, float seconds, unsigned int repeat, float delay);
    
    inline SEL_SCHEDULE getSelector() const { return _selector; };
    
    virtual void trigger() override;
    virtual void cancel() override;
    
protected:
    Ref* _target;
    SEL_SCHEDULE _selector;
};


class CC_DLL TimerTargetCallback : public Timer
{
public:
    TimerTargetCallback();
    
    /** Initializes a timer with a target, a lambda and an interval in seconds, repeat in number of times to repeat, delay in seconds. */
    /** ͨ��������һ��Ŀ�꣬һ��ѡ������һ������������ظ������Լ��ӳٵ���������ʼ����ʱ��. */
    bool initWithCallback(Scheduler* scheduler, const ccSchedulerFunc& callback, void *target, const std::string& key, float seconds, unsigned int repeat, float delay);
    
    /**
     * @js NA
     * @lua NA
     */
    inline const ccSchedulerFunc& getCallback() const { return _callback; };
    inline const std::string& getKey() const { return _key; };
    
    virtual void trigger() override;
    virtual void cancel() override;
    
protected:
    void* _target;
    ccSchedulerFunc _callback;
    std::string _key;
};

#if CC_ENABLE_SCRIPT_BINDING

class CC_DLL TimerScriptHandler : public Timer
{
public:
    bool initWithScriptHandler(int handler, float seconds);
    inline int getScriptHandler() const { return _scriptHandler; };
    
    virtual void trigger() override;
    virtual void cancel() override;
    
private:
    int _scriptHandler;
};

#endif

//
// Scheduler
//
struct _listEntry;
struct _hashSelectorEntry;
struct _hashUpdateEntry;

#if CC_ENABLE_SCRIPT_BINDING
class SchedulerScriptHandlerEntry;
#endif

/** @brief Scheduler���𴥷��ѱ����ȵĻص�����
�㲻Ӧ���������Ϸ�߼���ʹ��ϵͳ�ļ�ʱ��. ����Ӧ��ʹ�������.

���������͵Ļص����� (selector(ѡ����) ):

- update selector: 'update' ѡ��������ÿ֡������. ���ܹ��趨�����ȼ�.
- custom selector: custom ѡ��������ÿ֡���ã�������һ���趨��ʱ��������ʱ����

Ӧ�þ�������ʹ��'custom selectors'. ʹ��'update selector'���ӿ죬�����ĵ��ڴ����.

*/
class CC_DLL Scheduler : public Ref
{
public:
    // Priority level reserved for system services.
    static const int PRIORITY_SYSTEM;
    
    // Minimum priority level for user scheduling.
    static const int PRIORITY_NON_SYSTEM_MIN;
    /**
     * @js ctor
     */
    Scheduler();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~Scheduler();

    inline float getTimeScale() { return _timeScale; }
	
	/** �ı����б����ȵĻص�������ʱ��.
    �����ʹ���������������һ��'����ͷ'����'���'��Ч��.
    Ĭ��ֵ��1.0. ʹ�õ���1.0��ֵ������һ��'����ͷ'��Ч��.
    Ҫ����һ��'���'��Ч��, ʹ�ô���1.0��ֵ.
    @since v0.8
    @warning ����Ӱ��ÿһ�������ȵ�ѡ����(selector) / ����(action).
    */
    inline void setTimeScale(float timeScale) { _timeScale = timeScale; }

	/** 'update' ������.
     ����Զ��Ӧ�õ����������,������֪���Լ�����ʲô.
     * @js NA
     * @lua NA
     */
    void update(float dt);

    /////////////////////////////////////
    
    // schedule
    
	/** �����ȵķ�������ÿһ��ʱ����'interval'ָ������������ʱ����.
     ��������ȵķ�ʽ������ͣ״̬����ôֻ�������ָ�֮����ܱ�����.
     ���ʱ����'interval'��0, ������ÿ֡������, ���������, �Ƽ�ʹ��'scheduleUpdate'��������.
     ����ص�����'callback'�Ѿ�������,��ô����ֻ����²���interval,�����ٴ��ظ�����.
     repeat �ö����ظ�ִ��repeat + 1��, ������ö�������ִ�п���ʹ��kRepeatForever
     delay ��ʾ����������֮ǰ�ȴ���ʱ��ֵ
     @param key ��ʶ�ص���key
     @since v3.0
     */
    void schedule(const ccSchedulerFunc& callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key);

	/** ͨ������kRepeatForever��0ʱ�ӵ���scheduleCallback����
     @since v3.0
     */
    void schedule(const ccSchedulerFunc& callback, void *target, float interval, bool paused, const std::string& key);
    
    
	/** �ѱ����ȵķ�������ÿ��ʱ����'interval'�趨����������ʱ����.
     ��������ȵķ�������ͣ״̬, ��ôֱ�������ָ����ܱ�����.
     ���ʱ����'interval'��0,������ÿ֡����,������������Ƽ�ʹ��'scheduleUpdate'�滻.
     ����selector�Ѿ�������,��ôֻ�в���interval�ᱻ����,�������ٴ����µ���
     repeat �ö����ظ�ִ�� repeat + 1 ��, ������ö���������ͣ��ִ�У�ʹ��kRepeatForever
     delay ��ʾ����������֮ǰ�ȴ���ʱ����
     
     @since v3.0, ����repeat��delay�ڰ汾1.1����
     */
    void schedule(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused);
    
    /** ͨ������kRepeatForever��0ʱ�ӵ���scheduleSelector */
    void schedule(SEL_SCHEDULE selector, Ref *target, float interval, bool paused);
    
	/** ͨ�����������ȼ���Ŀ���ϵ���'update' ѡ����(selector).
     'update' selector ����ÿ֡������.
     ���ȼ�Խ�ͣ�Խ���ױ�����.
     @since v3.0
     @lua NA
     */
    template <class T>
    void scheduleUpdate(T *target, int priority, bool paused)
    {
        this->schedulePerFrame([target](float dt){
            target->update(dt);
        }, target, priority, paused);
    }

#if CC_ENABLE_SCRIPT_BINDING
    // schedule for script bindings
	/** �ѱ����ȵĽű��ص���������ÿ��ʱ����'interval'ָ������������ʱ����.
     ����ű��ص�������Ϊ��ͣ,��ô�������ָ�֮ǰ�ǲ��ᱻ���õ�.
     ���'interval'����0, ������ÿ֡������.
     ���ص��Ƚű���Ŀ��ID, ���ID�����ں���unscheduleScriptFunc()��ʹ��.
     */
    unsigned int scheduleScriptFunc(unsigned int handler, float interval, bool paused);
#endif
    /////////////////////////////////////
    
    // unschedule

	/** �ڸ�����key��target�ϳ����Իص������ĵ���.
     ������볷������'callbackPerFrame', ʹ��unscheduleUpdate����.
     @since v3.0
     */
    void unschedule(const std::string& key, void *target);

	/** �ڸ���Ŀ���ϳ�������selector.
     ������볷������ "update", ʹ��unscheudleUpdate����.
     @since v3.0
     */
    void unschedule(SEL_SCHEDULE selector, Ref *target);
    
	/** �ڸ�����Ŀ���ϳ���update selector����
     @since v0.99.3
     */
    void unscheduleUpdate(void *target);
    
	/** ��������Ŀ���ϵ�����selector����.
     ����"update" selector.
     @since v0.99.3
     @js  unscheduleCallbackForTarget
     @lua NA
     */
    void unscheduleAllForTarget(void *target);
    
	/** ��������Ŀ���ϵ�����selector(ѡ����).
     ����Զ��Ӧ�õ����������,������֪���Լ�����ʲô.
     @since v0.99.3
     */
    void unscheduleAll(void);
    
	/** ����͵����ȼ���������Ŀ���ϵ�����selector�ĵ���.
     �����������ʱ��Ӧ��ֻʹ��kPriorityNonSystemMin���߸���һ������ȼ�.
     @since v2.0.0
     */
    void unscheduleAllWithMinPriority(int minPriority);
    
#if CC_ENABLE_SCRIPT_BINDING
    /** ȡ����һ���ű���Ŀ�ĵ���. */
    void unscheduleScriptEntry(unsigned int scheduleScriptEntryID);
#endif
    
    /////////////////////////////////////
    
    // isScheduled
    
	 /** ���һ��������'key'��'target'�Ļص��Ƿ񱻵���.
     @since v3.0.0
     */
    bool isScheduled(const std::string& key, void *target);
    
	 /** ������Ŀ���ϵ�һ��selector(ѡ����)�Ƿ񱻵���.
     @since v3.0
     */
    bool isScheduled(SEL_SCHEDULE selector, Ref *target);
    
    /////////////////////////////////////
    
	 /** ��ͣĿ��.
     �ڸ���Ŀ���ϵ����б����ȵ�selector/update���ᱻ��ʱִ�У�ֱ�����Ŀ�걻�ָ�
     ���Ŀ�겻����,ʲô���鶼����.
     @since v0.99.3
     */
    void pauseTarget(void *target);

	 /** �ָ�Ŀ��.
     Ŀ��'target' �ᱻ�ָ�, �������е��ȵ�selector/update�������¼�ʱִ��.
     ���Ŀ�겻����,ʲô���鶼����.
     @since v0.99.3
     */
    void resumeTarget(void *target);


	/** ����Ŀ���Ƿ���ͣ
    @since v1.0.0
    * In js: var isTargetPaused(var jsObject)
    * @lua NA 
    */
    bool isTargetPaused(void *target);

	/** �����е�Ŀ������ͣ���е�selector(ѡ����).
       ����Զ��Ӧ�õ����������,������֪���Լ�����ʲô.
     @since v2.0.0
      */
    std::set<void*> pauseAllTargets();

	/** ����͵����ȼ������е�Ŀ������ͣ���е�selector(ѡ����).
      ��ֻӦ����kPriorityNonSystemMin���߸��ߵ����ȼ������ô˺���.
      @since v2.0.0
      */
    std::set<void*> pauseAllTargetsWithMinPriority(int minPriority);

	/** ��һ��Ŀ�꼯���лָ�selector.
     ���������������ȡ����pauseAllSelectors�ĵ���.
     @since v2.0.0
      */
    void resumeTargets(const std::set<void*>& targetsToResume);

	 /** ��cocos2d�߳��е��÷���. ������Ҫ������һ���߳��е���һ��cocos2d����ʱ����.
     ����������̰߳�ȫ��.
     @since v3.0
     */
    void performFunctionInCocosThread( const std::function<void()> &function);
    
    /////////////////////////////////////
    
    // Deprecated methods:
    
	/** �����ȵķ�������ÿ��ʱ����'interval'ָ������������ʱ����.
     ����ű��ص�������Ϊ��ͣ,��ô�������ָ�֮ǰ�ǲ��ᱻ���õ�.
     ���ʱ����'interval'��0,������ÿ֡����,������������Ƽ�ʹ��'scheduleUpdate'�滻..
     ����selector�Ѿ�������,��ôֻ�в���interval�ᱻ����,�������ٴ����µ���.
     repeat �ö����ظ�ִ��repeat + 1��, ������ö�������ִ�п���ʹ��kRepeatForever
     delay ��ʾ����������֮ǰ�ȴ���ʱ����
     @deprecated ��ʹ��'Scheduler::schedule'����˷���.
     @since v0.99.3, ����repeat��delay�ڰ汾v1.1�м���
     */
    CC_DEPRECATED_ATTRIBUTE void scheduleSelector(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused)
    {
        schedule(selector, target, interval, repeat, delay, paused);
    };
    
	/** ͨ������kRepeatForever��0��ʱ����scheduleSelector
     *  @deprecated ��ʹ��'Scheduler::schedule'����˷���.
     */
    CC_DEPRECATED_ATTRIBUTE void scheduleSelector(SEL_SCHEDULE selector, Ref *target, float interval, bool paused)
    {
        schedule(selector, target, interval, paused);
    };
    
	/** ��ָ����Ŀ������ָ�������ȼ�����'update' selector.
     'update' selector����ÿ֡����.
     ���ȼ�Խ�ͣ�Խ���ױ�����.
     @deprecated ��ʹ��'Scheduler::scheduleUpdate'����˷���.
     @since v0.99.3
     */
    template <class T>
    CC_DEPRECATED_ATTRIBUTE void scheduleUpdateForTarget(T* target, int priority, bool paused) { scheduleUpdate(target, priority, paused); };
    
	 /** �ڸ���Ŀ����ȡ������selector.
     �������ȡ������ "update", ʹ�� unscheudleUpdateForTarget.
     @deprecated ��ʹ��'Scheduler::unschedule'����˷���.
     @since v0.99.3
     */
    CC_DEPRECATED_ATTRIBUTE void unscheduleSelector(SEL_SCHEDULE selector, Ref *target) { unschedule(selector, target); };
    
	 /** �����һ��selector(ѡ����)�Ƿ��ڸ�����target�ϱ�����.
     @deprecated ��ʹ��'Scheduler::isScheduled'����˷���.
     @since v0.99.3
     */
    CC_DEPRECATED_ATTRIBUTE bool isScheduledForTarget(Ref *target, SEL_SCHEDULE selector) { return isScheduled(selector, target); };
    
	 /** ��ָ����Ŀ����ȡ������updateѡ����
     @deprecated ��ʹ��'Scheduler::unscheduleUpdate'����˷���.
     @since v0.99.3
     */
    CC_DEPRECATED_ATTRIBUTE void unscheduleUpdateForTarget(Ref *target) { return unscheduleUpdate(target); };
    
protected:
    
	 /** �ڸ�����Ŀ�����Ը��������ȼ�����'callback'����.
     'callback' selector(ѡ����)����ÿ֡����.
     ���ȼ�Խ��,Խ���ױ�����.
     @note �������ֻ���ڲ�ʹ��.
     @since v3.0
     */
    void schedulePerFrame(const ccSchedulerFunc& callback, void *target, int priority, bool paused);
    
    void removeHashElement(struct _hashSelectorEntry *element);
    void removeUpdateFromHash(struct _listEntry *entry);

    // update specific

    void priorityIn(struct _listEntry **list, const ccSchedulerFunc& callback, void *target, int priority, bool paused);
    void appendIn(struct _listEntry **list, const ccSchedulerFunc& callback, void *target, bool paused);


    float _timeScale;

    //
    // "updates with priority" stuff
    //
    struct _listEntry *_updatesNegList;        // list of priority < 0
    struct _listEntry *_updates0List;            // list priority == 0
    struct _listEntry *_updatesPosList;        // list priority > 0
    struct _hashUpdateEntry *_hashForUpdates; // hash used to fetch quickly the list entries for pause,delete,etc

    // Used for "selectors with interval"
    struct _hashSelectorEntry *_hashForTimers;
    struct _hashSelectorEntry *_currentTarget;
    bool _currentTargetSalvaged;
    // If true unschedule will not remove anything from a hash. Elements will only be marked for deletion.
    bool _updateHashLocked;
    
#if CC_ENABLE_SCRIPT_BINDING
    Vector<SchedulerScriptHandlerEntry*> _scriptHandlerEntries;
#endif
    
    // Used for "perform Function"
    std::vector<std::function<void()>> _functionsToPerform;
    std::mutex _performMutex;
};

// end of global group
/// @}

NS_CC_END

#endif // __CCSCHEDULER_H__
