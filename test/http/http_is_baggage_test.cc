#include <sourcemeta/core/http_baggage.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(the_example_from_the_specification) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage(
      "userId=alice,serverNode=DF%2028,isProduction=false"));
}

// The worked example carries three members, the first with two bare
// properties and the last with a property that has a value
// https://www.w3.org/TR/baggage/#example
TEST(the_worked_example_with_properties) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage(
      "key1=value1;property1;property2, key2 = value2, key3=value3; "
      "propertyKey=propertyValue"));
}

// There is no alternative for an empty member, so a list must hold one, which
// is the opposite of tracestate
// https://www.w3.org/TR/baggage/#definition
TEST(an_empty_field_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage(""));
}

TEST(an_empty_member_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=1,,b=2"));
}

TEST(a_whitespace_only_member_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=1, ,b=2"));
}

TEST(a_trailing_comma_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=1,"));
}

// A member must carry a pair, so a bare key is only allowed as a property
// https://www.w3.org/TR/baggage/#property
TEST(a_member_without_a_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("novalue"));
}

TEST(an_empty_member_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a="));
}

// "Leading and trailing whitespaces (OWS) are allowed and are not considered
// to be a part of the key", and the same holds of the value, so a member may
// be padded anywhere, which is also the opposite of tracestate
// https://www.w3.org/TR/baggage/#key
TEST(whitespace_around_the_equals_sign_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("userId = alice"));
}

TEST(whitespace_around_a_member_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("  a=1 , b=2  "));
}

// "Uniqueness of keys between multiple list-members in a baggage-string is
// not guaranteed", so a repeat is well formed, which tracestate refuses
// https://www.w3.org/TR/baggage/#baggage-string
TEST(a_duplicate_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1,a=2"));
}

// A key is the token this module already knows, so it admits uppercase and a
// dot, neither of which a tracestate key may carry
// https://www.w3.org/TR/baggage/#key
TEST(an_uppercase_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("UserId=alice"));
}

TEST(a_dotted_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("com.example.user=alice"));
}

TEST(a_key_that_is_not_a_token_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a(b)=1"));
}

TEST(a_key_containing_a_space_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a b=1"));
}

TEST(an_empty_key_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("=1"));
}

// A property set is semi-colon delimited and its entries are "keys and/or
// key-value pairs", so a bare key and a pair are both well formed there
// https://www.w3.org/TR/baggage/#property
TEST(a_bare_property_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1;flag"));
}

TEST(a_property_with_a_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1;k=v"));
}

TEST(a_property_with_an_empty_value_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1;k="));
}

TEST(several_properties_are_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1;one;two=2;three"));
}

TEST(whitespace_around_a_property_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1 ; k = v "));
}

TEST(a_property_key_that_is_not_a_token_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=1;k(x)=v"));
}

TEST(an_empty_property_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=1;;k=v"));
}

// A key is a token, and a token admits a percent, so the rule that a percent
// must introduce an encoded octet binds a value and not a key. These two
// inputs differ only in which side of the equals sign the percent falls on
// https://www.w3.org/TR/baggage/#key
// https://www.w3.org/TR/baggage/#value
TEST(a_bare_percent_in_a_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a%b=1"));
}

TEST(a_bare_percent_in_a_value_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=b%c"));
}

// A property key is a token on the same terms
TEST(a_bare_percent_in_a_property_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1;b%c"));
}

// The property set has no uniqueness rule of its own, so a repeated property
// key is well formed just as a repeated member key is
// https://www.w3.org/TR/baggage/#property
TEST(a_duplicate_property_key_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=1;p;p=v;p=v2"));
}

// Encoding is what lets a value carry a delimiter, so a value made of encoded
// tabs, quotes, semicolons and equals signs is well formed
// https://www.w3.org/TR/baggage/#value
TEST(a_value_of_encoded_delimiters_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage(
      "SomeKey=%09%20%22%27%3B%3Dasdf%21%40%23%24%25%5E%26%2A%28%29"));
}

// Whitespace is allowed around every key and value, at every level
// https://www.w3.org/TR/baggage/#key
TEST(whitespace_at_every_level_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage(
      "SomeKey \t = \t SomeValue \t ; \t SomeProp \t , \t SomeKey2 \t = \t "
      "SomeValue2 \t ; \t ValueProp \t = \t PropVal"));
}

// A value is checked wherever it appears, so what the value predicate refuses
// a member carrying it is refused for
// https://www.w3.org/TR/baggage/#value
TEST(a_member_value_with_a_bare_percent_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=100%"));
}

TEST(a_member_value_with_a_bad_escape_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=%ZZ"));
}

TEST(a_member_value_with_a_newline_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=one\ntwo"));
}

TEST(a_property_value_with_a_bad_escape_is_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage("a=1;k=%ZZ"));
}

TEST(a_member_value_may_carry_an_equals_sign) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=b=c"));
}

// A value is not capped by the grammar, and neither is the whole list, so one
// well past the byte figure in the limits is still well formed
// https://www.w3.org/TR/baggage/#limits
TEST(a_field_value_over_eight_thousand_bytes_is_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage("a=" + std::string(9000, 'x')));
}

TEST(a_null_byte_in_a_value_is_rejected) {
  EXPECT_FALSE(
      sourcemeta::core::http_is_baggage(std::string{"a=b"} + '\0' + "c"));
}

// The grammar allows one member and up to 179 more. The separate figure of 64
// members is the point past which a platform "MAY drop list-members" rather
// than a bound on what is well formed, so a list well past it is accepted
// https://www.w3.org/TR/baggage/#definition
// https://www.w3.org/TR/baggage/#limits
TEST(one_hundred_and_eighty_members_are_accepted) {
  EXPECT_TRUE(sourcemeta::core::http_is_baggage(
      "k0=v,k1=v,k2=v,k3=v,k4=v,k5=v,k6=v,k7=v,k8=v,k9=v,k10=v,k11=v,"
      "k12=v,k13=v,k14=v,k15=v,k16=v,k17=v,k18=v,k19=v,k20=v,k21=v,k22=v,"
      "k23=v,k24=v,k25=v,k26=v,k27=v,k28=v,k29=v,k30=v,k31=v,k32=v,k33=v,"
      "k34=v,k35=v,k36=v,k37=v,k38=v,k39=v,k40=v,k41=v,k42=v,k43=v,k44=v,"
      "k45=v,k46=v,k47=v,k48=v,k49=v,k50=v,k51=v,k52=v,k53=v,k54=v,k55=v,"
      "k56=v,k57=v,k58=v,k59=v,k60=v,k61=v,k62=v,k63=v,k64=v,k65=v,k66=v,"
      "k67=v,k68=v,k69=v,k70=v,k71=v,k72=v,k73=v,k74=v,k75=v,k76=v,k77=v,"
      "k78=v,k79=v,k80=v,k81=v,k82=v,k83=v,k84=v,k85=v,k86=v,k87=v,k88=v,"
      "k89=v,k90=v,k91=v,k92=v,k93=v,k94=v,k95=v,k96=v,k97=v,k98=v,k99=v,"
      "k100=v,k101=v,k102=v,k103=v,k104=v,k105=v,k106=v,k107=v,k108=v,"
      "k109=v,k110=v,k111=v,k112=v,k113=v,k114=v,k115=v,k116=v,k117=v,"
      "k118=v,k119=v,k120=v,k121=v,k122=v,k123=v,k124=v,k125=v,k126=v,"
      "k127=v,k128=v,k129=v,k130=v,k131=v,k132=v,k133=v,k134=v,k135=v,"
      "k136=v,k137=v,k138=v,k139=v,k140=v,k141=v,k142=v,k143=v,k144=v,"
      "k145=v,k146=v,k147=v,k148=v,k149=v,k150=v,k151=v,k152=v,k153=v,"
      "k154=v,k155=v,k156=v,k157=v,k158=v,k159=v,k160=v,k161=v,k162=v,"
      "k163=v,k164=v,k165=v,k166=v,k167=v,k168=v,k169=v,k170=v,k171=v,"
      "k172=v,k173=v,k174=v,k175=v,k176=v,k177=v,k178=v,k179=v"));
}

TEST(one_hundred_and_eighty_one_members_are_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_is_baggage(
      "k0=v,k1=v,k2=v,k3=v,k4=v,k5=v,k6=v,k7=v,k8=v,k9=v,k10=v,k11=v,"
      "k12=v,k13=v,k14=v,k15=v,k16=v,k17=v,k18=v,k19=v,k20=v,k21=v,k22=v,"
      "k23=v,k24=v,k25=v,k26=v,k27=v,k28=v,k29=v,k30=v,k31=v,k32=v,k33=v,"
      "k34=v,k35=v,k36=v,k37=v,k38=v,k39=v,k40=v,k41=v,k42=v,k43=v,k44=v,"
      "k45=v,k46=v,k47=v,k48=v,k49=v,k50=v,k51=v,k52=v,k53=v,k54=v,k55=v,"
      "k56=v,k57=v,k58=v,k59=v,k60=v,k61=v,k62=v,k63=v,k64=v,k65=v,k66=v,"
      "k67=v,k68=v,k69=v,k70=v,k71=v,k72=v,k73=v,k74=v,k75=v,k76=v,k77=v,"
      "k78=v,k79=v,k80=v,k81=v,k82=v,k83=v,k84=v,k85=v,k86=v,k87=v,k88=v,"
      "k89=v,k90=v,k91=v,k92=v,k93=v,k94=v,k95=v,k96=v,k97=v,k98=v,k99=v,"
      "k100=v,k101=v,k102=v,k103=v,k104=v,k105=v,k106=v,k107=v,k108=v,"
      "k109=v,k110=v,k111=v,k112=v,k113=v,k114=v,k115=v,k116=v,k117=v,"
      "k118=v,k119=v,k120=v,k121=v,k122=v,k123=v,k124=v,k125=v,k126=v,"
      "k127=v,k128=v,k129=v,k130=v,k131=v,k132=v,k133=v,k134=v,k135=v,"
      "k136=v,k137=v,k138=v,k139=v,k140=v,k141=v,k142=v,k143=v,k144=v,"
      "k145=v,k146=v,k147=v,k148=v,k149=v,k150=v,k151=v,k152=v,k153=v,"
      "k154=v,k155=v,k156=v,k157=v,k158=v,k159=v,k160=v,k161=v,k162=v,"
      "k163=v,k164=v,k165=v,k166=v,k167=v,k168=v,k169=v,k170=v,k171=v,"
      "k172=v,k173=v,k174=v,k175=v,k176=v,k177=v,k178=v,k179=v,k180=v"));
}
