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
Some benchmarking on **Intel(R) Core(TM) i7-8850H CPU @ 2.60GHz**: 

```shell
Perform insert and clear tests for several container types.
  for count=1000 max_value=10000000 type=8std_uset => 96 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_std => 1 millis.
  for count=1000 max_value=10000000 type=14bitmap_set_asm => 24 millis.
  for count=1000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 3 millis.
  for count=20000 max_value=10000000 type=8std_uset => 1695 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_std => 30 millis.
  for count=20000 max_value=10000000 type=14bitmap_set_asm => 52 millis.
  for count=20000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 184 millis.
  for count=200000 max_value=10000000 type=8std_uset => 24534 millis.
  for count=200000 max_value=10000000 type=14bitmap_set_std => 306 millis.
  for count=200000 max_value=10000000 type=14bitmap_set_asm => 346 millis.
  for count=200000 max_value=10000000 type=14fast_hashset30ILj2ELj8EE => 2295 millis.
```

