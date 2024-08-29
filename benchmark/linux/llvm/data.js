window.BENCHMARK_DATA = {
  "lastUpdate": 1724975865187,
  "repoUrl": "https://github.com/sourcemeta/jsontoolkit",
  "entries": {
    "Benchmark (linux/llvm)": [
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "80c122533f0ad7d3cea419889792bb8576364110",
          "message": "Fix benchmark names in the GitHub Actions matrix (#1055)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-28T23:19:48-04:00",
          "tree_id": "66d00469fedb1af257cd6c53e0e557397597dcde",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/80c122533f0ad7d3cea419889792bb8576364110"
        },
        "date": 1724901943453,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 390595.31360275426,
            "unit": "ns/iter",
            "extra": "iterations: 1757\ncpu: 390580.62435970403 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 17990.66705660949,
            "unit": "ns/iter",
            "extra": "iterations: 39322\ncpu: 17989.310996388787 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 9927.139015593151,
            "unit": "ns/iter",
            "extra": "iterations: 71251\ncpu: 9926.946625310524 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "79253e81f99a3ea39f4cda62fa3be2a0cda0d12c",
          "message": "Optimize how we determine instance location on target resolution (#1056)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-28T23:19:59-04:00",
          "tree_id": "21ffbaaf0bbb1ba5b227ee04c646f483adcd3ed8",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/79253e81f99a3ea39f4cda62fa3be2a0cda0d12c"
        },
        "date": 1724902082022,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 391340.94084190566,
            "unit": "ns/iter",
            "extra": "iterations: 1758\ncpu: 391323.2997724687 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 14516.194564378904,
            "unit": "ns/iter",
            "extra": "iterations: 47943\ncpu: 14516.475669023632 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 9358.559179271895,
            "unit": "ns/iter",
            "extra": "iterations: 73252\ncpu: 9358.467386555998 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "5f1e1a070854658dd704d41bc00a7826ecb5c0b6",
          "message": "Add various pointer-related evaluator optimization TODOs (#1054)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-28T23:39:09-04:00",
          "tree_id": "e7e3feb3eecf4608adb385da5be986705dd0c065",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/5f1e1a070854658dd704d41bc00a7826ecb5c0b6"
        },
        "date": 1724903088541,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 399347.54301995464,
            "unit": "ns/iter",
            "extra": "iterations: 1755\ncpu: 399335.0336182336 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 14388.134415161165,
            "unit": "ns/iter",
            "extra": "iterations: 49176\ncpu: 14387.528916544657 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 9016.234444531045,
            "unit": "ns/iter",
            "extra": "iterations: 76854\ncpu: 9016.035977307623 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "gorez.tony@gmail.com",
            "name": "Tony Gorez",
            "username": "tony-go"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "cf9503fd399f5dd7c78214759af043a36939763d",
          "message": "chore: highlights dialect support (#1057)\n\nPREVIEW: https://github.com/sourcemeta/jsontoolkit/tree/impr-rdme\r\n\r\nIt triggers me when I check the readme this morning!\r\n\r\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-08-29T16:46:05+02:00",
          "tree_id": "79ff4d206c3097e34c6786773432999ab682ad43",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/cf9503fd399f5dd7c78214759af043a36939763d"
        },
        "date": 1724943140303,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 406159.84167634137,
            "unit": "ns/iter",
            "extra": "iterations: 1718\ncpu: 406125.3259604191 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 14241.046504934622,
            "unit": "ns/iter",
            "extra": "iterations: 48640\ncpu: 14240.228906249997 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 9102.773863353867,
            "unit": "ns/iter",
            "extra": "iterations: 76299\ncpu: 9102.091783640679 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "201fab700c50f44b37dbc16aaae6f20b0990d13a",
          "message": "Support performing schema compilation in \"optimized\" mode (#1059)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T14:33:49-04:00",
          "tree_id": "ec9a8cde74fde53771c08ebc399f6336aef662cb",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/201fab700c50f44b37dbc16aaae6f20b0990d13a"
        },
        "date": 1724956784009,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 395168.2497155959,
            "unit": "ns/iter",
            "extra": "iterations: 1758\ncpu: 395162.85836177465 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 13850.601930251496,
            "unit": "ns/iter",
            "extra": "iterations: 50667\ncpu: 13850.243274715296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 9035.453937535262,
            "unit": "ns/iter",
            "extra": "iterations: 77612\ncpu: 9035.162320259753 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "821c1480dfbb7375154cbc3fc6b403a86956f461",
          "message": "Don't unnecessarily emit `properties` annotations in the \"optimized\" mode (#1058)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T14:44:14-04:00",
          "tree_id": "82f4754f9c8cba12afc013406f2362983b864362",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/821c1480dfbb7375154cbc3fc6b403a86956f461"
        },
        "date": 1724957425614,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 388656.61015058676,
            "unit": "ns/iter",
            "extra": "iterations: 1793\ncpu: 388647.1327384272 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12181.60014872749,
            "unit": "ns/iter",
            "extra": "iterations: 57824\ncpu: 12181.622215688987 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3351.6303284216983,
            "unit": "ns/iter",
            "extra": "iterations: 209091\ncpu: 3351.6112936472646 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Juan Cruz Viotti",
            "username": "jviotti",
            "email": "jv@jviotti.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "821c1480dfbb7375154cbc3fc6b403a86956f461",
          "message": "Don't unnecessarily emit `properties` annotations in the \"optimized\" mode (#1058)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T18:44:14Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/821c1480dfbb7375154cbc3fc6b403a86956f461"
        },
        "date": 1724958604549,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 391686.8216094411,
            "unit": "ns/iter",
            "extra": "iterations: 1777\ncpu: 391680.4327518289 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12082.298647568838,
            "unit": "ns/iter",
            "extra": "iterations: 57526\ncpu: 12081.461043701976 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3452.1658814993066,
            "unit": "ns/iter",
            "extra": "iterations: 206750\ncpu: 3451.8990423216446 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "eb8c8794a72352932adf5dcd3b23f5d344e3b320",
          "message": "Correctly format the new `report` step option (#1060)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T15:19:27-04:00",
          "tree_id": "958feb406f0b7b335ae3aba6ee60cd54549a81b2",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/eb8c8794a72352932adf5dcd3b23f5d344e3b320"
        },
        "date": 1724959514893,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 389044.2757659985,
            "unit": "ns/iter",
            "extra": "iterations: 1795\ncpu: 389023.5682451254 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12332.960086205128,
            "unit": "ns/iter",
            "extra": "iterations: 57073\ncpu: 12332.619977923016 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3440.468831725468,
            "unit": "ns/iter",
            "extra": "iterations: 203043\ncpu: 3440.4503282555916 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "2b5158f237584fff7e35d520c2e5a722d01fd678",
          "message": "Avoid unnecessary `patternProperties` annotations in optimized mode (#1061)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T15:19:49-04:00",
          "tree_id": "64acfd6035f0c97218d268782c3431ca857e827e",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/2b5158f237584fff7e35d520c2e5a722d01fd678"
        },
        "date": 1724959545533,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 390870.83041956363,
            "unit": "ns/iter",
            "extra": "iterations: 1716\ncpu: 390831.4994172494 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12126.290208944838,
            "unit": "ns/iter",
            "extra": "iterations: 57910\ncpu: 12125.390433431186 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3351.4041154441666,
            "unit": "ns/iter",
            "extra": "iterations: 211739\ncpu: 3351.420569663596 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "96a12c3bb487c65af92e6d93961570fe68740936",
          "message": "Establish implicit preconditions for compiler steps (#1062)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T16:45:55-04:00",
          "tree_id": "d2c3d2ea117edb2096b64caa2e2b27a96dc6099f",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/96a12c3bb487c65af92e6d93961570fe68740936"
        },
        "date": 1724964724599,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 392989.465129325,
            "unit": "ns/iter",
            "extra": "iterations: 1778\ncpu: 392980.3543307086 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12262.462665613992,
            "unit": "ns/iter",
            "extra": "iterations: 56985\ncpu: 12261.7863297359 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3399.7357811266284,
            "unit": "ns/iter",
            "extra": "iterations: 205818\ncpu: 3399.674353069217 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "0e5e3e22e1741af944490da28c2eb5ccda1b7b77",
          "message": "Add a benchmark case exercising schema `items` in Draft 4 (#1064)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T17:17:39-04:00",
          "tree_id": "b0fcd429666896585b6cb8073093849e61a5f3fb",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/0e5e3e22e1741af944490da28c2eb5ccda1b7b77"
        },
        "date": 1724966602936,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 391118.3772954927,
            "unit": "ns/iter",
            "extra": "iterations: 1797\ncpu: 391093.28825820814 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12544.142768686295,
            "unit": "ns/iter",
            "extra": "iterations: 56525\ncpu: 12543.917487837245 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3307.3900433451104,
            "unit": "ns/iter",
            "extra": "iterations: 212943\ncpu: 3307.267818148518 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13957.481800594996,
            "unit": "ns/iter",
            "extra": "iterations: 50139\ncpu: 13957.131574223657 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ddc6917ba4af30d18bf64aadb6c25eb6bca32190",
          "message": "Reduce generated steps for `items` schemas before 2019-09 (#1063)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T17:29:01-04:00",
          "tree_id": "c1743107e23c56de6ae4563e9ed5fb7238bf346c",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/ddc6917ba4af30d18bf64aadb6c25eb6bca32190"
        },
        "date": 1724967283041,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 397076.81792073854,
            "unit": "ns/iter",
            "extra": "iterations: 1741\ncpu: 397054.525560023 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 12209.443939526645,
            "unit": "ns/iter",
            "extra": "iterations: 57215\ncpu: 12208.590352180374 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3275.0369452681593,
            "unit": "ns/iter",
            "extra": "iterations: 194531\ncpu: 3274.960705491671 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 10562.578523530965,
            "unit": "ns/iter",
            "extra": "iterations: 66063\ncpu: 10562.243540257026 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9183959ccc0b63aa26a4202cd5adf66f51861b32",
          "message": "Start benchmarking optional schema properties with minimal matches (#1067)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-08-29T19:51:49-04:00",
          "tree_id": "edd348c77b5d73a66a852f01fdf50dfc5916b3d9",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/9183959ccc0b63aa26a4202cd5adf66f51861b32"
        },
        "date": 1724975864871,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Compile_Basic",
            "value": 399662.3879064432,
            "unit": "ns/iter",
            "extra": "iterations: 1753\ncpu: 399642.9395322304 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 11885.112393847392,
            "unit": "ns/iter",
            "extra": "iterations: 58642\ncpu: 11884.181542239348 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3301.820967355966,
            "unit": "ns/iter",
            "extra": "iterations: 213179\ncpu: 3301.701982840712 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 1384.1405102024542,
            "unit": "ns/iter",
            "extra": "iterations: 508504\ncpu: 1384.1125222220467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 10261.978306148718,
            "unit": "ns/iter",
            "extra": "iterations: 68176\ncpu: 10261.884108777283 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}