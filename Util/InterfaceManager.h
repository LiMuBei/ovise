#ifndef OVISE_INTERFACE_MANAGER_H
#define OVISE_INTERFACE_MANAGER_H

#include <boost/asio/io_service.hpp>

#include "NetworkInterface.h"

class EntityPool;

class CInterfaceManager
{
public:
	explicit CInterfaceManager( EntityPool& EntPool );
	~CInterfaceManager(void);

	template<class InterfaceType> inline
	void RegisterInterface( const std::string& InterfaceName )
	{
		mInterfaces[InterfaceName] = new InterfaceType( mEntityPool );
	}

	CNetworkInterface* GetInterface( const std::string& Name );
	std::set<std::string> GetInterfaceNames() const;

	bool HasInterfaceRunning();

	void PollInterfaces();

private:
	typedef std::map<std::string, CNetworkInterface* > InterfaceMapType;
	InterfaceMapType			mInterfaces;

	EntityPool&					mEntityPool;
};

#endif // OVISE_INTERFACE_MANAGER_H