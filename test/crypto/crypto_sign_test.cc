#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

// The message signed throughout, along with keys and known-answer signatures
// generated with OpenSSL over that message using each scheme
static const std::string MESSAGE{"sourcemeta core JWS signing input"};

// A 2048-bit RSA private key in unencrypted PKCS#8, with the deterministic
// RSASSA-PKCS1-v1_5 signatures it produces, one per supported hash function
static const std::string RSA_PRIVATE_KEY{
    R"(-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCf6K9Ys3QJNItd
zsY1wcY65kd27pBwZPdKUVNp/ZyTEtUQNwBTh8h7Pusq29loLOkonpxvkpmcNwif
6A4KsTQUjo5l0K+AAY2qgUfoPUAA2TyVZIVzVtai2LnHNeWzYg3h8NENoGhd5cGe
K6s4XzNMyWnIWJfiyCZo+lom4oYf3sDJeQeDgBgAU5TE4w8MzgSCV7GTNZGjTP+r
BF+Piopto2oXY/bh0s5yWeiMSoPWL5kpmfg/MPimXDzkNKNzpKb3Q46kVbnjPGJf
YR9/n17PjqP3X/Z3pKVLMWE3Cu9plgoEmX6816AnPGwdY/CsZw1HfWMRdrqR6mNs
CynQU4EnAgMBAAECggEADdJY8FEijcvNjWAwCw7NXuNEo2e0yGI8awoN1wglc85E
PaXjZf+YjpPo/nKzn1OHxmEJ+ILdZX6Zmj4T6KIEdGUWCIA54ICXNCOp3s2x2IuZ
DE38qEmF59wqbVOf9RoGmn7sMuyog2U2j8C1M1GB56Mz0i4GLYNLU/Y/u0NdKyfn
hYRijuGOIb/TcC0MOy269XhsAVr+Ntat0OzlITbaGKGW0yOr96hUY2Ocax99xbtj
MyiYhB5Hu99yHn7ZPRtfATnmAujjWPV7Knd9n1eO+hC6X8f+eKl9v+iqLuhNnFBG
Hhd3So/FZ0W3qXTXfC0Oq4TUsRjf4k4EtOcGf80SoQKBgQDNgIkLvSpSM0iXFRni
79eYMXwWIiwSjkDT15E1ByEucYKWn+lVD3PvuZKJxazJQk60j1hMpYoLCeI3REg9
kzY7dxXhkLyF1Y/mehPGK2dsth7J+hkOv6Ly4+oce7e9pb5UmEZ4mPnIQsWs7FcC
WKQDZBCR5yf3iGzeJD210UVz7wKBgQDHNAhal2uR5Cc6Tf9ig7tdjxnJGJDC/7yS
pDyhp7c2OjTqgxYLRVtXyjkc+P6kIxYG/AB8x1nOc6EwaNXYCOVr7PYA215Z5sVV
i3XG7ZvGwwMg+ox9dPFZR+0e2YWLo9JL1k3mYjbYBqtHScsO4UevcobeVabXvf5F
+C+Ef9euSQKBgFkln032W7uY/656uuYVgYNGRDwdytyp1TmQ1C8azqwlFa9d44zA
zVx7NjSKCjCskRQG8xkc3st3GCk9d9EuYWJegKF/dijgwjILVzSqc41XW/fmhKQ/
QeL4OADvMoAUvIJaJIgAZKsZPEONqxkbdtr5t65zhoT9K60aL6MGC4kFAoGBAKNP
JQof2TBhu0cxao15McIh7yHN4d/7iL0vqAVfWfQ7A+a+UPQsiBYQ04HNH/WwTf6r
2jpxtE5svVjSmX3izTMNsSBCt7c8Wcsn6gaMBYmNlqMSxNqVZMetD9iau8EfLOi/
aF3XZt7zmLd459/rELnlSmw8C+wS9sKmFU6rAEcBAoGBAKjY106zrDBKimEiTbJO
mdLIwwdsrh0uOwm5FmY4pIOEyEeFpqWFWOxDn4naG6Hp+YxihTLlwCaOKc+iAYEu
Xjersz7376iVIjBnOpRv8wRSrHPlp73lCM2Kqs94DawP4MDfO9qCQs0rBIhcDChZ
LotnyAB/PnO2eU6aOt6q6EcZ
-----END PRIVATE KEY-----
)"};

static const std::string MODULUS_HEX{
    "9fe8af58b37409348b5dcec635c1c63ae64776ee907064f74a515369fd9c9312"
    "d51037005387c87b3eeb2adbd9682ce9289e9c6f92999c37089fe80e0ab13414"
    "8e8e65d0af80018daa8147e83d4000d93c9564857356d6a2d8b9c735e5b3620d"
    "e1f0d10da0685de5c19e2bab385f334cc969c85897e2c82668fa5a26e2861fde"
    "c0c97907838018005394c4e30f0cce048257b1933591a34cffab045f8f8a8a6d"
    "a36a1763f6e1d2ce7259e88c4a83d62f992999f83f30f8a65c3ce434a373a4a6"
    "f7438ea455b9e33c625f611f7f9f5ecf8ea3f75ff677a4a54b3161370aef6996"
    "0a04997ebcd7a0273c6c1d63f0ac670d477d631176ba91ea636c0b29d0538127"};

static const std::string EXPONENT_HEX{"010001"};

static const std::string SIGNATURE_SHA256_HEX{
    "935fb06194272d8c59729a4ebc19b1152ed553030ba2175d52d4a65f6577444a"
    "cac9cc65296b85ec76e243a8b71c3ecc8db9d4595b7b7a598fd07fb538854f54"
    "0aaaaf9d7c00697f5e8146d48ef8968daf74c369fd32f51769a8dd53dec10223"
    "0987d7a89435d9868fc9ea0d3f98a62c7fd6d46b44bf4ac4ff4748d4471f620a"
    "cf47c6f6b81d2a5dcc68041f8625b563ffbb12f3e67d787bb0b61674e2b0831c"
    "1440d55c65d0f9be8c9c5d92d26942a5bb9bc16d48fa012a1f3060a1c4e18e00"
    "5f97c76e5e77aac1472365f92d6254d5e77ba1f73879c47b1cccf93dc7780546"
    "25229f7a6b39445d377f10c922399e31447ece1a40587dabd55660bc708bfde5"};

static const std::string SIGNATURE_SHA384_HEX{
    "7b23a38a1f9e222ee227c251548ca052ae35f7d748d4394e9b1ea6037d7f10d5"
    "aafc677a3864111e479a4d29937e980b00fcf2a481719dff7e53748eb4be1823"
    "88bd590a78ca5b0cbed432e54ef1dcb5ce00964c59c7eb02bb196f09904944a6"
    "8cbab16a4ee89111ec0bed469041b171b304692699ec0a93a4ef3dfa16612a79"
    "57aa7b9ebd6168af54c51e6b58e3cf2fc321e6d577be2d2f62629c0c0dd1e1b0"
    "9a26324cd64482d7e3ac7923eceb18c66b80fbf9e2c29e662a8c6ddea3057cb7"
    "34b27aa6572ac5dcb4aff40104457d9ec36282f8db2e29a5e78d0c5c055c979d"
    "52cf463962974976cce6c71179b9c95c04e2a3133d6157f911dbce83583a21d4"};

static const std::string SIGNATURE_SHA512_HEX{
    "15e37914281fb105f833706b3c1796dd6bbb740873668812df8d68727e0d80b2"
    "befee774d16f97927caa63a86fcb31d2f65c9572ba49750364de26e650899361"
    "746496c692ac9a0e1977b9df286cce35dd3cce2ef16246771fca40362f8aae90"
    "e99a5c4c28b8c5f738d2362bc528e6a5d8d68a79155ccce12e3e6ea10bda296e"
    "eddfe4514f9b84b60aca26cdef4e47d58987ff545de69f99e57d6230c173a309"
    "2ad2f17ce48ea2056603248c9b8bacdfb70ab173c2e7dfee39f421fdb85ac07f"
    "9d5d2685212eb389480adbea706ae807d48cc2d94ba559d4f6ff980df5b6404f"
    "0d30ce586d2f22ca253f74662cd3a366cb589f0d9514fe3d14c54649493f97e6"};

// The private components of the RSA key above, so that the same known-answer
// signatures can be reproduced from the raw integers rather than the document
static const std::string PRIVATE_EXPONENT_HEX{
    "0dd258f051228dcbcd8d60300b0ecd5ee344a367b4c8623c6b0a0dd7082573ce"
    "443da5e365ff988e93e8fe72b39f5387c66109f882dd657e999a3e13e8a20474"
    "6516088039e080973423a9decdb1d88b990c4dfca84985e7dc2a6d539ff51a06"
    "9a7eec32eca88365368fc0b5335181e7a333d22e062d834b53f63fbb435d2b27"
    "e78584628ee18e21bfd3702d0c3b2dbaf5786c015afe36d6add0ece52136da18"
    "a196d323abf7a85463639c6b1f7dc5bb63332898841e47bbdf721e7ed93d1b5f"
    "0139e602e8e358f57b2a777d9f578efa10ba5fc7fe78a97dbfe8aa2ee84d9c50"
    "461e17774a8fc56745b7a974d77c2d0eab84d4b118dfe24e04b4e7067fcd12a1"};
static const std::string PRIME1_HEX{
    "cd80890bbd2a523348971519e2efd798317c16222c128e40d3d7913507212e71"
    "82969fe9550f73efb99289c5acc9424eb48f584ca58a0b09e23744483d93363b"
    "7715e190bc85d58fe67a13c62b676cb61ec9fa190ebfa2f2e3ea1c7bb7bda5be"
    "5498467898f9c842c5acec570258a403641091e727f7886cde243db5d14573ef"};
static const std::string PRIME2_HEX{
    "c734085a976b91e4273a4dff6283bb5d8f19c91890c2ffbc92a43ca1a7b7363a"
    "34ea83160b455b57ca391cf8fea4231606fc007cc759ce73a13068d5d808e56b"
    "ecf600db5e59e6c5558b75c6ed9bc6c30320fa8c7d74f15947ed1ed9858ba3d2"
    "4bd64de66236d806ab4749cb0ee147af7286de55a6d7bdfe45f82f847fd7ae49"};
static const std::string EXPONENT1_HEX{
    "59259f4df65bbb98ffae7abae615818346443c1dcadca9d53990d42f1aceac25"
    "15af5de38cc0cd5c7b36348a0a30ac911406f3191cdecb7718293d77d12e6162"
    "5e80a17f7628e0c2320b5734aa738d575bf7e684a43f41e2f83800ef328014bc"
    "825a24880064ab193c438dab191b76daf9b7ae738684fd2bad1a2fa3060b8905"};
static const std::string EXPONENT2_HEX{
    "a34f250a1fd93061bb47316a8d7931c221ef21cde1dffb88bd2fa8055f59f43b"
    "03e6be50f42c881610d381cd1ff5b04dfeabda3a71b44e6cbd58d2997de2cd33"
    "0db12042b7b73c59cb27ea068c05898d96a312c4da9564c7ad0fd89abbc11f2c"
    "e8bf685dd766def398b778e7dfeb10b9e54a6c3c0bec12f6c2a6154eab004701"};
static const std::string COEFFICIENT_HEX{
    "a8d8d74eb3ac304a8a61224db24e99d2c8c3076cae1d2e3b09b9166638a48384"
    "c84785a6a58558ec439f89da1ba1e9f98c628532e5c0268e29cfa201812e5e37"
    "abb33ef7efa8952230673a946ff30452ac73e5a7bde508cd8aaacf780dac0fe0"
    "c0df3bda8242cd2b04885c0c28592e8b67c8007f3e73b6794e9a3adeaae84719"};

// Elliptic curve keys, one per supported curve, as the private scalar and the
// public point coordinates
static const std::string P256_D_HEX{
    "b6d7a9c5ca7e3b05c28f8fafe2c19cc51c5045703c0f94afbb4db285c1ef6ddd"};
static const std::string P256_QX_HEX{
    "d930111925aaf05f7b8aadcd838f30084376e9cc73cbc382cdb15afba65f9da2"};
static const std::string P256_QY_HEX{
    "e6593366a8563a47369b6cc93a8b65e8adffc7af934acf4e9f340ac1bdf90f14"};

static const std::string P384_D_HEX{
    "02d5a44b4f3b542bdcfd311b0697d2cc8f593453fce78a65af26fa2d5a2f1534"
    "6880d0904bafd103b1e0e6a0d7e25b50"};
static const std::string P384_QX_HEX{
    "9c0c86e4069fbc1334034e6746ba23bce4c280d36bc147245b422f9767ed6bf9"
    "60c1f5e13138dbff9f72d012cedc61ff"};
static const std::string P384_QY_HEX{
    "86d66436fd623dfd007354ef241d0b45d1480f5295f56c4e86243411cdf671d6"
    "d359a4aef3e36eb0a5acea677cd15f06"};

static const std::string P521_D_HEX{
    "011d9c388a9f757f0b665a7dfc5c877eed242ee30eeee9d4e7fa5bbfeaae19d9"
    "c7bfc093b7004cae81e4448eb7a0929080fff4600bd2493fd87eaf9557fed1aa"
    "45ff"};
static const std::string P521_QX_HEX{
    "01471aed01e0ebd9486a5acc37de6e1928e7aa9c4d542e63c4fae50ee9126d17"
    "1cd101e90f38f9d18dd29cc609489a4063e844e6b7246cb48808f910ae389e77"
    "216f"};
static const std::string P521_QY_HEX{
    "0144d992633416fec1c5e87e156f4551e26ff34f6a494f7ea9e891c8f41bc6ee"
    "5b08d302f139eda5656c3a139ab71f1db2879170a1a5dedc3214e3e7ce630fef"
    "4300"};

// RFC 6979 Appendix A.2.5: the NIST P-256 example key and the deterministic
// ECDSA signature it produces over the message below with SHA-256, reproduced
// only by a backend that derives the nonce deterministically
static const std::string RFC6979_MESSAGE{"sample"};
static const std::string RFC6979_P256_D_HEX{
    "c9afa9d845ba75166b5c215767b1d6934e50c3db36e89b127b8a622b120f6721"};
static const std::string RFC6979_P256_QX_HEX{
    "60fed4ba255a9d31c961eb74c6356d68c049b8923b61fa6ce669622e60f29fb6"};
static const std::string RFC6979_P256_QY_HEX{
    "7903fe1008b8bc99a41ae9e95628bc64f2f1b20c2d7e9f5177a3c294d4462299"};
static const std::string RFC6979_P256_SIGNATURE_HEX{
    "efd48b2aacb6a8fd1140dd9cd45e81d69d2c877b56aaf991c34d0ea84eaf3716"
    "f7cb1c942d657c41d436c7a1b6e29f65f3e900dbb9aff4064dc4ab2f843acda8"};

// Edwards keys as their raw seed and public key, with the deterministic Ed448
// signature over the message (RFC 8032 uses no randomization for Ed448)
static const std::string ED25519_SEED_HEX{
    "741ec35ce5e07089ca782f88de00d8103a1cd8e8b116acd8afd922c01bee7867"};
static const std::string ED25519_PUBLIC_HEX{
    "48539e4fb623b0d221f8cd7e9c20469c1f55598bb5bb236a8a2cc2661c1d743a"};

static const std::string ED448_SEED_HEX{
    "7027028140c9eb422935e1d309e01e66670389173da4f66c4016c2492513b4c1"
    "989263ea13250e65a784aa8c01a9960a7416c2e2879b2a2295"};
static const std::string ED448_PUBLIC_HEX{
    "04a74611a3714c06363868b8801b3c81d1b373ac7621eef20cf2793518450e60"
    "7e455b12f21c98fc5ad5098e15604e4e5236a8a455e1280f00"};
static const std::string ED448_SIGNATURE_HEX{
    "cb5ee48e2aa415ed66c0b0485683af33f66976193425e6306eda272f9258dd0b"
    "d9a14223a295425d21adc4128f326f80805532ac3af546eb806c922e7cfda76a"
    "e959fc284b2c1fe36291845a4ce6133d7a8ca63396cd36e4568628d74c4e583f"
    "47689456454a7561d89194386ca54fc82f00"};

namespace {

// Signing over the two elliptic curve schemes uses a random nonce, so the
// output is not a fixed known answer, and the aspect under test is that a fresh
// signature verifies for its message
auto ecdsa_signature_verifies(
    const sourcemeta::core::EllipticCurve curve,
    const sourcemeta::core::SignatureHashFunction hash,
    const std::string_view scalar_hex, const std::string_view coordinate_x_hex,
    const std::string_view coordinate_y_hex) -> bool {
  const auto coordinate_x{sourcemeta::core::hex_to_bytes(coordinate_x_hex)};
  const auto coordinate_y{sourcemeta::core::hex_to_bytes(coordinate_y_hex)};
  const auto key{sourcemeta::core::make_ec_private_key(
      curve, sourcemeta::core::hex_to_bytes(scalar_hex).value(),
      coordinate_x.value(), coordinate_y.value())};
  if (!key.has_value()) {
    return false;
  }

  const auto signature{
      sourcemeta::core::ecdsa_sign(key.value(), hash, MESSAGE)};
  const auto public_key{sourcemeta::core::make_ec_public_key(
      curve, coordinate_x.value(), coordinate_y.value())};
  return signature.has_value() && public_key.has_value() &&
         sourcemeta::core::ecdsa_verify(public_key.value(), hash, MESSAGE,
                                        signature.value());
}

} // namespace

TEST(make_private_key_parses_rsa) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().type() == sourcemeta::core::PrivateKey::Type::RSA);
}

TEST(make_rsa_private_key_parses_rsa) {
  const auto key{sourcemeta::core::make_rsa_private_key(
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
      sourcemeta::core::hex_to_bytes(PRIVATE_EXPONENT_HEX).value(),
      sourcemeta::core::hex_to_bytes(PRIME1_HEX).value(),
      sourcemeta::core::hex_to_bytes(PRIME2_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT1_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT2_HEX).value(),
      sourcemeta::core::hex_to_bytes(COEFFICIENT_HEX).value())};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().type() == sourcemeta::core::PrivateKey::Type::RSA);
}

TEST(make_rsa_private_key_from_components_matches_known_answer) {
  const auto key{sourcemeta::core::make_rsa_private_key(
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
      sourcemeta::core::hex_to_bytes(PRIVATE_EXPONENT_HEX).value(),
      sourcemeta::core::hex_to_bytes(PRIME1_HEX).value(),
      sourcemeta::core::hex_to_bytes(PRIME2_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT1_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT2_HEX).value(),
      sourcemeta::core::hex_to_bytes(COEFFICIENT_HEX).value())};
  const auto signature{sourcemeta::core::rsassa_pkcs1_v15_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA256, MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()),
            SIGNATURE_SHA256_HEX);
}

TEST(make_rsa_private_key_rejects_missing_component) {
  // Every two-prime component is required
  EXPECT_FALSE(sourcemeta::core::make_rsa_private_key(
                   sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                   sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                   std::string{},
                   sourcemeta::core::hex_to_bytes(PRIME1_HEX).value(),
                   sourcemeta::core::hex_to_bytes(PRIME2_HEX).value(),
                   sourcemeta::core::hex_to_bytes(EXPONENT1_HEX).value(),
                   sourcemeta::core::hex_to_bytes(EXPONENT2_HEX).value(),
                   sourcemeta::core::hex_to_bytes(COEFFICIENT_HEX).value())
                   .has_value());
}

TEST(make_ec_private_key_parses_elliptic_curve) {
  const auto key{sourcemeta::core::make_ec_private_key(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::hex_to_bytes(P256_D_HEX).value(),
      sourcemeta::core::hex_to_bytes(P256_QX_HEX).value(),
      sourcemeta::core::hex_to_bytes(P256_QY_HEX).value())};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().type() ==
              sourcemeta::core::PrivateKey::Type::EllipticCurve);
}

TEST(make_edwards_private_key_parses_edwards) {
  const auto key{sourcemeta::core::make_edwards_private_key(
      sourcemeta::core::EdwardsCurve::Ed25519,
      sourcemeta::core::hex_to_bytes(ED25519_SEED_HEX).value())};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(key.value().type() ==
              sourcemeta::core::PrivateKey::Type::Edwards);
}

TEST(rsassa_pkcs1_v15_sha256_matches_known_answer) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  const auto signature{sourcemeta::core::rsassa_pkcs1_v15_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA256, MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()),
            SIGNATURE_SHA256_HEX);
}

TEST(rsassa_pkcs1_v15_sha384_matches_known_answer) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  const auto signature{sourcemeta::core::rsassa_pkcs1_v15_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA384, MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()),
            SIGNATURE_SHA384_HEX);
}

TEST(rsassa_pkcs1_v15_sha512_matches_known_answer) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  const auto signature{sourcemeta::core::rsassa_pkcs1_v15_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA512, MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()),
            SIGNATURE_SHA512_HEX);
}

TEST(rsassa_pss_sha256_signature_verifies) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  // The salt is random, so the signature is not a fixed known answer
  const auto signature{sourcemeta::core::rsassa_pss_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA256, MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{sourcemeta::core::make_rsa_public_key(
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value())};
  EXPECT_TRUE(sourcemeta::core::rsassa_pss_verify(
      public_key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
      MESSAGE, signature.value()));
}

TEST(ecdsa_p256_sha256_signature_verifies) {
  EXPECT_TRUE(
      ecdsa_signature_verifies(sourcemeta::core::EllipticCurve::P256,
                               sourcemeta::core::SignatureHashFunction::SHA256,
                               P256_D_HEX, P256_QX_HEX, P256_QY_HEX));
}

TEST(ecdsa_p384_sha384_signature_verifies) {
  EXPECT_TRUE(
      ecdsa_signature_verifies(sourcemeta::core::EllipticCurve::P384,
                               sourcemeta::core::SignatureHashFunction::SHA384,
                               P384_D_HEX, P384_QX_HEX, P384_QY_HEX));
}

TEST(ecdsa_p521_sha512_signature_verifies) {
  EXPECT_TRUE(
      ecdsa_signature_verifies(sourcemeta::core::EllipticCurve::P521,
                               sourcemeta::core::SignatureHashFunction::SHA512,
                               P521_D_HEX, P521_QX_HEX, P521_QY_HEX));
}

TEST(ecdsa_deterministic_nonce_matches_rfc6979_known_answer) {
  const auto key{sourcemeta::core::make_ec_private_key(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::hex_to_bytes(RFC6979_P256_D_HEX).value(),
      sourcemeta::core::hex_to_bytes(RFC6979_P256_QX_HEX).value(),
      sourcemeta::core::hex_to_bytes(RFC6979_P256_QY_HEX).value())};
  EXPECT_TRUE(key.has_value());
  const auto first{sourcemeta::core::ecdsa_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
      RFC6979_MESSAGE)};
  const auto second{sourcemeta::core::ecdsa_sign(
      key.value(), sourcemeta::core::SignatureHashFunction::SHA256,
      RFC6979_MESSAGE)};
  EXPECT_TRUE(first.has_value());
  // A backend that derives the nonce deterministically (RFC 6979) produces the
  // same signature twice, and only then is the published known answer
  // reproducible. Randomized backends are covered by the round-trip tests
  if (first.has_value() && first == second) {
    EXPECT_EQ(sourcemeta::core::bytes_to_hex(first.value()),
              RFC6979_P256_SIGNATURE_HEX);
  }
}

TEST(ed25519_signature_verifies) {
  const auto key{sourcemeta::core::make_edwards_private_key(
      sourcemeta::core::EdwardsCurve::Ed25519,
      sourcemeta::core::hex_to_bytes(ED25519_SEED_HEX).value())};
  // The Apple backend produces valid but non-deterministic Ed25519 signatures,
  // so the signature is verified rather than compared to a known answer
  const auto signature{sourcemeta::core::eddsa_sign(key.value(), MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  const auto public_key{sourcemeta::core::make_eddsa_public_key(
      sourcemeta::core::EdwardsCurve::Ed25519,
      sourcemeta::core::hex_to_bytes(ED25519_PUBLIC_HEX).value())};
  EXPECT_TRUE(sourcemeta::core::eddsa_verify(public_key.value(), MESSAGE,
                                             signature.value()));
}

TEST(ed448_matches_known_answer) {
  const auto key{sourcemeta::core::make_edwards_private_key(
      sourcemeta::core::EdwardsCurve::Ed448,
      sourcemeta::core::hex_to_bytes(ED448_SEED_HEX).value())};
  const auto signature{sourcemeta::core::eddsa_sign(key.value(), MESSAGE)};
  EXPECT_TRUE(signature.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(signature.value()),
            ED448_SIGNATURE_HEX);
}

TEST(ecdsa_sign_rejects_rsa_key) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  EXPECT_FALSE(
      sourcemeta::core::ecdsa_sign(
          key.value(), sourcemeta::core::SignatureHashFunction::SHA256, MESSAGE)
          .has_value());
}

TEST(eddsa_sign_rejects_rsa_key) {
  const auto key{sourcemeta::core::make_private_key(RSA_PRIVATE_KEY)};
  EXPECT_FALSE(sourcemeta::core::eddsa_sign(key.value(), MESSAGE).has_value());
}

TEST(make_private_key_rejects_garbage) {
  EXPECT_FALSE(sourcemeta::core::make_private_key("not a pem").has_value());
}

TEST(make_edwards_private_key_rejects_short_ed25519_seed) {
  // Ed25519 requires a 32 byte seed
  EXPECT_FALSE(
      sourcemeta::core::make_edwards_private_key(
          sourcemeta::core::EdwardsCurve::Ed25519, std::string(31, '\x00'))
          .has_value());
}

TEST(make_edwards_private_key_rejects_short_ed448_seed) {
  // Ed448 requires a 57 byte seed
  EXPECT_FALSE(
      sourcemeta::core::make_edwards_private_key(
          sourcemeta::core::EdwardsCurve::Ed448, std::string(56, '\x00'))
          .has_value());
}

TEST(make_ec_private_key_rejects_empty_scalar) {
  EXPECT_FALSE(sourcemeta::core::make_ec_private_key(
                   sourcemeta::core::EllipticCurve::P256, std::string{},
                   sourcemeta::core::hex_to_bytes(P256_QX_HEX).value(),
                   sourcemeta::core::hex_to_bytes(P256_QY_HEX).value())
                   .has_value());
}

TEST(make_ec_private_key_rejects_oversized_scalar) {
  // A P-256 scalar cannot exceed the 32 byte field width
  EXPECT_FALSE(sourcemeta::core::make_ec_private_key(
                   sourcemeta::core::EllipticCurve::P256,
                   std::string(33, '\xff'),
                   sourcemeta::core::hex_to_bytes(P256_QX_HEX).value(),
                   sourcemeta::core::hex_to_bytes(P256_QY_HEX).value())
                   .has_value());
}
