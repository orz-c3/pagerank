# PageRank 基础版完成情况说明

## 1. 当前版本定位

本次提交为 **PageRank 基础实现版本**，主要完成作业中的基础算法部分，包括数据读取、PageRank 迭代计算、dead-ends 处理、spider-traps 缓解、收敛判断和 Top-100 结果输出。

当前版本使用 **C++17 单文件实现**，核心文件为：

```text
PageRank.cpp
```

程序读取当前目录下的：

```text
Data.txt
```

并输出 PageRank 得分最高的前 100 个节点到：

```text
Res.txt
```

输出格式为：

```text
NodeID Score
```

作业要求中明确需要读取 `Data.txt`，计算 PageRank，并在 teleport parameter 为 `0.85` 时输出 Top-100 节点及其分数。

---

## 2. 已完成的基础功能

当前基础版已经完成以下内容：

```text
1. 读取 Data.txt 中的有向边数据，格式为 FromNodeID ToNodeID。
2. 收集所有出现过的节点，包括只作为 ToNodeID 出现的节点。
3. 使用 long long 保存原始 NodeID，避免较大编号溢出。
4. 将原始 NodeID 映射为连续内部下标。
5. 使用 incoming 邻接表保存入边关系。
6. 使用 outDegree 数组保存每个节点的出度。
7. 删除重复边，避免重复边影响 PageRank 分配。
8. 设置 damping / teleport parameter = 0.85。
9. 初始化每个节点的 PageRank 为 1 / N。
10. 处理 dead-ends，即出度为 0 的节点。
11. 使用 teleport 项缓解 spider-traps。
12. 使用 L1 差值判断收敛，eps = 1e-8。
13. maxIter = 10000，仅作为防止死循环的兜底上限。
14. 未收敛时终止程序，不生成正式 Res.txt。
15. PageRank 总和不接近 1.0 时终止程序，不生成正式 Res.txt。
16. 输出 Top-100 原始 NodeID 和 PageRank 分数。
17. Res.txt 无表头，只保留结果数据行。
18. 分数输出精度为 fixed + setprecision(12)。
```

代码中已经使用 `PageRankResult` 结构体记录完整 PageRank 数组、是否收敛、最终迭代次数和最终 L1 差值；`writeTop100()` 只在收敛和总和校验通过后执行。

---

## 3. 正确性验证

当前版本运行后会在终端输出：

```text
Converged in ... iterations. Final diff = ...
Total PageRank = ...
```

其中：

```text
Final diff < 1e-8
Total PageRank ≈ 1.0
```

只有同时满足以下两个条件时，程序才会生成正式 `Res.txt`：

```text
1. PageRank 迭代达到收敛条件；
2. 所有节点的 PageRank 总和接近 1.0。
```

测试输出结果文件 `Res.txt` 已经满足：

```text
1. 共 100 行；
2. 无表头；
3. 每行格式为 NodeID Score；
4. Score 保留 12 位小数；
5. 按 PageRank 分数降序排列。
```

当前 `Res.txt` 的前几行结果如下：

```text
75 0.000198721357
8686 0.000190205478
9678 0.000188337048
5104 0.000187795344
725 0.000186793164
```

完整结果文件已经生成，并符合 Top-100 输出格式。

---

## 4. 性能测试

基础版已经完成内存与运行时间测试。

测试记录显示，程序连续运行 10 次，最大内存占用约为：

```text
1.89 MB ~ 1.91 MB
```

10 次平均内存占用为：

```text
1.90 MB
```

运行时间大部分约为：

```text
0.10 秒
```

首轮记录为：

```text
0.54 秒
```

该结果低于作业建议的内存上限 `80MB` 和运行时间上限 `60s`。

---

## 5. 编译与运行方式

推荐使用以下命令编译：

```bash
g++ -O2 -std=c++17 PageRank.cpp -o PageRank.exe
```

运行：

```bash
./PageRank.exe
```

Windows PowerShell 下可运行：

```powershell
.\PageRank.exe
```

运行前需要确保 `Data.txt` 位于程序同一目录下。

---

## 6. 当前版本限制

当前版本是 **基础 PageRank 实现版**，尚未作为最终优化版提交。

已经做到：

```text
1. 不构造完整 N × N 邻接矩阵；
2. 使用邻接表降低基础内存占用；
3. 正确处理 dead-ends 和 spider-traps；
4. 能稳定输出 teleport parameter = 0.85 的 Top-100 结果。
```

但以下内容仍属于后续优化负责人需要继续完成的部分：

```text
1. Sparse Matrix 稀疏矩阵优化；
2. Block Matrix 块矩阵优化；
3. 最终版内存优化方案说明；
4. 与基础版的性能对比分析；
5. 完整实验报告整理。
```

作业要求中明确提到，除基础 PageRank 外，还需要通过 Sparse Matrix、Block Matrix 等方式优化内存使用，因此当前基础版可作为后续优化实现的代码基础。

---

## 7. 建议提交说明

可以作为 GitHub commit message：

```text
完成 PageRank 基础版实现

- 实现 Data.txt 有向边读取
- 完成 NodeID 到内部下标映射
- 使用 long long 保存原始 NodeID
- 使用 incoming 邻接表和 outDegree 数组构建基础图结构
- 实现 damping = 0.85 的 PageRank 迭代计算
- 处理 dead-end 节点
- 使用 teleport 项缓解 spider-trap 问题
- 增加收敛检测，eps = 1e-8，maxIter = 10000
- 增加完整 PageRank 总和校验
- 未收敛或总和异常时不生成正式 Res.txt
- 输出 Top-100 节点到 Res.txt，无表头，12 位小数
- 完成基础运行测试和内存测试
```
