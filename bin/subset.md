# Subset 对象的子集 在Model层中的思路整理

### 交并补差
* 交集： intersection   AB有交有非交，在A且在B
* 并集： union          AB有交有非交，在A或在B
* 补集： complemention  A为B子集，在B不在A
* 差集： difference     AB有交有非交，在A不在B

### base
* 一个Struct有多个Property
* 一个Subset本质上还是一个Struct

### Subset里面的Property的类型:
* 某base里面的一级原始数据属性(integer/string)
* 某base里面的一级对象
* 某base里面的N级对象里面的一级原始数据属性
* 某base里面的N级对象里面的对象

### 可能需要的转换：
* 单base中单subset的抽取
* subset在某BASE部分的数据填入，如玩家的公会信息去到公会进程时填充公会信息
* subset不在某BASE部分的数据填入，如玩家信息获取时先到公会取到数据后回GAME时，公会部分数据不能覆盖，其他数据全量填入
* subset在某BASE部分的数据填出到另一subset或者struct中，如玩家的公会信息去到公会进程获取最新公会信息后回到GAME进程时，希望最精简的直接把这部分数据更新到本地的mem中，甚至对比判定是否有改变并［通知客户端or通知其他进程or通知其他玩家or存盘等］

