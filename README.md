# Hardware-Generation-Tool

## About
The project involved RTL design, simulation and synthesis of a configurable Artificial Neural Network creating a parameterizable HDL generation tool in C++ to create an optimized 3-Layer Neural Network. This software flexibly generates hardware to accelerate the evaluation of three layers of convolution. Each layer is based on an ARM’s AXI4 handshaking protocol and is individually configurable. The optimization algorithm decides the parallelism for each layer to maximize the throughput.

## Block Diagram
![3-Layer System](images/threeLS.png)


## Tradeoffs with parameters N, M, T, P:
•	Parameter T: As T increases, the problem size remains constant, cost increases as the area and the power increase, precision increases and the performance remains the same as the throughput remains the same.  
•	Parameter N: As N increases, the problem size increases, the cost increases as both, the power and the area increase, precision of the internal signals such as counters increase and the performance decreases as the throughput decreases.
•	Parameter M: As M increases, the problem size increases, cost increases as the area increases and the power increases in most cases, precision of the internal signals increases and the performance decreases as the throughput decreases.. 
•	Parameter P: As P increases, the problem size remains the same, the cost increases as the power and the area increase, the precision remains the same, the performance increases as the throughput increases. 


## Tradeoff Analyis
![Power vs T](images/PvsT.png) ![Throughput vs M](images/TvsM.png)


## Compilations steps:
1. make
2. ./testgen mode N M T P
3. ./gen mode N M T P 
