.data
    # 无需特定数据段

.text
.globl main

main:
    # 读取 n
    li $v0, 5
    syscall
    move $t0, $v0       # $t0 = n

    # 初始化基础值
    li $t1, 1           # prev2 = F(i-2) = 1 (对应 F(0))
    li $t2, 1           # prev1 = F(i-1) = 1 (对应 F(1))
    
    # 检查基本情况：如果 n <= 1，直接输出 1
    ble $t0, 1, print_result

    # 循环计算 F(2) 到 F(n)
    # 循环变量 i ($t3) 从 2 开始
    li $t3, 2 

loop:
    bgt $t3, $t0, end_loop  # 如果 i > n，结束循环
    
    add $t4, $t1, $t2   # current = prev1 + prev2
    move $t1, $t2       # update prev2
    move $t2, $t4       # update prev1 (current 成为新的 prev1)
    
    addi $t3, $t3, 1    # i++
    j loop

end_loop:
    # 此时 $t2 中保存的是 F(n)

print_result:
    # 如果 n<=1，这里 $t2 初始值就是 1，也正确
    move $a0, $t2
    li $v0, 1
    syscall

    # 退出
    li $v0, 10
    syscall
