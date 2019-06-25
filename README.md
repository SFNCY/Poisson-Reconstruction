# Possion Reconstruction
[Adaptive Multigrid Solvers官网主页](http://www.cs.jhu.edu/~misha/Code/PoissonRecon/Version10.04/)

本项目使用cmake在Ubuntu18.04下重新构建了Possion重建程序
- 编译
```
cd ScreenedPossion
./compile.sh
```

- 运行demo
```
cd ../build/bin
./PossionRec  --in ../../ScreenedPossion/Data/horse.npts --out horse.ply --depth 10
```
- 查看重建结果（需要安装meshlab）
```
meshlab horse.ply 
```

若要运行自己的点云，需要保证点云文件中具有法向数据


