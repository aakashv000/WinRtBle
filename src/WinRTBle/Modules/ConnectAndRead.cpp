#include "src\WinRTBle\pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Storage::Streams;

#include <codecvt>


//
//
//std::wstring advertisementTypeToString(BluetoothLEAdvertisementType advertisementType)
//{
//	std::wstring ret;
//
//	switch (advertisementType)
//	{
//	case BluetoothLEAdvertisementType::ConnectableUndirected:
//		ret = L"ConnectableUndirected";
//		break;
//	case BluetoothLEAdvertisementType::ConnectableDirected:
//		ret = L"ConnectableDirected";
//		break;
//	case BluetoothLEAdvertisementType::ScannableUndirected:
//		ret = L"ScannableUndirected";
//		break;
//	case BluetoothLEAdvertisementType::NonConnectableUndirected:
//		ret = L"NonConnectableUndirected";
//		break;
//	case BluetoothLEAdvertisementType::ScanResponse:
//		ret = L"ScanResponse";
//		break;
//	default:
//		break;
//	}
//
//	return ret;
//}
//
//std::wstring bluetoothAddressTypeToString(BluetoothAddressType bluetoothAddressType)
//{
//	std::wstring ret;
//
//	switch (bluetoothAddressType)
//	{
//	case BluetoothAddressType::Public:
//		ret = L"Public";
//		break;
//	case BluetoothAddressType::Random:
//		ret = L"Random";
//		break;
//	case BluetoothAddressType::Unspecified:
//		ret = L"Unspecified";
//		break;
//	default:
//		break;
//	}
//
//	return ret;
//}
//
//
//
//void PrintStringFromIBuffer(IBuffer buffer) {
//	DataReader reader = DataReader::FromBuffer(buffer);
//	std::wcout << "<" << reader.ReadString(buffer.Length()).c_str() << ">" << std::endl;		//Previously was std::wcout
//}
//
//
//std::string uint32_t_to_string(uint32_t value) {
//	std::stringstream ss;
//	ss << value;
//	std::string str;
//	ss >> str;
//	return str;
//}
//
//
//
//std::string string_from_wstring(std::wstring wstr) {
//	std::string str = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr);
//	return str;
//}
//
//
////////////// TODO: Replace all snippets for reading characteristic data with this function
////Works, but thread execution gets mixed up, due to async reading
//IAsyncAction ReadCharacteristicData(GattCharacteristic c) {
//	auto readResult = co_await c.ReadValueAsync();
//	if (readResult.Status() == GattCommunicationStatus::Success) {
//		DataReader reader = DataReader::FromBuffer(readResult.Value());
//		std::wcout << "ReadCharacteristicData: [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;		//### wcout is must( don't use cout )
//	}
//}
//
//
//
//
//
////class myEventHandler: TypedEventHandler< GattCharacteristic, GattValueChangedEventArgs >{
////	public event TypedEventHandler< GattCharacteristic, GattValueChangedEventArgs > ValueChanged;
////};
//
//
//void WaitThread(int threadId) {
//	std::cout << "In WaitThread: " << threadId << std::endl;
//	Sleep(10000);
//	std::cout << "Exiting WaitThread: " << threadId << std::endl;
//	//pthread_exit(NULL);
//	return;
//}
//
//
//
//
//
////Error: left of HasFlag must be class/struct/union
////std::set< std::string > GetAllCharProperties(GattCharacteristic c) {
////	std::set< std::string > properties;
////
////	if (c.CharacteristicProperties.HasFlag( GattCharacteristicProperties::None) ) {}
////	if (c.CharacteristicProperties.HasFlag( GattCharacteristicProperties::Read ) ) {
////		properties.insert("Read");
////	}
////	if (c.CharacteristicProperties.HasFlag( GattCharacteristicProperties::Write ) ) {
////		properties.insert("Write");
////	}
////	if (c.CharacteristicProperties.HasFlag( GattCharacteristicProperties::Notify ) ) {
////		properties.insert("Notify");
////	}
////	if (c.CharacteristicProperties.HasFlag( GattCharacteristicProperties::Indicate ) ) {
////		properties.insert("Indicate");
////	}
////
////	return properties;
////}
//
//
//
//void PrintStringSet(std::set< std::string > set) {
//	for (auto const & i : set) {
//		std::cout << i << std::endl;
//	}
//}

void PrintIntFromIBuffer(IBuffer buffer) {
	DataReader reader = DataReader::FromBuffer(buffer);
	//std::cout << "<" << reader.ReadString(buffer.Length())[0] << ">" << std::endl;	//Displaying the first byte received [Works]
	//std::cout << "<" << std::to_string( *( reader.ReadString( buffer.Length() ).c_str() )) << ">" << std::endl;		//Works
	std::cout << //"<" << 
		(*(reader.ReadString(buffer.Length()).c_str())) << //">" << 
		std::endl;		//#### Gets the integer value
}



IBuffer ToIBufferFromString(std::string data) {
	//First convert std::string to std::wstring
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(data);

	DataWriter writer;
	writer.WriteString(wide);
	return writer.DetachBuffer();
}


std::wstring guidToString(GUID uuid)
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



std::string uint64_t_to_string(int64_t value) {
	std::ostringstream o;
	o << value;
	return o.str();
}



std::wstring GetLastService_UuidId_wstring(GattDeviceServicesResult services) {
	GattDeviceService lastService = services.Services().GetAt(services.Services().Size() - 1);
	std::wstring uuid = guidToString(lastService.Uuid());
	return uuid;
}


// This is the TYPED event handler
void ValueChanged_Handler(GattCharacteristic sender, GattValueChangedEventArgs args) {
	//std::cout << "ValueChanged_Handler: thread id = " << std::this_thread::get_id() << std::endl;

	/*std::cout << "ValueChanged_Handler: ["
		<< uint64_t_to_string(sender.AttributeHandle()) << "]" << std::endl;*/

	PrintIntFromIBuffer(args.CharacteristicValue());

	//PrintStringFromIBuffer(args.CharacteristicValue());
	//ReadCharacteristicData( sender );			//Gives exactly the same data as -> PrintStringFromIBuffer(args.CharacteristicValue());
}



IAsyncAction WriteCCCD(GattCharacteristic c) {
	auto result = co_await c.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify);
}



IAsyncAction WriteStringToChar(GattCharacteristic c, std::string valueToWrite) {
	//std::string valueToWrite = "ff";
	if (!valueToWrite.empty()) {
		IBuffer writeBuffer = NULL;

		//Assuming valueToWrite is always UTF8 string
		writeBuffer = ToIBufferFromString(valueToWrite);

		try {
			//Writes the value from the buffer to the characteristic
			GattCommunicationStatus result = co_await c.WriteValueAsync(writeBuffer);

			//std::cout << "\t\t\t\tWrote " + valueToWrite + " to this characteristic" << std::endl;
		}
		catch (std::exception ex) {
			std::cerr << ex.what();
		}
	}
}



void CharPropertySpecificOperation(GattCharacteristic c) {
	if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Notify" << std::endl;

		//c.ValueChanged();
		WriteCCCD(c);
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Indicate) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Indicate" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Read) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Read" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Write) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Write" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Broadcast) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Broadcast" << std::endl;
	}
	else if (c.CharacteristicProperties() == (GattCharacteristicProperties::Read | GattCharacteristicProperties::Notify)) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Read and Notify" << std::endl;

		WriteCCCD(c);
	}
	else if (c.CharacteristicProperties() == (GattCharacteristicProperties::Write | GattCharacteristicProperties::WriteWithoutResponse)) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Write and WriteWithoutResponse" << std::endl;

		//move this to char. specific operation function
		/////~~HARDCODING~~
		if (uint64_t_to_string(c.AttributeHandle()) == "16") {
			WriteStringToChar(c, "ff");
		}
	}
	else if (c.CharacteristicProperties() == (GattCharacteristicProperties::Read | GattCharacteristicProperties::Write)) {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::Read and Write" << std::endl;
	}
	else {
		//std::cout << "\t\t\t\tGattCharacteristicProperties::*No match found*" << std::endl;
	}
}






IAsyncAction OpenDevice(unsigned long long deviceAddress)
{
	//std::cout << deviceAddress << std::endl;

	//std::cout << "OpenDevice() - before getting device: thread id = " << std::this_thread::get_id() << std::endl;

	auto device = co_await BluetoothLEDevice::FromBluetoothAddressAsync(deviceAddress);

	//std::cout << "OpenDevice() - after getting device: thread id = " << std::this_thread::get_id() << std::endl;

	/*if (device == NULL) {
		std::cout << "Device is null" << std::endl;
	}*/

	//std::wcout << std::hex <<
	//	"Device Information: " << std::endl <<
	//	"\tBluetoothAddress: [" << device.BluetoothAddress() << "]" << std::endl <<
	//	//"\tBluetoothAddressType: [" << bluetoothAddressTypeToString(device.BluetoothAddressType()) << "]" << std::endl <<
	//	//"\tConnectionStatus: [" << (device.ConnectionStatus() == BluetoothConnectionStatus::Connected ? "Connected" : "Disconnected") << "]" << std::endl <<
	//	"\tDeviceId: [" << device.DeviceId().c_str() << "]" << std::endl <<
	//	std::endl;

	auto services = co_await device.GetGattServicesAsync();

	//std::cout << "OpenDevice() - after getting GattServices: thread id = " << std::this_thread::get_id() << std::endl;

	for (GenericAttributeProfile::GattDeviceService const & s : services.Services())
	{
		/*std::wcout << std::hex <<
			"\t\tService - Guid: [" << guidToString(s.Uuid()) << "]" << std::endl;*/

		auto characteristics = co_await s.GetCharacteristicsAsync();

		for (GattCharacteristic const & c : characteristics.Characteristics())
		{
			// Adding the ValueChanged event to the Characteristic c
			auto x = c.ValueChanged(ValueChanged_Handler);// += ValueChanged_Handler;
			/////////////// TODO: Consider adding the ValueChanged event only to Notify characteristics, using CharPropertySpecificOperation function
			//std::string val = uint64_t_to_string(x.value);
			//std::cout << val << std::endl;

			//std::cout << "OpenDevice() - in Characteristics(): thread id = " << std::this_thread::get_id() << std::endl;

			/*std::wcout << std::hex <<
				"\t\t\tCharacteristic - Guid: [" << guidToString(c.Uuid()) << "]" << std::endl;*/


			CharPropertySpecificOperation(c);
			//PrintStringSet(GetAllCharProperties(c));

			//WriteCCCD(c);


			//// If it is a WRITE characteristic
			//if (c.CharacteristicProperties() == GattCharacteristicProperties::Write) {
			//	//std::cout << "\t\t\t\tGattCharacteristicProperties::Write" << std::endl;
			//
			//	std::string valueToWrite = "A";
			//	if (!valueToWrite.empty()) {
			//		IBuffer writeBuffer = NULL;
			//
			//		//Assuming valueToWrite is always UTF8 string
			//		writeBuffer = ToIBufferFromString(valueToWrite);
			//
			//		try {
			//			//Writes the value from the buffer to the characteristic
			//			GattCommunicationStatus result = co_await c.WriteValueAsync(writeBuffer);
			//
			//			std::cout << "\t\t\t\tWrote " + valueToWrite + " to this characteristic" << std::endl;
			//
			//			//while (true) {}		//## Works
			//		}
			//		catch (std::exception ex) {
			//			std::cerr << ex.what();
			//		}
			//	}
			//}

			//// If it a NOTIFY characteristic
			//else if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
			//	//std::cout << "\t\t\t\tGattCharacteristicProperties::Notify" << std::endl;
			//
			//	try {
			//		std::cout << "OpenDevice - in Notify Char. - before writing CCCD: thread id = " << std::this_thread::get_id() << std::endl;
			//
			//		// BT_Code: Must write the CCCD in order for server to send indications.
			//		// We receive them in the ValueChanged event handler.
			//		// Note that this sample configures either Indicate or Notify, but not both.
			//		auto result = co_await c.WriteClientCharacteristicConfigurationDescriptorAsync(
			//			GattClientCharacteristicConfigurationDescriptorValue::Notify);
			//
			//		std::cout << "OpenDevice - in Notify Char. - after writing CCCD: thread id = " << std::this_thread::get_id() << std::endl;
			//
			//		if (result == GattCommunicationStatus::Success) {
			//
			//			auto readResult = co_await c.ReadValueAsync();
			//			if (readResult.Status() == GattCommunicationStatus::Success) {
			//				DataReader reader = DataReader::FromBuffer(readResult.Value());
			//				std::wcout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;			//If cout instead of wcout is used -> data will be printed in some other format
			//			}
			//
			//			//std::thread notifyThread(GetNotificationData(c, 1));
			//		}
			//	}
			//	catch (const std::exception& e) {
			//		std::cout << e.what() << std::endl;
			//	}
			//}

			//// If it a INDICATE characteristic
			//else if (c.CharacteristicProperties() == GattCharacteristicProperties::Indicate) {
			//	//std::cout << "\t\t\t\tGattCharacteristicProperties::Indicate" << std::endl;
			//
			//	try {
			//		std::cout << "OpenDevice - in Indicate Char. - before writing CCCD: thread id = " << std::this_thread::get_id() << std::endl;
			//
			//		auto resultRead = co_await c.ReadClientCharacteristicConfigurationDescriptorAsync();
			//		if (resultRead.ClientCharacteristicConfigurationDescriptor() == GattClientCharacteristicConfigurationDescriptorValue::Indicate) {
			//			std::cout << "OpenDevice - Indicate - CCCD already set to Indicate" << std::endl;
			//		}
			//		else if (resultRead.ClientCharacteristicConfigurationDescriptor() == GattClientCharacteristicConfigurationDescriptorValue::Notify) {
			//			std::cout << "OpenDevice - Indicate - CCCD already set to Notify" << std::endl;
			//		}
			//		else {
			//			std::cout << "OpenDevice - Indicate - CCCD already set to None" << std::endl;
			//
			//			// BT_Code: Must write the CCCD in order for server to send indications.
			//			// We receive them in the ValueChanged event handler.
			//			// Note that this sample configures either Indicate or Notify, but not both.
			//
			//			//## If CCCD is already set to Indicate, setting it again MIGHT give an exception?
			//			auto result = co_await c.WriteClientCharacteristicConfigurationDescriptorAsync(
			//				GattClientCharacteristicConfigurationDescriptorValue::Indicate);
			//
			//			std::cout << "OpenDevice - in Indicate Char. - after writing CCCD: thread id = " << std::this_thread::get_id() << std::endl;
			//
			//			if (result == GattCommunicationStatus::Success) {
			//
			//				auto readResult = co_await c.ReadValueAsync();
			//				if (readResult.Status() == GattCommunicationStatus::Success) {
			//					DataReader reader = DataReader::FromBuffer(readResult.Value());
			//					std::wcout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;			//If cout instead of wcout is used -> data will be printed in some other format
			//				}
			//
			//				//std::thread notifyThread(GetNotificationData(c, 1));
			//			}
			//		}
			//	}
			//	catch (const std::exception& e) {
			//		std::cout << e.what() << std::endl;
			//	}
			//}

			//else	// if (c.CharacteristicProperties() == GattCharacteristicProperties::Read) and others
			//{
			//	auto readResult = co_await c.ReadValueAsync();
			//
			//	if (readResult.Status() == GattCommunicationStatus::Success)
			//	{
			//		std::wcout << "\t\t\t\tCharacteristic Data - Size: [" << readResult.Value().Length() << "]" << std::endl;
			//
			//		DataReader reader = DataReader::FromBuffer(readResult.Value());
			//		std::wcout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;
			//	}
			//	else if (readResult.Status() == GattCommunicationStatus::Unreachable) {
			//		std::cout << "\t\t\t\tGattCommunicationStatus::Unreachable" << std::endl;
			//	}
			//	else if (readResult.Status() == GattCommunicationStatus::AccessDenied) {
			//		std::cout << "\t\t\t\tGattCommunicationStatus::AccessDenied" << std::endl;
			//	}
			//	else if (readResult.Status() == GattCommunicationStatus::ProtocolError) {
			//		std::cout << "\t\t\t\tGattCommunicationStatus::ProtocolError" << std::endl;
			//	}
			//}

			//// Testing notification of Classroom Clicker [Works]
			////TODO: Shift to an char. specific operations
			///////~~HARDCODING~~
			//if (uint64_t_to_string(c.AttributeHandle()) == "16") {
			//	for (int i = 0; i < 10; i++) {
			//		WriteStringToChar(c, "ff");
			//		Sleep(1000);
			//	}
			//}
		}

		//## Hardcoded to check for the last service of the device -> Changed to automatically finding the last service
		if (guidToString(s.Uuid()) == GetLastService_UuidId_wstring(services)) {
			//std::cout << "OpenDevice - in Services() - before Sleep(): thread id = " << std::this_thread::get_id() << std::endl;
		
			// Keep this running so that notification continues to be received
			// This exits after the loop-time expires
			//for (int i = 0; i < 100; i++) { Sleep(1000); }			// #### Works same as Sleep(100*1000)
		
			int duration_of_receiving_notification = 1000;		//in milli-seconds
			Sleep(duration_of_receiving_notification);
		}
		else {
			//std::cout << string_from_wstring(guidToString(s.Uuid())) << std::endl;
		}
	}
	//std::cout << "OpenDevice - before device.Close(): thread id = " << std::this_thread::get_id() << std::endl;
	//????!!! This Sleep() doesn't work here as required( for continuing to receive notifications )
	//int duration_of_receiving_notification = 3000;		//in milli-seconds
	//Sleep( duration_of_receiving_notification );

	device.Close();

	//std::cout << "OpenDevice - after device.Close(): thread id = " << std::this_thread::get_id() << std::endl;
}

int main( int argc, char* argv[] )
{
	init_apartment();

	uint64_t deviceAddress = strtoull( argv[1], NULL, 10 ) ;		//Convert string to unsigned long long int

	//std::atomic<unsigned long long> deviceAddress = 0;
	//unsigned long long myDeviceAdd = 242485699286186;			//Passed this as argument( via Project Properties -> Debugging -> Command arguments )

	//std::cout << deviceAddress << std::endl;

	try
	{
		//std::cout << "main - before OpenDevice(): thread id = " << std::this_thread::get_id() << std::endl;

		OpenDevice(deviceAddress).get();

		//std::cout << "main - before system(pause): thread id = " << std::this_thread::get_id() << std::endl;

		//system("pause");

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
