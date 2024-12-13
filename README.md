# C语言实现基于RISC-V指令集的CPU控制器仿真 （以8的阶乘为例）
## 一、数据结构
#### 1.对内存大小，寄存器,内存数量 进行宏定义，定义程序计数器和停止标志的初始值

![image-3.png](img/image-3.png)
### 2.ALU所需指令的枚举
![image-4.png](img/image-4.png)
### 3.定义控制信号的结构体
![image-5.png](img/image-5.png)
## 二、指令循环
### 4.取指令部分

![image-6.png](img/image-6.png)
#### 返回1个32位无符号整数（unsigned int型），用if语句做越界判断。

### 5.指令译码（关键）
#### 5.1提取操作码，功能码，寄存器编号，处理立即数的符号扩展
![image-7.png](img/image-7.png)
#### RISC-V指令集：
#### 操作码opcode：7位（第0位-第6位）
#### funt7功能码：6位（第25位-第31位），  funt3功能码：3位（第12位-第14位）。
#### 寄存器编号：5位（分为rs1，rs2，rd三类寄存器），  立即数：12位。
![img-15.png](img/img-15.png)



#### 5.2初始化控制信号（更新）
![image-8.png](img/image-8.png)
##### LW指令
![image-9.png](img/image-9.png)
##### ADDI指令
![image-10.png](img/image-10.png)

##### BNE指令
![image-11.png](img/image-11.png)

##### MUL指令
![image-12.png](img/image-12.png)

### 6.ALU实现
![image-5.png](img/image-5.png)
#### 依据操作码来实现对应运算

### 7.指令执行
![image-13.png](img/image-13.png)
#### 根据指令译码生成的控制信号和寄存器值，执行相应的指令逻辑

### 8.输出寄存器的值
![image-14.png](img/image-14.png)


## 三、主函数
### 9. 主函数

#### 主存和源寄存器，目的寄存器的初始化 
![image-16.png](img/image-16.png)
#### 在一个指令循环中执行取指令，译码，执行指令，指令跳转

## 四、测试与运行
### 10.CPU编译（F12）测试（F11）实现8！的阶乘计算
### 11.CPU运行结果
![image-17.png](img/image-17.png)

![image-18.png](img/image-18.png)
### 上图是实验全部的的运行测试答案
