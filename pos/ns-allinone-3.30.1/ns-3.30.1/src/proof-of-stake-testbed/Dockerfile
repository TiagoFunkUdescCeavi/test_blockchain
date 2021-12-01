FROM ubuntu:18.04

RUN apt-get update
RUN apt-get -y install g++ python3
RUN apt-get -y install g++ python3 python3-dev pkg-config sqlite3 wget
RUN apt-get -y install wget git

RUN mkdir /project
RUN mkdir /rapidjson

RUN wget https://www.nsnam.org/release/ns-allinone-3.30.tar.bz2
RUN tar xjf ns-allinone-3.30.tar.bz2 -C /project

RUN git clone https://github.com/Tencent/rapidjson.git /rapidjson
RUN cp -R /rapidjson/include/rapidjson/ /project/ns-allinone-3.30/ns-3.30

RUN cd /project/ns-allinone-3.30/ns-3.30
RUN git clone https://github.com/honza66/proof-of-stake-testbed.git /project/ns-allinone-3.30/ns-3.30/src/proof-of-stake-testbed
WORKDIR /project/ns-allinone-3.30/ns-3.30
RUN CXXFLAGS="-std=c++11" ./waf configure --enable-examples --enable-static

CMD ["./waf", "--run", "src/proof-of-stake-testbed/examples/proof-of-stake-testbed-algorand" ]




