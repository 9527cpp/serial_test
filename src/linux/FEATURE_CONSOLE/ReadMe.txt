1: 虚拟串口虚拟出来一对串口
2: windows上打开其中之一(默认配置 9600)
3: 虚拟机上链接另一个串口
4: linux下枚举/dev/ttyS1(这就是那个串口),并打开该串口(默认配置9600)
5: 执行sudo ./uart_rda 开始接收数据
6: windows串口工具上 输入test1   或者 test2,aaaaa
