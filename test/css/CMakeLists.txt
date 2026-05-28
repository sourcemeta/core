sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME css
  SOURCES css2_hex_color_test.cc css2_color_keyword_test.cc
          css2_rgb_function_test.cc css2_color_test.cc)

target_link_libraries(sourcemeta_core_css_unit
  PRIVATE sourcemeta::core::css)
