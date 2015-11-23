#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>

namespace crypto {
	std::string generateSalt(int size = 40);

	std::string scrypt(std::string secret, std::string salt = "");

	bool verify(std::string secret, std::string hash);
}

#endif // CRYPTO_HPP
