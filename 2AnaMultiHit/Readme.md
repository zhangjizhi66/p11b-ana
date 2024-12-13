# 正背面多重性分析

主要处理两类情况:
1)粒子入射后,相邻2条或3条出现能量共享.此时需将能量加合,并取原始能量最大的条作为合并后的条编号.
2)多个粒子同时入射某一面的同一条(另一面不同条).此时需将该条能量拆分(按另一面的比例).

定义:"单一信号",即一重信号,或能量共享时可加合为一重信号的情形.

思路:
任何原始信号组合均可以"一面单一信号+另一面单一或非单一信号,且总能量匹配"为基本单元进行划分,称为"基本划分",且"基本划分"内不能存在其他"基本划分".对于"基本划分",将所有能量共享事件合并,然后在两面多重性不匹配时,拆分单一信号.

函数:
bool Single(std::vector<dets> &vec)  (判断与处理"单一信号")
- 判断 vec 是否为单一信号,返回判断结果.(2条能量共享要求id相邻;3条能量共享除id相邻外,还要求中间条的能量最高.)
- 若是单一信号,且有能量共享,则将 vec 中的信号合并为一重信号.
- 若不是单一信号,则不对 vec 进行任何操作.
    
bool Merge(std::vector<dets> &vec)  (合并能量共享信号)
- 对 vec 中所有能量共享信号进行合并.
- 遍历 vec 的所有容量为 3 的子集,检验子集是否为单一信号,若是则合并(使用 SingleParticle 函数).
- 随后,类似地遍历容量为 2 的子集.
- 本函数使用递归形式,即若存在子集满足 SingleParticle 的检验,则对余子集调用本函数.
    
bool Match(std::vector<dets> &vfront, std::vector<dets> &vback, double reso)  (判断与处理"基本划分")
- 先判断 vfront 和 vback 能量总和是否匹配.误差为 reso.若不匹配则返回非.
- 再判断 vfront 和 vback 是否至少有一者为单一信号(即能通过 SingleParticle 函数的检验).若否则返回非.(使用副本进行检验)
- 再判断子集是否存在"基本划分",确保划分为最小单元(调用本函数进行递归判断).
- 若以上判断均通过,则对 vfront 和 vback 合并能量共享信号(调用 Merge 函数).
- 此时,若两面信号数量不匹配,则拆分单一信号.

bool AnaMultiHit(std::vector<dets> &vfront, std::vector<dets> &vback, double reso)
- 遍历 vfront 和 vback 所有非空子集的组合,检验是否为"基本划分",若是则处理(使用 Match 函数).
- 若不存在"基本划分",则剩余信号视为假信号,全部丢弃.
- 本函数采用递归形式,一次递归只处理一个"基本划分",余子集继续调用本函数进行处理.
    
使用: 直接对正背面信号集调用 AnaMultiHit 函数.reso 的取值参考 1FrontBackCal/3Fit 输出文件中的拟合残差图.
    
测试: 运行时若不加参数,则开启多重性分析的测试模式.可在 DataAnalysis.cc 的 test 函数中调整模拟输入信号.