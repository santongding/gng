# score points
- 基础部分: 40 完成
- 自定义备份: 10 完成
- 压缩解压: 10 完成
- 打包解包: 10 完成
- 加密备份: 10 完成
- 增量备份: 5 完成
- 元数据支持: 10 完成

# build
## requirements
- CMAKE 3.16
- GCC
- protobuf: 下载: https://github.com/protocolbuffers/protobuf/releases/download/v21.9/protobuf-cpp-3.21.9.zip ,解压后按照其中的src/README.md进行安装

## build
切换到工程根目录, 执行以下命令
```
mkdir build
cd build
cmake ..
make
```
## run
切换到工程根目录, 执行以下命令获取帮助

build/gng -h

可以将执行文件软链接到/usr/local/bin中方便使用

工程根目录下创建.gngignore, 在其中写入路径的正则表达式即可忽略特定文件(每行一条)

例如
```
build/*
src/*.o
```
可忽略build文件夹下所有文件, 以及src文件夹下所有.o后缀文件

## install
没实现
