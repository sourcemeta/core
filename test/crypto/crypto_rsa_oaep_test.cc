#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// A 2048-bit RSA test key in unencrypted PKCS#8, from which the public key is
// derived for the encryption side of each round trip
static const std::string PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCwidOHq7tHMyQk
rKtggHsdpTNL7fv+Rjlve5AgNGbTDMJCWga7LbOxS1B/iaOVyn7pNX4+XVXgRGYG
qwOOaiKnU0S2aE7TL7aqTwaPLABGS3e0A2yx1vm6sDMi5UpvboEX5Da49xZp5nU3
EFS87UEGStBggySt6L1CFClaYyp7hq7C+/ah3DVQlrLYQzWACIm6U5T3MEMrC3kH
efg0RUPlFUgoBlX6JtA51Z7TB5BDsUGwj9oQhdrSOaMenYtsoVaDqna1SMZcsxJ7
x625rWaSz7F2gHJ+MVxsbFzMnrjF58OirgAO+6ARFkXbi7zzsDWl9eK1VRMe+61I
c4Bc4D2RAgMBAAECggEACUtlXZmOG3Fa6hsk7fiPutOmee08gOJ8vkTJtZdasUqE
/QIxQ+gz424LHb7e8+lIkaHPTOxkM8G4IypcbmJiQw/eUJH48fObdPeiiVXMSScq
pQmv3qRaSzDhDVmX99pQHcRWOPpp1b8kXVb81iJR48lbM5SeARG9iqHF/Fg3na+W
+XIzDS0+gHav7E+rmsKG1fqyyyXiTx+0dfx074/8FM7q8USeXaafeAEWIngXFiro
7xSVs081QOt80XmOtpOwz3yBz/PZX2WRDW2U0bfzWDAb9Fx+8wkL/HX6q/ozj5tH
B+x4ZQCA3+3aN6aYUqDEFk4damboi8C1QDVvnHPGAQKBgQDkN6RREwyP0USnbDGq
nrqKj9VH9Mkw34jl4AmvRqUHg9vX/j5z8SEm7sTrEPM2ZKUQSEvF6ZejBHTmiM1v
+5VE3+awVN1ejRCutZ6J4fXlBv4hqPo2yxZByKdssXwZHeCQXn1sMDhWkZCLsBcW
vV9isLTn7QRoyWU0AsdXkruDkQKBgQDGB54TNztJuR8qYlZtmh7FJp+crBC5AwRw
ibM487c3RWpxYNUcprsBJu3NaTmy3tR3Vdr0GowJagG5frvq0PtYvIRladNUwx+S
ZCL0Twl7BCGkncVGTWaahookxc7oSZiwKStYt4IVtiFm8WY2GTmsc3JUaV6oyskP
O48d3kgaAQKBgC9MtPq0twLLva3xh620LjdvriW2v0go3EfAkmFIhAW1t4fhwvHm
xRc7n4aUwcPBQlDImyPO0UXRM6VgLywJeZuHyFk9jY/+Lrn3HH+5XGx2cY9qZUmb
9+X0f3bQpkbSYWuqmdYlfFe83beF7JMi9rUOqL/06NgK/bW06bnlyIVxAoGBALgz
HbaCCYPpzjN+hSYt1bFM2+q/ok51oAuOCkdPa3zd9MTBbXnZHNlVfJ0045ad7sOR
+LhWeSvIjxIb8G/bM/C2afBrH0gYt6VTv6/AVPE4RpL4ridWl7nkq1rJJEytKHmz
0b4T88x+PVOj948ap6rhN9eMq6AWAC9LFzSRLkABAoGBAMiDTmhfnkMtv3Uh4ruT
vqfGU9UbfEmK5SGb6YTClT9hIZS5erKSwfETRsWqkRLzgFG0BkLQRzfxn/29DMQM
QHq1PUPjkaKpLxvFyUVnuOw0kkO3xTQPStepKrJpdvzzWCxgTa7wkeBagxW+Ibp1
VoqHMouZBlcLfRprpPpQWlay
-----END PRIVATE KEY-----
)"};

// A 2048-bit key whose PKCS#8 algorithm is id-RSASSA-PSS rather than
// rsaEncryption, so it is restricted to PSS and must refuse OAEP decryption
static const std::string PSS_RESTRICTED_PRIVATE_KEY_PEM{
    R"(-----BEGIN PRIVATE KEY-----
MIIEugIBADALBgkqhkiG9w0BAQoEggSmMIIEogIBAAKCAQEAmLPlf54G5cTEBgZs
sMSaCFtdTcy0Yjv6HAHK0BLYqdsnff6WMGmuSDdNZ/Oh++a3hTGV6ZdpvNFEo1MD
Te9CrIL9RwdrY+q9g4KsJhrmBdJPEfg0yIzJwTexQYiovy26X8KFQ3QJ3I6M3evZ
bjvskmIMbpvtbucT5JVRaQ1RwlxSzZj0EsY6ur5UVwgYYotsfmmqurzyyfqUrq6O
tifWiCDSpXk/0d920NikHFyR7xbRGVTXyL+iiCDHI7EO91NnA7RzIoFWRhAedkRQ
7Kt9qj4PdRwMc/bXeihq+7cb+zM17N1jcbLynu+39cPiq1YKqULCY3WiWDmjaq5U
XsONjQIDAQABAoIBAB0yHhG7ktmI+KqnPU5B1Kp+33TBzAZRLdV/gTmttMerC16z
V59bgVM04aObqQ+a0eFRNPazuKd9gmhQtZvHwGFv7QGQ2VdB+SiFCimB8JNR3cTT
hjIG+wcqgQVE3fCpi04GSMj4DW+iQKwojQqewfFN9k8KmIeg/kRwyR8zCPwGM36Y
OUPZmlhWTBYX3J/Qju2KQU+bvarJ+LWRifFl7CgTSFeIjOzFJfWf5yasP/U3CdEB
9qP+UhCswg2x2y+V0BeBM9KaEX5OOl/yAUggMYkQ5uq8ZOKpuCP4yS5yhd3Fvo1I
XIMCFBzfrzOyGKasSgQITo2Pltk0pOQUShyE+rECgYEA1wOh9AdvbQH3Qf19cvKZ
pGFs8zhoVEGJ2xz04wT78cfjZJE1Yd/WYN6GacN5dbYt/Psoc62v5l/xDB4MZXYh
B8noyAiYitIBkQ5P75ycrhjk+fqU/uhJvHNOb6tnu3XL4phJl2NmfYcnpMtjxVGT
fJdhfTGM6MScjYrxG711nf0CgYEAtc+O8KNdlSKufsrUgNzsJJxOgi2MOxDxu9Il
4YHoegl9Z6W1FCLVFyLcpkfCFjDewXQfyEhgkojwYDbbbwyHMOCugysnrEYWs1JX
btxRulHdzRhvIFHz3u51ewvAS6FDdpfM/dUcl1sdVcwRJ1c872zTOkcL9EMxqNto
xx8vetECgYBAWzX+dLtFRXFcryL9ZN/X89FIe3m+vl8k1mX2DWfb1piZYV05DmZ8
WB6jSX7xXLYnIoXZGgOsUMs1dUkAlXsNecHTHb+KzZDqef4zGg1Ljuf6aqZuJdjs
LxcrFYLW+UstZ6efSIFE0U9sY/RY+zHJ+QWVE1+5zB+PviasxuiNgQKBgGLutOuB
GhVjL+zS1lvg26b4X0g7HMmvaLs5mV9i32w46cKSyzxP0ACs+cCJ37VPlodSd1D3
AYX7ekIA19tPx+jy+kNqIkZ+RTADKIys2tQ2ZCmMmDvQHJI81DTGqjb9Y8aOx/+A
DfTWodnkF5l+wSvP3gkiTAD453bpHdTsxVthAoGAB3Um+ocT8vaeV1PaS90Ztll1
+mIMYna3Slag9HTvUJEDxndXDezR+vJPmX6bNgGyoSYu9mQ7yu9n/QlWl/eR2ilB
8xn3yBD8MzAdeRaWKQOrsg+2UIWW+I2H90xdyp57V15Hw0XTDZ05jAuflOFj+cPK
zz/p2wIRJlfNHxRap+Q=
-----END PRIVATE KEY-----
)"};

TEST(rsa_oaep_round_trips_with_sha256) {
  const std::string_view plaintext{"a content encryption key"};
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto wrapped{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, plaintext)};
  EXPECT_TRUE(wrapped.has_value());
  const auto unwrapped{sourcemeta::core::rsa_oaep_decrypt(
      private_key.value(), sourcemeta::core::RSAOAEPHash::SHA256,
      wrapped.value())};
  EXPECT_TRUE(unwrapped.has_value());
  EXPECT_EQ(unwrapped.value(), plaintext);
}

TEST(rsa_oaep_round_trips_with_sha1) {
  const std::string_view plaintext{"a content encryption key"};
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto wrapped{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA1, plaintext)};
  EXPECT_TRUE(wrapped.has_value());
  const auto unwrapped{sourcemeta::core::rsa_oaep_decrypt(
      private_key.value(), sourcemeta::core::RSAOAEPHash::SHA1,
      wrapped.value())};
  EXPECT_TRUE(unwrapped.has_value());
  EXPECT_EQ(unwrapped.value(), plaintext);
}

TEST(rsa_oaep_ciphertext_matches_the_modulus_size) {
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  const auto wrapped{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "short")};
  EXPECT_TRUE(wrapped.has_value());
  EXPECT_EQ(wrapped.value().size(), std::string::size_type{256});
}

TEST(rsa_oaep_encrypt_is_randomized) {
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  const auto first{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "identical")};
  const auto second{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "identical")};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  EXPECT_NE(first.value(), second.value());
}

TEST(rsa_oaep_decrypt_rejects_a_tampered_ciphertext) {
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  auto wrapped{
      sourcemeta::core::rsa_oaep_encrypt(
          public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "hello")
          .value()};
  wrapped.back() = static_cast<char>(wrapped.back() ^ 0x01);
  EXPECT_FALSE(
      sourcemeta::core::rsa_oaep_decrypt(
          private_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, wrapped)
          .has_value());
}

TEST(rsa_oaep_decrypt_rejects_the_wrong_hash) {
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  const auto wrapped{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "hello")};
  EXPECT_FALSE(sourcemeta::core::rsa_oaep_decrypt(
                   private_key.value(), sourcemeta::core::RSAOAEPHash::SHA1,
                   wrapped.value())
                   .has_value());
}

TEST(rsa_oaep_encrypt_rejects_a_too_long_plaintext) {
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  // The limit for SHA-256 over a 2048-bit modulus is 190 bytes
  const std::string plaintext(200, 'x');
  EXPECT_FALSE(
      sourcemeta::core::rsa_oaep_encrypt(
          public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, plaintext)
          .has_value());
}

TEST(rsa_oaep_decrypt_rejects_a_ciphertext_of_the_wrong_length) {
  const auto private_key{sourcemeta::core::make_private_key(PRIVATE_KEY_PEM)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto wrapped{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "hello")};
  EXPECT_TRUE(wrapped.has_value());

  const std::string appended{wrapped.value() + '\x00'};
  EXPECT_FALSE(
      sourcemeta::core::rsa_oaep_decrypt(
          private_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, appended)
          .has_value());

  const std::string truncated{wrapped.value().substr(1)};
  EXPECT_FALSE(
      sourcemeta::core::rsa_oaep_decrypt(
          private_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, truncated)
          .has_value());
}

TEST(rsa_oaep_decrypt_refuses_a_pss_restricted_key) {
  const auto private_key{
      sourcemeta::core::make_private_key(PSS_RESTRICTED_PRIVATE_KEY_PEM)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{
      sourcemeta::core::derive_public_key(private_key.value())};
  EXPECT_TRUE(public_key.has_value());
  const auto wrapped{sourcemeta::core::rsa_oaep_encrypt(
      public_key.value(), sourcemeta::core::RSAOAEPHash::SHA256, "hello")};
  EXPECT_TRUE(wrapped.has_value());
  EXPECT_FALSE(sourcemeta::core::rsa_oaep_decrypt(
                   private_key.value(), sourcemeta::core::RSAOAEPHash::SHA256,
                   wrapped.value())
                   .has_value());
}
