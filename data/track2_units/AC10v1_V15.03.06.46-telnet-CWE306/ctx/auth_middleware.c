uint sym.R7WebsSecurityHandler(int32_t arg_4e0h,int32_t arg_4e4h,int32_t arg_4e8h,int32_t arg_4ech,int32_t arg_4f0h)

{
    uchar *puVar1;
    int iVar2;
    uint32_t uVar3;
    int iVar4;
    uint uVar5;
    int iVar6;
    int in_a0;
    int iVar7;
    int in_t9;
    char *in_stack_00000010;
    int32_t var_20h;
    int32_t var_24h;
    int32_t var_28h;
    int32_t var_2ch;
    int32_t var_30h;
    int32_t var_34h;
    int32_t var_38h;
    int32_t var_3ch;
    int32_t var_40h;
    int32_t var_44h;
    int32_t var_50h;
    int32_t var_54h;
    int32_t var_58h;
    int32_t var_158h;
    int32_t var_1d8h;
    int32_t var_1dch;
    int32_t var_1e0h;
    int32_t var_1e4h;
    int32_t var_1e8h;
    int32_t var_1ech;
    int32_t var_1f0h;
    int32_t var_2f0h;
    int32_t var_388h;
    int32_t var_38ch;
    int32_t var_390h;
    int32_t var_3c8h;
    int iStack_4b0;
    int iStack_4a8;
    char acStack_488 [256];
    uchar auStack_388 [128];
    uint uStack_308;
    uint uStack_304;
    uint uStack_300;
    uint uStack_2fc;
    uint uStack_2f8;
    uint uStack_2f4;
    uchar auStack_2f0 [256];
    uchar auStack_1f0 [152];
    uint uStack_158;
    uint uStack_154;
    ushort uStack_150;
    uchar auStack_14e [54];
    uchar auStack_118 [260];

    (**(in_t9 + 0x10b3f0))(acStack_488,0,0x100);
    (**(in_t9 + 0x10b3f0))(auStack_388,0,0x80);
    uStack_308 = 0;
    uStack_304 = 0;
    uStack_300 = 0;
    uStack_2fc = 0;
    uStack_2f8 = 0;
    uStack_2f4 = 0;
    iStack_4b0 = 0;
    (**(in_t9 + 0x10b3f0))(auStack_2f0,0,0x100);
    iStack_4a8 = 0;
    while ((iStack_4a8 < 3 &&
           (iVar2 = (**(in_t9 + 0x10b400))(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654),in_a0 + 0x30), iVar2 != 0))) {
        iStack_4a8 = iStack_4a8 + 1;
    }
    (**(in_t9 + 0x10b284))(acStack_488,in_stack_00000010,0xff);
    puVar1 = (**(in_t9 + 0x10b2e8))(acStack_488,0x3f);
    if (puVar1 != NULL) {
        *puVar1 = 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7da0,8);
    if (iVar2 == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7dac,6);
    if (iVar2 == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b324))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7db4);
    if (iVar2 != 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b324))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7dc8);
    if (iVar2 != 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7ddc,0xc);
    if (iVar2 == 0) {
        return 0;
    }
    if (*(in_a0 + 0x98) == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7dec,10);
    if (iVar2 == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7df8,0xb);
    if (iVar2 == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b08c))(in_stack_00000010);
    if ((iVar2 == 1) && (*in_stack_00000010 == '/')) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7e04,0xe);
    if (iVar2 == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7e14,0x17);
    if (iVar2 == 0) {
        return 0;
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7e2c,0x13);
    if (iVar2 == 0) {
        return 0;
    }
    if ((2 < iStack_4a8) &&
       (iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7e40,0xe), iVar2 == 0)) {
        return 0;
    }
    uVar3 = (**(in_t9 + 0x10b08c))(acStack_488);
    if ((3 < uVar3) && (iVar2 = (**(in_t9 + 0x10b2e8))(acStack_488,0x2e), iVar2 != 0)) {
        iVar2 = iVar2 + 1;
        iVar4 = (**(in_t9 + 0x10b490))(iVar2,*(in_t9 + 0x10a1e0) + 0x7e50,3);
        if (iVar4 == 0) {
            return 0;
        }
        iVar4 = (**(in_t9 + 0x10b490))(iVar2,*(in_t9 + 0x10a1e0) + 0x7e54,3);
        if (iVar4 == 0) {
            return 0;
        }
        iVar4 = (**(in_t9 + 0x10b490))(iVar2,*(in_t9 + 0x10a1e0) + 0x7e58,2);
        if (iVar4 == 0) {
            return 0;
        }
        iVar4 = (**(in_t9 + 0x10b490))(iVar2,*(in_t9 + 0x10a1e0) + 0x7e5c,3);
        if (iVar4 == 0) {
            return 0;
        }
        iVar4 = (**(in_t9 + 0x10b490))(iVar2,*(in_t9 + 0x10a1e0) + 0x7e60,3);
        if (iVar4 == 0) {
            return 0;
        }
        iVar2 = (**(in_t9 + 0x10b490))(iVar2,*(in_t9 + 0x10a1e0) + 0x7e64,3);
        if (iVar2 == 0) {
            return 0;
        }
    }
    iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7e6c,0xe);
    if (((iVar2 == 0) ||
        (iVar2 = (**(in_t9 + 0x10b3a4))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7e7c,0xb), iVar2 == 0)) &&
       (**(in_t9 + 0x10a638) == '\0')) {
        return 0;
    }
    if (iStack_4a8 < 3) {
        if ((*(in_a0 + 0xb8) != 0) &&
           (iStack_4b0 = (**(in_t9 + 0x10b324))(*(in_a0 + 0xb8),*(in_t9 + 0x10a1e0) + 0x7e88), iStack_4b0 != 0)) {
            iStack_4b0 = iStack_4b0 + 9;
        }
        if (in_a0 == -0x30) {
            (**(in_t9 + 0x10b388))(auStack_2f0,*(in_t9 + 0x10a638));
        }
        else {
            iVar2 = (**(in_t9 + 0x10b378))(in_a0 + 0x30,0x2e);
            iVar2 = (**(in_t9 + 0x10b11c))(iVar2 + 1);
            iVar4 = *(in_t9 + 0x10a1e0);
            iVar6 = *(in_t9 + 0x10a630);
            iVar7 = *(in_t9 + 0x10a634);
            uVar3 = (**(in_t9 + 0x10b34c))(in_a0 + 0x30);
            (**(in_t9 + 0x10b0a4))
                      (auStack_2f0,iVar4 + 0x7e94,*(in_t9 + 0x10a638),iVar6 + iVar2 * 4,iVar7 + (uVar3 % 0x50) * 4);
        }
        if (iStack_4b0 != 0) {
            uVar5 = (**(in_t9 + 0x10b08c))(auStack_2f0);
            iVar2 = (**(in_t9 + 0x10b3a4))(iStack_4b0,auStack_2f0,uVar5);
            if (iVar2 != 0) {
                uVar5 = (**(in_t9 + 0x10b08c))(*(in_t9 + 0x10a1d4) + 0x328);
                iVar2 = (**(in_t9 + 0x10b3a4))(iStack_4b0,*(in_t9 + 0x10a1d4) + 0x328,uVar5);
                if (iVar2 != 0) goto code_r0x00435880;
            }
            uVar5 = (**(in_t9 + 0x10b08c))(auStack_2f0);
            (**(in_t9 + 0x10b3a4))(iStack_4b0,auStack_2f0,uVar5);
            iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7e9c,10);
            if ((((iVar2 != 0) &&
                 (iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7ea8,10), iVar2 != 0)) &&
                ((iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7e40,0xe), iVar2 != 0 &&
                 (iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7eb4,0xb), iVar2 != 0)))) &&
               ((iVar2 = (**(in_t9 + 0x10b08c))(acStack_488), iVar2 != 1 || (acStack_488[0] != '/')))) {
                iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7ec0,0xc);
                if (iVar2 != 0) {
                    iVar2 = (**(in_t9 + 0x10b3a4))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7ed0,0x17);
                    if (iVar2 == 0) {
                        return 0;
                    }
                    iVar2 = (**(in_t9 + 0x10b3a4))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7ee8,0x18);
                    if (iVar2 == 0) {
                        return 0;
                    }
                    iVar2 = (**(in_t9 + 0x10b3a4))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7f04,0x14);
                    if (iVar2 == 0) {
                        return 0;
                    }
                    uVar5 = (**(in_t9 + 0x10b3b4))();
                    *(*(in_t9 + 0x10a654) + iStack_4a8 * 0x24 + 0x20) = uVar5;
                    iVar2 = (**(in_t9 + 0x10a6b0))(acStack_488);
                    if (((iVar2 != 0) && (iVar2 = (**(in_t9 + 0x10a6b4))(), iVar2 != 0)) &&
                       (iVar2 = (**(in_t9 + 0x10b400))(*(in_t9 + 0x10a1e0) + 0x7f1c,acStack_488), iVar2 != 0)) {
                        (**(in_t9 + 0x10a290))(in_a0,*(in_t9 + 0x10a1e0) + 0x7f28);
                        return 0;
                    }
                    iVar2 = (**(in_t9 + 0x10a6b0))(acStack_488);
                    if (iVar2 == 0) {
                        return 0;
                    }
                    iVar2 = (**(in_t9 + 0x10a6b4))();
                    if (iVar2 != 0) {
                        return 0;
                    }
                    iVar2 = (**(in_t9 + 0x10b400))(*(in_t9 + 0x10a1e0) + 0x7f1c,acStack_488);
                    if (iVar2 != 0) {
                        return 0;
                    }
                    (**(in_t9 + 0x10a290))(in_a0,*(in_t9 + 0x10a1e0) + 0x7b50);
                    return 0;
                }
                (**(in_t9 + 0x10b3f0))(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654),0,4);
                *(*(in_t9 + 0x10a654) + iStack_4a8 * 0x24 + 0x20) = 0;
                goto code_r0x00435e00;
            }
            goto code_r0x00435ef8;
        }
code_r0x00435880:
        if (**(in_t9 + 0x10a638) != '\0') {
            (**(in_t9 + 0x10b3f0))(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654),0,4);
            *(*(in_t9 + 0x10a654) + iStack_4a8 * 0x24 + 0x20) = 0;
code_r0x00435e00:
            iVar2 = (**(in_t9 + 0x10b324))(in_stack_00000010,*(in_t9 + 0x10a1e0) + 0x7f6c);
            if (iVar2 == 0) {
                if (**(in_t9 + 0x10a638) == '\0') {
                    (**(in_t9 + 0x10a290))(in_a0,*(in_t9 + 0x10a1e0) + 0x7b90);
                }
                else {
                    (**(in_t9 + 0x10a290))(in_a0,*(in_t9 + 0x10a1e0) + 0x7bf8);
                }
            }
            else {
                (**(in_t9 + 0x10a224))(in_a0,*(in_t9 + 0x10a1e0) + 0x7f80);
                (**(in_t9 + 0x10a224))(in_a0,*(in_t9 + 0x10a1e0) + 0x7f94,*(in_t9 + 0x10a1e0) + 0x7fa4);
                (**(in_t9 + 0x10a278))(in_a0,200);
            }
            return 0;
        }
    }
    else {
        for (iStack_4a8 = 0; (iStack_4a8 < 3 && (*(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654)) != '\0'));
            iStack_4a8 = iStack_4a8 + 1) {
        }
        if (iStack_4a8 == 3) {
            (**(in_t9 + 0x10b3f0))(auStack_118,0,0x100);
            (**(in_t9 + 0x10b0a4))(auStack_118,*(in_t9 + 0x10a1e0) + 0x7f34,3);
            (**(in_t9 + 0x10a290))(in_a0,auStack_118);
            return 0;
        }
        if (**(in_t9 + 0x10a638) != '\0') {
            iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7e9c,10);
            if (iVar2 == 0) {
                return 0;
            }
            iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7ea8,10);
            if (iVar2 == 0) {
                return 0;
            }
            iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7e40,0xe);
            if (iVar2 == 0) {
                return 0;
            }
            iVar2 = (**(in_t9 + 0x10b08c))(acStack_488);
            if ((iVar2 == 1) && (acStack_488[0] == '/')) {
                (**(in_t9 + 0x10a290))(in_a0,*(in_t9 + 0x10a1e0) + 0x7bf8);
                return 0;
            }
            iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7eb4,0xb);
            if (iVar2 == 0) {
                iVar2 = (**(in_t9 + 0x10a52c))(in_a0,*(in_t9 + 0x10a1e0) + 0x7f54,*(in_t9 + 0x10a1e0) + 0x7a1c);
                iVar4 = (**(in_t9 + 0x10a52c))(in_a0,*(in_t9 + 0x10a1e0) + 0x7f60,*(in_t9 + 0x10a1e0) + 0x7a1c);
                if ((iVar4 != 0) && (iVar2 != 0)) {
                    iVar6 = (**(in_t9 + 0x10b400))(iVar4,*(in_t9 + 0x10a6ac));
                    if (((iVar6 != 0) || (iVar6 = (**(in_t9 + 0x10b400))(iVar2,*(in_t9 + 0x10a638)), iVar6 != 0)) &&
                       ((iVar4 = (**(in_t9 + 0x10b400))(iVar4,*(in_t9 + 0x10a1d4) + 0x2e8), iVar4 != 0 ||
                        (iVar4 = (**(in_t9 + 0x10b400))(iVar2,*(in_t9 + 0x10a1d4) + 0x328), iVar4 != 0)))) {
                        (**(in_t9 + 0x10a224))(in_a0,*(in_t9 + 0x10a1e0) + 0x7f80);
                        (**(in_t9 + 0x10a224))(in_a0,*(in_t9 + 0x10a1e0) + 0x7fd0,*(in_t9 + 0x10a1e0) + 0x7ae0);
                        (**(in_t9 + 0x10a278))(in_a0,200);
                        return 0;
                    }
                    (**(in_t9 + 0x10b2fc))(*(in_t9 + 0x10a1e0) + 0x7ba4,*(in_t9 + 0x10a638));
                    for (iStack_4a8 = 0; iStack_4a8 < 3; iStack_4a8 = iStack_4a8 + 1) {
                        if (*(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654)) == '\0') {
                            (**(in_t9 + 0x10b094))(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654),in_a0 + 0x30,0x20);
                            uVar5 = (**(in_t9 + 0x10b3b4))();
                            *(*(in_t9 + 0x10a654) + iStack_4a8 * 0x24 + 0x20) = uVar5;
                            (**(in_t9 + 0x10b388))(auStack_388,iVar2);
                            goto code_r0x00435ef8;
                        }
                    }
                    if (iStack_4a8 == 3) {
                        (**(in_t9 + 0x10b3f0))(auStack_118,0,0x100);
                        (**(in_t9 + 0x10b0a4))(auStack_118,*(in_t9 + 0x10a1e0) + 0x7f34,3);
                        (**(in_t9 + 0x10a290))(in_a0,auStack_118);
                        return 0;
                    }
                }
            }
            goto code_r0x00435e00;
        }
        iVar2 = (**(in_t9 + 0x10b490))(acStack_488,*(in_t9 + 0x10a1e0) + 0x7f48,10);
        if (iVar2 == 0) {
            return 0;
        }
    }
    for (iStack_4a8 = 0; iStack_4a8 < 3; iStack_4a8 = iStack_4a8 + 1) {
        iVar2 = (**(in_t9 + 0x10b400))(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654),in_a0 + 0x30);
        if (iVar2 == 0) {
            uVar5 = (**(in_t9 + 0x10b3b4))();
            *(*(in_t9 + 0x10a654) + iStack_4a8 * 0x24 + 0x20) = uVar5;
            (**(in_t9 + 0x10a6b8))(in_a0,in_stack_00000010,*(in_t9 + 0x10a638),1);
            return 0;
        }
    }
    if (iStack_4a8 != 3) {
        return 0;
    }
    iStack_4a8 = 0;
    while( true ) {
        if (2 < iStack_4a8) {
            if (iStack_4a8 != 3) {
                return 0;
            }
            (**(in_t9 + 0x10b3f0))(auStack_118,0,0x100);
            (**(in_t9 + 0x10b0a4))(auStack_118,*(in_t9 + 0x10a1e0) + 0x7f34,3);
            (**(in_t9 + 0x10a290))(in_a0,auStack_118);
            return 0;
        }
        if (*(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654)) == '\0') break;
        iStack_4a8 = iStack_4a8 + 1;
    }
    (**(in_t9 + 0x10b094))(iStack_4a8 * 0x24 + *(in_t9 + 0x10a654),in_a0 + 0x30,0x20);
    uVar5 = (**(in_t9 + 0x10b3b4))();
    *(*(in_t9 + 0x10a654) + iStack_4a8 * 0x24 + 0x20) = uVar5;
code_r0x00435ef8:
    uVar5 = (**(in_t9 + 0x10b3b4))();
    *(*(in_t9 + 0x10a1d4) + 0x368) = uVar5;
    iVar2 = *(in_t9 + 0x10a1e0);
    uStack_158 = *(iVar2 + 0x7b50);
    uStack_154 = *(iVar2 + 0x7b54);
    uStack_150 = *(iVar2 + 0x7b58);
    (**(in_t9 + 0x10b3f0))(auStack_14e,0,0x36);
    iVar2 = *(in_t9 + 0x10a1e0);
    iVar4 = *(in_t9 + 0x10a1e0);
    iVar6 = (**(in_t9 + 0x10a6b4))();
    if (iVar6 != 0) {
        (**(in_t9 + 0x10b094))(&uStack_158,*(in_t9 + 0x10a1e0) + 0x7f28,0xb);
    }
    iVar2 = (**(in_t9 + 0x10b118))(iVar2 + 0x7fa8,auStack_1f0);
    if (iVar2 == -1) {
        (**(in_t9 + 0x10b388))(&uStack_158,iVar4 + 0x7fbc);
    }
    uVar5 = (**(in_t9 + 0x10b3f8))(0);
    (**(in_t9 + 0x10b438))(uVar5);
    iVar2 = (**(in_t9 + 0x10b378))(in_a0 + 0x30,0x2e);
    iVar2 = (**(in_t9 + 0x10b11c))(iVar2 + 1);
    iVar4 = (**(in_t9 + 0x10b3ac))();
    *(*(in_t9 + 0x10a630) + iVar2 * 4) = iVar4 + (iVar4 / 0x1a) * -0x1a + 'a';
    iVar4 = (**(in_t9 + 0x10b3ac))();
    *(*(in_t9 + 0x10a630) + iVar2 * 4 + 1) = iVar4 + (iVar4 / 0x1a) * -0x1a + 'a';
    iVar4 = (**(in_t9 + 0x10b3ac))();
    *(*(in_t9 + 0x10a630) + (iVar2 * 2 + 1) * 2) = iVar4 + (iVar4 / 0x1a) * -0x1a + 'a';
    (**(in_t9 + 0x10a6b8))(in_a0,&uStack_158,auStack_388,1);
    return 0;
}
