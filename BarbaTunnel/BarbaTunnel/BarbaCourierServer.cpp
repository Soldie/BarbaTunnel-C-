#include "StdAfx.h"
#include "BarbaCourierServer.h"
#include "BarbaUtils.h"

BarbaCourierServer::BarbaCourierServer(BarbaCourierCreateStrcut* cs)
	: BarbaCourier(cs)
{
}


BarbaCourierServer::~BarbaCourierServer(void)
{
}

void BarbaCourierServer::SendFakeReply(BarbaSocket* barbaSocket, bool isOutgoing)
{
	if (isOutgoing)
	{

	}

	std::string fakeString = isOutgoing ? FakeHttpGetTemplate : FakeHttpPostTemplate;
	barbaSocket->Send((BYTE*)fakeString.data(), fakeString.length());
}

bool BarbaCourierServer::AddSocket(BarbaSocket* barbaSocket, LPCSTR httpRequest, bool isOutgoing)
{
	if (this->IsDisposing())
		throw new BarbaException(_T("Could not add to disposing object!"));

	Sockets_Add(barbaSocket, isOutgoing);

	ServerThreadData* threadData = new ServerThreadData(this, barbaSocket, httpRequest, isOutgoing);
	Threads.AddTail( (HANDLE)_beginthreadex(NULL, this->ThreadsStackSize, ServerWorkerThread, (void*)threadData, 0, NULL) );
	return true;
}

unsigned int BarbaCourierServer::ServerWorkerThread(void* serverThreadData)
{
	ServerThreadData* threadData = (ServerThreadData*)serverThreadData;
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	BarbaCourierServer* _this = (BarbaCourierServer*)threadData->Courier;
	BarbaSocket* socket = (BarbaSocket*)threadData->Socket;
	bool isOutgoing = threadData->IsOutgoing;
	LPCTSTR requestMode = isOutgoing ? _T("POST") : _T("GET");

	//report new connection in current thread
	_this->Log(_T("HTTP %s connection added. Connections Count: %d."), requestMode,  isOutgoing ? _this->OutgoingSockets.GetCount() : _this->IncomingSockets.GetCount());

	try
	{
		if (isOutgoing)
		{
			//send fake reply 
			SimpleBuffer fakeFileHeader;
			u_int remainBytes = _this->SendOutgoingFakeReply(socket, threadData->HttpRequest.data(), &fakeFileHeader);

			//sending fake file header
			_this->SendFakeFileHeader(socket, &fakeFileHeader);
		
			//process socket until socket closed
			_this->ProcessOutgoing(socket, remainBytes);
		}
		else
		{
			//send fake reply 
			_this->SendOutgoingFakeReply(socket, threadData->HttpRequest.data(), NULL);

			//wait for incoming fake file header
			_this->WaitForIncomingFakeHeader(socket, threadData->HttpRequest.data());

			//process socket until socket closed
			_this->ProcessIncoming(socket);
		}
	}
	catch(BarbaException* er)
	{
		_this->Log(_T("Error: %s"), er->ToString());
		delete er;
	}
	catch(...)
	{
		_this->Log(_T("Unknown Error!"));
	}

	//remove socket from store
	_this->Sockets_Remove(socket, isOutgoing);
	_this->Log(_T("HTTP %s connection removed. Connections Count: %d."), isOutgoing ? _T("GET") : _T("POST"), isOutgoing ? _this->OutgoingSockets.GetCount() : _this->IncomingSockets.GetCount());
	delete threadData;
	return 0;
}

u_int BarbaCourierServer::SendOutgoingFakeReply(BarbaSocket* socket, LPCTSTR httpRequest, SimpleBuffer* fakeFileHeader)
{
	bool outgoing = fakeFileHeader!=NULL;
	std::tstring fakefile = BarbaUtils::GetFileNameFromUrl(httpRequest);
	
	u_int fileSize;
	TCHAR filename[MAX_PATH];
	_tcscpy_s(filename, fakefile.data());
	GetFakeFile(filename, &fileSize, fakeFileHeader, false);

	std::tstring fakeReply = outgoing ? this->FakeHttpGetTemplate : this->FakeHttpPostTemplate;
	InitFakeRequestVars(fakeReply, NULL, filename, fileSize, fakeFileHeader->GetSize());

	if (outgoing)
		Log(_T("Sending fake GET reply! FileName: %s, FileSize: %u, FileHeaderSize: %u"), filename, fileSize, fakeFileHeader->GetSize());
	else
		Log(_T("Sending fake POST reply! FileName: %s"), filename);

	std::string fakeReplyA = fakeReply;
	if (socket->Send((BYTE*)fakeReplyA.data(), fakeReplyA.size())!=(int)fakeReplyA.size())
		throw new BarbaException(_T("Could not send fake reply"));

	return fileSize;
}
