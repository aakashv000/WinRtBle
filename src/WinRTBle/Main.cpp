//TODO: Make the code modular

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
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(data);

	DataWriter writer;
	writer.WriteString(wide);
	return writer.DetachBuffer();
}



/////////////////////////////////////////////TODO: make this a readValue function
//void GetNotificationData(GenericAttributeProfile::GattCharacteristic c, int repeatInterval) {
//	auto readResult = co_await c.ReadValueAsync();
//	DataReader reader = DataReader::FromBuffer(readResult.Value());
//	std::cout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;
//}




void ValueChanged_Handler(GattCharacteristic sender, GattValueChangedEventArgs args) {
	std::wcout << "ValueChanged_Handler: "
		<< sender.AttributeHandle() << " "
		<< std::endl;
}



void PrintCharProperties(GattCharacteristic c) {
	if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
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

		//c.ValueChanged();
	}
}



IAsyncAction OpenDevice(unsigned long long deviceAddress)
{
	auto device = co_await BluetoothLEDevice::FromBluetoothAddressAsync(deviceAddress);

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

		for (GenericAttributeProfile::GattCharacteristic const & c : characteristics.Characteristics())
		{
			std::wcout << std::hex <<
				"\t\t\tCharacteristic - Guid: [" << guidToString(c.Uuid()) << "]" << std::endl;

			PrintCharProperties(c);

			//If it is a WRITE characteristic
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
					}
					catch (std::exception ex) {
						std::cerr << ex.what();
					}
				}
			}
			//If it a NOTIFY characteristic
			else if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
				//std::cout << "\t\t\t\tGattCharacteristicProperties::Notify" << std::endl;

				try {
					// BT_Code: Must write the CCCD in order for server to send indications.
					// We receive them in the ValueChanged event handler.
					// Note that this sample configures either Indicate or Notify, but not both.
					auto result = co_await c.WriteClientCharacteristicConfigurationDescriptorAsync(
						GattClientCharacteristicConfigurationDescriptorValue::Notify);

					////////////////////////////////////////////////////////////////////////////////
					//GattCharacteristic gc = c;
					c.ValueChanged += ValueChanged_Handler;

					if (result == GattCommunicationStatus::Success) {

						auto readResult = co_await c.ReadValueAsync();
						if (readResult.Status() == GattCommunicationStatus::Success) {
							DataReader reader = DataReader::FromBuffer(readResult.Value());
							std::wcout << "\t\t\t\tCharacteristic Data - [" << reader.ReadString(readResult.Value().Length()).c_str() << "]" << std::endl;			//If cout instead of wcout is used -> data will be printed in some other format
						}

						//std::thread notifyThread(GetNotificationData(c, 1));
					}

					/*PBTH_LE_GATT_CHARACTERISTIC pCharBuffer;
					pCharBuffer = (PBTH_LE_GATT_CHARACTERISTIC)
						malloc(charBufferSize * sizeof(BTH_LE_GATT_CHARACTERISTIC));
						
					BLUETOOTH_GATT_EVENT_HANDLE EventHandle;

					BTH_LE_GATT_EVENT_TYPE EventType = CharacteristicValueChangedEvent;

					BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION EventParameterIn;
					EventParameterIn.Characteristics[0] = *currGattChar;*/
				}
				catch (const std::exception& e) {
					std::cout << e.what() << std::endl;
				}
			}
			else	//if (c.CharacteristicProperties() == GattCharacteristicProperties::Read) and others
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
	}

	device.Close();
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

			OpenDevice(deviceAddress).get();
		}
		else
			std::cout << "Device not found." << std::endl;
		
		system("pause");

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
