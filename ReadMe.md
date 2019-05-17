# 目录组织

- **Solver/**
  中心选址问题的 Visual C++ 项目.
  核心算法.

- **Protocol/**
  - PCenter.proto
    中心选址问题的输入输出数据定义文件.

- **Simulator/**
  提供算法交互接口的 Visual C++ 项目.
  实现批量测试与结果展示等功能.

- **Checker/**
  中心选址问题计算结果检查程序的 Visual C++ 项目.

- **Analyzer/**
  中心选址问题算例/结果/日志分析程序的 Visual C# 项目.

- **Deploy/**
  开发阶段程序部署目录, 包含程序运行所需要的所有可执行文件和数据.
  - **Instance/**
    输入数据.
  - **Solution/**
    求解结果.
  - **Visualization/**
    结果可视化.

- **Doc/**
  项目文档.
  - **ProblemDescription.md**
    中心选址问题描述文档.

- **Lib/**
  第三方库.
  - **protobuf**
    由 Google 开发的数据交换格式.
    访问 https://github.com/google/protobuf/releases 下载最新版本.
    - **bin/**
      生成对消息进行序列化与反序列化的代码的工具.
    - **include/**
      头文件. 应被设置为 "附加包含文件".
    - **lib/**
      静态链接库. 应被设置为 "附加库目录".
      该文件夹下的库文件应被设置为 "附加依赖项".



# 编译链接

## 修改代码

1. 在 `Solver.cpp` 中搜索 `TODO[0]`, 并在对应位置添加求解算法的代码.
2. 在 `Problem.h` 中搜索 `TODO[0]`, 并在对应位置修改提交至测评系统的信息.


## 编译链接

1. 自己编译 protobuf 或者在 [NPBenchmark](https://github.com/HUST-Smart/NPBenchmark/releases/tag/v1.0) 或群共享下载 `protobuf3.6.1_vs2017_MT+MTd_x64.zip` 并解压至 `Lib` 目录.
2. 运行 `Protocol/` 目录下的 `generate.bat` (未更改 `*.proto` 文件可跳过该步骤).
3. 安装 gurobi 并在 [NPBenchmark](https://github.com/HUST-Smart/NPBenchmark/releases/tag/v1.0) 或群共享下载 `gurobi8.1.1_vs2017_MT+MTd+MD+MDd_x64.zip` 并解压至 `Lib` 目录. (不使用求解器可跳过该步骤)
4. 打开根目录下的 Visual Studio 2017 解决方案文件 `IncrementalGraphDrawing.sln`, 设置输出选项为 Release x64, 生成解决方案.
