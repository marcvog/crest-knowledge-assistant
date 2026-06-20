#include <CrestApi/CrestApiBase.h>
#include <openssl/evp.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "version.h"

namespace Crest {

std::string CrestApiBase::getHash(std::string_view data) {
  // Create a context for the digest operation
  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (mdctx == nullptr) {
    throw std::runtime_error("Failed to create EVP_MD_CTX");
  }

  // Initialize the context with SHA-256
  if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Failed to initialize SHA-256 digest");
  }

  // Update the digest with the input data
  if (EVP_DigestUpdate(mdctx, data.data(), data.size()) != 1) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Failed to update SHA-256 digest");
  }

  // Finalize the hash computation
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;
  if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Failed to finalize SHA-256 digest");
  }

  // Convert the hash to a hexadecimal string
  std::string out;
  out.reserve(hash_len * 2);  // Reserve space to avoid reallocations
  for (std::size_t i = 0; i < hash_len; ++i) {
    out.push_back("0123456789abcdef"[hash[i] >> 4]);    // High nibble
    out.push_back("0123456789abcdef"[hash[i] & 0x0F]);  // Low nibble
  }

  // Clean up and return
  EVP_MD_CTX_free(mdctx);
  return out;
}

std::string CrestApiBase::getHashForFile(const std::string &file_path) {
  // Create a context for the digest operation
  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (mdctx == nullptr) {
    throw std::runtime_error("Failed to create EVP_MD_CTX");
  }

  // Initialize the context with SHA-256
  if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Failed to initialize SHA-256 digest");
  }

  // Open the file in binary mode
  std::ifstream file(file_path, std::ios::binary);
  if (!file) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Unable to open file: " + file_path);
  }

  // Read the file in chunks
  const size_t buffer_size = 8192;
  std::vector<unsigned char> buffer(buffer_size);
  while (file.read(reinterpret_cast<char *>(buffer.data()), buffer.size())) {
    if (EVP_DigestUpdate(mdctx, buffer.data(), file.gcount()) != 1) {
      EVP_MD_CTX_free(mdctx);
      throw std::runtime_error("Failed to update SHA-256 digest");
    }
  }
  if (EVP_DigestUpdate(mdctx, buffer.data(), file.gcount()) != 1) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Failed to finalize SHA-256 digest update");
  }

  // Finalize the hash computation
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;
  if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
    EVP_MD_CTX_free(mdctx);
    throw std::runtime_error("Failed to finalize SHA-256 digest");
  }

  // Convert the hash to a hexadecimal string
  std::stringstream ss;
  for (unsigned int i = 0; i < hash_len; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(hash[i]);
  }

  // Clean up and return
  EVP_MD_CTX_free(mdctx);
  return ss.str();
}

std::string CrestApiBase::getClientVersion() {
  return CRESTAPI_VERSION;
}

}  // namespace Crest
