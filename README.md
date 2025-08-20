# libyaml-fuzz

Goal: libFuzzer harness for libyaml (C), built with ASan/UBSan, plus CI job on GitLab.

## Build

```bash
git clone … && cd libyaml
mkdir build && cd build
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_C_FLAGS="-fsanitize=address,undefined -O1 -g" ..
make -j$(nproc)
clang -fsanitize=address,undefined -O1 -g -Iinclude fuzz_yaml.c -Lbuild -lyaml -lFuzzer -o fuzz_yaml

## Coverage

Filename                                 Regions    Missed Regions     Cover   Functions  Missed Functions  Executed       Lines      Missed Lines     Cover    Branches   Missed Branches     Cover
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
libyaml-fuzzer/fuzz_yaml_roundtrip.c          44                 7    84.09%           2                 0   100.00%          54                 5    90.74%          40                 9    77.50%
libyaml/src/api.c                           1582              1231    22.19%          53                28    47.17%         800               515    35.62%         672               526    21.73%
libyaml/src/dumper.c                         245                76    68.98%          11                 0   100.00%         197                13    93.40%         112                36    67.86%
libyaml/src/emitter.c                       3328               656    80.29%          47                 0   100.00%        1509               211    86.02%        2336               756    67.64%
libyaml/src/loader.c                         451               125    72.28%          14                 0   100.00%         287                28    90.24%         232                72    68.97%
libyaml/src/parser.c                        1031               103    90.01%          24                 1    95.83%         859                61    92.90%         640               119    81.41%
libyaml/src/reader.c                         240                26    89.17%           4                 0   100.00%         233                12    94.85%         172                15    91.28%
libyaml/src/scanner.c                       4882               820    83.20%          41                 1    97.56%        1487                89    94.01%        2881               746    74.11%
libyaml/src/writer.c                          88                28    68.18%           2                 0   100.00%          79                12    84.81%          46                14    69.57%

Files which contain no functions:
libyaml/build/include/config.h                 0                 0         -           0                 0         -           0                 0         -           0                 0         -
libyaml/include/yaml.h                         0                 0         -           0                 0         -           0                 0         -           0                 0         -
libyaml/src/yaml_private.h                     0                 0         -           0                 0         -           0                 0         -           0                 0         -
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TOTAL                                      11891              3072    74.17%         198                30    84.85%        5505               946    82.82%        7131              2293    67.84%

