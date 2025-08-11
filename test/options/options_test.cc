#include <gtest/gtest.h>

#include <sourcemeta/core/options.h>

TEST(Options, long_option_equals_parses_value_with_equal_sign) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "--foo=bar";
  const char *const argv[] = {arg0, arg1};
  const int argc = 2;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
  EXPECT_TRUE(app.contains("foo"));
}

TEST(Options, long_option_space_parses_value_after_space) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "--foo";
  const char arg2[] = "bar";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
}

TEST(Options, short_option_space_parses_short_option_with_space_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "-f";
  const char arg2[] = "path/to/x";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "path/to/x");
}

TEST(Options, short_option_attached_value_parses_short_option_attached_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "-fpath";
  const char *const argv[] = {arg0, arg1};
  const int argc = 2;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "path");
}

TEST(Options, combined_flags_parses_combined_short_flags) {
  sourcemeta::core::Options app;
  app.flag("alpha", {"a"});
  app.flag("beta", {"b"});
  app.flag("gamma", {"c"});

  const char arg0[] = "prog";
  const char arg1[] = "-abc";
  const char *const argv[] = {arg0, arg1};
  const int argc = 2;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("alpha").size(), 1);
  EXPECT_EQ(app.at("beta").size(), 1);
  EXPECT_EQ(app.at("gamma").size(), 1);
}

TEST(
    Options,
    combined_flags_and_option_with_value_parses_combined_flags_and_option_value) {
  sourcemeta::core::Options app;
  app.flag("alpha", {"a"});
  app.option("bopt", {"b"});

  const char arg0[] = "prog";
  const char arg1[] = "-abvalue";
  const char *const argv[] = {arg0, arg1};
  const int argc = 2;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("alpha").size(), 1);
  EXPECT_EQ(app.at("bopt").size(), 1);
  EXPECT_EQ(app.at("bopt")[0], "value");
}

TEST(Options, repeated_options_preserved_order) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "--foo=one";
  const char arg2[] = "--foo";
  const char arg3[] = "two";
  const char arg4[] = "-f";
  const char arg5[] = "three";
  const char *const argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
  const int argc = 6;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("foo").size(), 3);
  EXPECT_EQ(app.at("foo")[0], "one");
  EXPECT_EQ(app.at("foo")[1], "two");
  EXPECT_EQ(app.at("foo")[2], "three");
}

TEST(Options, flags_count_multiple_occurrences) {
  sourcemeta::core::Options app;
  app.flag("exclude", {"x"});

  const char arg0[] = "prog";
  const char arg1[] = "-x";
  const char arg2[] = "--exclude";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("exclude").size(), 2);
  EXPECT_TRUE(app.contains("exclude"));
}

TEST(Options, unknown_option_throws) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "--unknown";
  const char *const argv[] = {arg0, arg1};
  const int argc = 2;

  EXPECT_THROW(app.parse(argc, argv, {.skip = 1}),
               sourcemeta::core::OptionsUnknownOptionError);
}

TEST(Options, flag_given_value_throws) {
  sourcemeta::core::Options app;
  app.flag("verbose", {"v"});

  const char arg0[] = "prog";
  const char arg1[] = "--verbose=1";
  const char *const argv[] = {arg0, arg1};
  const int argc = 2;

  EXPECT_THROW(app.parse(argc, argv, {.skip = 1}),
               sourcemeta::core::OptionUnexpectedValueFlagError);
}

TEST(Options, positional_after_double_dash) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});
  app.flag("x", {"x"});

  const char arg0[] = "prog";
  const char arg1[] = "--";
  const char arg2[] = "-not-an-opt";
  const char arg3[] = "pos2";
  const char *const argv[] = {arg0, arg1, arg2, arg3};
  const int argc = 4;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.positional().size(), 2);
  EXPECT_EQ(app.positional()[0], "-not-an-opt");
  EXPECT_EQ(app.positional()[1], "pos2");
}

TEST(Options, positional_before_options) {
  sourcemeta::core::Options app;
  app.option("foo", {"foo"});

  const char arg0[] = "prog";
  const char arg1[] = "pos1";
  const char arg2[] = "--foo";
  const char arg3[] = "bar";
  const char *const argv[] = {arg0, arg1, arg2, arg3};
  const int argc = 4;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.positional().size(), 1);
  EXPECT_EQ(app.positional()[0], "pos1");
  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
}

TEST(Options, skip_parameter_works) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "cmd";
  const char arg2[] = "-f";
  const char arg3[] = "file.txt";
  const char *const argv[] = {arg0, arg1, arg2, arg3};
  const int argc = 4;

  app.parse(argc, argv, {.skip = 2});

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "file.txt");
}

TEST(Options, alias_mapping_recognizes_aliases) {
  sourcemeta::core::Options app;
  app.option("file", {"f", "a"});

  const char arg0[] = "prog";
  const char arg1[] = "-a";
  const char arg2[] = "ok";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "ok");
}

TEST(Options, option_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});
  app.flag("other", {"o"});

  const char arg0[] = "prog";
  const char arg1[] = "--file";
  const char arg2[] = "--other";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "--other");
  EXPECT_EQ(app.at("other").size(), 0);
}

TEST(Options, long_option_without_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "--file";
  const char *const argv[] = {arg0};
  const int argc = 1;

  EXPECT_THROW(app.parse(argc, argv),
               sourcemeta::core::OptionMissingOptionValueError);
}

TEST(Options, short_option_without_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "-f";
  const char *const argv[] = {arg0};
  const int argc = 1;

  EXPECT_THROW(app.parse(argc, argv),
               sourcemeta::core::OptionMissingOptionValueError);
}

TEST(Options, single_dash_is_consumed_as_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "-f";
  const char arg2[] = "-";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "-");
}

TEST(Options, empty_result_for_missing_option) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "prog";
  const char *const argv[] = {arg0};
  const int argc = 1;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_EQ(app.at("missing").size(), 0);
  EXPECT_FALSE(app.contains("missing"));
}

TEST(Options, mixed_complex_scenario_parses_complex_mixture_correctly) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});
  app.flag("verbose", {"v"});
  app.option("opt", {"o"});
  app.flag("x", {"x"});

  const char arg0[] = "prog";
  const char arg1[] = "-vx";
  const char arg2[] = "--file=first";
  const char arg3[] = "-o";
  const char arg4[] = "second";
  const char arg5[] = "-vfthird";
  const char arg6[] = "pos1";
  const char arg7[] = "--";
  const char arg8[] = "should-be-pos";
  const char arg9[] = "--also-pos";
  const char arg10[] = "extra";
  const char arg11[] = "ignored";
  const char arg12[] = "values";
  const char *const argv[] = {arg0, arg1, arg2, arg3,  arg4,  arg5, arg6,
                              arg7, arg8, arg9, arg10, arg11, arg12};
  const int argc = 13;

  app.parse(argc, argv, {.skip = 1});

  EXPECT_GE(app.at("verbose").size(), 2);
  EXPECT_EQ(app.at("x").size(), 1);
  EXPECT_GE(app.at("file").size(), 2);
  EXPECT_EQ(app.at("file")[0], "first");
  EXPECT_EQ(app.at("file")[1], "third");
  EXPECT_EQ(app.at("opt").size(), 1);
  EXPECT_EQ(app.at("opt")[0], "second");
  EXPECT_GE(app.positional().size(), 3);
  EXPECT_EQ(app.positional()[0], "pos1");
  EXPECT_EQ(app.positional()[1], "should-be-pos");
  EXPECT_EQ(app.positional()[2], "--also-pos");
}

TEST(Options, no_skip_includes_program_name_as_positional) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const char arg0[] = "prog";
  const char arg1[] = "--foo";
  const char arg2[] = "bar";
  const char *const argv[] = {arg0, arg1, arg2};
  const int argc = 3;

  app.parse(argc, argv);

  EXPECT_EQ(app.positional().size(), 1);
  EXPECT_EQ(app.positional()[0], "prog");
  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
}

TEST(Options, no_skip_treats_program_name_as_option_if_prefixed) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const char arg0[] = "-ffromprog";
  const char *const argv[] = {arg0};
  const int argc = 1;

  app.parse(argc, argv);

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "fromprog");
}
