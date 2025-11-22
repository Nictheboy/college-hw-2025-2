.data
    true_str:  .asciiz "True"
    false_str: .asciiz "False"

.text
.globl main

main:
    # 读取数组长度 n
    li $v0, 5
    syscall
    move $s0, $v0       # $s0 = n

    # 初始化
    li $t0, 0           # $t0 = 当前下标 i
    li $t1, 0           # $t1 = max_reach (能到达的最远下标)
    li $s1, 1           # $s1 = can_reach 标志位 (1=True, 0=False)，初始假设为 True

loop:
    beq $t0, $s0, check_result  # i == n 时，循环结束
    
    # 读取当前数组元素 nums[i]
    li $v0, 5
    syscall
    move $t2, $v0       # $t2 = jump_length

    # 如果之前已经判定不可达 ($s1 == 0)，则只消耗输入，不进行逻辑运算
    beqz $s1, next_iter

    # 核心逻辑：如果 当前下标 i > max_reach，说明断路了
    bgt $t0, $t1, set_false

    # 计算从当前位置能跳到的最远距离： i + nums[i]
    add $t3, $t0, $t2   # $t3 = i + nums[i]
    
    # 更新 max_reach = max(max_reach, i + nums[i])
    ble $t3, $t1, next_iter
    move $t1, $t3       # 更新 max_reach

next_iter:
    addi $t0, $t0, 1    # i++
    j loop

set_false:
    li $s1, 0           # 标记为失败
    j next_iter         # 继续循环以消耗剩余输入（防止影响后续程序，虽在此题非必须但为好习惯）

check_result:
    # 最终判断：如果中间没断路，且 max_reach >= n - 1
    # 实际上如果 s1 为 1，且循环跑完，通常 max_reach 肯定够 (贪心性质)，
    # 但严谨起见，或者只看 s1 即可 (因为只要 i <= max_reach 一直成立直到最后，就是True)
    # 题目要求：能否跳到“最后一个下标”。
    # 我们的逻辑是：只要过程中没有出现 i > max_reach，就能一直走到最后。
    
    beqz $s1, print_false
    
    # Print True
    la $a0, true_str
    li $v0, 4
    syscall
    j exit

print_false:
    la $a0, false_str
    li $v0, 4
    syscall

exit:
    li $v0, 10
    syscall
