#include <sourcemeta/core/diff.h>
#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <locale>      // std::locale, std::numpunct
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {

class GroupingNumPunct : public std::numpunct<char> {
protected:
  auto do_thousands_sep() const -> char override { return ','; }
  auto do_grouping() const -> std::string override { return "\3"; }
};

auto render(const sourcemeta::core::Diff &result, const std::size_t context = 3)
    -> std::string {
  std::ostringstream stream;
  sourcemeta::core::stringify(result, stream,
                              sourcemeta::core::Diff::Format::Unified,
                              {.context = context});
  return stream.str();
}

constexpr std::string_view LAO{
    "The Way that can be told of is not the eternal Way;\n"
    "The name that can be named is not the eternal name.\n"
    "The Nameless is the origin of Heaven and Earth;\n"
    "The Named is the mother of all things.\n"
    "Therefore let there always be non-being,\n"
    "  so we may see their subtlety,\n"
    "And let there always be being,\n"
    "  so we may see their outcome.\n"
    "The two are the same,\n"
    "But after they are produced,\n"
    "  they have different names.\n"};

constexpr std::string_view TZU{
    "The Nameless is the origin of Heaven and Earth;\n"
    "The named is the mother of all things.\n"
    "\n"
    "Therefore let there always be non-being,\n"
    "  so we may see their subtlety,\n"
    "And let there always be being,\n"
    "  so we may see their outcome.\n"
    "The two are the same,\n"
    "But after they are produced,\n"
    "  they have different names.\n"
    "They both may be called deep and profound.\n"
    "Deeper and more profound,\n"
    "The door of all subtleties!\n"};

} // namespace

TEST(identical_single_line) {
  const auto result{
      sourcemeta::core::diff("a\n", "a\n", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.original.size(), 1);
  EXPECT_EQ(result.modified.size(), 1);
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1}}));
  EXPECT_EQ(render(result), "");
}

TEST(identical_multiple_lines) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\n", "a\nb\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 3, 0, 3}}));
  EXPECT_EQ(render(result), "");
}

TEST(tokens_exclude_their_terminator) {
  const auto result{sourcemeta::core::diff(
      "alpha\nbeta\n", "alpha\nbeta\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.original.at(0), "alpha");
  EXPECT_EQ(result.original.at(1), "beta");
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 2, 0, 2}}));
  EXPECT_EQ(render(result), "");
}

TEST(both_inputs_empty) {
  const auto result{
      sourcemeta::core::diff("", "", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.original.size(), 0);
  EXPECT_EQ(result.modified.size(), 0);
  EXPECT_TRUE(result.original_ends_with_newline);
  EXPECT_TRUE(result.modified_ends_with_newline);
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{}));
  EXPECT_EQ(render(result), "");
}

TEST(original_empty) {
  const auto result{sourcemeta::core::diff(
      "", "x\ny\nz\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.original.size(), 0);
  EXPECT_EQ(result.modified.size(), 3);
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Insert, 0, 0, 0, 3}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -0,0 +1,3 @@\n"
                            "+x\n"
                            "+y\n"
                            "+z\n");
}

TEST(modified_empty) {
  const auto result{sourcemeta::core::diff(
      "x\ny\nz\n", "", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 3, 0, 0}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,3 +0,0 @@\n"
                            "-x\n"
                            "-y\n"
                            "-z\n");
}

TEST(single_line_replacement) {
  const auto result{sourcemeta::core::diff(
      "one\n", "two\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 0, 1}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1 +1 @@\n"
                            "-one\n"
                            "+two\n");
}

TEST(custom_labels) {
  const auto result{sourcemeta::core::diff(
      "one\n", "two\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 0, 1}}));
  std::ostringstream stream;
  sourcemeta::core::stringify(
      result, stream, sourcemeta::core::Diff::Format::Unified,
      {.original_label = "old.txt", .modified_label = "new.txt", .context = 3});
  EXPECT_EQ(stream.str(), "--- old.txt\n"
                          "+++ new.txt\n"
                          "@@ -1 +1 @@\n"
                          "-one\n"
                          "+two\n");
}

TEST(insertion_in_the_middle) {
  const auto result{sourcemeta::core::diff(
      "a\nc\n", "a\nb\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
                {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 1, 2},
                {sourcemeta::core::Diff::Operation::Type::Equal, 1, 2, 2, 3}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,2 +1,3 @@\n"
                            " a\n"
                            "+b\n"
                            " c\n");
}

TEST(deletion_in_the_middle) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\n", "a\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
                {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
                {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,3 +1,2 @@\n"
                            " a\n"
                            "-b\n"
                            " c\n");
}

TEST(replacement_in_the_middle) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\n", "a\nx\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
                {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
                {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2},
                {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 2, 3}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,3 +1,3 @@\n"
                            " a\n"
                            "-b\n"
                            "+x\n"
                            " c\n");
}

TEST(replacement_in_the_middle_without_context) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\n", "a\nx\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
                {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
                {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2},
                {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 2, 3}}));
  EXPECT_EQ(render(result, 0), "--- a\n"
                               "+++ b\n"
                               "@@ -2 +2 @@\n"
                               "-b\n"
                               "+x\n");
}

TEST(every_line_replaced) {
  const auto result{sourcemeta::core::diff(
      "a\nb\n", "x\ny\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 2, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 0, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,2 +1,2 @@\n"
                            "-a\n"
                            "-b\n"
                            "+x\n"
                            "+y\n");
}

TEST(entirely_unique_lines) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\n", "d\ne\nf\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 3, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 3, 3, 0, 3}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,3 +1,3 @@\n"
                            "-a\n"
                            "-b\n"
                            "-c\n"
                            "+d\n"
                            "+e\n"
                            "+f\n");
}

TEST(duplicate_line_insertion_lands_last) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\n", "a\nb\nb\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 2, 0, 2},
                {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 2, 3},
                {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 3, 4}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,3 +1,4 @@\n"
                            " a\n"
                            " b\n"
                            "+b\n"
                            " c\n");
}

TEST(duplicate_line_deletion_lands_last) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nb\nc\n", "a\nb\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 2, 0, 2},
                {sourcemeta::core::Diff::Operation::Type::Delete, 2, 3, 2, 2},
                {sourcemeta::core::Diff::Operation::Type::Equal, 3, 4, 2, 3}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,4 +1,3 @@\n"
                            " a\n"
                            " b\n"
                            "-b\n"
                            " c\n");
}

TEST(entirely_repeated_lines) {
  const auto result{sourcemeta::core::diff(
      "r\nr\nr\nr\n", "r\nr\nr\nr\nr\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 4, 0, 4},
          {sourcemeta::core::Diff::Operation::Type::Insert, 4, 4, 4, 5}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -2,3 +2,4 @@\n"
                            " r\n"
                            " r\n"
                            " r\n"
                            "+r\n");
}

TEST(trailing_empty_line_is_a_line) {
  const auto result{
      sourcemeta::core::diff("a\n", "a\n\n", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.original.size(), 1);
  EXPECT_EQ(result.modified.size(), 2);
  EXPECT_EQ(result.modified.at(1), "");
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1 +1,2 @@\n"
                            " a\n"
                            "+\n");
}

TEST(carriage_return_is_line_content) {
  const auto result{
      sourcemeta::core::diff("a\r\n", "a\n", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.original.at(0), "a\r");
  EXPECT_EQ(result.modified.at(0), "a");
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 0, 1}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1 +1 @@\n"
                            "-a\r\n"
                            "+a\n");
}

TEST(lines_consisting_of_a_single_period) {
  const auto result{sourcemeta::core::diff(
      ".\n", ".\n.\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1 +1,2 @@\n"
                            " .\n"
                            "+.\n");
}

TEST(two_separate_replacements) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\nd\n", "a\nx\nc\ny\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2},
          {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 2, 3},
          {sourcemeta::core::Diff::Operation::Type::Delete, 3, 4, 3, 3},
          {sourcemeta::core::Diff::Operation::Type::Insert, 4, 4, 3, 4}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,4 +1,4 @@\n"
                            " a\n"
                            "-b\n"
                            "+x\n"
                            " c\n"
                            "-d\n"
                            "+y\n");
}

TEST(insertion_at_the_start_without_context) {
  const auto result{sourcemeta::core::diff(
      "b\nc\nd\n", "a\nb\nc\nd\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Insert, 0, 0, 0, 1},
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 3, 1, 4}}));
  EXPECT_EQ(render(result, 0), "--- a\n"
                               "+++ b\n"
                               "@@ -0,0 +1 @@\n"
                               "+a\n");
}

TEST(deletion_at_the_start_without_context) {
  const auto result{sourcemeta::core::diff(
      "a\nb\nc\nd\n", "b\nc\nd\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
                {sourcemeta::core::Diff::Operation::Type::Equal, 1, 4, 0, 3}}));
  EXPECT_EQ(render(result, 0), "--- a\n"
                               "+++ b\n"
                               "@@ -1 +0,0 @@\n"
                               "-a\n");
}

TEST(changes_two_times_the_context_apart_share_a_hunk) {
  const auto result{sourcemeta::core::diff(
      "c1\ne1\ne2\ne3\ne4\ne5\ne6\nc2\n", "C1\ne1\ne2\ne3\ne4\ne5\ne6\nC2\n",
      sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Equal, 1, 7, 1, 7},
          {sourcemeta::core::Diff::Operation::Type::Delete, 7, 8, 7, 7},
          {sourcemeta::core::Diff::Operation::Type::Insert, 8, 8, 7, 8}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,8 +1,8 @@\n"
                            "-c1\n"
                            "+C1\n"
                            " e1\n"
                            " e2\n"
                            " e3\n"
                            " e4\n"
                            " e5\n"
                            " e6\n"
                            "-c2\n"
                            "+C2\n");
}

TEST(changes_beyond_two_times_the_context_split_the_hunk) {
  const auto result{
      sourcemeta::core::diff("c1\ne1\ne2\ne3\ne4\ne5\ne6\ne7\nc2\n",
                             "C1\ne1\ne2\ne3\ne4\ne5\ne6\ne7\nC2\n",
                             sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Equal, 1, 8, 1, 8},
          {sourcemeta::core::Diff::Operation::Type::Delete, 8, 9, 8, 8},
          {sourcemeta::core::Diff::Operation::Type::Insert, 9, 9, 8, 9}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,4 +1,4 @@\n"
                            "-c1\n"
                            "+C1\n"
                            " e1\n"
                            " e2\n"
                            " e3\n"
                            "@@ -6,4 +6,4 @@\n"
                            " e5\n"
                            " e6\n"
                            " e7\n"
                            "-c2\n"
                            "+C2\n");
}

TEST(two_complete_inputs_that_match) {
  const auto result{sourcemeta::core::diff(
      "a\nb\n", "a\nb\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_TRUE(result.original_ends_with_newline);
  EXPECT_TRUE(result.modified_ends_with_newline);
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 2, 0, 2}}));
  EXPECT_EQ(render(result), "");
}

TEST(two_incomplete_inputs_that_match) {
  const auto result{
      sourcemeta::core::diff("a\nb", "a\nb", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_FALSE(result.original_ends_with_newline);
  EXPECT_FALSE(result.modified_ends_with_newline);
  EXPECT_EQ(result.operations,
            (std::vector<sourcemeta::core::Diff::Operation>{
                {sourcemeta::core::Diff::Operation::Type::Equal, 0, 2, 0, 2}}));
  EXPECT_EQ(render(result), "");
}

TEST(a_dropped_terminator_changes_the_final_line) {
  const auto result{sourcemeta::core::diff(
      "a\nb\n", "a\nb", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_TRUE(result.original_ends_with_newline);
  EXPECT_FALSE(result.modified_ends_with_newline);
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,2 +1,2 @@\n"
                            " a\n"
                            "-b\n"
                            "+b\n"
                            "\\ No newline at end of file\n");
}

TEST(an_added_terminator_changes_the_final_line) {
  const auto result{sourcemeta::core::diff(
      "a\nb", "a\nb\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_FALSE(result.original_ends_with_newline);
  EXPECT_TRUE(result.modified_ends_with_newline);
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,2 +1,2 @@\n"
                            " a\n"
                            "-b\n"
                            "\\ No newline at end of file\n"
                            "+b\n");
}

TEST(both_inputs_incomplete_and_differing) {
  const auto result{
      sourcemeta::core::diff("a\nb", "a\nc", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_FALSE(result.original_ends_with_newline);
  EXPECT_FALSE(result.modified_ends_with_newline);
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,2 +1,2 @@\n"
                            " a\n"
                            "-b\n"
                            "\\ No newline at end of file\n"
                            "+c\n"
                            "\\ No newline at end of file\n");
}

TEST(incomplete_single_line_original) {
  const auto result{
      sourcemeta::core::diff("x", "y\nz\n", sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_FALSE(result.original_ends_with_newline);
  EXPECT_TRUE(result.modified_ends_with_newline);
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 1, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1, 1, 0, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1 +1,2 @@\n"
                            "-x\n"
                            "\\ No newline at end of file\n"
                            "+y\n"
                            "+z\n");
}

TEST(no_marker_when_both_inputs_are_complete) {
  const auto result{sourcemeta::core::diff(
      "a\nb\n", "a\nc\n", sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 1, 2, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 2, 2, 1, 2}}));
  EXPECT_EQ(render(result), "--- a\n"
                            "+++ b\n"
                            "@@ -1,2 +1,2 @@\n"
                            " a\n"
                            "-b\n"
                            "+c\n");
}

TEST(hunk_headers_are_not_formatted_through_the_stream_locale) {
  std::ostringstream probe;
  probe.imbue(std::locale{probe.getloc(), new GroupingNumPunct{}});
  probe << 1498;
  EXPECT_EQ(probe.str(), "1,498");

  std::string original;
  for (std::size_t index{0}; index < 1500; ++index) {
    original += "line\n";
  }

  const auto modified{original + "extra\n"};
  const auto result{sourcemeta::core::diff(
      original, modified, sourcemeta::core::Diff::Mode::Line,
      sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Equal, 0, 1500, 0, 1500},
          {sourcemeta::core::Diff::Operation::Type::Insert, 1500, 1500, 1500,
           1501}}));

  std::ostringstream stream;
  stream.imbue(std::locale{stream.getloc(), new GroupingNumPunct{}});
  sourcemeta::core::stringify(result, stream,
                              sourcemeta::core::Diff::Format::Unified);
  EXPECT_EQ(stream.str(), "--- a\n"
                          "+++ b\n"
                          "@@ -1498,3 +1498,4 @@\n"
                          " line\n"
                          " line\n"
                          " line\n"
                          "+extra\n");
}

TEST(lao_versus_tzu) {
  const auto result{
      sourcemeta::core::diff(LAO, TZU, sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 2, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 3, 4, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 4, 4, 1, 3},
          {sourcemeta::core::Diff::Operation::Type::Equal, 4, 11, 3, 10},
          {sourcemeta::core::Diff::Operation::Type::Insert, 11, 11, 10, 13}}));
  EXPECT_EQ(render(result),
            "--- a\n"
            "+++ b\n"
            "@@ -1,7 +1,6 @@\n"
            "-The Way that can be told of is not the eternal Way;\n"
            "-The name that can be named is not the eternal name.\n"
            " The Nameless is the origin of Heaven and Earth;\n"
            "-The Named is the mother of all things.\n"
            "+The named is the mother of all things.\n"
            "+\n"
            " Therefore let there always be non-being,\n"
            "   so we may see their subtlety,\n"
            " And let there always be being,\n"
            "@@ -9,3 +8,6 @@\n"
            " The two are the same,\n"
            " But after they are produced,\n"
            "   they have different names.\n"
            "+They both may be called deep and profound.\n"
            "+Deeper and more profound,\n"
            "+The door of all subtleties!\n");
}

TEST(lao_versus_tzu_without_context) {
  const auto result{
      sourcemeta::core::diff(LAO, TZU, sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 2, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 3, 4, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 4, 4, 1, 3},
          {sourcemeta::core::Diff::Operation::Type::Equal, 4, 11, 3, 10},
          {sourcemeta::core::Diff::Operation::Type::Insert, 11, 11, 10, 13}}));
  EXPECT_EQ(render(result, 0),
            "--- a\n"
            "+++ b\n"
            "@@ -1,2 +0,0 @@\n"
            "-The Way that can be told of is not the eternal Way;\n"
            "-The name that can be named is not the eternal name.\n"
            "@@ -4 +2,2 @@\n"
            "-The Named is the mother of all things.\n"
            "+The named is the mother of all things.\n"
            "+\n"
            "@@ -11,0 +11,3 @@\n"
            "+They both may be called deep and profound.\n"
            "+Deeper and more profound,\n"
            "+The door of all subtleties!\n");
}

TEST(lao_versus_tzu_with_one_line_of_context) {
  const auto result{
      sourcemeta::core::diff(LAO, TZU, sourcemeta::core::Diff::Mode::Line,
                             sourcemeta::core::Diff::Algorithm::Myers)};
  EXPECT_EQ(
      result.operations,
      (std::vector<sourcemeta::core::Diff::Operation>{
          {sourcemeta::core::Diff::Operation::Type::Delete, 0, 2, 0, 0},
          {sourcemeta::core::Diff::Operation::Type::Equal, 2, 3, 0, 1},
          {sourcemeta::core::Diff::Operation::Type::Delete, 3, 4, 1, 1},
          {sourcemeta::core::Diff::Operation::Type::Insert, 4, 4, 1, 3},
          {sourcemeta::core::Diff::Operation::Type::Equal, 4, 11, 3, 10},
          {sourcemeta::core::Diff::Operation::Type::Insert, 11, 11, 10, 13}}));
  EXPECT_EQ(render(result, 1),
            "--- a\n"
            "+++ b\n"
            "@@ -1,5 +1,4 @@\n"
            "-The Way that can be told of is not the eternal Way;\n"
            "-The name that can be named is not the eternal name.\n"
            " The Nameless is the origin of Heaven and Earth;\n"
            "-The Named is the mother of all things.\n"
            "+The named is the mother of all things.\n"
            "+\n"
            " Therefore let there always be non-being,\n"
            "@@ -11 +10,4 @@\n"
            "   they have different names.\n"
            "+They both may be called deep and profound.\n"
            "+Deeper and more profound,\n"
            "+The door of all subtleties!\n");
}
