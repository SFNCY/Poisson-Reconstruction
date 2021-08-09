if [ ! -d "./build" ]; then
    mkdir ./build/
fi

# sudo rm -rf ./build/*
cd ./build
cmake ..
make -j8
./bin/PossionRec  --in ../Data/horse.npts --out horse.ply --depth 6 --performance true --Verbose true