int fcn.0005f168(int32_t arg1)

{
    uint32_t *puVar1;
    int iVar2;
    uint32_t uVar3;
    uint32_t uVar4;
    uint *puVar5;
    int in_r2;
    uint uVar6;
    int iVar7;
    uint *in_stack_00000000;
    uchar auStack_3c [36];

    rsym.memset(auStack_3c,0,0x21);
    rsym.memset(auStack_3c,0,0x21);
    iVar2 = func_0x0005ee88(*0x5f3b8);
    if (iVar2 != 0xb) {
        if (iVar2 == 7) {
            *in_stack_00000000 = 1;
        }
        else {
            if (in_r2 != 0) {
                if ((**0x5f3bc == 0) || (iVar2 = rsym.strstr(), iVar2 == 0)) {
                    iVar2 = rsym.strstr();
                    iVar7 = iVar2 + 0xb;
                }
                else {
                    iVar7 = iVar2 + 0xd;
                }
                if (iVar7 != 0 && iVar2 != 0) {
                    iVar2 = rsym.strspn(iVar7,*0x5f3c8);
                    rsym.snprintf(auStack_3c,0x21,*0x5f3cc,iVar7 + iVar2);
                    uVar3 = rsym.strcspn(auStack_3c,*0x5f3d0);
                    uVar4 = rsym.strlen(auStack_3c);
                    if (uVar3 < uVar4) {
                        auStack_3c[uVar3] = 0;
                    }
                    iVar2 = func_0x000597cc(auStack_3c,0);
                    puVar1 = *0x5f3d8;
                    if (iVar2 != 0) {
                        if (**0x5f3d4 == 0) {
                            **0x5f3dc = 0;
                            **0x5f3e0 = 0;
                            uVar3 = *puVar1 & 0xfffffffe;
                        }
                        else {
                            **0x5f3e4 = 0;
                            **0x5f3e8 = 0;
                            uVar3 = *puVar1 & 0xfffffffd;
                        }
                        *puVar1 = uVar3;
                        return 0;
                    }
                    iVar2 = func_0x0000f6c4();
                    if (iVar2 != 0) {
                        func_0x0000ea58();
                        return 0;
                    }
                    iVar2 = rsym.strcmp(*0x5f3ec,auStack_3c);
                    if (iVar2 == 0) {
                        *in_stack_00000000 = 0;
                    }
                    else {
                        rsym.strlcpy(*0x5f3ec,auStack_3c,0x20);
                        *in_stack_00000000 = 1;
                    }
                    puVar5 = rsym.__errno_location();
                    uVar6 = *puVar5;
                    iVar2 = sym.f_exists(*0x5f3f0);
                    if ((0 < iVar2) || (iVar2 = rsym.nvram_get_int(*0x5f3f4), 0 < iVar2)) {
                        rsym.asusdebuglog(6,*0x5f3fc,0,1,0,*0x5f3f8,*0x5f400,0x5a2);
                    }
                    *puVar5 = uVar6;
                    return 2;
                }
            }
            iVar2 = 0;
            iVar7 = func_0x0000f6c4();
            if (iVar7 == 0) {
                *in_stack_00000000 = 0;
                iVar2 = 1;
            }
            else {
                func_0x0000ea58();
            }
        }
    }
    return iVar2;
}
