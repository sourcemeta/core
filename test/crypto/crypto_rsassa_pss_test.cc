#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

// The same 2048-bit test key as the RSASSA-PKCS1-v1_5 tests, along with
// RSASSA-PSS signatures over the message below using a salt as long as the
// hash function output, one per supported hash function
static const std::string MESSAGE{"sourcemeta core JWS signing input"};

static const std::string MODULUS_HEX{
    "c18bb08bad1c930bb6eed38f60299c28f28e0dabf53557fa8ac9a63044413a3c"
    "74671a8b316ec972b25faa13826b10e0d78f90da1b697805202faf58c5768361"
    "ad0006364e5f5bbf6e95a222f894be4cdccfe52aca34b48a1f92a1192214f78e"
    "33cfb063b04a83b4234ef91248ba16c8a35655551cf4dfdb44bcc2e77e415d94"
    "097be4d911dac040098e4a76662d854c9218a8869be7f00bdac03e3b1e5aa9ec"
    "0d8d5c683ccbdbcf34eb04d7d0ba000b4692287d1a875927bd55325f1461d738"
    "4fa1506e0d5d5d9c7f531112485219c783228ae9b81ff65725513d559131ee06"
    "c5299d6531d0b9abe13ee19924f1fa47f25136fc948daf0cac721a277255af3d"};

static const std::string EXPONENT_HEX{"010001"};

static const std::string SIGNATURE_SHA256_HEX{
    "9ce80cd6ab0f6b11df7b5cbabd2c3a274bb25065d1be44d42ea7fc6b36d77f15"
    "971e5741dc19ae58c4a7459d3a571407efe41c125935e43804ecbd97ce36a6e4"
    "4ee4d4a700e2fb40e5694178c9897dbb50cf0d9fa7de7fb8501d601edde08519"
    "74fe169bcc92ca5bc1a1cefb97df8c5bbc1ae7adda9e4a64df252b9dca76cb87"
    "1ff302d5021fde0e035f394fbd41ff8e96a922096d267e9007fe01368198dec7"
    "a2c5571766e3a21dfb58edcaf111ce5c0f28db6447f04348bc03a0b83bd25b04"
    "7fe292ef9df85629ef98c66df0a3a113bd0bb9156a49297da8ecd8eb374b6f09"
    "1d596e1d7917c654a8332c0cfe72593cdd4d8c5ec32d18849018e24ba2ea7460"};

static const std::string SIGNATURE_SHA384_HEX{
    "8bff2ed16017c973ea443044b8c44d2618ba6c5c5b0de90a3ac62312cfa6f28b"
    "5b17cc113a9628d2b83c116cba723eb33677b5ad12a85708f41e26817621a1ef"
    "c60fa8bb11cf2279e8be2d3301e0811fbcf2a8c7b36b50521ecb6124b9d82b2b"
    "3c1b7c4a370684945e1e3ff1ff791daff9bacb2b7ab1aa2847e1bac32326b4ca"
    "c214bfc5e81b703ed9c6e8fdfe6864f641e03c10b7da97f521f1faedaebd4668"
    "ffbd16bd5498e668389ab1c424b78ba8c71c5f6bb0702eaff279abdeb43a42a6"
    "cb7c616a28a3eb563a49d0ff8584d18265deba60ef0bed17ef447477c6f6668f"
    "d97a130d6988a508c9fa333a31a37d1ee3200b777850b2305c3171600f770f82"};

static const std::string SIGNATURE_SHA512_HEX{
    "10c523c1742867b219193507ebbc5cd3d149d39400d615ef30136374b8dd454f"
    "a677352abd46260f4e33a27660c6d7f3576c2645e49412176fb921bcc3842d63"
    "d7360fc08a0797f5a6e7a05a0be67987525f308badbb415797f899316070b7a4"
    "9fbe7570b59c41e5e5b2ff21f101afed24ce3c7311b8a21a35410fb80178f29b"
    "e86929b2d7640cd405cda0a654f17868d4c0e4ffb684eb3b395d1d30ce75057b"
    "b01a89a6c3a7f9b933f119b41e4e5b2e3c341cf9d672a3103959af3e6c8de763"
    "5701b695a32a2e9e7759d1c7d72543c1a9940c0694e2bcec7642f564e51b6de2"
    "1bf25ca96017bd5318ea9c24aa4b2c3cdc0f0c1d86362e586a60929b81a1d4c8"};

// A signature over the same message and hash that uses a 10-byte salt
// rather than one as long as the hash function output
static const std::string SIGNATURE_SHA256_SALT_10_HEX{
    "9a8f3208169b0c6f10f0e7246d2a3e028b746a1215daaa9b89fbb93dc3a09f47"
    "ae4a60c696107d93bfb1e1b6ed1e830980b972b17659f2d023c4c5a3aaa2497e"
    "1d7f70b431ea552ae38d95bbf5c777d40f8c35882e91a3fb3b2cf567a5f0c3ac"
    "2654762a3617d5144dc874fcb3859d8dc231edc328ba171ef839d940e04eb2a2"
    "bb213fd436913b33d2201c9270689a7f80df627b8136cd45d5709ac03d243d96"
    "aa97acd1758541e65c43c0abc531788819c8d305b6bd941925b135e9ba14850f"
    "5bd2e46d39dd19cd094bdce21cd851a3991c4cfe833b379e06db61187f83c7c6"
    "f285d81850055de9dd4f1fd6e69eb491783ad071a9007f7503687b3606e53fc3"};

// The RSASSA-PKCS1-v1_5 signature over the same message and hash, which
// must never verify under RSASSA-PSS
static const std::string SIGNATURE_SHA256_PKCS1_V15_HEX{
    "b03580590d438d7a0cec9dbaf9046414c8474ec5bfa0809ce456036110730d72"
    "1ce914ccda9da01d80d4f21490a1e9263391f434ede0c901d4bfcf983b8ad1c1"
    "cc97d879a0315b0b69ee13c5a2fd2ddfce4d407d65351a902f90630eb398a2d6"
    "7cc0232ec78365fbdffe540324fdafac86e6a5e371e4cc71b71c0d3758b2e6fa"
    "9648089d46217e51df97f1e73f2cbc475477b5406d12ecf3b64957b165821295"
    "3c69777330d9909d2a4a62f95672e0ff45b1b2691250f52841182b11869c88b0"
    "7080b42118a2380d03d58f488a4241f1624cbda93d9f50b95fbd0f2ff7e35b54"
    "f0c410dd41988958e0400e20442efdd2f8a7c963b9e874f50bf489b553269ba4"};

namespace {
auto verify_pss(const sourcemeta::core::SignatureHashFunction hash,
                const std::string_view modulus, const std::string_view exponent,
                const std::string_view message,
                const std::string_view signature) -> bool {
  const auto key{sourcemeta::core::make_rsa_public_key(modulus, exponent)};
  return key.has_value() && sourcemeta::core::rsassa_pss_verify(
                                key.value(), hash, message, signature);
}
} // namespace

TEST(verify_sha256_valid_signature) {
  EXPECT_TRUE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                 sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(verify_sha384_valid_signature) {
  EXPECT_TRUE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA384,
                 sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA384_HEX).value()));
}

TEST(verify_sha512_valid_signature) {
  EXPECT_TRUE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA512,
                 sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA512_HEX).value()));
}

// RFC 7518 Section 3.5: "The size of the salt value is the same size as the
// hash function output", hence signatures carrying any other salt length
// must be rejected
TEST(verify_rejects_salt_length_other_than_digest) {
  EXPECT_FALSE(verify_pss(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_SALT_10_HEX).value()));
}

TEST(verify_rejects_pkcs1_v15_signature) {
  EXPECT_FALSE(verify_pss(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
      sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_PKCS1_V15_HEX).value()));
}

TEST(verify_rejects_different_message) {
  EXPECT_FALSE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                 sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                 "a different message",
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(verify_rejects_tampered_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()};
  signature[0] = static_cast<char>(signature[0] ^ 0x01);
  EXPECT_FALSE(verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                          sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                          sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                          MESSAGE, signature));
}

TEST(verify_rejects_hash_function_mismatch) {
  EXPECT_FALSE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA384,
                 sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(verify_rejects_truncated_signature) {
  auto signature{sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()};
  signature.pop_back();
  EXPECT_FALSE(verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                          sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                          sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                          MESSAGE, signature));
}

TEST(verify_rejects_signature_equal_to_modulus) {
  EXPECT_FALSE(verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                          sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                          sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                          MESSAGE,
                          sourcemeta::core::hex_to_bytes(MODULUS_HEX).value()));
}

TEST(verify_rejects_empty_signature) {
  EXPECT_FALSE(verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                          sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(),
                          sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                          MESSAGE, ""));
}

TEST(verify_rejects_empty_modulus) {
  EXPECT_FALSE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA256, "",
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(verify_rejects_empty_exponent) {
  EXPECT_FALSE(verify_pss(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(), "", MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(verify_modulus_with_leading_zeros) {
  EXPECT_TRUE(
      verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                 sourcemeta::core::hex_to_bytes("0000" + MODULUS_HEX).value(),
                 sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(), MESSAGE,
                 sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}

TEST(verify_rejects_oversized_modulus) {
  const std::string modulus(520, '\xFF');
  const std::string signature(520, '\x01');
  EXPECT_FALSE(verify_pss(sourcemeta::core::SignatureHashFunction::SHA256,
                          modulus,
                          sourcemeta::core::hex_to_bytes(EXPONENT_HEX).value(),
                          MESSAGE, signature));
}

TEST(verify_rejects_oversized_exponent) {
  const std::string exponent(520, '\xFF');
  EXPECT_FALSE(verify_pss(
      sourcemeta::core::SignatureHashFunction::SHA256,
      sourcemeta::core::hex_to_bytes(MODULUS_HEX).value(), exponent, MESSAGE,
      sourcemeta::core::hex_to_bytes(SIGNATURE_SHA256_HEX).value()));
}
