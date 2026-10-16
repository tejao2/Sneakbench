uint main(int argc,char **argv,char **envp)

{
    int iVar1;
    int iVar2;
    uint uVar3;
    char *pcVar4;
    int iVar5;
    uchar auStack_1ad8 [28];
    uint uStack_1abc;
    uint uStack_1ab8;
    uint uStack_1ab4;
    uchar auStack_1a7c [128];
    char acStack_19fc [1024];
    uint auStack_15fc [18];
    uint uStack_15b4;
    int iStack_1584;
    int iStack_156c;

    iVar1 = sym.imp.CGI_Get_Input(argc);
    iVar2 = sym.imp.CGI_Find_Parameter(iVar1,0x44cd0);
    uVar3 = 0;
    if (iVar2 != 0) {
        uVar3 = *(iVar2 + 4);
    }
    iVar2 = sym.imp.CGI_Find_Parameter(iVar1,str.subfunc);
    pcVar4 = NULL;
    if (iVar2 != 0) {
        pcVar4 = *(iVar2 + 4);
    }
    iVar2 = sym.imp.CGI_Find_Parameter(iVar1,0x43854);
    iVar5 = 0;
    if (iVar2 != 0) {
        iVar5 = 0;
        if (*(iVar2 + 4) != 0) {
            iVar5 = sym.imp.strtol(*(iVar2 + 4),0,10);
        }
    }
    iVar2 = sym.imp.auth_get_session(uVar3,1,0x4fdc0);
    if (iVar2 != 0) {
        func_0x00015248();
        func_0x000159c8(0);
        if (iVar1 != 0) {
            rsym.Ini_Conf_Unlock_File(iVar1);
        }
        func_0x00015280();
        return 0;
    }
    func_0x00015248();
    iVar2 = sym.imp.User_Belongs_To_Group(*0x369c8,str.administrators);
    if ((iVar2 == 0) &&
       ((((iVar5 != 0 && (iVar5 != 10)) || (pcVar4 == NULL)) ||
        (iVar2 = rsym.GetRemoteDeviceLunListEx(pcVar4,0x42098), iVar2 != 0)))) {
        if (((*pcVar4 != 'q') || (pcVar4[1] != 'H')) || ((pcVar4[2] != 'A' || (pcVar4[3] != '\0')))) {
            func_0x000159c8(0);
        }
        if (iVar1 != 0) {
            rsym.Ini_Conf_Unlock_File(iVar1);
        }
        func_0x00015280();
    }
    else {
        sym.imp.shm_add_http_user(*0x369c8);
        func_0x000159c8(1);
        func_0x00015828();
        func_0x00035fcc(pcVar4,iVar1,iVar5);
        func_0x000237f8();
        if (iVar1 != 0) {
            rsym.Ini_Conf_Unlock_File(iVar1);
        }
        func_0x00015280();
    }
    return 0;
}
