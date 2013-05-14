#pragma once
#include "BarbaServerConnection.h"
#include "BarbaServerHttpCourier.h"

class BarbaServerHttpConnection :
	public BarbaServerConnection
{
public:
	explicit BarbaServerHttpConnection(BarbaServerConfig* config, 
		u_long clientVirtualIp, u_long clientIp, u_short tunnelPort, u_long sessionId);
	virtual ~BarbaServerHttpConnection(void);
	virtual bool ShouldProcessPacket(PacketHelper* packet);
	virtual bool ProcessPacket(PacketHelper* packet, bool send);
	virtual u_short GetTunnelPort() {return this->TunnelPort;}
	virtual u_long GetSessionId() {return this->SessionId;}
	void Init(LPCTSTR requestData);
	bool AddSocket(BarbaSocket* Socket, LPCSTR httpRequest, LPCTSTR requestData, bool isOutgoing);

private:
	BarbaServerHttpCourier* Courier;
	u_long SessionId;
	u_short TunnelPort;
	u_long ClientLocalIp;
};

