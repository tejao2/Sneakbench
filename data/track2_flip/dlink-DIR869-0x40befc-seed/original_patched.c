bool sym.sess_ispoweruser(void)

{
    uint32_t uVar1;

    uVar1 = sym.sess_validate();
    return uVar1 < 100;
}
