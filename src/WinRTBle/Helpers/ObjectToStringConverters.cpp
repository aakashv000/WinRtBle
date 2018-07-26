#include <string>
//#include <guiddef.h>
//#include <winnt.h>
#include "winrt/Windows.Storage.Streams.h"			//for everything in guidToString()
#include <iostream>									//for wcout

using namespace winrt::Windows::Storage::Streams;	//for IBuffer and DataReader


inline std::wstring guidToString(GUID uuid)
{
	std::wstring guid;
	WCHAR* wszUuid = NULL;
	if (::UuidToString(&uuid, (RPC_WSTR*)&wszUuid) == RPC_S_OK)
	{
		guid = wszUuid;
		::RpcStringFree((RPC_WSTR*)&wszUuid);
	}

	return guid;
}


inline std::wstring getStringFromIBuffer(IBuffer buffer) {
	DataReader reader = DataReader::FromBuffer(buffer);
	
	std::wstring str = reader.ReadString(buffer.Length()).c_str();
	return str;
}