#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_sl(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [10] = handle_sl,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

/* Overview:
 *   处理MIPS中lw和sw指令地址未对齐的异常。
 *   当这些指令访问的地址没有按照4字节对齐时，会触发此异常。
 *   函数会修改相应指令，确保重新执行时能够保证四字节对齐。
 * 
 * 参数：
 *   - tf: 陷阱帧，包含异常发生时的CPU状态
 */
void do_sl(struct Trapframe *tf) {
    u_int bad_addr = tf->cp0_badvaddr;
    u_int epc = tf->cp0_epc;
    u_int instr = *(u_int *)epc;
    u_int opcode = (instr >> 26) & 0x3F;  // 提取操作码
    u_int base = (instr >> 21) & 0x1F;    // 提取基址寄存器
    u_int rt = (instr >> 16) & 0x1F;      // 提取目标寄存器
    u_int offset = instr & 0xFFFF;        // 提取偏移量(有符号扩展)
    
    // 如果偏移量是负数（最高位为1），则进行符号扩展
    if (offset & 0x8000) {
        offset |= 0xFFFF0000;
    }
    
    // 检查是否为lw或sw指令
    if (opcode == 0x23 || opcode == 0x2B) {  // lw = 0x23, sw = 0x2B
        // 计算对齐后的地址（向下对齐到4字节边界）
        u_int aligned_addr = bad_addr & ~0x3;
        
        if (opcode == 0x23) {  // 处理lw指令
            // 读取对齐后地址的值
            u_int data = *(u_int *)aligned_addr;
            
            // 根据未对齐地址的偏移量，移位数据
            int shift = (bad_addr & 0x3) * 8;
            data = (data >> shift);
            
            // 将数据存储到目标寄存器
            tf->regs[rt] = data;
        } else {  // 处理sw指令
            // 读取当前对齐地址的值
            u_int current = *(u_int *)aligned_addr;
            u_int data = tf->regs[rt];
            
            // 根据未对齐地址的偏移量，合并数据
            int shift = (bad_addr & 0x3) * 8;
            u_int mask = 0xFFFFFFFF << shift;
            
            // 清除目标位置的原始数据，然后写入新数据
            current &= ~mask;
            current |= (data << shift);
            
            // 写回对齐后的地址
            *(u_int *)aligned_addr = current;
        }
        
        // 跳过当前指令，继续执行下一条指令
        tf->cp0_epc += 4;
    } else {
        // 如果不是lw或sw指令，则报错
        print_tf(tf);
        panic("Address alignment exception for non-lw/sw instruction: opcode=%x", opcode);
    }
}
