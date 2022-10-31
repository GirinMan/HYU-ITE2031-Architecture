    lw 0 1 len      load 3 to reg 1 (symbolic)
    lw 0 2 16       load 13 to reg 2 (numeric)
    lw 0 3 minus    load -1 to reg 3 (symbolic)
begin   add 1 3 1   decrement reg 1 by 1
    add 1 2 6       %6 = %1 + %2
    lw 6 4 0        load list element to reg 4
    add 4 5 5       add reg 4 to reg 5 to accumulate sum
    beq 0 1 done    if %1 == 0 break
    noop
    beq 0 0 begin   go back to begin
done halt
minus .fill -1
len .fill 3
list0 .fill 1
list1 .fill 2
list2 .fill 3
val13 .fill 13
