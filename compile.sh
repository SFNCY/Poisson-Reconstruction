if [ ! -d "../build" ]; then
    mkdir ../build/
fi

sudo rm -rf ./build/*
cd ../build
cmake ../ScreenedPossion
make -j4