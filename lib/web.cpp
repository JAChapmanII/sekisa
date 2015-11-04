#include "web.hpp"
using std::string;
using std::map;

#include <vector>
using std::vector;

#include <curl/curl.h>
#include "err.hpp"
#include "util.hpp"

namespace web {
	string escape(string str);
	size_t writeCallback(void *data, size_t size, size_t memb, void *userdata);
	size_t headerCallback(void *data, size_t size, size_t memb, void *userdata);

	// TODO: request headers?
	// TODO: timeout/abort?
	Response post(Uri uri, Parameters params) {
		CURL *curl{nullptr};
		CURLcode res{CURLE_OK};
		Response response{};

		// TODO: make Parameters a real struct, add this method?
		string postData{};
		// TODO: escaping
		vector<string> postParameters{};
		for(auto &param : params)
			// TODO: better escaping
			postParameters.push_back(escape(param.first) + "=" + escape(param.second));

		postData = util::join(postParameters, "&");


		curl = curl_easy_init();
		if(!curl)
			throw make_except("curl_easy_init failed");

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "sekisa/0.0");
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

		curl_slist *headers{nullptr};
		headers = curl_slist_append(headers, "Expect: ");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			// res is not an HTTP response code, but an error code
			response.code = -res;
			// TODO: throw?
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
			return response;
		}

		// grab actual HTTP response code
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.code);

		// TODO: RAII?
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

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

