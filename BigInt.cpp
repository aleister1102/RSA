#include "BigInt.h";
#include "BigIntConverter.h"
#include "BigIntIO.h"

uint32_t getMaxByteCount(int a, int b)
{
	return a > b ? a : b;
}

byte getLastByte(const BigInt& n)
{
	return n.bytes[n.byteCount - 1];
}

byte getFirstByte(const BigInt& n)
{
	return n.bytes[0];
}

byte getLastBit(const BigInt& n)
{
	byte lastByte = getLastByte(n);
	return lastByte & ByteMask[7];
}

byte getFirstBit(const BigInt& n)
{
	byte firstByte = getFirstByte(n);
	return firstByte & ByteMask[0];
}

bool BigInt::isPositive()
{
	byte lastBit = getLastBit(*this);
	bool res = *this != 0 && lastBit == 0; // khác 0 và có bit cuối là 0
	return  res;
}

bool BigInt::isNegative()
{
	byte lastBit = getLastBit(*this);
	bool res = *this != 0 && lastBit != 0; // khác 0 và có bit cuối khác 0
	return res;
}

bool BigInt::isOdd()
{
	byte firstBit = getFirstBit(*this);
	return firstBit == 1;
}

bool BigInt::isEven()
{
	byte firstBit = getFirstBit(*this);
	return firstBit == 0;
}

byte getBit(byte number, int index)
{
	return (number >> index) & 1;
}

void setBit(byte& number, int index, byte bit)
{
	if (bit)
	{
		number = (1 << index) | number;
	}
	else {
		number = (~(1 << index)) & number;
	}
}

void swap(BigInt& a, BigInt& b) {
	BigInt temp = a;
	a = b;
	b = temp;
}

void addMoreBytes(BigInt& n, int amount)
{
	n.byteCount += amount;
	auto newMem = (byte*)realloc(n.bytes, n.byteCount * sizeof(byte));
	n.bytes = newMem ? newMem : nullptr;

	int paddingPosition = n.byteCount - amount;
	for (int i = 0; i < amount; i++)
	{
		if (n.bytes)
			n.bytes[paddingPosition + i] = ZERO;
	}
}

void removeLastBytesIfNull(BigInt& n)
{
	byte lastByte = getLastByte(n);

	while (lastByte == 0 && n.byteCount > 1)
	{
		n.byteCount -= 1;
		auto newMem = (byte*)realloc(n.bytes, n.byteCount * sizeof(byte));
		n.bytes = newMem ? newMem : nullptr;

		lastByte = getLastByte(n);
	}
}

void shareByteCount(BigInt& a, BigInt& b)
{
	// Trường hợp hai số có kích thước byte khác nhau
	if (a.byteCount != b.byteCount)
	{
		// Lấy số có kích thước byte ít hơn và cấp phát thêm vùng nhớ
		BigInt* lesserByteNumber = (a.byteCount > b.byteCount) ? &b : &a;
		addMoreBytes(*lesserByteNumber, abs((int)a.byteCount - (int)b.byteCount));
	}
}

BigInt operator+(BigInt a, BigInt b)
{
	uint32_t maxByteCount = getMaxByteCount(a.byteCount, b.byteCount);
	BigInt res(maxByteCount + 1); // cấp phát dư 1 byte

	shareByteCount(a, b);

	// Nếu tổng hai byte có giá trị lớn hơn 255
	// thì cần phải nhớ 1
	bool carry = a.bytes[0] + b.bytes[0] > 255 ? 1 : 0;
	res.bytes[0] = a.bytes[0] + b.bytes[0];

	for (int i = 1; i < maxByteCount; i++) {
		res.bytes[i] = a.bytes[i] + b.bytes[i] + (carry ? 1 : 0);

		// Nếu tổng hai byte và nhớ một ở byte trước cũng lớn hơn 255
		// thì nhớ 1 sang byte tiếp theo55
		carry = (a.bytes[i] + b.bytes[i] + (carry ? 1 : 0)) > 255;
	}

	// Nếu sau khi cộng tất cả các byte mà vẫn còn nhớ
	// thì cộng sang byte dư ra
	if (carry)
	{
		res.bytes[res.byteCount - 1] += 1;
	}

	// Kiểm tra nếu byte dư ra là rỗng thì xóa
	removeLastBytesIfNull(res);

	//io.writeOutputs(a, b, res, " + ");

	return res;
}

BigInt operator+(BigInt a, int value)
{
	BigInt result(a.byteCount);
	BigInt b = value;

	result = a + b;

	return result;
}

void operator+=(BigInt& a, BigInt b)
{
	a = a + b;
}

void twoComplement(BigInt& n)
{
	for (int i = 0; i < n.byteCount; i++)
	{
		// Lật bit
		n.bytes[i] = ~n.bytes[i];
	}

	n = n + 1;
}

BigInt operator-(BigInt a, BigInt b)
{
	uint32_t maxByteCount = getMaxByteCount(a.byteCount, b.byteCount);
	BigInt res(maxByteCount);

	shareByteCount(a, b);

	// Cách 1: trừ từng byte
	//
	// Nếu hiệu hai byte có giá trị nhỏ hơn 0
	// thì cần phải mượn 1
	bool borrow = (a.bytes[0] - b.bytes[0] < 0) ? 1 : 0;
	res.bytes[0] = a.bytes[0] - b.bytes[0];

	for (int i = 1; i < maxByteCount; i++) {
		res.bytes[i] = a.bytes[i] - b.bytes[i] - (borrow ? 1 : 0);

		// Nếu hiệu hai byte và mượn 1 ở byte trước cũng nhỏ hơn 0
		// thì mượn của byte tiếp theo
		borrow = (a.bytes[i] - b.bytes[i] - (borrow ? 1 : 0)) < 0;
	}

	//io.writeOutputs(a, b, res, " - ");

	return res;
}

BigInt operator-(BigInt a, int value)
{
	BigInt res(a.byteCount);
	BigInt b = value;

	res = a - b;
	return res;
}

void operator-=(BigInt& a, BigInt b)
{
	a = a - b;
}

bool operator==(BigInt a, BigInt b)
{
	// Kiểm tra xem hiệu của hai số có khác 0 hay không
	BigInt difference = a - b;
	bool res = true;

	for (int i = 0; i < difference.byteCount; i++)
	{
		if (difference.bytes[i] != 0)
		{
			res = false;
			break;
		}
	}

	//io.writeOutputs(a, b, res, " == ");

	return res;
}

bool operator==(BigInt a, int value)
{
	BigInt b = value;

	return a == b;
}

bool operator!=(BigInt a, BigInt b)
{
	bool res = !(a == b);

	//io.writeOutputs(a, b, res, " != ");
	return res;
}

bool operator!=(BigInt a, int value)
{
	BigInt b = value;

	return a != b;
}

void fillLastBytesWithNull(BigInt& n, int amount)
{
	int end = n.byteCount < amount ? 0 : n.byteCount - amount;

	for (int i = n.byteCount - 1; i >= end; i--)
	{
		n.bytes[i] = ZERO;
	}
}

void shiftByteRight(BigInt& n, int distance)
{
	for (int i = 0; i < n.byteCount - distance; i++)
	{
		n.bytes[i] = n.bytes[i + distance];
	}

	fillLastBytesWithNull(n, distance);
}

void copyLowBitsToHighBits(byte& a, byte& b, int amount)
{
	for (int i = 0; i < amount; i++)
	{
		// Lấy ra bit thứ i của a
		byte ithBit = getBit(a, i);

		// Gán bit đó cho bit thứ 8 - amount + i của b
		setBit(b, 8 - amount + i, ithBit);
	}
}

BigInt operator>>(BigInt n, int steps)
{
	BigInt res = n;
	int byteDistance = steps / 8;
	int bitDistance = steps % 8;

	//int extraBytes = byteDistance + (bitDistance ? 1 : 0);
	//// Thêm byte mở rộng
	//addMoreBytes(res, extraBytes);
	//// Dịch các byte về bên trái
	//shiftByteLeft(res, extraBytes);

	if (byteDistance)
	{
		shiftByteRight(res, byteDistance);
	}

	if (bitDistance) {
		for (int i = 0; i < res.byteCount; i++)
		{
			res.bytes[i] >>= bitDistance;

			if (i < res.byteCount - 1)
			{
				copyLowBitsToHighBits(res.bytes[i + 1], res.bytes[i], bitDistance);
			}
		}
	}

	//io.writeOutputs(n, steps, res, " >> ");
	return res;
}

void operator >>= (BigInt& number, int steps) {
	number = number >> steps;
}

void fillFirstBytesWithNull(BigInt& n, int amount)
{
	int end = n.byteCount < amount ? n.byteCount : amount;
	for (int i = 0; i < end; i++)
	{
		n.bytes[i] = ZERO;
	}
}

void shiftByteLeft(BigInt& n, int distance)
{
	for (int i = n.byteCount - 1; i >= distance - 1; i--)
	{
		n.bytes[i] = n.bytes[i - distance];
	}

	fillFirstBytesWithNull(n, distance);
}

void copyHighBitsToLowBits(byte a, byte& b, int amount)
{
	for (int i = 7; i > 7 - amount; i--)
	{
		// Lấy ra bit thứ i của a
		byte ithBit = getBit(a, i);

		// Gán bit đó cho bit thứ i + amount - 8 của b
		setBit(b, i + amount - 8, ithBit);
	}
}

BigInt operator<<(BigInt n, int steps)
{
	int byteDistance = steps / 8;
	int bitDistance = steps % 8;
	BigInt res = n;

	// Thêm byte mở rộng
	addMoreBytes(res, byteDistance + (bitDistance ? 1 : 0));

	if (byteDistance)
	{
		shiftByteLeft(res, byteDistance);
	}

	if (bitDistance) {
		for (int i = res.byteCount - 1; i >= 0; i--)
		{
			res.bytes[i] <<= bitDistance;

			if (i > 0)
			{
				copyHighBitsToLowBits(res.bytes[i - 1], res.bytes[i], bitDistance);
			}
		}
	}

	//io.writeOutputs(n, steps, res, " << ");
	return res;
}

void operator<<=(BigInt& number, int steps)
{
	number = number << steps;
}

BigInt operator &(BigInt a, BigInt b) {
	uint32_t maxByteCount = getMaxByteCount(a.byteCount, b.byteCount);
	BigInt res(maxByteCount);

	shareByteCount(a, b);

	for (int i = 0; i < maxByteCount; i++)
	{
		res.bytes[i] = a.bytes[i] & b.bytes[i];
	}

	//io.writeOutputs(a, b, res, " & ");
	return res;
}

BigInt operator |(BigInt a, BigInt b)
{
	uint32_t maxByteCount = getMaxByteCount(a.byteCount, b.byteCount);
	BigInt res(maxByteCount);

	shareByteCount(a, b);

	for (int i = 0; i < maxByteCount; i++)
	{
		res.bytes[i] = a.bytes[i] | b.bytes[i];
	}

	//io.writeOutputs(a, b, res, " | ");
	return res;
}

bool operator<(BigInt a, BigInt b) {
	shareByteCount(a, b);

	BigInt different = a - b;

	bool res = different.isNegative() && a != b;

	//io.writeOutputs(a, b, res, " < ");
	return res;
}

bool operator<=(BigInt a, BigInt b)
{
	BigInt difference = a - b;

	bool res = difference.isNegative() || a == b;

	io.writeOutputs(a, b, res, " <= ");
	return res;
}

bool operator>(BigInt a, BigInt b) {
	shareByteCount(a, b);

	BigInt different = a - b;

	bool res = different.isPositive() && a != b;

	io.writeOutputs(a, b, res, " > ");
	return res;
}

bool operator>=(BigInt a, BigInt b) {
	shareByteCount(a, b);

	BigInt different = a - b;

	bool res = different.isPositive() || a == b;

	io.writeOutputs(a, b, res, " >= ");
	return res;
}

BigInt operator*(BigInt a, BigInt b)
{
	BigInt res = 0;
	BigInt q = a;
	BigInt p = b;

	// b sẽ là số nhỏ hơn
	if (a < b)
		swap(a, b);

	int k = 0;
	while (p != 0)
	{
		if (p.isOdd())
			res += q;

		q <<= 1; // nhân đôi q
		p >>= 1; // chia đôi q

		// Nếu đã dời hết một octet (7 lần)
		// thì cấp phát thêm cho a một byte nữa
		if (++k == 7)
		{
			addMoreBytes(q, 1);
			k = 0;
		}
	}

	io.writeOutputs(a, b, res, " * ");
	return res;
}

uint32_t getBitCount(BigInt& n) {
	uint32_t bitCount = (n.byteCount * 8);

	for (int i = 7; i >= 0; i--) {
		if (getBit(n.bytes[n.byteCount - 1], i) == 1) break;
		bitCount--;
	}
	return bitCount;
}

bool isLesserThanZero(BigInt& n)
{
	byte lastBit = getLastBit(n);
	bool result = lastBit == 0 ? false : true;
	return result;
}

BigInt operator/(BigInt a, BigInt b)
{
	io.displayInputs(a, b, "/");

	uint32_t maxByteCount = getMaxByteCount(a.byteCount, b.byteCount);

	BigInt r(maxByteCount);
	int k = b.byteCount * 8;

	while (k > 0)
	{
		// Dịch trái chuỗi [r, a]

		// Lấy bit cuối của a
		byte lastBitA = getLastBit(a);

		//cout << "Before shift: " <<
		//	io.outputBin(r) << " " <<
		//	io.outputBin(a) << endl;

		a << 1;
		r << 1;

		// Gán cho bit đầu của r
		setBit(r.bytes[0], 0, lastBitA);

		cout << "After shift: " <<
			io.outputBin(r) << " " <<
			io.outputBin(a) << endl;

		r -= b;
		cout << "After subtract: " << io.outputBin(r) << endl;

		// Nếu r < 0
		if (isLesserThanZero(r))
		{
			// Khôi phục r và gán bit đầu của a là 0
			r += b;
			setBit(a.bytes[0], 0, 0);

			cout << "Restore: " << io.outputBin(r) << endl;
		}
		else
		{
			// Gán bit đầu của a là 1
			setBit(a.bytes[0], 0, 1);
		}

		k -= 1;
	}

	return a;
}