Para compilar usando sanitizer (dentro da respectiva pasta):
g++ mochila.cpp -o mochila -fsanitize=address -static-libasan -g
g++ pcv.cpp -o pcv -fsanitize=address -static-libasan -g

Para executar o problema da mochila (dentro da respectiva pasta):
./mochila ../entradas/mochila/mochila_100_1000_1 0
./mochila ../entradas/mochila/mochila_100_1000_1 1
./mochila ../entradas/mochila/mochila_4_20 0
./mochila ../entradas/mochila/mochila_4_20 1

Para executar o PCV (dentro da respectiva pasta):
./pcv ../entradas/pcv/tsp_51
./pcv ../entradas/pcv/tsp_5