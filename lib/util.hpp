#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <string>
#include <map>
#include <utility>

namespace util {
	template<typename T> T split(std::string str, std::string on = " \t\r\n");
	std::vector<std::string> split(std::string str, std::string on = " \t\r\n");

	// join range of arbitrary T into a string (uses <<)
	template<typename It>
		std::string join(It strsB, It strsE, std::string with = ", ");
	// join an entire container C into a string (uses <<)
	template<typename C> std::string join(C strs, std::string with = ", ");

	template<typename T> T noop(T t);

	// same as above, but transforms all elements with F first
	template<typename It, typename F>
		std::string join(It strsB, It strsE, std::string with, F func);
	template<typename C, typename F>
		std::string join(C strs, std::string with, F func);
	// TODO: get with to default to something?

	template<typename T> std::vector<T> subvector(std::vector<T> vec,
			size_t s, size_t n);
	template<typename T> std::vector<T> last(std::vector<T> vec, size_t n);

	std::string trim(std::string str, std::string what = " \t\r\n");
	std::string trimWhitespace(std::string str);

	bool startsWith(std::string str, std::string prefix);
	bool endsWith(std::string str, std::string suffix);

	template<typename T> T fromString(std::string str);
	template<typename T> std::string asString(T val);

	template<typename T>
			bool contains(std::vector<T> vec, T val);
	template<typename K, typename V>
			bool contains(std::map<K, V> map, K key);
	template<typename K, typename V>
			bool contains(std::vector<std::pair<K, V>> list, K key);
	bool contains(std::string str, char c);
	bool contains(std::string str, std::string s);

	// appends an ordinal string to the number (1 -> 1st, 2 -> 2nd, etc)
	std::string toOrdinal(long num);

	namespace file {
		bool exists(std::string filename);
		bool executable(std::string filename);
		//time_t mtime(std::string filename);
	}
}

#include "util.imp"

#endif // UTIL_HPP
