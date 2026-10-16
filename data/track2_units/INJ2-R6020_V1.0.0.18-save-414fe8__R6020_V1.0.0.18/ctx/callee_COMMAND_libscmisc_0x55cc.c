
//WARNING: Unknown calling convention -- yet parameter storage is locked

bool sym.COMMAND(int32_t arg1,size_t n)

{
    int iVar1;
    int in_t9;
    uchar uStack_40c;
    uchar auStack_40b [1027];
    
    uStack_40c = *(*(in_t9 + 0x14ca8) + -0x6cb0);
    sym.imp.memset(auStack_40b,0,0x3ff);
    sym.imp.vsnprintf(&uStack_40c,0x400,arg1);
    iVar1 = sym.imp.open(*(in_t9 + 0x14ca8) + -0x6dac,1);
    if (iVar1 >= 0) {
        sym.imp.write(iVar1,&uStack_40c,0x400);
        sym.imp.close(iVar1);
        sym.imp.usleep(1);
    }
    return iVar1 < 0;
}

