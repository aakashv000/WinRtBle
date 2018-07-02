//TODO: Make the code modular
// TODO: Remove redundant include's

#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace winrt::Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;

#include <codecvt>
//#include <bluetoothleapis.h>
//using namespace System::ComponentModel;
//#include "winrt\impl\Windows.Devices.Bluetooth.GenericAttributeProfile.0.h"
//#include "winrt\impl\Windows.Devices.Bluetooth.GenericAttributeProfile.1.h"
//#include "winrt\impl\Windows.Devices.Bluetooth.GenericAttributeProfile.2.h"
#include <string>
//#include <pthread.h>
//#include <unistd.h>



std::wstring guidToString(GUID uuid)
{
	std::wstring guid;
	WCHAR* wszUuid = NULL;
	if (::UuidToString(&uuid, (RPC_WSTR*) &wszUuid) == RPC_S_OK)
	{
		guid = wszUuid;
		::RpcStringFree((RPC_WSTR*) &wszUuid);
	}

	return guid;
}


std::wstring advertisementTypeToString(BluetoothLEAdvertisementType advertisementType)
{
	std::wstring ret;

	switch (advertisementType)
	{
	case BluetoothLEAdvertisementType::ConnectableUndirected:
		ret = L"ConnectableUndirected";
		break;
	case BluetoothLEAdvertisementType::ConnectableDirected:
		ret = L"ConnectableDirected";
		break;
	case BluetoothLEAdvertisementType::ScannableUndirected:
		ret = L"ScannableUndirected";
		break;
	case BluetoothLEAdvertisementType::NonConnectableUndirected:
		ret = L"NonConnectableUndirected";
		break;
	case BluetoothLEAdvertisementType::ScanResponse:
		ret = L"ScanResponse";
		break;
	default:
		break;
	}

	return ret;
}

std::wstring bluetoothAddressTypeToString(BluetoothAddressType bluetoothAddressType)
{
	std::wstring ret;

	switch (bluetoothAddressType)
	{
	case BluetoothAddressType::Public:
		ret = L"Public";
		break;
	case BluetoothAddressType::Random:
		ret = L"Random";
		break;
	case BluetoothAddressType::Unspecified:
		ret = L"Unspecified";
		break;
	default:
		break;
	}

	return ret;
}


IBuffer ToIBufferFromString(std::string data) {
	//First convert std::string to std::wstring
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(data);

	DataWriter writer;
	writer.WriteString(wide);
	return writer.DetachBuffer();
}


std::string wstring_from_string(std::wstring wstr) {
	std::string str = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( wstr );
	return str;
}


void PrintStringFromIBuffer(IBuffer buffer) {
	DataReader reader = DataReader::FromBuffer( buffer );
	std::wcout << reader.ReadString(buffer.Length()).c_str() << std::endl;
}


std::string uint64_t_to_string(int64_t value) {
	std::ostringstream o;
	o << value;
	return o.str();
}

std::string uint32_t_to_string(uint32_t value) {
	std::stringstream ss;
	ss << value;
	std::string str;
	ss >> str;
	return str;
}



//////////// TODO: Replace all snippets for reading characteristic data with this function
//Works, but thread execution gets mixed up, due to async reading
IAsyncAction ReadCharacteristicData(GattCharacteristic c) {
	auto readResult = co_await c.ReadValueAsync();
	if (readResult.Status() == GattCommunicationStatus::Success) {
		DataReader reader = DataReader::FromBuffer(readResult.Value());
		std::wcout << "ReadCharacteristicData: [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;		//### wcout is must( don't use cout )
	}
}



// This is the TYPED event handler
void ValueChanged_Handler(GattCharacteristic sender, GattValueChangedEventArgs args) {
	std::cout << "ValueChanged_Handler: thread id = " << std::this_thread::get_id() << std::endl;

	std::cout << "ValueChanged_Handler: ["
		<< uint64_t_to_string(sender.AttributeHandle())
		<< "]" << std::endl;

	PrintStringFromIBuffer(args.CharacteristicValue());
	//ReadCharacteristicData( sender );			//Gives exactly the same data as -> PrintStringFromIBuffer(args.CharacteristicValue());
}


//class myEventHandler: TypedEventHandler< GattCharacteristic, GattValueChangedEventArgs >{
//	public event TypedEventHandler< GattCharacteristic, GattValueChangedEventArgs > ValueChanged;
//};


void WaitThread( int threadId) {
	std::cout << "In WaitThread: " << threadId << std::endl;
	Sleep(10000);
	std::cout << "Exiting WaitThread: " << threadId << std::endl;
	//pthread_exit(NULL);
	return;
}





void PrintCharProperties(GattCharacteristic c) {
	if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
		//c.ValueChanged();

		std::cout << "\t\t\t\tGattCharacteristicProperties::Notify" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Indicate) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Indicate" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Read) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Read" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Write) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Write" << std::endl;
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Broadcast) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Broadcast" << std::endl;
	}
	else {
		std::cout << "\t\t\t\tGattCharacteristicProperties::*No match found*" << std::endl;
	}
}



IAsyncAction OpenDevice(unsigned long long deviceAddress)
{
	std::cout << "OpenDevice() - before getting device: thread id = " << std::this_thread::get_id() << std::endl;

	auto device = co_await BluetoothLEDevice::FromBluetoothAddressAsync(deviceAddress);

	std::cout << "OpenDevice() - after getting device: thread id = " << std::this_thread::get_id() << std::endl;

	std::wcout << std::hex <<
		"Device Information: " << std::endl <<
		"\tBluetoothAddress: [" << device.BluetoothAddress() << "]" << std::endl <<
		"\tBluetoothAddressType: [" << bluetoothAddressTypeToString(device.BluetoothAddressType()) << "]" << std::endl <<
		"\tConnectionStatus: [" << (device.ConnectionStatus() == BluetoothConnectionStatus::Connected ? "Connected" : "Disconnected") << "]" << std::endl <<
		"\tDeviceId: [" << device.DeviceId().c_str() << "]" << std::endl <<
		std::endl;

	auto services = co_await device.GetGattServicesAsync();

	for (GenericAttributeProfile::GattDeviceService const & s : services.Services())
	{
		std::wcout << std::hex <<
			"\t\tService - Guid: [" << guidToString(s.Uuid()) << "]" << std::endl;

		auto characteristics = co_await s.GetCharacteristicsAsync();

		for ( GattCharacteristic const & c : characteristics.Characteristics())
		{
			// Adding the ValueChanged event to the Characteristic c
			auto x = c.ValueChanged( ValueChanged_Handler);// += ValueChanged_Handler;
			/////////////// TODO: Consider adding the ValueChanged event only to Notify characteristics, using PrintCharProperties function
			
			std::string val = uint64_t_to_string(x.value);
			std::cout << val << std::endl;

			std::cout << "OpenDevice() - in Characteristics(): thread id = " << std::this_thread::get_id() << std::endl;

			std::wcout << std::hex <<
				"\t\t\tCharacteristic - Guid: [" << guidToString(c.Uuid()) << "]" << std::endl;

			PrintCharProperties(c);

			// If it is a WRITE characteristic
			if (c.CharacteristicProperties() == GattCharacteristicProperties::Write) {
				//std::cout << "\t\t\t\tGattCharacteristicProperties::Write" << std::endl;

				std::string valueToWrite = "A";
				if (!valueToWrite.empty()) {
					IBuffer writeBuffer = NULL;

					//Assuming valueToWrite is always UTF8 string
					writeBuffer = ToIBufferFromString(valueToWrite);

					try {
						//Writes the value from the buffer to the characteristic
						GattCommunicationStatus result = co_await c.WriteValueAsync(writeBuffer);

						std::cout << "\t\t\t\tWrote " + valueToWrite + " to this characteristic" << std::endl;

						//while (true) {}		//## Works
					}
					catch (std::exception ex) {
						std::cerr << ex.what();
					}
				}
			}
			// If it a NOTIFY characteristic
			else if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
				//std::cout << "\t\t\t\tGattCharacteristicProperties::Notify" << std::endl;

				try {
					// BT_Code: Must write the CCCD in order for server to send indications.
					// We receive them in the ValueChanged event handler.
					// Note that this sample configures either Indicate or Notify, but not both.
					auto result = co_await c.WriteClientCharacteristicConfigurationDescriptorAsync(
						GattClientCharacteristicConfigurationDescriptorValue::Notify);


					if (result == GattCommunicationStatus::Success) {

						auto readResult = co_await c.ReadValueAsync();
						if (readResult.Status() == GattCommunicationStatus::Success) {
							DataReader reader = DataReader::FromBuffer(readResult.Value());
							std::wcout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;			//If cout instead of wcout is used -> data will be printed in some other format
						}

						//std::thread notifyThread(GetNotificationData(c, 1));
					}
				}
				catch (const std::exception& e) {
					std::cout << e.what() << std::endl;
				}
			}
			else	// if (c.CharacteristicProperties() == GattCharacteristicProperties::Read) and others
			{
				auto readResult = co_await c.ReadValueAsync();

				if (readResult.Status() == GattCommunicationStatus::Success)
				{
					std::wcout << "\t\t\t\tCharacteristic Data - Size: [" << readResult.Value().Length() << "]" << std::endl;

					DataReader reader = DataReader::FromBuffer(readResult.Value());
					std::wcout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;
				}
				else if (readResult.Status() == GattCommunicationStatus::Unreachable) {
					std::cout << "\t\t\t\tGattCommunicationStatus::Unreachable" << std::endl;
				}
				else if (readResult.Status() == GattCommunicationStatus::AccessDenied) {
					std::cout << "\t\t\t\tGattCommunicationStatus::AccessDenied" << std::endl;
				}
				else if (readResult.Status() == GattCommunicationStatus::ProtocolError) {
					std::cout << "\t\t\t\tGattCommunicationStatus::ProtocolError" << std::endl;
				}
			}
		}


		if ( wstring_from_string( guidToString(s.Uuid())) == "6e400001-b5a3-f393-e0a9-e50e24dcca9e") {
			// Keep this running so that notification continues to be received
			// This exits after the loop-time expires
			//for (int i = 0; i < 100; i++) { Sleep(1000); }			// #### Works same as Sleep(100*1000)

			std::cout << "OpenDevice - in Services() - before Sleep(): thread id = " << std::this_thread::get_id() << std::endl;

			int duration_of_receiving_notification = 3000;		//in milli-seconds
			Sleep( duration_of_receiving_notification );
		}
		else {
			std::cout << wstring_from_string(guidToString(s.Uuid())) << std::endl;
		}
	}
	std::cout << "OpenDevice - before device.Close(): thread id = " << std::this_thread::get_id() << std::endl;
	//????!!! This Sleep() doesn't work here as required( for continuing to receive notifications )
	//int duration_of_receiving_notification = 3000;		//in milli-seconds
	//Sleep( duration_of_receiving_notification );

	device.Close();

	std::cout << "OpenDevice - after device.Close(): thread id = " << std::this_thread::get_id() << std::endl;
}

int main()
{
	init_apartment();

	std::atomic<unsigned long long> deviceAddress = 0;

	try
	{
		BluetoothLEAdvertisementWatcher watcher;

		std::wcout << std::hex <<
			"BluetoothLEAdvertisementWatcher:" << std::endl <<
			"\tMaxOutOfRangeTimeout: [0x" << watcher.MaxOutOfRangeTimeout().count() << "]" << std::endl <<
			"\tMaxSamplingInterval:  [0x" << watcher.MaxSamplingInterval().count() << "]" << std::endl <<
			"\tMinOutOfRangeTimeout: [0x" << watcher.MinOutOfRangeTimeout().count() << "]" << std::endl <<
			"\tMinSamplingInterval:  [0x" << watcher.MinSamplingInterval().count() << "]" << std::endl <<
			std::endl;

		watcher.ScanningMode(BluetoothLEScanningMode::Passive);

		watcher.Received([&](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs)
		{
			watcher.Stop();

			std::wcout <<
				"AdvertisementReceived:" << std::endl <<
				"\tLocalName: [" << eventArgs.Advertisement().LocalName().c_str() << "]" << std::endl <<
				"\tAdvertisementType: [" << advertisementTypeToString(eventArgs.AdvertisementType()) << "]" << std::endl <<
				"\tBluetoothAddress: [0x" << std::hex << eventArgs.BluetoothAddress() << "]" << std::endl <<
				"\tRawSignalStrengthInDBm: [" << std::dec << eventArgs.RawSignalStrengthInDBm() << "]" << std::endl <<
				std::endl;

			for (GUID const & g : eventArgs.Advertisement().ServiceUuids())
				std::wcout << "ServiceUUID: [" << guidToString(g) << "]" << std::endl;

			deviceAddress = eventArgs.BluetoothAddress();
		});

		std::cout << "Waiting for device: " << std::endl;

		watcher.Start();

		int count = 0;

		while ((count++ < 10) && deviceAddress == 0)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << '.';
		}

		std::cout << std::endl << "Finished waiting for device." << std::endl;

		if (deviceAddress != 0)
		{
			std::cout << "Device found." << std::endl;

			std::cout << "main - before OpenDevice(): thread id = " << std::this_thread::get_id() << std::endl;

			OpenDevice(deviceAddress).get();
		}
		else
			std::cout << "Device not found." << std::endl;
		

		std::cout << "main - before system(pause): thread id = " << std::this_thread::get_id() << std::endl;

		/*while (true) {
			std::cout << "|";
		}*/
		//Sleep(1000);
		system("pause");
		
		/*pthread_t t;
		int i = 0;
		pthread_create(&t, NULL, WaitThread, (void *)i );*/
		
		//std::thread t1( WaitThread, 1 );
		//t1.join();

		//pthread_exit(NULL);
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
