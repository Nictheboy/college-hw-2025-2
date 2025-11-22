.data
    # 分配数组 D 的空间，大小为 400 字节（100 个整数），初始化为 0
    D_array: .word 0:100  
    
    # 输出提示字符串
    msg_start: .asciiz "Running loop with a=3, b=2...\n"
    msg_val:   .asciiz "D index "
    msg_eq:    .asciiz " = "
    newline:   .asciiz "\n"

.text
.globl main

main:
    # --- 1. 初始化部分 (Setup) ---
    
    # 打印开始提示
    li      $v0, 4
    la      $a0, msg_start
    syscall

    # 设定测试变量: a = 3, b = 2
    li      $s0, 3          # $s0 = a
    li      $s1, 2          # $s1 = b
    la      $s2, D_array    # $s2 = D (数组基地址)

    # --- 2. 你的核心代码 (User Code) ---
    # ==========================================
    
    # 初始化 i = 0
    li      $t0, 0              # i = 0

loop_outer:
    # 检查外层循环条件 i < a
    bge     $t0, $s0, loop_end  # 如果 i >= a，结束

    # 初始化 j = 0
    li      $t1, 0              # j = 0

loop_inner:
    # 检查内层循环条件 j < b
    bge     $t1, $s1, loop_outer_continue # 如果 j >= b，跳出内层

    # --- 循环体开始 ---
    
    # 计算数值: val = i + j
    add     $t2, $t0, $t1       # $t2 = i + j

    # 计算地址: D[4*j]
    # Index = 4 * j
    sll     $t3, $t1, 2         # $t3 = j * 4 (逻辑索引)
    
    # 计算字节偏移量: Offset = Index * 4 (int 为 4 字节)
    # 关键点验证：sll 2位 再 sll 2位 = 左移4位 = 乘以16。
    # 4*j 个 int = 16*j 个 byte。这是正确的。
    sll     $t3, $t3, 2         # $t3 = (j * 4) * 4 = j * 16
    
    # 计算最终内存地址: Addr = D + Offset
    add     $t3, $s2, $t3       # $t3 = &D[4*j]

    # 存值: D[4*j] = i + j
    sw      $t2, 0($t3)

    # --- 循环体结束 ---

    # j++
    addi    $t1, $t1, 1
    j       loop_inner

loop_outer_continue:
    # i++
    addi    $t0, $t0, 1
    j       loop_outer

loop_end:
    # ==========================================
    # --- 3. 验证部分 (Check Results) ---
    
    # 我们预期 D[0] = 2, D[4] = 3 (因为 a=3, 最后 i=2)
    # 让我们打印 D[0] 和 D[4] 的值来验证

    # --- 打印 D[0] ---
    la      $a0, msg_val    # "D index "
    li      $v0, 4
    syscall

    li      $a0, 0          # 打印下标 0
    li      $v0, 1
    syscall

    la      $a0, msg_eq     # " = "
    li      $v0, 4
    syscall

    lw      $a0, 0($s2)     # 加载 D[0] (偏移 0)
    li      $v0, 1          # 打印整数
    syscall

    la      $a0, newline
    li      $v0, 4
    syscall

    # --- 打印 D[4] ---
    # D[4] 的偏移量是 4 * 4 = 16字节
    # 这里要注意：C代码写的是 D[4*j]，当 j=1 时，写的是 C语言数组的第4个元素
    # 在内存中，这是第 4 * 4 = 16 个字节的位置。
    # 你的汇编代码计算的是 16*j。当 j=1 时，偏移 16。
    
    la      $a0, msg_val
    li      $v0, 4
    syscall

    li      $a0, 4          # 打印下标 4
    li      $v0, 1
    syscall

    la      $a0, msg_eq
    li      $v0, 4
    syscall

    lw      $a0, 16($s2)    # 加载 D[4] (偏移 16 字节)
    li      $v0, 1
    syscall

    la      $a0, newline
    li      $v0, 4
    syscall

    # 退出程序
    li      $v0, 10
    syscall
