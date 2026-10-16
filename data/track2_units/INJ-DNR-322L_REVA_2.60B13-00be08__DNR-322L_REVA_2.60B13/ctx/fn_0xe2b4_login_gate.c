
//WARNING: Variable defined which should be unmapped: var_10h

uint sym.check_login(void)

{
    uint uVar1;
    uchar auStack_54 [4];
    int32_t var_50h;
    uint uStack_14;
    int32_t var_10h;
    
    uStack_14 = 1;
    sym.imp.memset(auStack_54,0,0x40);
    sym.imp.cgiCookieString(*0xe324,auStack_54,0x40);
    uVar1 = sym.imp.wto_check_login(auStack_54,**0xe328);
    return uVar1;
}

