#include "src/WinRTBle/pch.h"

using namespace winrt;			//For init_apartment()
using namespace Windows::Devices::Bluetooth::Advertisement;			//Must be below using namespace winrt
#include <set>
//#include "src/WinRTBle/Helpers/converters.cpp"


int main()
{
	init_apartment();			//This is required.

	//std::atomic<unsigned long long> deviceAddress = 0;
	uint64_t deviceAddress;
	std::set< uint64_t > found_devices;

	try
	{
		BluetoothLEAdvertisementWatcher watcher;

		/*std::wcout << std::hex <<
			"BluetoothLEAdvertisementWatcher:" << std::endl <<
			"\tMaxOutOfRangeTimeout: [0x" << watcher.MaxOutOfRangeTimeout().count() << "]" << std::endl <<
			"\tMaxSamplingInterval:  [0x" << watcher.MaxSamplingInterval().count() << "]" << std::endl <<
			"\tMinOutOfRangeTimeout: [0x" << watcher.MinOutOfRangeTimeout().count() << "]" << std::endl <<
			"\tMinSamplingInterval:  [0x" << watcher.MinSamplingInterval().count() << "]" << std::endl <<
			std::endl;*/

		watcher.ScanningMode(BluetoothLEScanningMode::Passive);

		watcher.Received([&](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs)
		{
			//watcher.Stop();

			//std::wcout <<
			//	"AdvertisementReceived:" << std::endl <<
			//	"\tLocalName: [" << eventArgs.Advertisement().LocalName().c_str() << "]" << std::endl <<
			//	//"\tAdvertisementType: [" << advertisementTypeToString(eventArgs.AdvertisementType()) << "]" << std::endl <<
			//	"\tBluetoothAddress: [0x" << std::hex << eventArgs.BluetoothAddress() << "]" << std::endl <<
			//	"\tRawSignalStrengthInDBm: [" << std::dec << eventArgs.RawSignalStrengthInDBm() << "]" << std::endl <<
			//	std::endl;

			/*for (GUID const & g : eventArgs.Advertisement().ServiceUuids())
				std::wcout << "ServiceUUID: [" << guidToString(g) << "]" << std::endl;*/

			deviceAddress = eventArgs.BluetoothAddress();

			/*if (found_devices.count(deviceAddress) == 0) {
				found_devices.insert(deviceAddress);
			}*/
			found_devices.insert(deviceAddress);
		});

		//std::cout << "Waiting for device: " << std::endl;

		watcher.Start();

		int count = 0;
		int timeout = 1;
		while ( (count++ < timeout) )// && deviceAddress == 0)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			//std::cout << '.';
		}

		watcher.Stop();

		/*std::cout << std::endl << "Finished waiting for device." << std::endl;

		std::cout << "List of unique devices found = " << found_devices.size() << ":" << std::endl;
		for (std::set<unsigned long long>::iterator it = found_devices.begin(); it != found_devices.end(); ++it) {
			std::wcout << std::hex << (*it) << std::endl;
		}*/
		for (auto const & device : found_devices) {
			std::cout << device << std::endl;
			//std::cout << decimalToBinary(device) << std::endl;
			//std::cout << binaryToHexadecimal (decimalToBinary(device)) << std::endl;
			//#### No need of conversion of address into hex string coz we are taking the device address from console and thus we don't need to get it as a variable
			//std::wcout << std::hex << device << std::endl;
		}


		if ( found_devices.size() == 0 )
		{
			std::cout << "No device found" << std::endl;			//TODO: What should be printed if no device is found
		}

		//std::cout << "main - before system(pause): thread id = " << std::this_thread::get_id() << std::endl;

		/*while (true) {
		std::cout << "|";
		}*/
		//Sleep(1000);

		//system("pause");

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