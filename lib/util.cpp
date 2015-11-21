#include "util.hpp"
using std::vector;
using std::string;
#include <unistd.h>

#include <random>
using std::random_device;
using std::default_random_engine;
using std::uniform_int_distribution;

string util::charset::lower = "abcdefghijklmnopqrstuvwxyz";
string util::charset::upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
string util::charset::alpha = lower + upper;
string util::charset::num = "0123456789";
string util::charset::alnum = alpha + num;

static random_device rd{};
static default_random_engine rengine{rd()};

vector<string> util::split(string str, string on) {
	vector<string> fields;
	size_t idx = 0, m = 0;
	while((idx = str.find_first_not_of(on, idx)) != string::npos) {
		m = str.find_first_of(on, idx);
		// str ends with a word
		if(m == string::npos) {
			fields.push_back(str.substr(idx));
			return fields;
		}

		// we have a word spanning idx to m
		fields.push_back(str.substr(idx, m - idx));

		// jump to next potential
		idx = m;
	}
	return fields;
}

string util::trim(string str, string what) {
	size_t firstNotOf = str.find_first_not_of(what);
	if(firstNotOf == string::npos)
		return str;
	str = str.substr(firstNotOf);
	return str.substr(0, str.find_last_not_of(what) + 1);
}
string util::trimWhitespace(std::string str) {
	return trim(str, " \t\r\n");
}

bool util::file::exists(string filename) {
	return (access(filename.c_str(), F_OK) == 0);
}
bool util::file::executable(string filename) {
	return (access(filename.c_str(), X_OK) == 0);
}

bool util::startsWith(string str, string prefix) {
	if(prefix.length() > str.length())
		return false;
	return str.substr(0, prefix.length()) == prefix;
}
bool util::endsWith(string str, string suffix) {
	if(suffix.length() > str.length())
		return false;
	return str.substr(str.length() - suffix.length()) == suffix;
}

bool util::contains(std::string str, char c) {
	return (str.find(c) != string::npos);
}
bool util::contains(string str, string s) {
	return (str.find(s) != string::npos);
}

string util::toOrdinal(long num) {
	string nums = asString(num);
	switch(nums[nums.length() - 1]) {
		case '0': case '4': case '5': case '6': case '7': case '8': case '9':
			nums += "th"; break;
		case '1': nums += "st"; break;
		case '2': nums += "nd"; break;
		case '3': nums += "rd"; break;
		default:
			break;
	}
	return nums;
}

char util::randomCharacter(string set) {
	uniform_int_distribution<int> udist(1, set.length());
	return set[udist(rengine) - 1];
}
string util::randomString(long length, string set) {
	string res; res.reserve(length);
	for(int i = 0; i < length; ++i)
		res += randomCharacter(set);
	return res;
}

