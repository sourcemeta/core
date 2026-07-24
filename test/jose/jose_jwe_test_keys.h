#ifndef SOURCEMETA_CORE_TEST_JOSE_JWE_TEST_KEYS_H_
#define SOURCEMETA_CORE_TEST_JOSE_JWE_TEST_KEYS_H_

#include <string_view> // std::string_view

// A 2048-bit RSA keypair and a NIST P-256 keypair as JSON Web Keys, generated
// out of band, shared by the JWE round-trip fixtures

static constexpr std::string_view JWE_RSA_PRIVATE_JWK{
    R"({"kty":"RSA","n":"qp63b87vkztqtUnT7TvIhst4O0QraUSHOPKep5EjRM5EVGOcu56-24usR2wz7_d6UVOgjzcXNQbgzTtcN_mNQRzi3WC_pGT2N5Z1eYUyUCmHMxANntSMmgpAxoKw2jfpt6P0Fc5L4B08JKaXeS8vnpeKjOeX3UkzR9ub66Mp3EGnFRL15D1h9l4ohFmHDm4GPX5NwV5XMw_0ha_BZihp6wtsUBwIcrM1ipGbiOQtHB1sXsUc_pPKLNeR-HzC8PmVtWDOqGSJFsiF6SvxBsgqKpNyov8sYTj_AM3JB57BUrHSZFoNYAcRmWu__EwU1awt9oXbcHyAbJLI85rlxtP6hw","e":"AQAB","d":"H9H69wy6bJSLuaS7tAZOxyjiUvdLO1Yu--VvxmOQls_7OVvcirc39B0P88APzaSzeMBmdN864kZWnaAA-mV9vROEkfHtu9yieMqEPTYkpIOiNdk8GQ-d60qXOw57qi78VSXsKzbuuqzpHxK8tDqjIGo1ykKU9Y2l1TR1V1R2RZ_g1cwzTuOCrWOjgxGOnEqUmePWbEYKdXp794WoBM6_9_SMqjckpoZgkBOHdUKFn4gEkwlinZKkjrTKbeVZBoXKPptUOH774Vnu29JTHRbgt1dKSybnKWxEQvEWUnz5rUMohCpZnudG2r9AfChWBwGWlxD68pTXayIHxvIJSpnyDQ","p":"3KuxlJ4uTEzudFA_2FrSRJy_hzKTewzViPvnWruOj3GWXJbzKcZNbSsVyJUcNRF7q9OWQuzJo0QTteL-ImjzrtOQxEheukBO0IP6CfBmcnVHIGPRhySmVJlEbRyMXZXna4srHaKNenhjNBs0LZelP7CWbYhK4MRexkHiWT-waTM","q":"xe-rGqU0jcXffUVeNezw-SdWxEscBwCaC6bne4dC3PZs09i08TIcGK_-h8uusNaEjXfcs9MopHzu214wWMwpMvTW8g0Y5DSDBlJEbgxWub7pZ-JYlijtks449MtIFs00WJlYiaYk95yQjDoM6e0yrsvTFyl-he06eVO79T_5MV0","dp":"PY0Gc1XLxCyRNpBLvqVWkRzHr4ZD7MafqR3aYkx-bzRXpGvWV4iggBrMcK5mt8awoDTcbL5vX8LVXJ58FE0B63nHn3gwM_NMCI1HaQ5NBRABTt-IyhFG88HsfCLCZDgz3KNs209D9y0tPpN38l-iECnJS7QAbsFXTiWCZTKGmlE","dq":"JX1wnhpeat_opbSG1JXtSnO-Bm3ngBgIFV7mh0omSu3NzQ5Fr017bt2eyx-0FcB72MO0JMdA8m7LWvdrFtaMkx82ux_p8AdWLdNVLLbfkkzrroZ7mu1jmiO_O_Zo0BDL8mARGflDFTctVbIFEiWgLOVkNQXh9gHpymMmvZwBWe0","qi":"AxoWhAMC61kkqGnsIYvVvy0JpqmLvaTKwg11an5aCDkCvXFpoe1dtndpTBw9iW3TAe8feFe-4496fontVN9MlVyLtDTwi-CiM74BzvN3d5T8CdlM_LoxEq8HtyZCCH_kvFT3eT_PcLtJmvNuWpnElv7MSnj8Z4XgttqlmUnhiR4"})"};

static constexpr std::string_view JWE_RSA_PUBLIC_JWK{
    R"({"kty":"RSA","n":"qp63b87vkztqtUnT7TvIhst4O0QraUSHOPKep5EjRM5EVGOcu56-24usR2wz7_d6UVOgjzcXNQbgzTtcN_mNQRzi3WC_pGT2N5Z1eYUyUCmHMxANntSMmgpAxoKw2jfpt6P0Fc5L4B08JKaXeS8vnpeKjOeX3UkzR9ub66Mp3EGnFRL15D1h9l4ohFmHDm4GPX5NwV5XMw_0ha_BZihp6wtsUBwIcrM1ipGbiOQtHB1sXsUc_pPKLNeR-HzC8PmVtWDOqGSJFsiF6SvxBsgqKpNyov8sYTj_AM3JB57BUrHSZFoNYAcRmWu__EwU1awt9oXbcHyAbJLI85rlxtP6hw","e":"AQAB"})"};

static constexpr std::string_view JWE_EC_PRIVATE_JWK{
    R"({"kty":"EC","crv":"P-256","x":"Xj79PaECEYp-X00UVpF6m03kgyZA00m1hAg44rjPdb4","y":"ua7Y-_UMIK_dcm-DtHUdDWChUGMW9XoHu_8ElacPy2U","d":"Tuno8uisHfJyCKlCiaekQzGnkKGjwljr_XDrBaivmGg"})"};

static constexpr std::string_view JWE_EC_PUBLIC_JWK{
    R"({"kty":"EC","crv":"P-256","x":"Xj79PaECEYp-X00UVpF6m03kgyZA00m1hAg44rjPdb4","y":"ua7Y-_UMIK_dcm-DtHUdDWChUGMW9XoHu_8ElacPy2U"})"};

#endif
