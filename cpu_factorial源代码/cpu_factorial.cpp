#include <stdio.h>
#include <stdint.h>

#define REG_NUM 32
#define MEM_SIZE 32

uint32_t reg[REG_NUM]; // 寄存器
uint32_t mem[MEM_SIZE]; // 内存
uint32_t PC = 0; // 程序计数器
int halt_flag = 0; // 停止标志

//设置控制信号结构体
typedef enum {
    MUL, ADDI, BNE, ADD, LW,
} ALUOperation;

//设置控制信号结构体
typedef struct {
    uint8_t RegDst;
    uint8_t ALUSrc;
    uint8_t MemtoReg;
    uint8_t RegWrite;
    uint8_t MemRead;
    uint8_t MemWrite;
    uint8_t Branch;
    uint8_t Jump;
    ALUOperation ALUOp;
} ControlSignals;

// ALU
uint32_t ALU(uint32_t A, uint32_t B, ALUOperation op) {
    switch (op) {
        case ADD: return A + B;
        case ADDI: return A + B;
        case MUL: return A * B;
        case BNE: return (A != B) ? 1 : 0;
        default: return 0;
    }
}

// 从内存中获取指令
uint32_t fetch() {
    if (PC / 4 >= MEM_SIZE) {
        halt_flag = 1;
        return 0;
    }
    return mem[PC / 4];
}


//解码
void decode(uint32_t instruction, ControlSignals *ctrl, uint32_t *rs1, uint32_t *rs2, uint32_t *rd, uint32_t *imm) {
    uint32_t opcode = instruction & 0x7F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t funct7 = (instruction >> 25) & 0x7F;
    *rd = (instruction >> 7) & 0x1F;
    *rs1 = (instruction >> 15) & 0x1F;
    *rs2 = (instruction >> 20) & 0x1F;
    *imm = 0;

//初始化控制信号
    ctrl->RegDst = 0;
    ctrl->ALUSrc = 0;
    ctrl->MemtoReg = 0;
    ctrl->RegWrite = 0;
    ctrl->MemRead = 0;
    ctrl->MemWrite = 0;
    ctrl->Branch = 0;
    ctrl->Jump = 0;

//操作数
    switch (opcode) {
        case 0x03: // LW指令
            ctrl->ALUSrc = 1;
            ctrl->MemtoReg = 1;
            ctrl->RegWrite = 1;
            ctrl->MemRead = 1;
            *imm = (instruction >> 20) & 0xFFF;
            if (*imm & 0x800) *imm |= 0xFFFFF000; // Sign extend
            ctrl->ALUOp = ADD;
            break;
        case 0x13: // ADDI指令
            ctrl->ALUSrc = 1;
            ctrl->RegWrite = 1;
            *imm = (instruction >> 20) & 0xFFF;
            if (*imm & 0x800) *imm |= 0xFFFFF000;
            ctrl->ALUOp = ADDI;
            break;
        case 0x63: // BNE指令
            ctrl->Branch = 1;
            ctrl->ALUOp = BNE;
            *imm = ((instruction >> 31) & 0x1) << 12 |
                   ((instruction >> 7) & 0x1) << 11 |
                   ((instruction >> 25) & 0x3F) << 5 |
                   ((instruction >> 8) & 0xF) << 1;
            if (*imm & 0x1000) *imm |= 0xFFFFE000; // Sign extend
            break;
        case 0x33: // MUL指令
            ctrl->RegDst = 1;
            ctrl->RegWrite = 1;
            switch (funct3) {
                case 0x00:
                    if (funct7 == 0x01) {
                        ctrl->ALUOp = MUL;
                    } else {
                        ctrl->ALUOp = ADD;
                    }
                    break;
            }
            break;
    }
}

// 执行指令
void execute(uint32_t rs1, uint32_t rs2, uint32_t rd, uint32_t imm, ControlSignals ctrl) {
    uint32_t ALUResult;
    if (ctrl.ALUSrc) {
        ALUResult = ALU(reg[rs1], imm, ctrl.ALUOp); //处理立即数操作
    } else {
        ALUResult = ALU(reg[rs1], reg[rs2], ctrl.ALUOp); // 处理寄存器操作
    }

    if (ctrl.MemRead) {
        reg[rd] = mem[ALUResult / 4];
    } else if (ctrl.RegWrite) {
        reg[rd] = ALUResult;//写入寄存器
    }

    if (ctrl.Branch) { //分支
        ALUResult = ALU(reg[rs1], reg[rs2], ctrl.ALUOp);


        printf("\n");
        if (ALUResult != 0) {
            PC += (int32_t)imm << 2;
            return;
        }
    }

    if (!ctrl.Branch) {
        PC += 4;
    }
}

//输出所有寄存器的值
void print_registers() {
    for (int i = 0; i < REG_NUM; i++) {
        printf("R%02d = %d\n", i, reg[i]);
    }
}

int main() {
    PC = 0;
    for (int i = 0; i < REG_NUM; i++) reg[i] = 0;
    for (int i = 0; i < MEM_SIZE; i++) mem[i] = 0;
    mem[10] = 1;       // 内存地址 40: 值为 1（用于加载到寄存器）
    mem[11] = 2;       // 内存地址 44: 值为 2（用于加载到寄存器）
    mem[12] = 9;       // 内存地址 48: 值为 9（用于分支比较）

    mem[0] = 0x02802083; // LW R1, 100(R0)
    mem[1] = 0x02c02103; // LW R2, 104(R0)
    mem[2] = 0x03002183; // LW R3, 108(R0)
    mem[3] = 0x022080B3; // MUL R1, R1, R2
    mem[4] = 0x00110113; // ADDI R2, R2, 1
    mem[5] = 0xFE311FE3; // BNE R2, R3, -2


    while (PC < MEM_SIZE && !halt_flag) {
        uint32_t instruction = fetch();
        if (halt_flag) break;

        ControlSignals ctrl;
        uint32_t rs1, rs2, rd, imm;
        decode(instruction, &ctrl, &rs1, &rs2, &rd, &imm);
        execute(rs1, rs2, rd, imm, ctrl);

        printf("PC: %d, reg[1]: %d, reg[2]: %d, reg[3]: %d\n", PC, reg[1], reg[2], reg[3]);
        if(reg[2]==reg[3]&&reg[2]!=0&&reg[3]!=0)
        {
            break;
        }
    }
    printf("\n");
 printf("寄存器的值为\n");
    print_registers();  // 调用打印寄存器值的函数
    return 0;
}

