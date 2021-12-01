Proof-of-stake Testbed
=======================

Pre-requisities
--------------

- git
- c++11
- ns-3.30
- rapidjson

Installation
------------------------------------
1) Install ns-3 simulator by https://www.nsnam.org/docs/release/3.30/tutorial/html/getting-started.html
2) Add rapidjson library
 - https://rapidjson.org/
 - Just copy `include/rapidjson` folder of repo https://github.com/Tencent/rapidjson to project's include path `./ns-allione-3.30/ns-3.30`
3) Clone that module
 - Clone into `./ns-allione-3.30/ns-3.30/src`

Run
------------------------------------
1) Open directory - `cd ./ns-allione-3.30/ns-3.30`
2) Build - `CXXFLAGS="-std=c++11" ./waf configure --enable-examples --enable-static`
3) Run - `./waf --run src/proof-of-stake-testbed/examples/proof-of-stake-testbed-<protocol_name>`
3.1) Available `protocol_names` - algorand, ouroboros, casper

You can run other examples `./waf --run src/proof-of-stake-testbed/examples/xxxxxx`


NOTES
------------------------------------
python run
./waf --pyrun proof-of-stake-testbed/first.py

run c++
./waf --run src/proof-of-stake-testbed/examples/proof-of-stake-testbed

https://www.nsnam.org/docs/manual/html/new-modules.html

CXXFLAGS="-std=c++11" ./waf configure --enable-examples --enable-static


