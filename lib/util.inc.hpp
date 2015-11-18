#include <sstream>
#include <algorithm>

template<typename T> T util::noop(T t) { return t; }

template<typename It> std::string util::join(
		It strsB, It strsE, std::string with) {
	return util::join(strsB, strsE, with, noop<decltype(*strsB)>);
}
template<typename C> std::string util::join(C strs, std::string with) {
	return util::join(std::begin(strs), std::end(strs), with);
}

template<typename It, typename F> std::string util::join(
		It strsB, It strsE, std::string with, F func) {
	if(strsE == strsB) // TODO: begin after end?
		return "";

	std::string res{""};
	for(auto i = strsB; i != strsE; ++i)
		res += (std::string)(func(*i)) + with;

	return res.substr(0, res.length() - with.length());
}
template<typename C, typename F> std::string util::join(
		C strs, std::string with, F func) {
	return util::join(std::begin(strs), std::end(strs), with, func);
}

template<typename T> std::vector<T> util::subvector(std::vector<T> vec,
		size_t s, size_t n) {
	std::vector<T> res;
	if(s >= vec.size())
		return res;
	if(s + n >= vec.size())
		n = vec.size() - s;
	res.insert(res.begin(), vec.begin() + s, vec.begin() + s + n);
	return res;
}
template<typename T> std::vector<T> util::last(std::vector<T> vec, size_t n) {
	if(n > vec.size())
		n = vec.size();
	return subvector(vec, vec.size() - n, n);
}

template<typename T> T util::fromString(std::string str) {
	std::stringstream ss(str);
	T result;
	ss >> result;
	return result;
}
template<typename T> std::string util::asString(T val) {
	std::stringstream ss;
	ss << val;
	return ss.str();
}

template<typename T> bool
		util::contains(std::vector<T> vec, T val) {
	return (std::find(vec.begin(), vec.end(), val) != vec.end());
}
template<typename K, typename V> bool
		util::contains(std::map<K, V> map, K key) {
	return (map.find(key) != map.end());
}
template<typename K, typename V>
		bool util::contains(std::vector<std::pair<K, V>> list, K key) {
	for(auto i : list)
		if(i.first == key)
			return true;
	return false;
}

