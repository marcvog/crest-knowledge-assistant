#ifndef JWT_UTILS_H
#define JWT_UTILS_H

#include <ctime>
#include <string>

namespace Crest {

/**
 * Utility class for decoding and inspecting JWT (JSON Web Token) claims.
 */
class JwtUtils {
 public:
  /**
   * Extracts the `exp` (expiration) claim from a JWT access token.
   * Optionally subtracts a grace period to account for clock skew.
   *
   * @param jwt A JWT string (in the format header.payload.signature).
   * @param graceSeconds Optional number of seconds to subtract from the
   * extracted expiration time.
   * @return The expiration timestamp as time_t (Unix time).
   * @throws std::runtime_error if the token is malformed or the exp claim is
   * missing.
   */
  static std::time_t extractExp(const std::string& jwt, int graceSeconds = 0);

 private:
  /**
   * Decodes a Base64URL-encoded string (used in JWT header and payload).
   *
   * @param input The Base64URL-encoded string.
   * @return The decoded string.
   */
  static std::string base64urlDecode(const std::string& input);
};
}  // namespace Crest

#endif  // JWT_UTILS_H
