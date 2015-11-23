#include "crypto.hpp"
using std::string;
using std::to_string;
#include <algorithm>
using std::fill;
#include <random>
using std::random_device;
using std::uniform_int_distribution;
#include <vector>
using std::vector;
#include <sstream>
using std::stringstream;
#include <iomanip>
using std::hex;

#include "util.hpp"

extern "C" {
#include "lib/crypto/crypto_scrypt.h"
}

static char toHexDigit(int i) {
	if(i >= 0 && i <= 9)
		return '0' + i;
	if(i >= 10 && i <= 15)
		return 'a' + i - 10;
	throw -1; // TODO
}
static string toHex(uint8_t byte) {
	return string(1, toHexDigit((byte & 0xf0) >> 4)) +
		string(1, toHexDigit(byte & 0x0f));
}
static string toHexString(long num) {
	stringstream ss;
	ss << hex << num;
	return ss.str();
}

string crypto::generateSalt(int size) {
	random_device rd;
	uniform_int_distribution<int> dist(0, 15);
	string salt(size, ' ');
	for(int i = 0; i < size; ++i)
		salt[i] = toHexDigit(dist(rd));
	return salt;
}

string crypto::scrypt(string secret, string salt) {
	if(salt.empty())
		salt = generateSalt();

	int hashVersion = 0;
	int N = 0x400, r = 0x8, p = 0x26; // TODO
	uint8_t buf[32] = { 0 };
	fill(buf, buf + 32, 0);
	int res = crypto_scrypt((uint8_t *)secret.c_str(), secret.length(),
			(uint8_t *)salt.c_str(), salt.length(), N, r, p, buf, 32);
	if(res != 0)
		throw -1; // TODO

	// just because we can
	uint8_t magic[6] = { 't', 'p', 'y', 'r', 'c', 's' };
	string b16;
	for(int i = 0; i < 32; ++i)
		b16 += toHex(buf[i] ^ magic[i % 6]);

	return toHexString(hashVersion)
		+ "$" + toHexString(N)
		+ "$" + toHexString(r)
		+ "$" + toHexString(p)
		+ "$" + salt + "$" + b16;
}

bool crypto::verify(string secret, string hash) {
	vector<string> parts = util::split(hash, "$");
	if(parts.size() != 6)
		return false;
	int hashVersion = 0;
	int N = 0x400, r = 0x8, p = 0x26; // TODO
	if(parts[0] != toHexString(hashVersion)
			|| parts[1] != toHexString(N)
			|| parts[2] != toHexString(r)
			|| parts[3] != toHexString(p))
		return false;
	return scrypt(secret, parts[4]) == hash;
}

