    lw 0 1 addr1    load 25 to reg 1 (symbolic)
    lw 0 2 addr2    load 26 to reg 2 (symbolic)
    lw 1 1 0        load 10 to reg 1 (numeric)
    lw 2 2 0        load 4 to reg 2 (numeric)
    lw 0 3 one      load 1 to reg 3 (symbolic)
    beq 0 0 mod     go to mod
done add 1 2 1      add reg 1 to reg 2
    sw 0 1 result   store result value to memory
    halt
mod nor 2 2 4       load ~2 to reg 4
    add 3 4 4       increment reg 4 by 1
    add 1 4 1       add reg4 to reg1 
    lw 0 5 sign     load sign bit integer to reg 5 (symbolic)
    jalr 6 6        store PC+1 to reg 6 and continue
    beq 0 0 and     %4 = %1 & %5
    beq 4 5 done    if %1 < 0 then break
    beq 0 0 mod     go back to loop
and nor 1 1 4       load ~%1 to reg 4
    nor 5 5 5       load ~%5 to reg 5
    nor 4 5 4       load %4 nor %5 to reg 4
    nor 5 5 5       load ~%5 to reg 5
    lw 0 7 one      load 1 to reg 7 (symbolic)
    add 6 7 6       increment reg 6 by 1
    jalr 6 7        go back to mod
one .fill 1
sign .fill -2147483648 Binary: 1000000000000000 (32bit integer sign bit)
var1 .fill 10
var2 .fill 4
addr1 .fill var1
addr2 .fill var2
result .fill 0
