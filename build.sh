clear

mkdir -p build
cd build

mkdir -p linux
cd linux

mkdir -p cmake
mkdir -p out
cd cmake

cmake ../../../
make

mv web_server ../out
cd ../out

cp ../../../config/server.config ./server.config
cp -r ../../../website ./
chmod +x web_server

echo ""
./web_server
