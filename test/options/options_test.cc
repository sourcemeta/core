#include <sourcemeta/core/options.h>
#include <sourcemeta/core/test.h>

#include <array> // std::array

TEST(long_option_equals_parses_value_with_equal_sign) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const std::array<const char *, 2> argv{{"prog", "--foo=bar"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
  EXPECT_TRUE(app.contains("foo"));
}

TEST(long_option_space_parses_value_after_space) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const std::array<const char *, 3> argv{{"prog", "--foo", "bar"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
}

TEST(short_option_space_parses_short_option_with_space_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 3> argv{{"prog", "-f", "path/to/x"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "path/to/x");
}

TEST(short_option_attached_value_parses_short_option_attached_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 2> argv{{"prog", "-fpath"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "path");
}

TEST(combined_flags_parses_combined_short_flags) {
  sourcemeta::core::Options app;
  app.flag("alpha", {"a"});
  app.flag("beta", {"b"});
  app.flag("gamma", {"c"});

  const std::array<const char *, 2> argv{{"prog", "-abc"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("alpha").size(), 1);
  EXPECT_EQ(app.at("beta").size(), 1);
  EXPECT_EQ(app.at("gamma").size(), 1);
}

TEST(
    combined_flags_and_option_with_value_parses_combined_flags_and_option_value) {
  sourcemeta::core::Options app;
  app.flag("alpha", {"a"});
  app.option("bopt", {"b"});

  const std::array<const char *, 2> argv{{"prog", "-abvalue"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("alpha").size(), 1);
  EXPECT_EQ(app.at("bopt").size(), 1);
  EXPECT_EQ(app.at("bopt")[0], "value");
}

TEST(repeated_options_preserved_order) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const std::array<const char *, 6> argv{
      {"prog", "--foo=one", "--foo", "two", "-f", "three"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("foo").size(), 3);
  EXPECT_EQ(app.at("foo")[0], "one");
  EXPECT_EQ(app.at("foo")[1], "two");
  EXPECT_EQ(app.at("foo")[2], "three");
}

TEST(flags_count_multiple_occurrences) {
  sourcemeta::core::Options app;
  app.flag("exclude", {"x"});

  const std::array<const char *, 3> argv{{"prog", "-x", "--exclude"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("exclude").size(), 2);
  EXPECT_TRUE(app.contains("exclude"));
}

TEST(unknown_option_throws) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const std::array<const char *, 2> argv{{"prog", "--unknown"}};

  try {
    app.parse(static_cast<int>(argv.size()), argv.data());
    FAIL();
  } catch (const sourcemeta::core::OptionsUnknownOptionError &error) {
    EXPECT_STREQ(error.what(), "Unknown option");
    EXPECT_EQ(error.option(), "unknown");
  }
}

TEST(flag_given_value_throws) {
  sourcemeta::core::Options app;
  app.flag("verbose", {"v"});

  const std::array<const char *, 2> argv{{"prog", "--verbose=1"}};

  try {
    app.parse(static_cast<int>(argv.size()), argv.data());
    FAIL();
  } catch (const sourcemeta::core::OptionsUnexpectedValueFlagError &error) {
    EXPECT_STREQ(error.what(), "This flag cannot take a value");
    EXPECT_EQ(error.option(), "verbose");
  }
}

TEST(positional_after_double_dash) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});
  app.flag("x", {"x"});

  const std::array<const char *, 4> argv{{"prog", "--", "-not-an-opt", "pos2"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_EQ(app.positional().size(), 2);
  EXPECT_EQ(app.positional()[0], "-not-an-opt");
  EXPECT_EQ(app.positional()[1], "pos2");
}

TEST(positional_before_options) {
  sourcemeta::core::Options app;
  app.option("foo", {"foo"});

  const std::array<const char *, 4> argv{{"prog", "pos1", "--foo", "bar"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_EQ(app.positional().size(), 1);
  EXPECT_EQ(app.positional()[0], "pos1");
  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
}

TEST(skip_parameter_works) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 4> argv{{"prog", "cmd", "-f", "file.txt"}};

  app.parse(static_cast<int>(argv.size()), argv.data(), {.skip = 1});

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "file.txt");
}

TEST(alias_mapping_recognizes_aliases) {
  sourcemeta::core::Options app;
  app.option("file", {"f", "a"});

  const std::array<const char *, 3> argv{{"prog", "-a", "ok"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "ok");
}

TEST(option_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});
  app.flag("other", {"o"});

  const std::array<const char *, 3> argv{{"prog", "--file", "--other"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "--other");
  EXPECT_EQ(app.at("other").size(), 0);
}

TEST(long_option_without_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 2> argv{{"prog", "--file"}};

  try {
    app.parse(static_cast<int>(argv.size()), argv.data());
    FAIL();
  } catch (const sourcemeta::core::OptionsMissingOptionValueError &error) {
    EXPECT_STREQ(error.what(), "This option must take a value");
    EXPECT_EQ(error.option(), "file");
  }
}

TEST(short_option_without_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 2> argv{{"prog", "-f"}};

  try {
    app.parse(static_cast<int>(argv.size()), argv.data());
    FAIL();
  } catch (const sourcemeta::core::OptionsMissingOptionValueError &error) {
    EXPECT_EQ(error.option(), "f");
  }
}

TEST(single_dash_is_consumed_as_value) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 3> argv{{"prog", "-f", "-"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "-");
}

TEST(empty_result_for_missing_option) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 1> argv{{"prog"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
}

TEST(mixed_complex_scenario_parses_complex_mixture_correctly) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});
  app.flag("verbose", {"v"});
  app.option("opt", {"o"});
  app.flag("x", {"x"});

  const std::array<const char *, 13> argv{
      {"prog", "-vx", "--file=first", "-o", "second", "-vfthird", "pos1", "--",
       "should-be-pos", "--also-pos", "extra", "ignored", "values"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

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

TEST(no_skip_includes_program_name_as_positional) {
  sourcemeta::core::Options app;
  app.option("foo", {"f"});

  const std::array<const char *, 3> argv{{"prog", "--foo", "bar"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_TRUE(app.positional().empty());
  EXPECT_EQ(app.at("foo").size(), 1);
  EXPECT_EQ(app.at("foo")[0], "bar");
}

TEST(no_skip_treats_program_name_as_option_if_prefixed) {
  sourcemeta::core::Options app;
  app.option("file", {"f"});

  const std::array<const char *, 2> argv{{"prog", "-ffromprog"}};

  app.parse(static_cast<int>(argv.size()), argv.data());

  EXPECT_EQ(app.at("file").size(), 1);
  EXPECT_EQ(app.at("file")[0], "fromprog");
}
