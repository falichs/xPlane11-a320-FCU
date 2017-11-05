#pragma once
#include "AsyncSerial.h"

/**
* Asynchronous serial class with read callback. User code can write data
* from one thread, and read data will be reported through a callback called
* from a separate thred.
*/
class AsyncSerialCallback : public AsyncSerial
{


public:


	AsyncSerialCallback();


	/**
	* Opens a serial device.
	* \param devname serial device name, example "/dev/ttyS0" or "COM1"
	* \param baud_rate serial baud rate
	* \param opt_parity serial parity, default none
	* \param opt_csize serial character size, default 8bit
	* \param opt_flow serial flow control, default none
	* \param opt_stop serial stop bits, default 1
	* \throws boost::system::system_error if cannot open the
	* serial device
	*/
	AsyncSerialCallback(const std::string& devname, 
						unsigned int baud_rate,
	                    boost::asio::serial_port_base::parity opt_parity = boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none),
	                    boost::asio::serial_port_base::character_size opt_csize = boost::asio::serial_port_base::character_size(8),
	                    boost::asio::serial_port_base::flow_control opt_flow = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none),
	                    boost::asio::serial_port_base::stop_bits opt_stop = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one)
	                    );


	/**
	* Set the read callback, the callback will be called from a thread
	* owned by the CallbackAsyncSerial class when data arrives from the
	* serial port.
	* \param callback the receive callback
	*/
	void setCallback(const boost::function<void(const char*, size_t)>& callback);


	/**
	* Removes the callback. Any data received after this function call will
	* be lost.
	*/
	void clearCallback();


	virtual ~AsyncSerialCallback();


};
