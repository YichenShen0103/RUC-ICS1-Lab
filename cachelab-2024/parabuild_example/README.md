# parabuild 使用指南

## introduction

这是一个多进程并行化编译工具，用于批量调整代码中的参数，以便于进行性能测试。欢迎大家使用，点个 star，~~贡献代码~~。

本实验其实只要单进程就够了，我们提供了一个例子在 [`parabuild.sh`](./parabuild.sh) 中。

完整的介绍请参考 [github 主页](https://github.com/panjd123/parabuild-rust)。

## setup

安装必要的依赖，rust 套件以及 parabuild

```shell
sudo apt-get install lsof rsync
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
. $HOME/.cargo/env
cargo install parabuild
```

## usage

```shell
parabuild --help

# example in cachelab
./parabuild_example/parabuild.sh
```

output:

```
data:
{
  "BLOCK_SIZE_1": 4,
  "loop1": "i",
  "loop2": "j",
  "loop3": "k"
}
stdout:
case  | miss_cache | miss_reg | latency | speedup           
------+------------+----------+---------+-------------------
case0 | 2          | 20       | 50      | 1.0               
case1 | 2536       | 10240    | 48280   | 1.6861640430820215
case2 | 36800      | 66560    | 618560  | 1.0               
case3 | 41846      | 72075    | 699765  | 1.0
Weighted Speedup: 1.2058


data:
{
  "BLOCK_SIZE_1": 4,
  "loop1": "k",
  "loop2": "i",
  "loop3": "j"
}
stdout:
case  | miss_cache | miss_reg | latency | speedup          
------+------------+----------+---------+------------------
case0 | 2          | 20       | 50      | 1.0              
case1 | 2410       | 10240    | 46390   | 1.754860961414098
case2 | 36800      | 66560    | 618560  | 1.0              
case3 | 41846      | 72075    | 699765  | 1.0
Weighted Speedup: 1.2265


data:
{
  "BLOCK_SIZE_1": 8,
  "loop1": "i",
  "loop2": "j",
  "loop3": "k"
}
stdout:
case  | miss_cache | miss_reg | latency | speedup           
------+------------+----------+---------+-------------------
case0 | 2          | 20       | 50      | 1.0               
case1 | 1712       | 9216     | 34896   | 2.3328748280605227
case2 | 36800      | 66560    | 618560  | 1.0               
case3 | 41846      | 72075    | 699765  | 1.0
Weighted Speedup: 1.3999


data:
{
  "BLOCK_SIZE_1": 8,
  "loop1": "k",
  "loop2": "i",
  "loop3": "j"
}
stdout:
case  | miss_cache | miss_reg | latency | speedup          
------+------------+----------+---------+------------------
case0 | 2          | 20       | 50      | 1.0              
case1 | 1700       | 9216     | 34716   | 2.344970618734877
case2 | 36800      | 66560    | 618560  | 1.0              
case3 | 41846      | 72075    | 699765  | 1.0
Weighted Speedup: 1.4035


Compilation Summary
===================
Success: 4      Failed: 0

Execution Summary
===================
Success: 4      Failed: 0   
```