#pragma once

class BarbaSocket
{
public:
	explicit BarbaSocket(SOCKET s, u_long remoteIp);
	virtual ~BarbaSocket();
	//@return 0 if connection closed
	int Receive(BYTE* buf, size_t bufCount, bool waitAll);
	int Send(BYTE* buf, size_t bufCount);
	void Close();
	bool IsWritable();
	//@return empty string if not found
	std::string ReadHttpHeader(int maxlen=5000);
	size_t GetSentBytesCount() {return this->SentBytesCount;}
	size_t GetReceiveBytesCount() {return this->ReceivedBytesCount;}
	void SetNoDelay(bool value);
	void SetReceiveTimeOut(long second);
	void SetSendTimeOut(long second);
	u_long GetRemoteIp() { return this->RemoteIp;}

	static bool InitializeLib(); 
	static void UninitializeLib(); 

protected:
	u_long RemoteIp;
	BarbaSocket();
	size_t SentBytesCount;
	size_t ReceivedBytesCount;
	SOCKET _Socket;
	void ThrowSocketError(int er);
	void ThrowSocketError();
};  

//BarbaSocketClient
class BarbaSocketClient : public BarbaSocket 
{
public:
	explicit BarbaSocketClient(u_long serverIp, u_short port);
	virtual ~BarbaSocketClient(){}
};


//BarbaSocketServer 
class BarbaSocketServer : public BarbaSocket 
{
public:
	explicit BarbaSocketServer(u_short port);
	virtual ~BarbaSocketServer(){}
	BarbaSocket* Accept();
	u_short GetListenPort() {return this->ListenPort;}

private:
	u_short ListenPort;
};
