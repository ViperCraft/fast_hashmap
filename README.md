# C++ header only, integer key only hashmap: fast_hashmap

Up to 10x more faster than std::unordered_set for operations with unique insert. About 7x slower than bitmap, but takes about 5-10 times less memory for real-world workloads.

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

