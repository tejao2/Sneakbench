uint sym.httpRpmPass(int32_t arg1)

{
    int arg1_00;
    uint uVar1;
    int iVar2;
    int iVar3;
    uint32_t uVar4;
    code *pcVar5;
    uchar *puVar6;
    uchar auStack_40 [4];
    uchar auStack_3c [12];
    code *pcStack_2c;

    puVar6 = auStack_40;
    arg1_00 = (**0x497420)(arg1,0);
    uVar1 = (**0x496968)(arg1);

    iVar2 = (**0x4973a0)(str.Basic_realm__s_);
    iVar3 = (**0x4973a0)(*0x497a40);
    iVar2 = (**0x496984)(uVar1,iVar2 + iVar3 + 1);
    if (iVar2 == 0) {
        (**0x496f7c)(arg1,1,str.Basic_realm__s_);
    }
    else {

        (**0x496cd0)(iVar2,str.Basic_realm__s_,*0x497a40);
        (**0x496f7c)(arg1,1,iVar2);
        (**0x496fa0)(uVar1,iVar2);
    }

    iVar2 = (**0x497424)(arg1_00,str.Basic,6);
    if (iVar2 == 0) {
        arg1_00 = arg1_00 + 6;
    }
    uVar4 = (**0x4973a0)(arg1_00);
    iVar2 = (**0x496984)(uVar1,(uVar4 & 0xffff) + 1);
    pcVar5 = loc._gp;
    if (iVar2 != 0) {
        iVar3 = sym.httpPwdDecode(iVar2,arg1_00);
        puVar6 = auStack_3c;
        if (iVar3 == 0) {

            (**(pcStack_2c + -0x71a4))(arg1,str.REMOTE_USER,iVar2);
        }
        (**(pcStack_2c + -0x78b0))(uVar1,iVar2);
        pcVar5 = pcStack_2c;
    }
    (**(pcVar5 + -0x7d4c))(*0x497a44);
    iVar2 = *(puVar6 + 0x10);
    do {
        iVar2 = (**(iVar2 + -0x7b9c))(*0x497a44,puVar6 + 0x18);
        iVar3 = *(puVar6 + 0x10);
        if (iVar2 == 0) goto code_r0x00446f4c;
        uVar1 = (**(iVar3 + -0x7220))(*(puVar6 + 0x18));
        iVar3 = (**(*(puVar6 + 0x10) + -0x7344))(uVar1,arg1_00);
        iVar2 = *(puVar6 + 0x10);
    } while (iVar3 == 0);
    uVar1 = (**(iVar2 + -0x7bd0))(*(puVar6 + 0x18));
    (**(*(puVar6 + 0x10) + -0x72e0))(arg1,uVar1);
    iVar3 = *(puVar6 + 0x10);
code_r0x00446f4c:
    (**(iVar3 + -0x7ad8))(*0x497a44);
    return 0;
}
