#ifndef __IO_ENGINE_H
#define __IO_ENGINE_H

#include <boost/asio/io_service.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/thread.hpp>
#include <set>
#include <vector>

class StrandEx_;
class ActorTimer_;

/*!
@brief io_service��������
*/
class io_engine
{
	friend StrandEx_;
	friend ActorTimer_;
public:
	enum priority
	{
		above_normal = THREAD_PRIORITY_ABOVE_NORMAL,
		below_normal = THREAD_PRIORITY_BELOW_NORMAL,
		highest = THREAD_PRIORITY_HIGHEST,
		idle = THREAD_PRIORITY_IDLE,
		lowest = THREAD_PRIORITY_LOWEST,
		normal = THREAD_PRIORITY_NORMAL,
		time_critical = THREAD_PRIORITY_TIME_CRITICAL
	};
public:
	io_engine();
	~io_engine();
public:
	/*!
	@brief ��ʼ���е���������������������Ϻ���������
	@param threadNum �����������߳���
	*/
	void run(size_t threadNum = 1);

	/*!
	@brief �ȴ���������������ʱ����
	*/
	void stop();

	/*!
	@brief ��������������߳�
	*/
	void suspend();

	/*!
	@brief �ָ������������߳�
	*/
	void resume();

	/*!
	@brief ��⵱ǰ�����Ƿ��ڱ���������ִ��
	*/
	bool runningInThisIos();

	/*!
	@brief �������߳���
	*/
	size_t threadNumber();

	/*!
	@brief �������߳����ȼ�����
	*/
	void runPriority(priority pri);

	/*!
	@brief ��ȡ��ǰ���������ȼ�
	*/
	priority getPriority();

	/*!
	@brief ��ȡ���������δ�run()��stop()��ĵ�������
	*/
	long long getRunCount();

	/*!
	@brief ��ȡCPU������
	*/
	static unsigned physicalConcurrency();

	/*!
	@brief ��ȡCPU�߳���
	*/
	static unsigned hardwareConcurrency();

	/*!
	@brief ����CPU����
	*/
	void cpuAffinity(unsigned mask);

	/*!
	@brief �����߳�ID
	*/
	const std::set<boost::thread::id>& threadsID();

	/*!
	@brief ��������������
	*/
	operator boost::asio::io_service& () const;

	/*!
	@brief ��ȡtlsֵ
	@param 0 <= i < 64
	*/
	static void* getTlsValue(int i);

	/*!
	@brief ����tlsֵ
	@param 0 <= i < 64
	*/
	static void setTlsValue(int i, void* val);

	/*!
	@brief ��ȡĳ��tls�����ռ�
	@param 0 <= i < 64
	*/
	static void** getTlsValuePtr(int i);
private:
	void* getImpl();
	void freeImpl(void* impl);
	void* getTimer();
	void freeTimer(void* timer);
private:
	bool _opend;
	priority _priority;
	std::set<boost::thread::id> _threadsID;
	void* _implPool;
	void* _timerPool;
	std::vector<HANDLE> _handleList;
	boost::atomic<long long> _runCount;
	boost::mutex _ctrlMutex;
	boost::mutex _runMutex;
	boost::asio::io_service _ios;
	boost::asio::io_service::work* _runLock;
	boost::thread_group _runThreads;
	static boost::thread_specific_ptr<void*> _tls;///<64λvoid*
};

#endif