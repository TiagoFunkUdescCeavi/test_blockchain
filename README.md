# test_blockchain

Em cada pasta a implementação de cada algoritmo, inclusive a versão ns3 necessária.


Para configuração, compilação e execução, cada pasta possui um pdf com instruções e comandos.

Na pasta pow, está o algoritmo Proof of Work:

Vá para a pasta: ns-allinone-3.25/ns-3.25/

Para configurar
CXXFLAGS="-std=c++11" ./waf configure --build-profile=optimized --out=build/optimized --with-pybindgen=/home/bill/Desktop/workspace/ns-allinone-3.24/pybindgen-0.17.0.post41+ngd10fa60 --enable-mpi --enable-static

Para compilar:
./wf

Para executar:
./waf --run "bitcoin-test --noBlocks=10 --nodes=60"

Na pasta pos, está o algoritmo Proof of Stake:

Vá para a pasta: ns-allinone-3.30.1/ns-3.30.1

Para configurar:
CXXFLAGS="-std=c++11" ./waf configure --enable-examples --enable-static

Para compilar:
./wf

Para executar:
./waf --run src/proof-of-stake-testbed/examples/proof-of-stake-testbed-algorand
