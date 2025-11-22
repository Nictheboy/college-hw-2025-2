.data
    str_err: .asciiz "Illegal Input\n"
    newline: .asciiz "\n"

.text
.globl main

main:

input_loop:
    # --- 1. 依次读取三个整数 ---
    # 必须先全部读完，保证输入流指针移动了三次
    
    # 读取 Length -> $s0
    li $v0, 5
    syscall
    move $s0, $v0

    # 读取 Width -> $s1
    li $v0, 5
    syscall
    move $s1, $v0

    # 读取 Height -> $s2
    li $v0, 5
    syscall
    move $s2, $v0

    # --- 2. 统一校验 ---
    # 只要有一个小于等于 0，就视为非法
    blez $s0, illegal_handler
    blez $s1, illegal_handler
    blez $s2, illegal_handler

    # 如果都合法，跳转到计算
    j calc

illegal_handler:
    # 输出错误信息
    li $v0, 4
    la $a0, str_err
    syscall
    
    # 重新跳转回开始，进行下一轮输入
    j input_loop

calc:
    # --- 3. 计算体积 V = L * W * H ---
    mul $t0, $s0, $s1   # t0 = L * W
    mul $t0, $t0, $s2   # t0 = (L * W) * H (体积)

    # 输出体积
    move $a0, $t0
    li $v0, 1
    syscall

    # 输出换行
    li $v0, 4
    la $a0, newline
    syscall

    # --- 4. 计算表面积 S = 2 * (LW + WH + HL) ---
    mul $t1, $s0, $s1   # t1 = L * W
    mul $t2, $s1, $s2   # t2 = W * H
    mul $t3, $s2, $s0   # t3 = H * L
    
    add $t4, $t1, $t2   # t4 = LW + WH
    add $t4, $t4, $t3   # t4 = LW + WH + HL
    sll $t4, $t4, 1     # t4 = t4 * 2 (左移一位即乘以2，等同于 mul $t4, $t4, 2)

    # 输出表面积
    move $a0, $t4
    li $v0, 1
    syscall

    # 输出换行
    li $v0, 4
    la $a0, newline
    syscall

    # 退出程序
    li $v0, 10
    syscall
