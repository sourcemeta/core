sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME text
  SOURCES text_to_title_case_test.cc text_to_lowercase_test.cc
          text_is_lowercase_test.cc
          text_is_alpha_test.cc text_is_digit_test.cc text_is_alphanum_test.cc
          text_truncate_test.cc text_remove_suffix_ignore_case_test.cc
          text_equals_ignore_case_test.cc
          text_starts_with_ignore_case_test.cc
          text_unquote_test.cc
          text_less_ignore_case_test.cc
          text_trim_test.cc text_take_until_test.cc text_split_once_test.cc
          text_rsplit_once_test.cc
          text_split_test.cc text_join_to_test.cc
          text_hex_to_bytes_test.cc text_bytes_to_hex_test.cc
          text_is_hex_digit_test.cc text_hex_digit_value_test.cc
          text_strip_left_test.cc text_strip_right_test.cc
          text_pad_left_test.cc text_squeeze_test.cc)

target_link_libraries(sourcemeta_core_text_unit
  PRIVATE sourcemeta::core::text)
