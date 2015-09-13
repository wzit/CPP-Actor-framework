#ifndef __STRAND_EX_H
#define __STRAND_EX_H

#ifdef ENABLE_STRAND_IMPL_POOL
#include <boost/asio/detail/config.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/handler_type_requirements.hpp>
#include <boost/asio/detail/strand_service.hpp>
#include <boost/asio/detail/wrapped_handler.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/push_options.hpp>
#include "try_move.h"

class boost_strand;
class io_engine;

/*!
@brief 修改标准boost strand，impl_改为独占
*/
class StrandEx_
{
	friend io_engine;
	friend boost_strand;
private:
	StrandEx_(io_engine& ios);
	~StrandEx_();

	boost::asio::io_service& get_io_service();
	bool running_in_this_thread() const;
	bool empty();
	bool ready_empty();
	bool waiting_empty();

	template <typename Handler>
	void post(Handler&& handler)
	{
		_service.post(_impl, TRY_MOVE(handler));
	}

	template <typename Handler>
	void dispatch(Handler&& handler)
	{
		_service.dispatch(_impl, TRY_MOVE(handler));
	}
private:
	io_engine& _ios;
	boost::asio::detail::strand_service& _service;
	boost::asio::detail::strand_service::implementation_type _impl;
};
#endif

#endif