#include "web.hpp"
using std::string;
using std::map;

#include <vector>
using std::vector;

#include <curl/curl.h>
#include "err.hpp"
#include "util.hpp"

struct CURLWrapper {
	CURLWrapper() {
		curl = curl_easy_init();
		if(!curl)
			throw make_except("curl_easy_init failed");

		this->setHeader("Expect", "");
	}
	~CURLWrapper() {
		if(headers)
			curl_slist_free_all(headers);
		if(curl)
			curl_easy_cleanup(curl);
	}

	template<typename T>
	CURLcode setopt(CURLoption option, T parameter) {
		return curl_easy_setopt(curl, option, parameter);
	}

	void setHeader(string header, string value) {
		auto combined = header + ": " + value;
		headers = curl_slist_append(headers, combined.c_str());
	}

	CURLcode perform() {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		return curl_easy_perform(curl);
	}

	long getCode() {
		long code{-1};
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
		return code;
	}

	CURL *curl{nullptr};
	curl_slist *headers{nullptr};
};

struct CURLHeaders {
	CURLHeaders() {
	}
	~CURLHeaders() {
	}
};

namespace web {
	string escape(string str);
	size_t writeCallback(void *data, size_t size, size_t memb, void *userdata);
	size_t headerCallback(void *data, size_t size, size_t memb, void *userdata);

	// TODO: request headers?
	// TODO: timeout/abort?
	Response post(Uri uri, Parameters params) {
		Response response{};

		// TODO: make Parameters a real struct, add this method?
		string postData{};
		// TODO: escaping
		vector<string> postParameters{};
		for(auto &param : params)
			// TODO: better escaping
			postParameters.push_back(escape(param.first) + "=" + escape(param.second));

		postData = util::join(postParameters, "&");

		CURLWrapper curl{};
		curl.setopt(CURLOPT_USERAGENT, "sekisa/0.0");
		curl.setopt(CURLOPT_URL, uri.c_str());

		curl.setopt(CURLOPT_POST, 1L);
		curl.setopt(CURLOPT_POSTFIELDS, postData.c_str());
		curl.setopt(CURLOPT_POSTFIELDSIZE, postData.size());

		curl.setopt(CURLOPT_WRITEFUNCTION, writeCallback);
		curl.setopt(CURLOPT_WRITEDATA, &response);

		curl.setopt(CURLOPT_HEADERFUNCTION, headerCallback);
		curl.setopt(CURLOPT_HEADERDATA, &response);

		auto res = curl.perform();
		if(res != CURLE_OK) {
			// res is not an HTTP response code, but an error code
			response.code = -res;
			// TODO: throw?
			return response;
		}

		response.code = curl.getCode();
		return response;
	}

	string escape(string str) {
		string res{""};
		for(char c : str)
			switch(c) {
				case '&':
					res += "%26";
					break;
				case '=':
					res += "%3D";
					break;
				default:
					res += c;
			}
		return res;
	}

	size_t writeCallback(void *data, size_t size, size_t memb, void *userdata) {
		Response &response = *(Response *)userdata;
		// TODO: bleh
		response.body.append((char *)data, size * memb);

		return (size * memb);
	}

	size_t headerCallback(void *data, size_t size, size_t memb, void *userdata) {
		Response &response = *(Response *)userdata;
		string headerData{(char *)data, size * memb};

		auto fields = util::split(headerData, ":");
		if(fields.size() == 1)
			fields.push_back("");

		string header = fields[0],
				content = util::join(fields.begin() + 1, fields.end(), ":");

		response.headers[util::trim(header)] = util::trim(content);

		return (size * memb);
	}
}

