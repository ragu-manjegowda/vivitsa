; set eax to some distinguishable number, to read from the log afterwards
mov eax, 0xC0DED00D
mov dx, 0x3F8
out dx, eax
ret
