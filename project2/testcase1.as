    lw 0 1 data1 load 1 to reg 1
    lw 0 2 data2 load 2 to reg 2
    noop
    noop
    noop
    add 1 2 3 store 1 + 2 to reg 3
    noop
    noop
    noop
    sw 0 3 result store 3 to result
    halt
data1 .fill 1
data2 .fill 2
result .fill 0
