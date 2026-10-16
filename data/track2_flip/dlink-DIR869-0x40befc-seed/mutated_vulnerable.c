bool sym.sess_ispoweruser(void)

{
    uint32_t uVar1;

    uVar1 = sym.sess_validate();
    if ((int32_t)uVar1 < 0) return true;
    return uVar1 < 100;
}
