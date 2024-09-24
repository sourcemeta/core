window.BENCHMARK_DATA = {
  "lastUpdate": 1727216388549,
  "repoUrl": "https://github.com/sourcemeta/jsontoolkit",
  "entries": {
    "Benchmark (windows/msvc)": [
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
          "id": "15595e0c311ea528f8262d2811a59bdd650406d8",
          "message": "Use compiler suffixes for all benchmarked platforms (#1153)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T12:06:02-04:00",
          "tree_id": "78a440b30c56913824be11c277c0372775572040",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/15595e0c311ea528f8262d2811a59bdd650406d8"
        },
        "date": 1725552946341,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3770.748445092342,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3766.734345117241 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3706.1971527297233,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3690.003747760328 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 662.1094866071177,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 662.6674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12516.440624999348,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 12695.3125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9435.946268096817,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6200.698214284119,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5213.589999998476,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5312.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6340.532142857569,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
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
          "id": "379f384c0fc5b06020d8e0bdd867c649ca55c10c",
          "message": "Clarify that schema walkers do not depend on property order (#1155)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T13:12:01-04:00",
          "tree_id": "46351d59852c80f5ae01fdf626cbf71c10afb664",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/379f384c0fc5b06020d8e0bdd867c649ca55c10c"
        },
        "date": 1725556944528,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3690.106981951854,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3664.1945282243487,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 654.2485714285411,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 655.6919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12689.101785712508,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12834.82142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9434.18913308481,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6141.654464285702,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5105.696000000535,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5156.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6234.88214285674,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
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
          "id": "cce494944151091adc5fbb8c44d5781fd89670e7",
          "message": "Enforce a specific ordering when framing schema pointers (#1154)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T13:19:15-04:00",
          "tree_id": "92d9e9b674ab9806d6eae4ed63e430fe15ae638d",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/cce494944151091adc5fbb8c44d5781fd89670e7"
        },
        "date": 1725557354908,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3755.3274012011834,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3732.0088714134936,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3766.734345117241 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 650.5333928570524,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13183.050000001069,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 13392.857142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9395.336627961191,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6179.1455357130035,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5191.222321428769,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5161.830357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6336.053571428936,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6417.410714285715 ns\nthreads: 1"
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
          "id": "4d28211c0ebd46dd6495e35aa4a6fb71fccc86fd",
          "message": "Make compiler Draft 6 object tests independent of ordering (#1156)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T14:09:52-04:00",
          "tree_id": "0233a86935bfd2cce67ef06a0f1ce81df1c3bb13",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/4d28211c0ebd46dd6495e35aa4a6fb71fccc86fd"
        },
        "date": 1725560419134,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3831.3858368110587,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3850.439552786513 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3688.8260269115876,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3690.003747760328 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 657.3707142857797,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 655.6919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12999.907589700075,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 12869.641206958897 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9499.510937500232,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 9277.34375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6214.325892856769,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5184.926999999107,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5156.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6325.375892856756,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
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
          "id": "1167bc4b20358edaee38789106b748f3f8b277ff",
          "message": "Make compiler 2019-09 object tests independent of ordering (#1157)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T14:29:34-04:00",
          "tree_id": "c37692279e6a4f55c0c622eeae2ce79617358c7f",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/1167bc4b20358edaee38789106b748f3f8b277ff"
        },
        "date": 1725561546559,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3837.3547547234957,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3850.439552786513 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3688.2164496138457,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 657.4807142856441,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 655.6919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13016.574999998706,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 13113.839285714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9466.49657814014,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6228.670535713426,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5204.794999999649,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5156.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6333.6598214281785,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
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
          "id": "bb3882da4c41e8df8e2d7ddd3db251b313bd9999",
          "message": "Stop assuming object ordering on the compiler/evaluator (#1158)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T15:51:36-04:00",
          "tree_id": "c8c0b5b0a3436309112966dbe00948ae736bbd4b",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/bb3882da4c41e8df8e2d7ddd3db251b313bd9999"
        },
        "date": 1725566440828,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3784.484134850684,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3850.439552786513 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 3564.5071695172355,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3529.5688022055315 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 644.8943749999461,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12900.176785714328,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 13113.839285714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9436.598497330706,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6152.067857142031,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5118.213392857943,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5022.321428571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6251.173999999082,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 6250 ns\nthreads: 1"
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
          "id": "88705fa8e137c403709ca800024ee6617257a721",
          "message": "Make use of `unordered_map` for JSON objects (#1142)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T16:06:03-04:00",
          "tree_id": "ced2076b78728c57cd1d0ad014e3abd6d8e73c4d",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/88705fa8e137c403709ca800024ee6617257a721"
        },
        "date": 1725568029080,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3649.8457257563623,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2133.745000000076,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2148.4375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 636.8503348214308,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12804.80892857229,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12834.82142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 8957.857142857556,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 9068.080357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5995.420535714605,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4976.476785714112,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 4882.8125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6240.454464285148,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
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
          "id": "567e714a7f552a680da7a161a7c97c95d18c895a",
          "message": "Upgrade Noa to the latest version (#1166)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-05T16:52:58-04:00",
          "tree_id": "e5b0af78c69d18c8e0dc8b078c733b1f01b73d1b",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/567e714a7f552a680da7a161a7c97c95d18c895a"
        },
        "date": 1725570160540,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3626.172715277862,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2137.2390625003845,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2148.4375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 635.5453571428272,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12736.387500000643,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 8969.438975719671,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8998.28572193874 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5969.68035714391,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5004.847000000154,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6049.828571428456,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
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
          "id": "c389796a6892e4d8428a0db545fa7bb6b16c4ac8",
          "message": "Add a Draft 4 benchmark case that exercises non-recursive `$ref` (#1168)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-06T12:16:35-04:00",
          "tree_id": "ed6e71c22d52ad62bbaf4fdd6573c9782991a46b",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/c389796a6892e4d8428a0db545fa7bb6b16c4ac8"
        },
        "date": 1725639950981,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3680.2584281106115,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2105.9179083902814,
            "unit": "ns/iter",
            "extra": "iterations: 344615\ncpu: 2085.6608098892966 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 665.0138392858805,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 669.6428571428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12681.17642331621,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 12555.74751898429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9562.371596556637,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5998.9633928572475,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5012.509999999111,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6151.771428572975,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 11209.671875000993,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11230.46875 ns\nthreads: 1"
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
          "id": "8b1d2e700655bb27a552b8343d956e943c34ee09",
          "message": "Don't mark `$ref` destination on non-recursive cases (#1169)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-06T13:49:49-04:00",
          "tree_id": "ad3c6d0cf5dc0c1def74ae610194af73cef836ec",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/8b1d2e700655bb27a552b8343d956e943c34ee09"
        },
        "date": 1725645564122,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3604.735526201097,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2106.0693750001747,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2099.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 640.5600892857625,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12661.47678571323,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9519.771786732896,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5941.629464285622,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4972.919000000502,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6045.835714286138,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8039.443080356535,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
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
          "id": "84974daae1bbc6f8041e9d759e5117a2e0e064a3",
          "message": "Make generic pointer token operators explicit (#1170)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-06T14:14:06-04:00",
          "tree_id": "bbffc7c80b7ca3213e28dca0f41ed14d323abf13",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/84974daae1bbc6f8041e9d759e5117a2e0e064a3"
        },
        "date": 1725647054384,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3675.7498647321263,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3599.3239297786968 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2191.4262499997503,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2197.265625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 645.0017857142752,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12958.549961831932,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 12869.641206958897 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9558.466256847229,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6095.612500000616,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5113.868000000821,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5156.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6279.162499999253,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6277.901785714285 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8363.723214285405,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Tony Gorez",
            "username": "tony-go",
            "email": "gorez.tony@gmail.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "84974daae1bbc6f8041e9d759e5117a2e0e064a3",
          "message": "Make generic pointer token operators explicit (#1170)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-06T18:14:06Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/84974daae1bbc6f8041e9d759e5117a2e0e064a3"
        },
        "date": 1725650068977,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3683.2482442004116,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2158.42124999952,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2197.265625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 654.4204464286654,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 655.6919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13747.318092331669,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 13811.32227088272 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9633.504687499795,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 9521.484375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6077.124107142547,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5081.70199999995,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6120.740178571639,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8457.864920247304,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8370.498345989527 ns\nthreads: 1"
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
          "id": "dd78dbdf45cdcd83b16c594991c203b16fd9d1f3",
          "message": "Check static reference labels before unconditionally unrolling (#1172)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-06T15:31:48-04:00",
          "tree_id": "2ea7fd56aaf6844f68d5d8432bf6c0706c848985",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/dd78dbdf45cdcd83b16c594991c203b16fd9d1f3"
        },
        "date": 1725651726988,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3656.5958014818907,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3690.003747760328 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2383.5847281419938,
            "unit": "ns/iter",
            "extra": "iterations: 298667\ncpu: 2354.210542175734 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 646.2435714286332,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12845.97678571434,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12834.82142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9342.041330173215,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5971.999999999866,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4954.825071272721,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4973.497744194415 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6044.862499999835,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8297.176798317381,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8370.498345989527 ns\nthreads: 1"
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
          "id": "2ade645fab3ac37ecdf09ca57aa54eedf5071a77",
          "message": "feat(jsonpointer): introduce `WeakPointer` (#1173)\n\nThe current `Pointer` abstraction stores properties by making copies\r\ninstead of keeping references. The new `WeakPointer` is designed to\r\neliminate unnecessary copies by holding references to the properties\r\ninstead, thereby improving efficiency.\r\n\r\n---------\r\n\r\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-06T21:49:49+02:00",
          "tree_id": "0c08db90905f5f8c98cfaf9b9fe82b2ccbf26c9b",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/2ade645fab3ac37ecdf09ca57aa54eedf5071a77"
        },
        "date": 1725652822253,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3625.525132990641,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3599.3239297786968 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2302.6606249999304,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2294.921875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 653.1998883928683,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 645.2287946428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12752.901785717086,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12834.82142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9325.979348306355,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9207.54818058848 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5941.028571429894,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4967.936999998983,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6024.76999999908,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8274.024553571735,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "64e3fa611d3fd0c8f71125f2e40778fb5d1f4b0d",
          "message": "Measure compile time on `validator_helper.cc` (#1174)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-06T17:03:37-04:00",
          "tree_id": "a45077ba6d2e87129c2e629f3515d8f5d8b725f9",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/64e3fa611d3fd0c8f71125f2e40778fb5d1f4b0d"
        },
        "date": 1725657390508,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3730.3567315060213,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2294.6016801320434,
            "unit": "ns/iter",
            "extra": "iterations: 298667\ncpu: 2301.8947523496067 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 649.0042410714889,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 645.2287946428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13292.610714285858,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 13392.857142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9613.650000000362,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 9521.484375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6003.482142856699,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4996.678999999631,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6057.455357143178,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8322.654017856987,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "67cd9bacfaab33f22a422a0b02718c9e2f54293d",
          "message": "Fix static reference cycles between subschemas (#1175)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-06T17:54:21-04:00",
          "tree_id": "061a3da65c5742645137b73bfce95f5011d96e83",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/67cd9bacfaab33f22a422a0b02718c9e2f54293d"
        },
        "date": 1725660205443,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3701.071962132465,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3690.003747760328 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2093.527187500044,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2099.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 646.1502678571865,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12928.024428464334,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 12869.641206958897 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9418.67357734951,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5987.238392858022,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4967.77450247776,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 5081.617260372554 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6110.321428571247,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8300.886160714783,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "f640240e814603f709efb86dc2391c53b519485b",
          "message": "Property detect recursion when compiling `$ref` (#1176)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T11:34:07-04:00",
          "tree_id": "ece2265798ba8b8134e320f7b345622c78ed4262",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/f640240e814603f709efb86dc2391c53b519485b"
        },
        "date": 1725896665296,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3497.8406739811085,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3449.351329428133 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2199.7834375000025,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2197.265625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 647.2148437499976,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 645.2287946428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13275.94519667287,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 13497.428582908113 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9480.50544417316,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9416.810639238218 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6063.272000000098,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5045.001000000866,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6111.4633928564135,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8854.765826939163,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8789.023263289004 ns\nthreads: 1"
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
          "id": "fc68c8a8a30f8f7dfc5468589aa39745115aeb26",
          "message": "Improve compilation speed of highly recursive schemas (#1177)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T11:49:54-04:00",
          "tree_id": "2596c042dc82088a83dba0a8009837492239da4b",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/fc68c8a8a30f8f7dfc5468589aa39745115aeb26"
        },
        "date": 1725897583380,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3618.5267708165743,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2341.5549759432374,
            "unit": "ns/iter",
            "extra": "iterations: 298667\ncpu: 2354.210542175734 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 645.4684821427747,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12956.173409940457,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 12869.641206958897 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9362.87723214286,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 9416.85267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5934.30624999972,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4951.282902930768,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4973.497744194415 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6109.054464285382,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8748.00916067371,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8998.28572193874 ns\nthreads: 1"
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
          "id": "6c78f131029c647be79cc5da5874fa8d50415cde",
          "message": "Better handle invalid static references when compiling (#1178)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T12:19:35-04:00",
          "tree_id": "cd30c8ddb822ace930770c235bfe5ca1bf7e927e",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/6c78f131029c647be79cc5da5874fa8d50415cde"
        },
        "date": 1725899319840,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3661.7918539430807,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3599.3239297786968 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2283.0985679703754,
            "unit": "ns/iter",
            "extra": "iterations: 298667\ncpu: 2301.8947523496067 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 671.9875000000185,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 680.1060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13235.531761018055,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 13183.534894933504 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9623.335610110009,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9626.073097887956 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6304.693750000264,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6417.410714285715 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5169.015999999829,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5312.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6369.234821428498,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6417.410714285715 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8910.886160713766,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8893.69419642857 ns\nthreads: 1"
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
          "id": "98b9007745106d35a64489b6f2733e6e419e5ecd",
          "message": "Guard against infinite recursion on the schema evaluator (#1179)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T13:04:20-04:00",
          "tree_id": "a3a873969ba58a3b3b555a93ce1b64901941f46b",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/98b9007745106d35a64489b6f2733e6e419e5ecd"
        },
        "date": 1725902068505,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3619.607460609146,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2153.0931249998275,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2197.265625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 652.129285714206,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12379.848214286376,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12276.785714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9072.07467823628,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8998.28572193874 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5984.4866071431725,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4996.507000000747,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6054.914285714728,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8194.266741072503,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
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
          "id": "a0aafd3f62c03514db9b71b1c89f2ed69669b130",
          "message": "Fix sorting JSON arrays of objects (#1180)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T19:02:01Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/a0aafd3f62c03514db9b71b1c89f2ed69669b130"
        },
        "date": 1725909149675,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3570.2232741048774,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2124.42218750013,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2148.4375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 641.1806249998011,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12358.078571427963,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 8880.879103218114,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8789.023263289004 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5956.014999999298,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5001.776000001428,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6044.908928570538,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8146.8660714304315,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
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
          "id": "a0aafd3f62c03514db9b71b1c89f2ed69669b130",
          "message": "Fix sorting JSON arrays of objects (#1180)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T15:02:01-04:00",
          "tree_id": "ef2092c5a36af64acd7b3d9f53e27c549bc2a32f",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/a0aafd3f62c03514db9b71b1c89f2ed69669b130"
        },
        "date": 1725909188340,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3630.727014164315,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3690.003747760328 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2117.879374999987,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2148.4375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 651.6411607142305,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12551.11607142924,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 8819.580357143206,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8893.69419642857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5955.21500000018,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4976.7050000002655,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6067.8160714289215,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8452.149553571177,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8544.921875 ns\nthreads: 1"
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
          "id": "a0aafd3f62c03514db9b71b1c89f2ed69669b130",
          "message": "Fix sorting JSON arrays of objects (#1180)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-09T19:02:01Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/a0aafd3f62c03514db9b71b1c89f2ed69669b130"
        },
        "date": 1725995613123,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3652.7730859467797,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2112.004120540474,
            "unit": "ns/iter",
            "extra": "iterations: 344615\ncpu: 2085.6608098892966 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 658.6141964285324,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 669.6428571428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12800.808928570208,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12834.82142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9038.76545194052,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9207.54818058848 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5977.01785714325,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4944.520999999895,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6045.105999999123,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8423.595982142628,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "fda77d5cda5588f9c22888d95eb2ae4e8b41b259",
          "message": "refactor: stringify and get to handle WeakPointer (#1182)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-11T18:27:02+02:00",
          "tree_id": "86994b08718ad6c4264cff4f6c5bed9800f6f156",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/fda77d5cda5588f9c22888d95eb2ae4e8b41b259"
        },
        "date": 1726072622972,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3579.460750962788,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3599.3239297786968 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2128.6956250001767,
            "unit": "ns/iter",
            "extra": "iterations: 320000\ncpu: 2148.4375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 643.3550223212998,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 645.2287946428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12484.251785715676,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9147.319431609942,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 9207.54818058848 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5976.888392857356,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4956.316964286397,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 4882.8125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6013.281249999685,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8436.164062499378,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Tony Gorez",
            "username": "tony-go",
            "email": "gorez.tony@gmail.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "fda77d5cda5588f9c22888d95eb2ae4e8b41b259",
          "message": "refactor: stringify and get to handle WeakPointer (#1182)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-11T16:27:02Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/fda77d5cda5588f9c22888d95eb2ae4e8b41b259"
        },
        "date": 1726081989999,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3659.835428865092,
            "unit": "ns/iter",
            "extra": "iterations: 186667\ncpu: 3683.029137447969 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2104.9194028119027,
            "unit": "ns/iter",
            "extra": "iterations: 344615\ncpu: 2085.6608098892966 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 662.265513392768,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 662.6674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 13012.963558197627,
            "unit": "ns/iter",
            "extra": "iterations: 49778\ncpu: 12869.641206958897 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 9067.17693224746,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8998.28572193874 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6129.724107142108,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5058.164000001852,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5156.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6114.036607142128,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8419.865536312802,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8370.498345989527 ns\nthreads: 1"
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
          "id": "6e9309cdab7eb6bebf3f9eb5a5e8dd5677524231",
          "message": "Support pushing a `Pointer` to the back of a `WeakPointer` (#1184)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-11T16:18:58-04:00",
          "tree_id": "ca7b69e72df554f608d379693a6050c0759b2544",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/6e9309cdab7eb6bebf3f9eb5a5e8dd5677524231"
        },
        "date": 1726086522603,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3586.17230456512,
            "unit": "ns/iter",
            "extra": "iterations: 194783\ncpu: 3609.7862749829296 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2102.7630834406154,
            "unit": "ns/iter",
            "extra": "iterations: 344615\ncpu: 2085.6608098892966 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 635.4034598213027,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12463.124999998498,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 8944.551140396243,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8789.023263289004 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5935.807142857844,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4932.42339948559,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6021.705357142082,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 8410.5122767854,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "5152b09cd26a9755ab4af6a15011918d6f4e6e57",
          "message": "feat(jsonschema): use `WeakPointer `in `EvaluationContext`  (#1186)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-12T10:18:52-04:00",
          "tree_id": "1fa2e1cd978558f9a2d7512dbfe55a11b3923c10",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/5152b09cd26a9755ab4af6a15011918d6f4e6e57"
        },
        "date": 1726151277304,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3143.2830357139046,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3138.9508928571427 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1686.5115045682385,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 620.5881999999292,
            "unit": "ns/iter",
            "extra": "iterations: 1000000\ncpu: 609.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11581.510937499927,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6603.187500000932,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5854.6803571419205,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4821.00205580861,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4813.068779171382 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5938.3723214279735,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7232.48102678653,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7324.21875 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Tony Gorez",
            "username": "tony-go",
            "email": "gorez.tony@gmail.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "5152b09cd26a9755ab4af6a15011918d6f4e6e57",
          "message": "feat(jsonschema): use `WeakPointer `in `EvaluationContext`  (#1186)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-12T14:18:52Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/5152b09cd26a9755ab4af6a15011918d6f4e6e57"
        },
        "date": 1726168367219,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3087.7562499997325,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1669.203949193901,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 607.6084821428043,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11569.132142860943,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6753.218749998219,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5919.673000000785,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4881.914113315252,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4973.497744194415 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5978.371428572667,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7198.216071429896,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 7254.464285714285 ns\nthreads: 1"
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
          "id": "4f689bd9da2db682f0faef1253e2634b3f89893d",
          "message": "Add benchmark case for `patternProperties` with true schemas (#1187)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-12T17:36:17-04:00",
          "tree_id": "f42e68df5723aa3e1347893a24156e73e92a3002",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/4f689bd9da2db682f0faef1253e2634b3f89893d"
        },
        "date": 1726177525030,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3089.169978243242,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 3114.5430872517377 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1639.5567592256832,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1649.691975652695 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 617.7470535713984,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11315.046428571966,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6518.46071428557,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5739.459821429347,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4736.790389991177,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5782.189285714675,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7233.989955357195,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7149.832589285715 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 15258.953125000726,
            "unit": "ns/iter",
            "extra": "iterations: 44800\ncpu: 14997.20982142857 ns\nthreads: 1"
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
          "id": "9959e1fbec73e66142af7266fbfe3cdb3043f27d",
          "message": "Test `patternProperties` with wildcard schema on 2019-09 (#1188)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-12T17:49:12-04:00",
          "tree_id": "a81fe767de7c98c3f2816c491e3fef4606d107ec",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/9959e1fbec73e66142af7266fbfe3cdb3043f27d"
        },
        "date": 1726178342436,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3119.204464285628,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3138.9508928571427 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1637.6573660714655,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 615.6332142856919,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11282.723437499699,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11230.46875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6509.093749999449,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6417.410714285715 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5752.680000000509,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4762.572907529241,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4813.068779171382 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5847.480357143046,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7242.4620535700105,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7149.832589285715 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 15381.665178568126,
            "unit": "ns/iter",
            "extra": "iterations: 44800\ncpu: 15345.982142857143 ns\nthreads: 1"
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
          "id": "8e19adee60109171661aa5dad7a672383d660252",
          "message": "Optimize `patternProperties` set to wildcard schemas before 2019-09 (#1189)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-12T18:05:17-04:00",
          "tree_id": "701099754023aa5ec390ddd3b367d94f8343b429",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/8e19adee60109171661aa5dad7a672383d660252"
        },
        "date": 1726179278461,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3097.5736607134404,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1644.5178571429558,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Optional_Properties_Minimal_Match",
            "value": 602.638214285735,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11487.128571425435,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6536.537946428515,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6452.287946428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5691.988392858402,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4710.593290708664,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4649.139195659996 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5777.740000000904,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7338.311383929659,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7324.21875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8028.24665178749,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
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
          "id": "0e6f059ece2c01f7346c9b1ad467efe198f73fcf",
          "message": "Add a new JSON Schema benchmark case for few optional `properties` (#1190)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T11:03:32-04:00",
          "tree_id": "d96bef385261c0d7d6cd8142508e03169a4afa2c",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/0e6f059ece2c01f7346c9b1ad467efe198f73fcf"
        },
        "date": 1726240428260,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3142.5919642862664,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3138.9508928571427 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1643.0962053572023,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 611.61669642859,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 935.8459661403339,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 941.6848474621216 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11468.53749999975,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6515.732142858611,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5889.363392857458,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4871.030889311845,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5963.917857142422,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7444.805803569209,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7498.604910714285 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8215.39397321404,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
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
          "id": "6e26075d85e584ff1b03d516afcc8d230a576cad",
          "message": "Prefer unrolling `properties` for a small amount of optional subschemas (#1191)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T11:39:33-04:00",
          "tree_id": "56ffe2ff4cc0d4a217e3cd475474c7870aa642b5",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/6e26075d85e584ff1b03d516afcc8d230a576cad"
        },
        "date": 1726242519879,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3300.561563377353,
            "unit": "ns/iter",
            "extra": "iterations: 213333\ncpu: 3295.9035873493553 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1652.0464160406268,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1611.3270459863531 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 605.5844642856657,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 641.3469866072116,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11447.38749999874,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11474.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6601.482142857305,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5894.82999999973,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4839.215035012612,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5958.398214285613,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7425.185714286695,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 7393.973214285715 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8276.363839286712,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "8a89db2c8b6b3c0e6ea4576037e92939d726d657",
          "message": "Print the compiled schema in `benchmark/validator_helper.cc` (#1192)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T12:16:49-04:00",
          "tree_id": "06f36d077509c1295da08df179bc08033b2694b6",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/8a89db2c8b6b3c0e6ea4576037e92939d726d657"
        },
        "date": 1726245093295,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3099.6263392860815,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1619.043973214218,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 599.2136607142738,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 633.913214285745,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11670.66071428532,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6545.184821428661,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5941.949999999403,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4909.462000000531,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6023.649999999999,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7596.256696428431,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7672.991071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8405.164062499993,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "8a89db2c8b6b3c0e6ea4576037e92939d726d657",
          "message": "Print the compiled schema in `benchmark/validator_helper.cc` (#1192)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T16:16:49Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/8a89db2c8b6b3c0e6ea4576037e92939d726d657"
        },
        "date": 1726254823671,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3092.739285714328,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1626.6654017858937,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 595.9927999999763,
            "unit": "ns/iter",
            "extra": "iterations: 1000000\ncpu: 593.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 633.455892857171,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11437.12321428682,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6523.4589285714555,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5952.480357142877,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4903.975000000855,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6025.99375000068,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7357.620535714459,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7324.21875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8208.068080356614,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
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
          "id": "7051a3c836cebfa9e3b6c8d612877616f1ed1d75",
          "message": "Implement new `AssertionPropertyType(Strict)` instructions (#1193)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T16:15:04-04:00",
          "tree_id": "a818160d7697d154c45e185dfc84051fbce7c761",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/7051a3c836cebfa9e3b6c8d612877616f1ed1d75"
        },
        "date": 1726259100720,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3253.931249999604,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3208.7053571428573 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1678.237938679899,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 614.9486607142762,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 592.4450000000953,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11992.301562500528,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 12207.03125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6740.599999998511,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6835.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5849.950000000247,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4842.311577956397,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5939.420999998219,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 7852.353794642092,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7672.991071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8374.667410713244,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "dcf4042039449479b985ddcd7aa30446682bf342",
          "message": "Don't emit a wrapper instruction when unrolling `$ref` (#1195)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T17:22:36-04:00",
          "tree_id": "b117ef5c42037a7c908b5dc585a7f9d22436d854",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/dcf4042039449479b985ddcd7aa30446682bf342"
        },
        "date": 1726263112296,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3073.4397321430556,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1651.0183586928968,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1649.691975652695 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 619.1101785712964,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 585.2369642856355,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11839.576785713949,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6726.0357142860785,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5915.2319999998335,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4860.720612249956,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5966.155357143066,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1188.4440625003378,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1196.2890625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8388.24218750034,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
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
          "id": "dcf4042039449479b985ddcd7aa30446682bf342",
          "message": "Don't emit a wrapper instruction when unrolling `$ref` (#1195)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-13T21:22:36Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/dcf4042039449479b985ddcd7aa30446682bf342"
        },
        "date": 1726513990126,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3013.8950502361386,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1658.2118136975132,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 605.3946428571472,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 589.9480357141687,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12012.989062498036,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11962.890625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6812.500000001315,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6835.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5833.01800000072,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4789.2046555385905,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6024.331249999396,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1187.7237499999994,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8164.355069845156,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8161.235887339789 ns\nthreads: 1"
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
          "id": "241387ecf50f11838b5b10cc5db97f200835d080",
          "message": "Benchmark a Draft 4 `$ref` to a single property (#1198)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-16T20:07:07-03:00",
          "tree_id": "04ec68f635b588870ff3b87d397b6d0d0bd63aa1",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/241387ecf50f11838b5b10cc5db97f200835d080"
        },
        "date": 1726528598710,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3041.1602678570252,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1628.0210080216484,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1611.3270459863531 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 611.2630357142125,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 586.3154999999551,
            "unit": "ns/iter",
            "extra": "iterations: 1000000\ncpu: 593.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11816.69464285611,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6723.175000000684,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5660.959821428589,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4743.5550388885695,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5770.113392857062,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1146.1150000000332,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1143.9732142857142 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8019.621651785665,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 593.1309821428548,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
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
          "id": "7a0e25844a9e9f9668c722227b6315f1d6b3eabb",
          "message": "Add a `has` function to check if a document contains a given Pointer (#1201)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-16T20:25:12-03:00",
          "tree_id": "255395af8621818a3e035bc2536bc7c9e01a7917",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/7a0e25844a9e9f9668c722227b6315f1d6b3eabb"
        },
        "date": 1726529773339,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3067.868750000408,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1632.1799107143725,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 608.2867857143373,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 580.118303571453,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12343.624999999747,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12276.785714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6891.356249999449,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6835.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5725.125000000162,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4785.347321428292,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 4743.303571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5923.069642857318,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1145.9500000000844,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1147.4609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8037.207589285548,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 593.0941071429377,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
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
          "id": "bbd5ea9920ab4f42578a002c98cce65cb27b4e4a",
          "message": "Stop assuming that relative instance locations have up to 1 token (#1202)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-16T20:29:06-03:00",
          "tree_id": "dff7d7b4bfd477a4536333d7243dfc6a9d6cae30",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/bbd5ea9920ab4f42578a002c98cce65cb27b4e4a"
        },
        "date": 1726530048228,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3160.681250000558,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3138.9508928571427 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1762.0448768262163,
            "unit": "ns/iter",
            "extra": "iterations: 373333\ncpu: 1757.8140694768476 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 640.5598214286101,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 595.4863392856525,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11792.667857142598,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6935.8102678585,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6975.446428571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5851.109821429239,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4902.351988705902,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5952.052000000094,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 6093.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1208.5514062501802,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1196.2890625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8065.665178571229,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 621.454910714192,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
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
          "id": "d57beae35a288341306ea7b0cf285e75cfcbf95a",
          "message": "Generalize `AssertionPropertyType` to work on multi-token paths (#1203)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-16T20:53:03-03:00",
          "tree_id": "2ef8d64834f04dfbae169521f0e1fc4d196ba20e",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/d57beae35a288341306ea7b0cf285e75cfcbf95a"
        },
        "date": 1726531374669,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3022.048214284772,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 2999.441964285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1651.6879464286865,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 629.6154464284394,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 589.7159000001011,
            "unit": "ns/iter",
            "extra": "iterations: 1000000\ncpu: 593.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12009.349999999586,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 12207.03125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6574.740178572419,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5639.312999999219,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4694.440612590438,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4708.436849189396 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5714.669000001322,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1164.147142856921,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 7894.6562499991405,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7847.377232142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 590.3569642857829,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
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
          "id": "3785582e0eeff22aeeee61d70922324ce1c7d750",
          "message": "Unroll `properties` of 1 keyword even more aggressively (#1199)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-16T21:06:17-03:00",
          "tree_id": "bcec3be9ad840bc4c57a9712e939355b35b5aea3",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/3785582e0eeff22aeeee61d70922324ce1c7d750"
        },
        "date": 1726532156992,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3032.164774438195,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 3048.276213054892 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1636.1217168830967,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1649.691975652695 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 616.2248214286998,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 427.71791504525777,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11542.885714285538,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6545.711607143647,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5671.5749999989375,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4715.052935386061,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4708.436849189396 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5709.127678571235,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1154.4870312501841,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 7882.447399788681,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 7742.7109700403125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 432.53106250006107,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
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
          "id": "e86ad1e333c11b6fc2659a0f32ae883ef1040e20",
          "message": "refactor: clean bundle from std::future (#1197)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-17T11:59:00+02:00",
          "tree_id": "3a8e00b54de6c330306398ded943f2be286dec84",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/e86ad1e333c11b6fc2659a0f32ae883ef1040e20"
        },
        "date": 1726567699809,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3536.868651357257,
            "unit": "ns/iter",
            "extra": "iterations: 213333\ncpu: 3588.8727951137425 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 2092.069120612777,
            "unit": "ns/iter",
            "extra": "iterations: 344615\ncpu: 2085.6608098892966 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 752.8852678570428,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 749.8604910714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 502.733400000011,
            "unit": "ns/iter",
            "extra": "iterations: 1000000\ncpu: 500 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11899.665625000112,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6794.247767856833,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6801.060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 6660.330357143655,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 5478.7820000001375,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5468.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5861.384821428268,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1179.0926785713348,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8247.84041142657,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8161.235887339789 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 442.1720625000347,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Tony Gorez",
            "username": "tony-go",
            "email": "gorez.tony@gmail.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "e86ad1e333c11b6fc2659a0f32ae883ef1040e20",
          "message": "refactor: clean bundle from std::future (#1197)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-17T09:59:00Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/e86ad1e333c11b6fc2659a0f32ae883ef1040e20"
        },
        "date": 1726600456874,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2940.3441212274033,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1612.9976698675453,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1611.3270459863531 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 605.925446428606,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 427.72831250005083,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11558.723437499197,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6542.313392858188,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5663.69800000075,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4719.858972899508,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4708.436849189396 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5707.915178569628,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1168.535357142884,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8017.883928572087,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 435.42020865937866,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 442.5921450478798 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Tony Gorez",
            "username": "tony-go",
            "email": "gorez.tony@gmail.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "e86ad1e333c11b6fc2659a0f32ae883ef1040e20",
          "message": "refactor: clean bundle from std::future (#1197)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-17T09:59:00Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/e86ad1e333c11b6fc2659a0f32ae883ef1040e20"
        },
        "date": 1726686820494,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2910.268926879763,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2915.742464661201 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1617.8963496232823,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1611.3270459863531 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 615.4758928571725,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 424.34508416107474,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11829.198437499856,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11962.890625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6531.764285715066,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5642.668000000413,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4684.669162207741,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4708.436849189396 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5691.483035714587,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5580.357142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1168.6029687499656,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8080.241071429636,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 434.69418749992883,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "Tony Gorez",
            "username": "tony-go",
            "email": "gorez.tony@gmail.com"
          },
          "committer": {
            "name": "GitHub",
            "username": "web-flow",
            "email": "noreply@github.com"
          },
          "id": "e86ad1e333c11b6fc2659a0f32ae883ef1040e20",
          "message": "refactor: clean bundle from std::future (#1197)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-17T09:59:00Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/e86ad1e333c11b6fc2659a0f32ae883ef1040e20"
        },
        "date": 1726773211186,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2968.9917680639956,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1633.8404460890072,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1649.691975652695 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 625.4478571429039,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 431.41890446274925,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11811.901785713579,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6587.257142857286,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5656.612000000223,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4725.010546898569,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4708.436849189396 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5730.617857142509,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1187.1848214285917,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8029.118620006887,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 7951.97342869005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 441.2515624999713,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
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
            "email": "jv@jviotti.com",
            "name": "Juan Cruz Viotti",
            "username": "jviotti"
          },
          "distinct": true,
          "id": "1e772cfa4c19fbae1234af8cebb40911a82f2ea3",
          "message": "Update contribution agreement link\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-20T10:55:51+03:00",
          "tree_id": "b49f9b7b7e161b7d80b1ac8adc4038136036af35",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/1e772cfa4c19fbae1234af8cebb40911a82f2ea3"
        },
        "date": 1726819581485,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2931.1850849698135,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2915.742464661201 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1624.895758928245,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 624.1845535715892,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 432.9665832761376,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11558.096874999534,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11474.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6618.761607143142,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5698.604464285495,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4844.72889448437,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4813.068779171382 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5777.976785714551,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1179.8909374999055,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8086.719866070727,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 439.70981249998425,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
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
            "name": "Juan Cruz Viotti",
            "username": "jviotti",
            "email": "jv@jviotti.com"
          },
          "id": "1e772cfa4c19fbae1234af8cebb40911a82f2ea3",
          "message": "Update contribution agreement link\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-20T07:55:51Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/1e772cfa4c19fbae1234af8cebb40911a82f2ea3"
        },
        "date": 1726859552068,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2904.462040214146,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2849.4755904643557 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1623.4004464286452,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 604.1857142857258,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 421.0522287358133,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11494.048214282364,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6449.179464287422,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5593.862499998588,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5580.357142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4635.375971820983,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4603.804919207409 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5657.894642855662,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5580.357142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1153.8058928571056,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1143.9732142857142 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 7981.732142857213,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 430.6820092584096,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
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
          "id": "be7d39bf8a716eed4f928a4213e4becf2919b418",
          "message": "Pull ClangFormat from `pip` on CI (#1209)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-21T13:38:50-03:00",
          "tree_id": "877498fe1266562c795cfcf2602ddec4bb551dc0",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/be7d39bf8a716eed4f928a4213e4becf2919b418"
        },
        "date": 1726937300673,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2965.8249536662247,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1613.3189732143092,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 614.1255357142938,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 429.42656933000563,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11525.062500000771,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6598.435714286057,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5740.125892857456,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4820.480777215212,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5816.117857142851,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1171.3431249999574,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8116.256696428793,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 446.0694006064073,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 445.0333629374921 ns\nthreads: 1"
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
          "id": "1f0cfa2e2bdf9fd8d9413b281e2f5aa30319a480",
          "message": "Make compiler JSON formatter more resilient to unknown keywords (#1194)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-21T14:42:54-03:00",
          "tree_id": "cbbdb8927bfe3adbd5a87ebb53dda1c9ee80f815",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/1f0cfa2e2bdf9fd8d9413b281e2f5aa30319a480"
        },
        "date": 1726941139245,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2924.6640004413052,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2915.742464661201 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1603.5794642859407,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 613.8299999999422,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 423.4408750001251,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 419.921875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11498.616071431146,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11439.732142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6696.429464286422,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5602.031250000599,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5580.357142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4655.377578967403,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4603.804919207409 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5644.843749999089,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5580.357142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1162.7771428570504,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1143.9732142857142 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 7942.603827658952,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 7951.97342869005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 434.6361874999616,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
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
          "id": "efb9776ebf10bc6db145c806e00991f07e5a9bdd",
          "message": "refactor: use `std::uint8_t` for enum (#1210)\n\nFirst advice taken from the great tidy!\r\n\r\n---------\r\n\r\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-23T20:14:30+02:00",
          "tree_id": "ba709d087f8f8090158225d48d7cbd5e63049e88",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/efb9776ebf10bc6db145c806e00991f07e5a9bdd"
        },
        "date": 1727115916851,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2963.785842426494,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1603.7721626525783,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1611.3270459863531 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 590.7068750001291,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 425.29803966706794,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11453.767187500574,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11474.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6765.745535714375,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5863.274107144077,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4670.101564574265,
            "unit": "ns/iter",
            "extra": "iterations: 154483\ncpu: 4652.615498145427 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5681.637499999884,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1192.625312500084,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8002.720982143481,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 439.033625000036,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
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
          "id": "b5496c2dc2fb883822daa1e9c9c8dfadd718c434",
          "message": "Benchmark `additionalProperties` with only `type` inside (#1211)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T14:46:17-04:00",
          "tree_id": "3281e129384d71e2bcf612f88470ada519e24eb2",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/b5496c2dc2fb883822daa1e9c9c8dfadd718c434"
        },
        "date": 1727117783581,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2984.9017553824383,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1623.6733100403148,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1649.691975652695 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 607.8116071429512,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 433.11099999996827,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11495.201562500059,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11474.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6723.01785714272,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5811.19642857126,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4818.276603296468,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4813.068779171382 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5843.740000000253,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1183.1432142855647,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1171.875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8223.366413540778,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8161.235887339789 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 438.5238750001008,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 1599.6614064766843,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1572.9621163200113 ns\nthreads: 1"
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
          "id": "a4c71c90c47bd1f5b977eaec440a4370e1433213",
          "message": "Upgrade ClangFormat to v19.1.0 (#1212)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T14:47:03-04:00",
          "tree_id": "a202780771c0140a90ac21537d7acae5efb463ab",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/a4c71c90c47bd1f5b977eaec440a4370e1433213"
        },
        "date": 1727118197228,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2933.6355809640017,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2915.742464661201 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1634.0462053570093,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 623.4852678572013,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 627.7901785714286 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 443.61294590729307,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 445.0333629374921 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11493.320312499833,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11474.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6699.348214287478,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5785.50357142912,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4807.117550997643,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5859.0223214289545,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1215.5746428573723,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1227.6785714285713 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8162.9497767846515,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 449.8988236877546,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 455.1477575497078 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 1619.5848975993179,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1611.3270459863531 ns\nthreads: 1"
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
          "id": "a4c71c90c47bd1f5b977eaec440a4370e1433213",
          "message": "Upgrade ClangFormat to v19.1.0 (#1212)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T18:47:03Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/a4c71c90c47bd1f5b977eaec440a4370e1433213"
        },
        "date": 1727119189212,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2999.1861367582283,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1640.259821428458,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 616.5778571428859,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 440.0714999999877,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11430.782812499985,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11474.609375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6851.9848214277445,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5893.737000001237,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4889.443750000412,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 4882.8125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5944.633035714527,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 1183.6816071426679,
            "unit": "ns/iter",
            "extra": "iterations: 560000\ncpu: 1199.7767857142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8633.811456200337,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8579.760804639265 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 447.5693125000646,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 449.21875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 1608.2392857142177,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1604.3526785714287 ns\nthreads: 1"
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
          "id": "23ee0c8f9f348933503d51c4e18393293681ee94",
          "message": "Introduce a new `LoopPropertiesType(Strict)` compiler step (#1213)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T15:22:50-04:00",
          "tree_id": "c920dbab7f396ae4987fb77ed96531eefb72703e",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/23ee0c8f9f348933503d51c4e18393293681ee94"
        },
        "date": 1727119989968,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3029.699858772314,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 3048.276213054892 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1638.6676339286462,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 608.3584821429408,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 430.5101855708911,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12094.737499998993,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11997.767857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6767.978571427793,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6835.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5806.8455357138355,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4786.101192946576,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5894.770535714348,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 690.6915178570878,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 697.5446428571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8104.43526785625,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 428.1153213572947,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 960.373767690115,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 962.6111774057243 ns\nthreads: 1"
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
          "id": "16ce6900ee9de7b126c1c429dfab820f24360b2c",
          "message": "Improve error when making a `$ref` to a non-schema (#1214)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T19:51:50-04:00",
          "tree_id": "349d0d36196714dc7602db185fa598f77a782a88",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/16ce6900ee9de7b126c1c429dfab820f24360b2c"
        },
        "date": 1727136122616,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3007.090237457712,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1659.683928570997,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 608.1151785713652,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 420.7865679800311,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 414.3415825980151 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12071.51093749914,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 12207.03125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6754.177455357636,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6626.674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5804.689999999937,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4800.087187578119,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5894.0464285715925,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 683.5138392858653,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 683.59375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8127.135044644668,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 428.338191927859,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 968.258674884513,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 962.6111774057243 ns\nthreads: 1"
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
          "id": "373d89e2a842504d995aa04cad8aac96b23e33b2",
          "message": "Remake `Pointer` method `.has()` as `.try_get()` (#1215)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T21:15:13-04:00",
          "tree_id": "e9711d17de0586e3ba8733dcd01d076d10946c18",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/373d89e2a842504d995aa04cad8aac96b23e33b2"
        },
        "date": 1727141108278,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2990.14754717126,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1641.8209821428115,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1674.107142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 603.5468750001526,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 433.9938124999776,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12169.557142856645,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12276.785714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6746.772321429363,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6626.674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5730.243000000428,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4722.095131335689,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5807.437500000186,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 673.5187499999589,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 662.6674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8047.209821430303,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 437.6111249999326,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 960.5544372525145,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 941.6848474621216 ns\nthreads: 1"
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
          "id": "15667db3116ab9dc8eeeb2b886d2125e6cc6b050",
          "message": "Perform a one-time pointer resolution on `AssertionPropertyType(Strict)` (#1216)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-23T21:27:25-04:00",
          "tree_id": "a4be1f55db789719fa1152fe386ebc4902d85e04",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/15667db3116ab9dc8eeeb2b886d2125e6cc6b050"
        },
        "date": 1727141859273,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3011.2193528954645,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1637.7823660716574,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 603.4238392857105,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 424.1712113661121,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11844.148214281957,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6692.300000001491,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5720.284821429848,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4735.876304353784,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5790.156250000678,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 678.0556250000863,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 669.6428571428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 7961.864009537345,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 7951.97342869005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 428.99099999999635,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 961.8965348674129,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 962.6111774057243 ns\nthreads: 1"
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
          "id": "6f69aa975142a567cc584f20e8f834f0c1fae011",
          "message": "fix(jsonschema): delete effectless std::move (#1217)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-24T16:30:44+02:00",
          "tree_id": "7dc5ce46b7f7193ed5fdbfe10c98938591e0a276",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/6f69aa975142a567cc584f20e8f834f0c1fae011"
        },
        "date": 1727188883495,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2960.8382918624275,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1641.6683035715655,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 592.947410714219,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 585.9375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 422.85839642886083,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11825.039285716748,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6632.163392857088,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6556.919642857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5794.548999999733,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4843.591021063195,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5850.531250000505,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 671.0652678569724,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 669.6428571428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8108.074517521967,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 7951.97342869005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 438.1630000000314,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 955.8331893601974,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 962.6111774057243 ns\nthreads: 1"
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
          "id": "f9d541d14b82d9eb2363963356cf7eec094050bf",
          "message": "fix(jsonschema): remove effect less move (#1218)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-24T16:47:06+02:00",
          "tree_id": "1844eb1708a7a262ed8ed0a33ef4563489c0177d",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/f9d541d14b82d9eb2363963356cf7eec094050bf"
        },
        "date": 1727189841008,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2959.803468355172,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2915.742464661201 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1638.3881696427748,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 594.6882142857086,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 418.7242015863575,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 414.3415825980151 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11879.09375000018,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 11962.890625 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6645.65446428611,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5742.172321429199,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5719.866071428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4742.304112286105,
            "unit": "ns/iter",
            "extra": "iterations: 149333\ncpu: 4917.700709153369 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5822.635714285493,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 676.2833705356337,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 680.1060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 7947.424107141997,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 7847.377232142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 430.8307500000552,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 954.39452928824,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 941.6848474621216 ns\nthreads: 1"
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
          "id": "18bda4cba448e9a4c8e1656086a4bc4556354502",
          "message": "fix(jsonschema): use ref in fragment_string (#1219)\n\nSigned-off-by: Tony Gorez <gorez.tony@gmail.com>",
          "timestamp": "2024-09-24T17:10:15+02:00",
          "tree_id": "10a45d322327c6a2722d2eb2e626fb8ad890730d",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/18bda4cba448e9a4c8e1656086a4bc4556354502"
        },
        "date": 1727191283917,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3050.1851231396136,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 3048.276213054892 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1651.348883928705,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1639.2299107142858 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 603.2872321428938,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 435.2566875000008,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 11843.142857141434,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11718.75 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6710.8526785710455,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6696.428571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5834.596000000828,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4788.175011763464,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5868.388392857225,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 683.3033035714412,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 669.6428571428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8175.093414761422,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8161.235887339789 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 449.3687323119514,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 434.91896832527635 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 974.0759374999186,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 976.5625 ns\nthreads: 1"
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
          "id": "47c995618cdc0d5e47cd9d68b6fe187b91af8fd1",
          "message": "Increase threshold for preferring `properties` loop vs unroll (#1220)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T13:43:08-04:00",
          "tree_id": "e2ba5153c0fa548dbc44c683ffb36ba42eb14860",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/47c995618cdc0d5e47cd9d68b6fe187b91af8fd1"
        },
        "date": 1727200411469,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3022.4225896889034,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 3048.276213054892 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1644.7160714285758,
            "unit": "ns/iter",
            "extra": "iterations: 448000\ncpu: 1674.107142857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 605.1322321427831,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 422.49835619399664,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12114.410714283558,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12276.785714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6857.005357143667,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6975.446428571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5826.204464284501,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4819.442137894709,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5895.870535714007,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 670.0623883928764,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 662.6674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8179.939732142166,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 435.99987500002163,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 957.2670280058697,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 962.6111774057243 ns\nthreads: 1"
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
          "id": "8bc8744a66238e783c2786f24e60588fef19e37c",
          "message": "Introduce a new `AssertionTypeStringBounded` instruction (#1221)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T14:49:55-04:00",
          "tree_id": "cdc7ce6fef46945530970c8ff070a036f62c382f",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/8bc8744a66238e783c2786f24e60588fef19e37c"
        },
        "date": 1727204474879,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2983.647668041959,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1691.5769029617175,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 619.0045535715528,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 426.1313031902249,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12133.796428571486,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 11997.767857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6872.9107142852445,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6801.060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5934.274107141439,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4923.876248996301,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4973.497744194415 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 6017.696428571639,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6138.392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 682.046316964145,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 680.1060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8285.861607143985,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8370.535714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 433.42244941865835,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 433.1752908979249 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 983.2392187497873,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 976.5625 ns\nthreads: 1"
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
          "id": "8bc8744a66238e783c2786f24e60588fef19e37c",
          "message": "Introduce a new `AssertionTypeStringBounded` instruction (#1221)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T18:49:55Z",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/8bc8744a66238e783c2786f24e60588fef19e37c"
        },
        "date": 1727205188403,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 2958.1248489101617,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1677.9710415370603,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 605.5762499999315,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 599.8883928571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 420.92283362616354,
            "unit": "ns/iter",
            "extra": "iterations: 1659259\ncpu: 423.75843674797005 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12124.245312499226,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 12207.03125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6962.854464282893,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6975.446428571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5794.184821431664,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4797.6784577497065,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5890.343750000479,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 678.0800892853124,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 683.59375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8200.167410714292,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 428.3539375001055,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 973.632343750097,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 952.1484375 ns\nthreads: 1"
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
          "id": "0fd8c922be04f008997ff3250594e426f08ea4fa",
          "message": "Introduce a new `SchemaCompilerAssertionTypeArrayBounded` (#1222)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T15:50:47-04:00",
          "tree_id": "6ee72a198c99865f76ec9f398bde723e6157235c",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/0fd8c922be04f008997ff3250594e426f08ea4fa"
        },
        "date": 1727208059647,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3062.019196428472,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3069.1964285714284 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1657.356613377233,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1688.0569053190366 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 610.9516071429002,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 427.7566875001071,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12251.155357140371,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12276.785714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6835.630580356776,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6801.060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5827.261607141817,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4843.494145976943,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4865.378228016275 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5934.634000000187,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 686.4872767854889,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 680.1060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8107.915178572662,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 435.604187499905,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 429.6875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 987.8629687499085,
            "unit": "ns/iter",
            "extra": "iterations: 640000\ncpu: 1000.9765625 ns\nthreads: 1"
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
          "id": "b252e8b13c15936b06c50dba15baf2eca76efa49",
          "message": "Introduce a new `SchemaCompilerAssertionTypeObjectBounded` (#1223)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T16:08:01-04:00",
          "tree_id": "23f057fc68961731e007543e9867e8cca4ea244d",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/b252e8b13c15936b06c50dba15baf2eca76efa49"
        },
        "date": 1727209072306,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3085.437946428523,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3138.9508928571427 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1705.4263356518952,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1726.4218349853784 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 626.5556249999804,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 447.0038088382948,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 445.0333629374921 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12415.457142858648,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12555.80357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6913.619642856718,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 6975.446428571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5852.516964286077,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4844.465178572526,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 4743.303571428572 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5953.700000000188,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5998.883928571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 671.2578125000083,
            "unit": "ns/iter",
            "extra": "iterations: 896000\ncpu: 662.6674107142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8092.954049312928,
            "unit": "ns/iter",
            "extra": "iterations: 74667\ncpu: 8161.235887339789 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 444.6568161634277,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 445.0333629374921 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 961.555151091452,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 962.6111774057243 ns\nthreads: 1"
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
          "id": "d4ec572a9eb758d78eec6b6004fa93dcab53cb99",
          "message": "Short-circuit `unique()` on JSON arrays of 0/1 items (#1224)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T17:07:43-04:00",
          "tree_id": "3d64da82f7470ef056215ef78c6dc3edabec6f4a",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/d4ec572a9eb758d78eec6b6004fa93dcab53cb99"
        },
        "date": 1727213002056,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3014.3115242867143,
            "unit": "ns/iter",
            "extra": "iterations: 235789\ncpu: 2982.0093388580467 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1666.4664733485893,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1649.691975652695 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 614.3907142856619,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 613.8392857142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 439.1955625000321,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 439.453125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12219.889285712985,
            "unit": "ns/iter",
            "extra": "iterations: 56000\ncpu: 12276.785714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 6891.2555803584455,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 6801.060267857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5832.843999999113,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5937.5 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4834.02599999863,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5000 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5890.959000000748,
            "unit": "ns/iter",
            "extra": "iterations: 100000\ncpu: 5781.25 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 663.4529464284761,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 669.6428571428571 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8031.284598215242,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8021.763392857143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 450.7533125000407,
            "unit": "ns/iter",
            "extra": "iterations: 1600000\ncpu: 449.21875 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 948.7250675334762,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 941.6848474621216 ns\nthreads: 1"
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
          "id": "a92e2f9a59b5f945c20de9242ab0567e53f25e57",
          "message": "Benchmark JSON uniqueness on arrays of objects (#1225)\n\nSigned-off-by: Juan Cruz Viotti <jv@jviotti.com>",
          "timestamp": "2024-09-24T18:07:08-04:00",
          "tree_id": "0751900474f0068b430934b5a4c3fc842a7b6270",
          "url": "https://github.com/sourcemeta/jsontoolkit/commit/a92e2f9a59b5f945c20de9242ab0567e53f25e57"
        },
        "date": 1727216376728,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "JSON_Array_Of_Objects_Unique",
            "value": 5043.423214285017,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5022.321428571428 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Meta_1_No_Callback",
            "value": 3239.7549107139002,
            "unit": "ns/iter",
            "extra": "iterations: 224000\ncpu: 3208.7053571428573 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Required_Properties",
            "value": 1706.1175182248335,
            "unit": "ns/iter",
            "extra": "iterations: 407273\ncpu: 1726.4218349853784 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Many_Optional_Properties_Minimal_Match",
            "value": 633.1360714284529,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 641.7410714285714 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Few_Optional_Properties_Minimal_Match",
            "value": 446.88547851288143,
            "unit": "ns/iter",
            "extra": "iterations: 1544828\ncpu: 445.0333629374921 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Items_Schema",
            "value": 12378.045312498642,
            "unit": "ns/iter",
            "extra": "iterations: 64000\ncpu: 12207.03125 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Nested_Object",
            "value": 7089.6857142871895,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 7114.955357142857 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Optional",
            "value": 5810.356250001242,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Closed",
            "value": 4791.614077334029,
            "unit": "ns/iter",
            "extra": "iterations: 144516\ncpu: 4757.258711838135 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Properties_Triad_Required",
            "value": 5891.182142856825,
            "unit": "ns/iter",
            "extra": "iterations: 112000\ncpu: 5859.375 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Non_Recursive_Ref",
            "value": 692.1544642856198,
            "unit": "ns/iter",
            "extra": "iterations: 1120000\ncpu: 697.5446428571429 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Pattern_Properties_True",
            "value": 8069.7232142869425,
            "unit": "ns/iter",
            "extra": "iterations: 89600\ncpu: 8196.14955357143 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Ref_To_Single_Property",
            "value": 454.8426784281,
            "unit": "ns/iter",
            "extra": "iterations: 1445161\ncpu: 454.10165372577865 ns\nthreads: 1"
          },
          {
            "name": "JSONSchema_Validate_Draft4_Additional_Properties_Type",
            "value": 984.1214356598172,
            "unit": "ns/iter",
            "extra": "iterations: 746667\ncpu: 983.537507349327 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}