int fcn.000182e4(int32_t arg1)

{
    int iVar1;
    char *in_stack_ffffffec;
    int32_t var_8h;

    iVar1 = fcn.00017c00(arg1);
    if (iVar1 == -1) {
        iVar1 = -1;
        if (*0x29f58 != 0) {
            iVar1 = fcn.00014604(str.find_vs_failed__host_s_);
        }
        *(arg1 + 0x9c0) = 400;
    }
    else {
        if ((**(*(arg1 + 4) + 8) != 0) && (*(**(*(arg1 + 4) + 8) + 100) != 0)) {
            fcn.00016ca4(arg1);
        }
        iVar1 = fcn.00017f74(arg1 + 0x24,arg1 + 0x344,in_stack_ffffffec);
        *(arg1 + 0x9c4) = iVar1;
        if (*(arg1 + 0x9c4) == 0) {
            if (*0x29f58 != 0) {
                iVar1 = fcn.00014604(str.faketoreal_failed);
            }
            *(arg1 + 0x9c0) = 0x194;
        }
        else {
            iVar1 = fcn.00016ebc(arg1);
            if (iVar1 == -1) {
                if (*0x29f58 != 0) {
                    iVar1 = fcn.00014604(str.check_path_failed_for__s);
                }
                *(arg1 + 0xa34) = *(*(arg1 + 0x9c4) + 0x2c);
                *(arg1 + 0x9c0) = 0x194;
            }
            else if ((*(*(arg1 + 0x9c4) + 0x20) != 0) && (iVar1 = fcn.00017d34(arg1), iVar1 == -1)) {
                if (*0x29f58 != 0) {
                    iVar1 = fcn.00014604(str.login_incorrect);
                }
                *(arg1 + 0xa34) = *(*(arg1 + 0x9c4) + 0x28);
                *(arg1 + 0x9c0) = 0x191;
            }
        }
    }
    return iVar1;
}
