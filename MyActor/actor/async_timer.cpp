#include "async_timer.h"
#include "scattered.h"
#include "io_engine.h"
#ifndef DISABLE_BOOST_TIMER
#ifdef DISABLE_HIGH_TIMER
#include <boost/asio/deadline_timer.hpp>
typedef boost::asio::deadline_timer timer_type;
typedef boost::posix_time::microseconds micseconds;
#else
#include <boost/chrono/system_clocks.hpp>
#include <boost/asio/high_resolution_timer.hpp>
typedef boost::asio::basic_waitable_timer<boost::chrono::high_resolution_clock> timer_type;
typedef boost::chrono::microseconds micseconds;
#endif
#else
#include "waitable_timer.h"
typedef WaitableTimerEvent_ timer_type;
typedef long long micseconds;
#endif

TimerBoost_::TimerBoost_(const shared_strand& strand)
:_weakStrand(strand->_weakThis), _looping(false), _timerCount(0),
_extMaxTick(0), _extFinishTime(-1), _handlerQueue(MEM_POOL_LENGTH)
{
#ifdef DISABLE_BOOST_TIMER
	_timer = new timer_type(strand->get_io_engine(), this);
#else
	_timer = new timer_type(strand->get_io_engine());
#endif
}

TimerBoost_::~TimerBoost_()
{
	assert(_handlerQueue.empty());
	delete (timer_type*)_timer;
}

TimerBoost_::timer_handle TimerBoost_::timeout(long long us, async_timer&& host, bool deadline)
{
	if (!_lockStrand)
	{
		_lockStrand = _weakStrand.lock();
#ifdef DISABLE_BOOST_TIMER
		_lockIos.create(_lockStrand->get_io_service());
#endif
	}
	assert(_lockStrand->running_in_this_thread());
	timer_handle timerHandle;
	timerHandle._null = false;
	timerHandle._beginStamp = get_tick_us();
	long long et = deadline ? us : (timerHandle._beginStamp + us) & -256;
	if (et >= _extMaxTick)
	{
		_extMaxTick = et;
		timerHandle._queueNode = _handlerQueue.insert(_handlerQueue.end(), make_pair(et, std::move(host)));
	}
	else
	{
		timerHandle._queueNode = _handlerQueue.insert(make_pair(et, std::move(host)));
	}

	if (!_looping)
	{//定时器已经退出循环，重新启动定时器
		_looping = true;
		assert(_handlerQueue.size() == 1);
		_extFinishTime = et;
		timer_loop(et, et - timerHandle._beginStamp);
	}
	else if ((unsigned long long)et < (unsigned long long)_extFinishTime)
	{//定时期限前于当前定时器期限，取消后重新计时
		boost::system::error_code ec;
		((timer_type*)_timer)->cancel(ec);
		_timerCount++;
		_extFinishTime = et;
		timer_loop(et, et - timerHandle._beginStamp);
	}
	return timerHandle;
}

void TimerBoost_::cancel(timer_handle& th)
{
	if (!th._null)
	{//删除当前定时器节点
		assert(_lockStrand && _lockStrand->running_in_this_thread());
		th._null = true;
		handler_queue::iterator itNode = th._queueNode;
		if (_handlerQueue.size() == 1)
		{
			_extMaxTick = 0;
			_handlerQueue.erase(itNode);
			//如果没有定时任务就退出定时循环
			boost::system::error_code ec;
			((timer_type*)_timer)->cancel(ec);
			_timerCount++;
			_looping = false;
		}
		else if (itNode->first == _extMaxTick)
		{
			_handlerQueue.erase(itNode++);
			if (_handlerQueue.end() == itNode)
			{
				itNode--;
			}
			_extMaxTick = itNode->first;
		}
		else
		{
			_handlerQueue.erase(itNode);
		}
	}
}

void TimerBoost_::timer_loop(long long abs, long long rel)
{
	int tc = ++_timerCount;
#ifdef DISABLE_BOOST_TIMER
	((timer_type*)_timer)->async_wait(micseconds(abs), micseconds(rel), tc);
#else
	boost::system::error_code ec;
	((timer_type*)_timer)->expires_from_now(micseconds(rel), ec);
#ifdef ENABLE_POST_FRONT
	((timer_type*)_timer)->async_wait(_lockStrand->wrap_asio_front([this, tc](const boost::system::error_code&)
#else
	((timer_type*)_timer)->async_wait(_lockStrand->wrap_asio([this, tc](const boost::system::error_code&)
#endif
	{
		event_handler(tc);
	}));
#endif
}

#ifdef DISABLE_BOOST_TIMER
void TimerBoost_::post_event(int tc)
{
	assert(_lockStrand);
#ifdef ENABLE_POST_FRONT
	_lockStrand->post_front([this, tc]
#else
	_lockStrand->post([this, tc]
#endif
	{
		event_handler(tc);
		if (!_lockStrand)
		{
			_lockIos.destroy();
		}
	});
}
#endif

void TimerBoost_::event_handler(int tc)
{
	assert(_lockStrand->running_in_this_thread());
	if (tc == _timerCount)
	{
		_extFinishTime = 0;
		long long ct = get_tick_us();
		while (!_handlerQueue.empty())
		{
			handler_queue::iterator iter = _handlerQueue.begin();
			if (iter->first > ct + 500)
			{
				_extFinishTime = iter->first;
				timer_loop(_extFinishTime, _extFinishTime - ct);
				return;
			}
			else
			{
				iter->second->timeout_handler();
				_handlerQueue.erase(iter);
			}
		}
		_looping = false;
		_lockStrand.reset();
	}
	else if (tc == _timerCount - 1)
	{
		_lockStrand.reset();
	}
}
//////////////////////////////////////////////////////////////////////////

AsyncTimer_::AsyncTimer_(TimerBoost_& timerBoost)
:_timerBoost(timerBoost), _handler(NULL) {}

AsyncTimer_::~AsyncTimer_()
{
	assert(!_handler);
}

void AsyncTimer_::cancel()
{
	if (_handler)
	{
		_handler->destroy();
		_reuMem.deallocate(_handler);
		_handler = NULL;
		_timerBoost.cancel(_timerHandle);
	}
}

shared_strand AsyncTimer_::self_strand()
{
	return _timerBoost._weakStrand.lock();
}

void AsyncTimer_::timeout_handler()
{
	_timerHandle.reset();
	wrap_base* cb = _handler;
	_handler = NULL;
	cb->invoke();
	_reuMem.deallocate(cb);
}