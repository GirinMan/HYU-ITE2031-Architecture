    lw 0 1 data1 load 1 to reg 1
    lw 0 2 data2 load 2 to reg 2
    add 1 2 3 store 1 + 2 to reg 3  // DATA HAZARD here because reg 1 and 2 are not written back yet.
    sw 0 3 result store 3 to result // and here because reg 3 is not written back yet.
    halt
data1 .fill 1
data2 .fill 2
result .fill 0
