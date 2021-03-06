#include "qt_strand.h"
#include "bind_qt_run.h"

#ifdef ENABLE_QT_UI
#ifdef ENABLE_QT_ACTOR
qt_strand::qt_strand()
{
	_ui = NULL;
#if (ENABLE_QT_ACTOR && ENABLE_UV_ACTOR)
	_strandChoose = boost_strand::strand_ui;
#endif
}

qt_strand::~qt_strand()
{
	assert(!_ui);
}

shared_qt_strand qt_strand::create(io_engine& ioEngine, bind_qt_run_base* ui)
{
	shared_qt_strand res(new qt_strand);
	res->_ioEngine = &ioEngine;
	res->_ui = ui;
	res->_actorTimer = new ActorTimer_(res);
	res->_timerBoost = new TimerBoost_(res);
	res->_weakThis = res;
	return res;
}

void qt_strand::release()
{
	assert(in_this_ios());
	_ui = NULL;
}

bool qt_strand::released()
{
	assert(in_this_ios());
	return !_ui;
}

shared_strand qt_strand::clone()
{
	assert(_ui);
	return create(*_ioEngine, _ui);
}

bool qt_strand::in_this_ios()
{
	assert(_ui);
	return _ui->run_in_ui_thread();
}

bool qt_strand::running_in_this_thread()
{
	assert(_ui);
	return _ui->running_in_this_thread();
}

bool qt_strand::sync_safe()
{
	return true;
}

bool qt_strand::is_running()
{
	return true;
}

#endif
#endif