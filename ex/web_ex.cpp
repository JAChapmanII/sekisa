#include <iostream>
using std::cout;
using std::endl;

#include "web.hpp"

int main(int, char **) {
	auto response = web::post("https://serv2.pink/api/umiki/v1", {
			{ "api_key", "bad" }
		});

	cout << "==== HTTP CODE ====" << endl;
	cout << response.code << endl;
	cout << "==== HTTP BODY ====" << endl;
	cout << response.body << endl;
	cout << "==== HEADERS ====" << endl;
	for(auto &header : response.headers)
		cout << header.first << ": " << header.second << endl;

	return 0;
}

