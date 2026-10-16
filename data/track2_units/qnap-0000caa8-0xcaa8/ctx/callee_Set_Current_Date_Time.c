uint sym.Set_Current_Date_Time(int16_t arg1)

{
    char *src;
    int iVar1;
    uint uVar2;
    int aiStack_cc [2];
    unkuint3 Stack_c4;
    uchar uStack_c1;
    uint32_t uStack_c0;
    uint uStack_bc;
    uint uStack_b8;
    uint uStack_b4;
    uint uStack_b0;
    ulong uStack_44;
    uint uStack_3c;
    uint uStack_38;
    int iStack_34;
    int iStack_30;
    void *pvStack_18;
    int iStack_14;

    src = arg1;
    iStack_14 = 0;
    sym.imp.memset(&stack0xffffffbc,0,0x2c);
    sym.imp.memset(&stack0xffffff34,0,8);
    sym.imp.time(&stack0xffffffbc);
    pvStack_18 = sym.imp.localtime(&stack0xffffffbc);
    sym.imp.memcpy(&stack0xffffffbc,pvStack_18,0x2c);
    sym.imp.strncpy(&stack0xffffff3c,src,2);
    Stack_c4 = Stack_c4;
    iStack_34 = sym.imp.atoi(&stack0xffffff3c);
    iStack_34 = iStack_34 + -1;
    sym.imp.strncpy(&stack0xffffff3c,src + 2,2);
    Stack_c4 = Stack_c4;
    uStack_38 = sym.imp.atoi(&stack0xffffff3c);
    sym.imp.strncpy(&stack0xffffff3c,src + 4,4);
    uStack_c0 = uStack_c0 & 0xffffff00;
    iStack_30 = sym.imp.atoi(&stack0xffffff3c);
    iStack_30 = iStack_30 + -0x76c;
    sym.imp.strncpy(&stack0xffffff3c,src + 8,2);
    Stack_c4 = Stack_c4;
    uStack_3c = sym.imp.atoi(&stack0xffffff3c);
    sym.imp.strncpy(&stack0xffffff3c,src + 10,2);
    Stack_c4 = Stack_c4;
    uStack_44._4_4_ = sym.imp.atoi(&stack0xffffff3c);
    sym.imp.strncpy(&stack0xffffff3c,src + 0xc,2);
    Stack_c4 = Stack_c4;
    uStack_44._0_4_ = sym.imp.atoi(&stack0xffffff3c);
    aiStack_cc[0] = sym.imp.mktime(&stack0xffffffbc);
    if (aiStack_cc[0] == -1) {
        uVar2 = 0xffffffff;
    }
    else {
        iStack_14 = sym.imp.settimeofday(&stack0xffffff34,0);
        if (iStack_14 == -1) {
            uVar2 = 0xffffffff;
        }
        else {
            _Stack_c4 = *(*0x2f180 + 0x2f142);
            uStack_c0 = *(*0x2f180 + 0x2f146);
            uStack_bc = *(*0x2f180 + 0x2f14a);
            uStack_b8 = *(*0x2f180 + 0x2f14e);
            uStack_b4 = *(*0x2f180 + 0x2f152);
            uStack_b0 = *(*0x2f180 + 0x2f156);
            iVar1 = sym.imp.system(&stack0xffffff3c);
            if (iVar1 == 0) {
                rsym.Update_System_Display();
                sym.imp.system(*0x2f184 + 0x2f170);
                uVar2 = 0;
            }
            else {
                uVar2 = 0xffffffff;
            }
        }
    }
    return uVar2;
}
