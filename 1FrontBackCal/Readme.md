# DSSD 正背面关联

## 1GetTCutG: 确定关联的大致范围，生成 TCutG

画出每个 DSSD 的正面-背面能量二维关联图（所有条），要求正面背面均为一重信号; 横条为y方向,竖条为x方向,作图时两者不能颠倒

用 TBrowser 打开 ana_5394_W1000_notwave.root 文件,可以看到二维图中间有斜率接近1的明显条带，用 TCutG 选出该条带，丢弃条带外的零散点

TCutG 命名规则为 d1cut d2cut 等，数字代表 DSSD 的序号；相应文件命名为 d1cut.C d2cut.C 等

## 2GetTGraph: 将一重事件存储为 TGraph 用于后续拟合

将所有用来拟合的数据存为 TGraph

将每一对正背面条 TGraph 的点数存到 TH2F 中; 找出统计最高的像素作为拟合起点

## 3Fit: 拟合得到刻度系数

拟合TGraph，得到刻度参数，参数存到 fitparameter.txt

运行日志为 fit.log，内有每次拟合的条信息、拟合误差、拟合数据范围、拟合点数等信息

用 TBrowser 打开 ana_5394_W1000_notwave.root 文件, 检查拟合情况和残差

## 4Calibrate: 刻度所有数据

