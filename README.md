# C++ header only, integer key only hashmap: fast_hashmap

## About
Up to **10x** more faster than **std::unordered_set<uint32_t>** for operations with unique insert. About 7x slower than bitmap, but takes about 5-10 times less memory for real-world workloads.

## Licensing

Apache License 2.0.

# Usage

To compile unittest/benchmark

```shell
$ ./build_tests.sh
```

To run unittest

```shell
$ ./build/test/unit/unittest
```

To run benchmarks

```shell
$ ./build/benchs/bench
```

## Benchmarks
Benchmarking on **Intel(R) Core(TM) i7-8850H CPU @ 2.60GHz**: 

```shell
Perform insert and clear tests for several container types.
  for count=500 max_value=10000000 type=8std_uset => 123 millis.
  for count=500 max_value=10000000 type=14bitmap_set_std => 67 millis.
  for count=500 max_value=10000000 type=14bitmap_set_raw => 69 millis.
  for count=500 max_value=10000000 type=13bitmap_fclear => 8 millis.
  for count=500 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 5 millis.
  for count=1000 max_value=10000000 type=8std_uset => 231 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_std => 70 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_raw => 71 millis.
  for count=1000 max_value=10000000 type=13bitmap_fclear => 16 millis.
  for count=1000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 9 millis.
  for count=8000 max_value=10000000 type=8std_uset => 1957 millis.
  for count=8000 max_value=10000000 type=14bitmap_set_std => 108 millis.
  for count=8000 max_value=10000000 type=14bitmap_set_raw => 107 millis.
  for count=8000 max_value=10000000 type=13bitmap_fclear => 115 millis.
  for count=8000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 169 millis.
  for count=20000 max_value=10000000 type=8std_uset => 5205 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_std => 175 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_raw => 171 millis.
  for count=20000 max_value=10000000 type=13bitmap_fclear => 189 millis.
  for count=20000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 558 millis.
  for count=100000 max_value=10000000 type=8std_uset => 34928 millis.
  for count=100000 max_value=10000000 type=14bitmap_set_std => 633 millis.
  for count=100000 max_value=10000000 type=14bitmap_set_raw => 642 millis.
  for count=100000 max_value=10000000 type=13bitmap_fclear => 683 millis.
  for count=100000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 3294 millis.
  for count=250000 max_value=10000000 type=8std_uset => 110642 millis.
  for count=250000 max_value=10000000 type=14bitmap_set_std => 1546 millis.
  for count=250000 max_value=10000000 type=14bitmap_set_raw => 1591 millis.
  for count=250000 max_value=10000000 type=13bitmap_fclear => 1684 millis.
  for count=250000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 8466 millis.
```

Benchmarking on **Intel(R) Xeon(R) Gold 6230 CPU @ 2.10GHz**:

```shell
Perform insert and clear tests for several container types.
  for count=500 max_value=10000000 type=8std_uset => 81 millis.
  for count=500 max_value=10000000 type=14bitmap_set_std => 159 millis.
  for count=500 max_value=10000000 type=14bitmap_set_raw => 144 millis.
  for count=500 max_value=10000000 type=13bitmap_fclear => 12 millis.
  for count=500 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 6 millis.
  for count=1000 max_value=10000000 type=8std_uset => 164 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_std => 161 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_raw => 145 millis.
  for count=1000 max_value=10000000 type=13bitmap_fclear => 20 millis.
  for count=1000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 13 millis.
  for count=8000 max_value=10000000 type=8std_uset => 1419 millis.
  for count=8000 max_value=10000000 type=14bitmap_set_std => 241 millis.
  for count=8000 max_value=10000000 type=14bitmap_set_raw => 225 millis.
  for count=8000 max_value=10000000 type=13bitmap_fclear => 244 millis.
  for count=8000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 224 millis.
  for count=20000 max_value=10000000 type=8std_uset => 3812 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_std => 368 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_raw => 326 millis.
  for count=20000 max_value=10000000 type=13bitmap_fclear => 348 millis.
  for count=20000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 746 millis.
  for count=100000 max_value=10000000 type=8std_uset => 32960 millis.
  for count=100000 max_value=10000000 type=14bitmap_set_std => 1231 millis.
  for count=100000 max_value=10000000 type=14bitmap_set_raw => 1058 millis.
  for count=100000 max_value=10000000 type=13bitmap_fclear => 1133 millis.
  for count=100000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 4490 millis.
  for count=250000 max_value=10000000 type=8std_uset => 88410 millis.
  for count=250000 max_value=10000000 type=14bitmap_set_std => 2936 millis.
  for count=250000 max_value=10000000 type=14bitmap_set_raw => 2475 millis.
  for count=250000 max_value=10000000 type=13bitmap_fclear => 2632 millis.
  for count=250000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 12785 millis.
```

Benchmarking on **Intel(R) Xeon(R) CPU E5-2660 v4 @ 2.00GHz**:

```shell
Perform insert and clear tests for several container types.
  for count=500 max_value=10000000 type=8std_uset => 97 millis.
  for count=500 max_value=10000000 type=14bitmap_set_std => 219 millis.
  for count=500 max_value=10000000 type=14bitmap_set_raw => 220 millis.
  for count=500 max_value=10000000 type=13bitmap_fclear => 14 millis.
  for count=500 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 9 millis.
  for count=1000 max_value=10000000 type=8std_uset => 199 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_std => 224 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_raw => 224 millis.
  for count=1000 max_value=10000000 type=13bitmap_fclear => 26 millis.
  for count=1000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 19 millis.
  for count=8000 max_value=10000000 type=8std_uset => 1815 millis.
  for count=8000 max_value=10000000 type=14bitmap_set_std => 303 millis.
  for count=8000 max_value=10000000 type=14bitmap_set_raw => 293 millis.
  for count=8000 max_value=10000000 type=13bitmap_fclear => 311 millis.
  for count=8000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 289 millis.
  for count=20000 max_value=10000000 type=8std_uset => 5579 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_std => 443 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_raw => 414 millis.
  for count=20000 max_value=10000000 type=13bitmap_fclear => 437 millis.
  for count=20000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 897 millis.
  for count=100000 max_value=10000000 type=8std_uset => 31550 millis.
  for count=100000 max_value=10000000 type=14bitmap_set_std => 1385 millis.
  for count=100000 max_value=10000000 type=14bitmap_set_raw => 1242 millis.
  for count=100000 max_value=10000000 type=13bitmap_fclear => 1286 millis.
  for count=100000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 5614 millis.
  for count=250000 max_value=10000000 type=8std_uset => 87235 millis.
  for count=250000 max_value=10000000 type=14bitmap_set_std => 3222 millis.
  for count=250000 max_value=10000000 type=14bitmap_set_raw => 2873 millis.
  for count=250000 max_value=10000000 type=13bitmap_fclear => 2950 millis.
  for count=250000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 14317 millis.
  ```
