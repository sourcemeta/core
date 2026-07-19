#ifndef SOURCEMETA_CORE_CRYPTO_WINDOWS_H_
#define SOURCEMETA_CORE_CRYPTO_WINDOWS_H_

// CNG key export helpers shared by the signing and verification backends, so
// the curve mapping and the native blob export stay single-sourced

#include <sourcemeta/core/crypto_verify.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h> // ULONG, LPCWSTR

#include <bcrypt.h> // BCryptExportKey, BCRYPT_KEY_HANDLE, BCRYPT_SUCCESS

#include <cstddef>  // std::size_t
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string

namespace sourcemeta::core {

inline auto ec_curve_from_field_bytes(const std::size_t field_bytes) noexcept
    -> std::optional<EllipticCurve> {
  switch (field_bytes) {
    case 32:
      return EllipticCurve::P256;
    case 48:
      return EllipticCurve::P384;
    case 66:
      return EllipticCurve::P521;
    default:
      return std::nullopt;
  }
}

// Export a native key blob, sizing the buffer in a first call and filling it in
// a second
inline auto export_key_blob(BCRYPT_KEY_HANDLE key, LPCWSTR blob_type)
    -> std::optional<std::string> {
  ULONG size{0};
  if (!BCRYPT_SUCCESS(
          BCryptExportKey(key, nullptr, blob_type, nullptr, 0, &size, 0))) {
    return std::nullopt;
  }

  std::string blob;
  blob.resize(size);
  if (!BCRYPT_SUCCESS(BCryptExportKey(
          key, nullptr, blob_type,
          reinterpret_cast<unsigned char *>(blob.data()), size, &size, 0))) {
    return std::nullopt;
  }

  blob.resize(size);
  return blob;
}

} // namespace sourcemeta::core

#endif
