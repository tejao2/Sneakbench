uint fcn.00016548(int32_t arg1)

{
    char *arg2;
    uchar *s;
    uint uVar1;
    int iVar2;
    int32_t in_stack_fffff7a4;
    char *src;
    int32_t var_840h;
    char acStack_495 [69];
    char acStack_450 [16];
    int32_t var_440h;
    uchar auStack_430 [992];
    uchar auStack_50 [4];
    void *s2;
    uchar auStack_40 [4];
    void *var_3ch;
    char *pcStack_30;
    char *var_2ch;
    char *var_28h;
    int32_t var_24h;
    int32_t var_20h;
    int32_t var_1ch;
    int32_t var_18h;
    int32_t var_14h;
    int32_t var_10h;
    int32_t var_ch;
    void *s1;

    var_18h = 0;
    var_1ch = sym.imp.strlen(arg1);
    sym.imp.memset(acStack_450,0,0x400);
    sym.imp.strncpy(acStack_450,arg1,var_1ch);
    var_18h = var_18h + var_1ch;
    var_ch = sym.imp.strstr(acStack_450,str.tok);
    if (var_ch == 0) {
        var_ch = sym.imp.strstr(acStack_450,str._tok);
    }
    if (var_ch == 0) {
        uVar1 = 0xffffffff;
    }
    else {
        var_20h = acStack_495 + (var_18h - var_ch);
        if (0 < var_20h) {
            sym.imp.memmove(var_ch,var_ch + 0x45,var_20h);
        }
        arg2 = acStack_495 + var_18h;
        var_18h = var_18h + -0x45;
        var_24h = fcn.000164a8(var_ch + 5,arg2);
        if (var_24h < 0) {
            uVar1 = 0xffffffff;
        }
        else {
            iVar2 = var_18h + 0x20;
            s = auStack_430 + var_18h;
            var_18h = iVar2;
            sym.imp.memset(s,0,0x400 - iVar2);
            var_28h = &UndefinedInstruction;
            var_2ch = sym.imp.getenv(0x36210);
            if (var_2ch == NULL) {
                iVar2 = sym.imp.strstr(arg1,str.uid);
                if (iVar2 == 0) {
                    fcn.00009f40(str.cant_found_key_from_cookie_or_UID__n);
                    return 0xffffffff;
                }
                iVar2 = sym.imp.strstr(arg1,str.uid);
                pcStack_30 = iVar2 + 4;
                for (var_14h = pcStack_30; ((*var_14h != '&' && (*var_14h != '\0')) && (*var_14h != ' '));
                    var_14h = var_14h + 1) {
                }
                var_10h = var_14h - pcStack_30;
                sym.imp.strncpy(&src,pcStack_30,var_10h);
            }
            else {
                sym.imp.strcpy(&src,var_2ch + var_28h);
                var_10h = sym.imp.strlen(&src);
            }
            fcn.00026bd8(acStack_450,var_18h,in_stack_fffff7a4);
            iVar2 = sym.imp.memcmp(auStack_40,auStack_50,0x10);
            uVar1 = 0;
        }
    }
    return uVar1;
}
