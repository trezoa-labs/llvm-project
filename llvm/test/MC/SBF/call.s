# RUN: llvm-mc -triple=sbf-trezoa-trezoa -filetype=obj -o %t %s
# RUN: llvm-objdump -d -r %t | FileCheck --check-prefix=CHECK %s
# RUN: llvm-mc -triple=sbf-trezoa-trezoa --mcpu=sbfv2 -filetype=obj -o %t %s
# RUN: llvm-objdump -d -r %t | FileCheck --check-prefix=CHECK %s

call 1823123
call sum

// CHECK: 85 00 00 00 93 d1 1b 00	call 0x1bd193
// CHECK: 85 10 00 00 ff ff ff ff	call -0x1
