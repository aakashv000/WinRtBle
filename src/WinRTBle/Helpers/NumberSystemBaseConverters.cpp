#include <string>
#include <iostream>
//used inline to avoid making a separate header file

std::string makeBinaryIntoHexCompatible(std::string);


inline std::string decimalToBinary(uint64_t decimal)
{
	std::string binary, newBinary = "";
	std::string::reverse_iterator iter;
	uint64_t remainder;

	while (decimal > 0)
	{
		remainder = decimal % 2;

		if (remainder == 0)
			binary += '0';
		else if (remainder == 1)
			binary += '1';

		decimal /= 2;
	}

	for (iter = binary.rbegin(); iter != binary.rend(); iter++)
	{
		newBinary += *iter;
	}

	return newBinary;
}

inline std::string binaryToHexadecimal(std::string _binary)
{
	std::string binary = makeBinaryIntoHexCompatible(_binary);

	std::string hex, newHex = "";
	std::string::reverse_iterator iter;

	uint64_t incr = 1;

	uint64_t exponent = 0;

	uint64_t total = 0;

	for (iter = binary.rbegin(); iter != binary.rend(); iter++)
	{
		if (*iter == '1')
			total += pow(2, exponent);

		if (incr == 4)
		{
			switch (total)
			{
			case 0:
				hex += '0';
				break;
			case 1:
				hex += '1';
				break;
			case 2:
				hex += '2';
				break;
			case 3:
				hex += '3';
				break;
			case 4:
				hex += '4';
				break;
			case 5:
				hex += '5';
				break;
			case 6:
				hex += '6';
				break;
			case 7:
				hex += '7';
				break;
			case 8:
				hex += '8';
				break;
			case 9:
				hex += '9';
				break;
			case 10:
				hex += 'a';
				break;
			case 11:
				hex += 'b';
				break;
			case 12:
				hex += 'c';
				break;
			case 13:
				hex += 'd';
				break;
			case 14:
				hex += 'e';
				break;
			case 15:
				hex += 'f';
				break;
			}

			incr = 0;
			exponent = -1;
			total = 0;
		}

		incr++;
		exponent++;
	}

	newHex += "";

	for (iter = hex.rbegin(); iter != hex.rend(); iter++)
	{
		newHex += *iter;
	}

	return newHex;
}

//To make the binary have digits in multiples of 4 ( else the above converter will lose data )
inline std::string makeBinaryIntoHexCompatible(std::string binary) {
	int rem = binary.length() % 4;
	int noOfZeroPadding = 0;
	if (rem != 0) {
		noOfZeroPadding = 4 - rem;
	}

	std::string paddedBinary = std::string( noOfZeroPadding, '0' ) + binary;

	return paddedBinary;
}