#include "IO.h"
#include <fstream>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::ios;
using std::fstream;
using std::make_tuple;

bool IO::isValidBinaryStr(string binStr)
{
	for (char c : binStr)
	{
		if (c - '0' > 1)
			return false;
	}

	return true;
}

bool IO::isValidDecimalStr(string decStr)
{
	for (char c : decStr)
	{
		if (!isdigit(c))
			return false;
	}

	return true;
}

bool IO::isFileExisted(string filename)
{
	fstream fs(filename, ios::in);
	bool isExisted = fs.is_open();
	if (!isExisted) io.writeConsole("File '" + filename + "' is not existed");
	return isExisted;
}

void IO::clearFile(string filename)
{
	fstream f(filename, ios::out);
	f.close();
}

void IO::readInputs(vector<tuple<BigInt, BigInt>>& testCases)
{
	fstream f("input.txt", ios::in);
	string numberA, numberB;

	if (f.is_open())
	{
		while (!f.eof())
		{
			f >> numberA;
			f >> numberB;

			if (numberA == "" || numberB == "")
				continue;

			BigInt a = converter.decimalStrToBigInt(numberA);
			BigInt b = converter.decimalStrToBigInt(numberB);

			testCases.push_back(make_tuple(a, b));
		}
	}

	f.close();
}

void IO::readBinaryInputs(vector<tuple<BigInt, BigInt>>& testCases)
{
	fstream f("input.txt", ios::in);
	string numberA, numberB;

	if (f.is_open())
	{
		while (!f.eof())
		{
			f >> numberA;
			f >> numberB;

			if (numberA == "" || numberB == "")
				continue;

			BigInt a = converter.binaryStrToBigInt(numberA);
			BigInt b = converter.binaryStrToBigInt(numberB);

			testCases.push_back(make_tuple(a, b));
		}
	}

	f.close();
}

void IO::writeOutput(string output)
{
	fstream f("output.txt", ios::app);

	f << output << endl;

	f.close();
}

void IO::writeOutput(fstream& fs, string output)
{
	fs << output;
}

void IO::writeOutputs(BigInt a, BigInt b, BigInt res, string op)
{
	fstream f("output.txt", ios::app);

	f << converter.bigIntToDecimalStr(a)
		<< op
		<< converter.bigIntToDecimalStr(b)
		<< " = "
		<< converter.bigIntToDecimalStr(res)
		<< endl;

	f.close();
}

void IO::writeOutputs(BigInt a, int b, BigInt res, string op)
{
	fstream f("output.txt", ios::app);

	f << converter.bigIntToDecimalStr(a)
		<< op
		<< b
		<< " = "
		<< converter.bigIntToDecimalStr(res) << endl;

	f.close();
}

void IO::writeBinaryOutputs(BigInt a, BigInt b, BigInt res, string op)
{
	fstream f("output.txt", ios::app);

	f << converter.bigIntToBinaryStr(a)
		<< op
		<< converter.bigIntToBinaryStr(b)
		<< " = "
		<< converter.bigIntToBinaryStr(res)
		<< endl;

	f.close();
}

void IO::writeLog(string log)
{
	fstream f("log.txt", ios::app);

	f << log << endl;

	f.close();
}

void IO::writeConsole(string output)
{
	std::cout << output << endl;
}

bool IO::openFile(fstream& fs, string filename, ios::openmode mode)
{
	fs.open(filename, mode);

	if (!fs.is_open())
	{
		io.writeLog("[IO::openFile] file: " + filename + " is not existed");
		return false;
	}
	else {
		return true;
	}
}

string IO::readContent(fstream& fs)
{
	string content;

	while (!fs.eof())
	{
		string buffer;
		getline(fs, buffer, '\n');
		content += buffer;
	}

	return content;
}

tuple<string, string, string> IO::inputKeys(int format) {
	string nStr, eStr, dStr;

	if (format == 10)
	{
		do {
			cout << "[Encrypt] enter decimal public key n: "; cin >> nStr;
		} while (!io.isValidDecimalStr(nStr));
		do {
			cout << "[Encrypt] enter decimal public key e: "; cin >> eStr;
		} while (!io.isValidDecimalStr(eStr));
		do {
			cout << "[Encrypt] enter decimal private key d: "; cin >> dStr;
		} while (!io.isValidDecimalStr(dStr));
	}
	else if (format == 2) {
		do {
			cout << "[Encrypt] enter binary public key n: "; cin >> nStr;
		} while (!io.isValidBinaryStr(nStr));
		do {
			cout << "[Encrypt] enter binary public key e: "; cin >> eStr;
		} while (!io.isValidBinaryStr(eStr));
		do {
			cout << "[Encrypt] enter binary private key d: "; cin >> dStr;
		} while (!io.isValidBinaryStr(dStr));
	}

	return make_tuple(nStr, eStr, dStr);
}

tuple<string, string> IO::inputFilesForEncryption()
{
	string plainTextFile, cipherTextFile;

	do {
		cout << "[Encrypt] enter plain text file name: "; cin >> plainTextFile;
	} while (!io.isFileExisted(plainTextFile));
	do {
		cout << "[Encrypt] enter cipher text file name: "; cin >> cipherTextFile;
	} while (!io.isFileExisted(cipherTextFile));

	return make_tuple(plainTextFile, cipherTextFile);
}

tuple<string, string> IO::inputFilesForDecryption()
{
	string cipherTextFile, decryptedFile;

	do {
		cout << "[Decrypt] enter cipher text file name: "; cin >> cipherTextFile;
	} while (!io.isFileExisted(cipherTextFile));
	do {
		cout << "[Decrypt] enter decrypted plain text file name: "; cin >> decryptedFile;
	} while (!io.isFileExisted(decryptedFile));

	return make_tuple(cipherTextFile, decryptedFile);
}