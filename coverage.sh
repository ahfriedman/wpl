rm -rf cov/ # Remove the coverage folder
cmake -S . -B cov -DCODE_COVERAGE=ON -DBUILD_DCMAKE_BUILD_TYPE=Debug -DBUILD_COVERAGE=ON && cmake --build cov && cd cov && make coverage

