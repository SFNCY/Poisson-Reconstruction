# Possion Surface Reconstruction　泊松表面重建
[Adaptive Multigrid Solvers官网主页](http://www.cs.jhu.edu/~misha/Code/PoissonRecon/Version10.04/)

本项目使用cmake在Ubuntu18.04下重新构建了Possion重建程序
- 编译
```
cd Possion-Reconstruction
./compile.sh
```

- 运行demo
```
cd ../build/bin
./PossionRec  --in ../../Possion-Reconstruction/Data/horse.npts --out horse.ply --depth 10
```
- 查看重建结果（需要安装meshlab）
```
meshlab horse.ply 
```
<img src="https://github.com/liyucheng251/Possion-Reconstruction/blob/master/img/horse.png" height=400>　<img src="https://github.com/liyucheng251/Possion-Reconstruction/blob/master/img/bunny.png" height=400>

若要运行自己的点云，需要保证点云文件中具有法向数据


