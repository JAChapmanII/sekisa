#ifndef WEB_HPP
#define WEB_HPP

#include <string>
#include <map>

namespace web {
	using Headers = std::map<std::string, std::string>;
	using Parameters = std::map<std::string, std::string>;
	using Uri = std::string;

	struct Response {
			long code;
			std::string body;
			Headers headers;
	};

	Response post(Uri uri, Parameters params);
}

#endif // WEB_HPP
