#include "AsyncSerialCallback.h"

using namespace boost;
using namespace std;


AsyncSerialCallback::AsyncSerialCallback() : AsyncSerial()
{

}

AsyncSerialCallback::AsyncSerialCallback(const string& devname,
	unsigned int baud_rate,
	asio::serial_port_base::parity opt_parity,
	asio::serial_port_base::character_size opt_csize,
	asio::serial_port_base::flow_control opt_flow,
	asio::serial_port_base::stop_bits opt_stop)
	: AsyncSerial(devname, baud_rate, opt_parity, opt_csize, opt_flow, opt_stop)
{

}

void AsyncSerialCallback::setCallback(const
	boost::function<void(const char*, size_t)>& callback)
{
	setReadCallback(callback);
}

void AsyncSerialCallback::clearCallback()
{
	clearReadCallback();
}

AsyncSerialCallback::~AsyncSerialCallback()
{
	clearReadCallback();
}