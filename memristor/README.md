# memristor

main.cpp包括了如下几个部分：参数设置，Cell类，Array类，ArrayPeripheral类和ArrayIO类，以及一个示例的主函数。

说明：

1. 如有需要，修改开头的参数设置，包括：
   * 阵列最大规模（maxArraySize，当前是512，表示一个512*512的计算单元），输入无符号整型数的最大位数（maxInputVBits，当前是8，表示uint8），权重整型数的最大位数（maxCellBits，当前是8，表示int9），输出当前没有做限制，是C++内部的int范围；
   * 是否使用器件和模拟电路的非理想特性，包括deviceVar，circuitNoise，ADCQuantization，当前均为false，即不使用非理想特性，输出的计算结果是理想的准确值。

2. 在main()函数中实例化一个arrayIO类进行计算，步骤是：
   * ArrayIO a(m, n)：定义一个m行n列的忆阻器阵列a；
   * a.Program(conductance)：将(m, n)的二维权重vector赋值给阵列a；
   * out = a.Calculate(in)：输入一个(m, )的一维vector in，计算出输出的(n, )的一维vector out；
   * a.timeCost成员变量返回对计算的时间估计，单位是us。
