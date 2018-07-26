#include "src\WinRTBle\pch.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;

#include "src\WinRTBle\Helpers\ObjectToStringConverters.cpp"
#include "src\WinRTBle\Helpers\DataTypeToStringConverters.cpp"



std::wstring GetLastService_UuidId_wstring(GattDeviceServicesResult services) {
	GattDeviceService lastService = services.Services().GetAt(services.Services().Size() - 1);
	std::wstring uuid = guidToString(lastService.Uuid());
	return uuid;
}



// This is the TYPED event handler
void ValueChanged_Handler(GattCharacteristic sender, GattValueChangedEventArgs args) {
	std::cout << "ValueChanged_Handler: ["
		<< uint64_t_to_string(sender.AttributeHandle()) << "]" << std::endl;

	//PrintIntFromIBuffer(args.CharacteristicValue());
	std::wstring data = getStringFromIBuffer(args.CharacteristicValue());
	//ReadCharacteristicData( sender );			//Gives exactly the same data as -> PrintStringFromIBuffer(args.CharacteristicValue());

	std::wcout << '[' << data << ']' << std::endl;
}



//Only writes CCCD for Notify characteristics
IAsyncAction WriteCCCD(GattCharacteristic c) {
	auto result = co_await c.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify);
}



void CharPropertySpecificOperation(GattCharacteristic c) {
	if (c.CharacteristicProperties() == GattCharacteristicProperties::Notify) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Notify" << std::endl;

		c.ValueChanged( ValueChanged_Handler );
		
		WriteCCCD(c);
	}
	else if (c.CharacteristicProperties() == GattCharacteristicProperties::Indicate) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Indicate" << std::endl;

		//WriteCCCD(c);
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
	else if (c.CharacteristicProperties() == (GattCharacteristicProperties::Read | GattCharacteristicProperties::Notify)) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Read and Notify" << std::endl;

		c.ValueChanged( ValueChanged_Handler );
		WriteCCCD(c);
	}
	else if (c.CharacteristicProperties() == (GattCharacteristicProperties::Write | GattCharacteristicProperties::WriteWithoutResponse)) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Write and WriteWithoutResponse" << std::endl;
	}
	else if (c.CharacteristicProperties() == (GattCharacteristicProperties::Read | GattCharacteristicProperties::Write)) {
		std::cout << "\t\t\t\tGattCharacteristicProperties::Read and Write" << std::endl;
	}
	else {
		std::cout << "\t\t\t\tGattCharacteristicProperties::*No match found*" << std::endl;
	}
}



IAsyncAction OpenDevice(uint64_t deviceAddress) {
	auto device = co_await BluetoothLEDevice::FromBluetoothAddressAsync(deviceAddress);

	auto services = co_await device.GetGattServicesAsync();

	for (GattDeviceService const & s : services.Services()) {
		std::wcout << std::hex << "\t\tService - Guid: [" << guidToString(s.Uuid()) << "]" << std::endl;
		
		auto characteristics = co_await s.GetCharacteristicsAsync();

		std::cout << "\t\tChar. count = " << characteristics.Characteristics().Size() << std::endl;

		for (GattCharacteristic const & c : characteristics.Characteristics()) {
			std::wcout << std::hex << "\t\t\tCharacteristic - Guid: [" << guidToString(c.Uuid()) << "]" << std::endl;

			CharPropertySpecificOperation(c);
		}

		if (guidToString(s.Uuid()) == GetLastService_UuidId_wstring(services)) {		//If you have reached the end of last service
			int duration_in_sec = 15;
			Sleep(duration_in_sec * 1000);
		}
	}
}



int main() {
	init_apartment();

	uint64_t deviceAddress;
	unsigned long long myDevice = 53526638260286;

	try {
		BluetoothLEAdvertisementWatcher watcher;

		watcher.ScanningMode(BluetoothLEScanningMode::Passive);

		watcher.Received([&](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs) {
			if (eventArgs.BluetoothAddress() == myDevice) {
				deviceAddress = myDevice;

				watcher.Stop();
			}
		});

		watcher.Start();

		while (deviceAddress == 0) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << '.';
		}

		std::cout << "Device found: " << deviceAddress << ' ' << myDevice << std::endl;

		OpenDevice(deviceAddress).get();

		system("pause");
	}
	catch (const std::exception & ex) {
		std::cout << ex.what() << std::endl;
	}
}