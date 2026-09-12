#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uritemplate.h>

TEST(letters_and_digits) {
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'a'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'Z'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'0'));
}

TEST(the_named_single_characters) {
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'!'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'='));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U']'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'_'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'~'));
}

TEST(the_apostrophe_the_errata_restores) {
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'\''));
}

TEST(the_expression_delimiters_are_not_literals) {
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'{'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'}'));
}

TEST(a_percent_sign_begins_a_production_of_its_own) {
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'%'));
}

TEST(characters_the_grammar_leaves_out) {
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U' '));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'"'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'<'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'>'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\\'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'^'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'`'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'|'));
}

TEST(control_characters_are_not_literals) {
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\0'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\n'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\x7F'));
}

TEST(a_character_the_other_productions_admit) {
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'é'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'中'));
}

TEST(a_private_use_character) {
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U''));
}

TEST(the_boundaries_of_the_first_of_those_productions) {
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\x9F'));
  EXPECT_TRUE(sourcemeta::core::URITemplate::is_literal(U'\xA0'));
}

TEST(characters_beyond_those_two_productions) {
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\xFFFF'));
  EXPECT_FALSE(sourcemeta::core::URITemplate::is_literal(U'\xFFF0'));
}
