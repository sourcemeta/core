sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME text
  SOURCES text_to_title_case_test.cc text_to_lowercase_test.cc
          text_is_lowercase_test.cc
          text_truncate_test.cc text_remove_suffix_ignore_case_test.cc
          text_trim_test.cc text_take_until_test.cc text_split_once_test.cc
          text_split_test.cc text_join_to_test.cc
          text_hex_to_bytes_test.cc)

target_link_libraries(sourcemeta_core_text_unit
  PRIVATE sourcemeta::core::text)
