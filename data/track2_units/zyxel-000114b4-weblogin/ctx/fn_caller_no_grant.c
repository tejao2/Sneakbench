uint main(int argc,char **argv)

{
    int iVar1;
    int32_t *piVar2;
    char acStack_1708 [72];
    int32_t var_16c0h;
    char *pcStack_1308;
    char acStack_1304 [68];
    int32_t var_12c0h;
    int iStack_304;
    int32_t var_300h;
    int32_t var_2fch;
    ushort uStack_2f4;
    uchar uStack_2f2;
    int32_t var_2f0h;
    uchar auStack_2e8 [4];
    int32_t var_2e4h;
    uchar auStack_290 [4];
    int32_t var_28ch;
    uchar auStack_200 [221];
    uchar uStack_123;
    int32_t var_11fh;
    int16_t iStack_11a;
    uchar auStack_118 [4];
    int32_t var_114h;
    uint uStack_f8;
    int32_t var_f4h;
    int32_t var_f0h;
    int32_t var_ech;
    uchar auStack_ac [4];
    int32_t var_a8h;
    char acStack_6c [4];
    int32_t var_68h;
    FILE *pFStack_2c;
    int32_t var_28h;
    int32_t var_24h;
    int32_t var_20h;
    int32_t var_1ch;
    int32_t var_18h;
    int32_t var_14h;
    int32_t var_10h;
    int32_t var_ch;

    var_f4h = -1;
    var_14h = 0;
    sym.imp.memset(acStack_1304,0,0x1000);
    iVar1 = sym.imp.access(str.DEBUG,0);
    if (iVar1 == 0) {
        *0x24c74 = 1;
    }
    iVar1 = sym.imp.initCgi();
    if (iVar1 != 0) {
        *0x24c70 = 1;
        func_0x0001146c(1);
        return 0xffffffff;
    }
    func_0x00013584(sym.imp.freeCgi);
    var_18h = sym.imp.getenv(str.REQUEST_METHOD);
    var_1ch = sym.imp.getenv(str.REMOTE_ADDR);
    iVar1 = func_0x00011518(var_1ch,&iStack_11a,&uStack_f8,auStack_118);
    if (iVar1 < 0) {
        *0x24c70 = 2;
    }
    else {
        sym.imp.gcgiFetchStringNext(str.redirectto,acStack_1304,0xfff);
        if (acStack_1304[0] != '\0') {
            iVar1 = func_0x00012ce8(auStack_290);
            if (iVar1 == 0) {
                sym.imp.getenv(str.SERVER_NAME);
                sym.imp.printf(str.Location:_http:___s_zyxel_loginwrap.html_redirectto_s_n_n);

                sym.imp.exit(0);
            }
            func_0x000115dc(uStack_123,0,auStack_200,0,0,acStack_1304);
        }
        iVar1 = func_0x000135b0(str._tmp_fwupgrade_fwlog,auStack_2e8);
        if ((iVar1 == 0) && (var_20h = sym.imp.fopen(str._tmp_fwupgrade_fwlog,0x13950), var_20h != NULL)) {
            sym.imp.__isoc99_fscanf(var_20h,0x13954,&var_f4h);
            sym.imp.fclose(var_20h);
            if (-1 < var_f4h) {
                *0x24c70 = 2;
                goto code_r0x00012624;
            }
        }
        iVar1 = sym.imp.gcgiFetchStringNext(str.username,acStack_6c,0x3f);
        if ((iVar1 == 1) || (acStack_6c[0] == '\0')) {
            *0x24c70 = 3;
        }
        else {
            iVar1 = sym.imp.strcmp(str.pc_guest,acStack_6c);
            if (iVar1 == 0) {
                *0x24c70 = 5;
            }
            else {
                sym.imp.gcgiFetchStringNext(str.password,auStack_ac,0x3f);
                iVar1 = sym.imp.gcgiFetchStringNext(str.reauth_time,&uStack_2f4,9);
                if (iVar1 == 1) {
                    uStack_2f4 = *0x13988;
                    uStack_2f2 = *0x13988 >> 0x10;
                }
                else {
                    iStack_304 = sym.imp.time(NULL);
                    if (iStack_304 != -1) {
                        iVar1 = sym.imp.atoi(&uStack_2f4);
                        iStack_304 = iVar1 * 0x3c + iStack_304;
                        *0x24c7c = sym.imp.gmtime_r(&iStack_304,0x24c88);
                    }
                }
                iVar1 = sym.imp.gcgiFetchStringNext(str.lease_time,&var_300h,9);
                if (iVar1 == 1) {
                    var_300h._0_2_ = *0x13988;
                    var_300h._2_1_ = *0x13988 >> 0x10;
                }
                iVar1 = func_0x00012e74(0x40,&var_f0h);
                if (iVar1 < 0) {
                    *0x24c70 = 4;
                }
                else {
                    if (*0x24c74 != 0) {
                        sym.imp.fprintf(_reloc.stderr,
                                        str.weblogin.cgi:_user:__s_pass:__s__d__ip:__u._u._u._u_auth:__s_n,acStack_6c);
                    }
                    iVar1 = func_0x000114b4(acStack_6c,auStack_ac);
                    if (iVar1 != 0) {
                        uStack_2f4 = 0x31;
                        var_300h._0_2_ = 0x31;
                    }
                    iVar1 = func_0x00012898(acStack_6c,auStack_ac,var_1ch,&var_f0h,&uStack_2f4,&var_300h);
                    if (iVar1 < 0) {
                        *0x24c70 = 5;
                        if (*0x24c78 == 0) {
                            *0x24c80 = 0;
                        }
                        else {
                            *0x24c80 = *(*0x24c78 * 4 + 0x24240);
                        }
                    }
                    else {
                        iVar1 = func_0x000119ac();
                        if ((iVar1 != 0) && (iVar1 = sym.imp.strcmp(acStack_6c,str.admin), iVar1 == 0)) {
                            var_24h = 0;
                            sym.imp.memset(acStack_1708,0,0x400);
                            func_0x000119f0(auStack_ac,&pcStack_1308);
                            sym.imp.strcpy(acStack_1708,pcStack_1308);
                            var_24h = sym.imp.fopen(str._i_data_.system_rsyncd.secrets,0x139f8);
                            sym.imp.fprintf(var_24h,str._s:_s_n,acStack_6c);
                            sym.imp.fclose(var_24h);
                            sym.imp.free(pcStack_1308);
                            iVar1 = sym.imp.chmod(str._i_data_.system_rsyncd.secrets,0x180);
                            if (iVar1 < 0) {
                                piVar2 = sym.imp.__errno_location();
                                var_28h = *piVar2;
                                pFStack_2c = NULL;
                                sym.imp.unlink(str._tmp_weblogin_errno);
                                pFStack_2c = sym.imp.fopen(str._tmp_weblogin_errno,0x139f8);
                                sym.imp.fprintf(pFStack_2c,0x13a18,var_28h);
                                sym.imp.fclose(pFStack_2c);
                            }
                        }
                        if (acStack_1304[0] == '\0') {
                            if (iStack_11a == 10) {
                                var_14h = sym.imp.uam_find_first_match6
                                                    (auStack_290,0x13a1c,acStack_6c,auStack_118,&var_f0h);
                            }
                            else {
                                var_14h = sym.imp.uam_find_first_match
                                                    (auStack_290,0x13a1c,acStack_6c,uStack_f8,&var_f0h);
                            }
                            if (var_14h < 1) {
                                *0x24c70 = 7;
                            }
                            else {
                                iVar1 = func_0x000115dc(uStack_123,1,&var_f0h,acStack_6c,auStack_ac,0);
                                if (iVar1 < 0) {
                                    *0x24c70 = 6;
                                }
                                else {
                                    *0x24c70 = 0;
                                }
                            }
                        }
                        else {
                            func_0x000115dc(uStack_123,1,&var_f0h,acStack_6c,auStack_ac,acStack_1304);
                            *0x24c70 = 0;
                        }
                    }
                }
            }
        }
    }
code_r0x00012624:
    func_0x0001146c(*0x24c70);

    sym.imp.exit(0);
}
