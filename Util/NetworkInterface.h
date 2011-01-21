#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H


#include "../Core/EntityPool.h"
#include <boost/asio.hpp>

#ifdef WIN32
#define INTERFACE_API __declspec(dllexport)
#else
#define INTERFACE_API
#endif

/** Base class for network interfaces. 
	Network plugins shall derive from this. The given io_service object will be polled
	from the main loop enabling interfaces to manipulate the entity pool in a 
	synchronized fashion.
*/
class CNetworkInterface
{
public:
	CNetworkInterface( boost::asio::io_service& IOService, EntityPool& EntPool );
	virtual ~CNetworkInterface();

	virtual bool Init() = 0;

protected:
	boost::asio::io_service& mIOService;
	EntityPool&				 mEntityPool;
	std::string				 mName;
};


#endif
