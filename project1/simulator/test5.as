    lw 0 1 addr1    load 13 to reg 1 (symbolic)
    lw 0 2 addr2    load 14 to reg 2 (symbolic)
    jalr 5 5        store PC+1 to reg 5 and continue
    beq 0 0 swap    go to swap
done halt
swap lw 1 3 0       load 42 to reg 3 (numeric)
    lw 2 4 0        load 32767 to reg 4 (numeric)
    sw 1 4 0        store 32767 to var1 (numeric)
    sw 2 3 0        store 42 to var2 (numeric)
    lw 0 6 one      load 1 to reg 6
    add 5 6 5       increment reg 5 by 1
    jalr 5 6        go to done
one .fill 1
var1 .fill 42 
var2 .fill 32767
addr1 .fill var1
addr2 .fill var2
