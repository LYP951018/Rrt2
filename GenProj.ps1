cd Build
rm ./CMakeCache.txt
cmake .. -T host=x64 -A x64 -DCMAKE_BUILD_TYPE=Debug
cd -