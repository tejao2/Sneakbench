/* safe_system is imported by cgi/network_mgr.cgi (PLT stub at 0x000092a8).
 * Its implementation lives in the shared object lib/libsafe_system.so
 * (safe_system at 0x00001690, safe_exec at 0x000011e0). Both bodies below are
 * decompiled from that shared object: the wrapper packs its varargs into a
 * discrete argv[] array and hands (argv0, argv) to safe_exec, which fork()s and
 * calls execvp(argv0, argv). There is no system/popen/"sh -c" on this path, so
 * caller-supplied strings stay literal argv elements and are never re-parsed by
 * a shell. safe_system passes 1 as the last safe_exec argument (it waits for the
 * child); safe_system_nonblock is the same wrapper with 0 there. */

/* ---- safe_system: builds argv[] from varargs, calls safe_exec ---- */

//WARNING: Variable defined which should be unmapped: var_28h
//WARNING: Restarted to delay deadcode elimination for space: stack

uint sym.safe_system(int32_t arg1,int32_t arg2,int32_t arg_4h,int32_t arg_8h)

{
    uint *puVar1;
    uint in_r2;
    int in_r3;
    int *piVar2;
    uint uVar3;
    int iVar4;
    int iVar5;
    int *piVar6;
    int32_t var_28h;

    puVar1 = rsym.calloc(0x80,4);
    if (puVar1 == NULL) {
        uVar3 = 0xffffffff;
    }
    else {
        *puVar1 = in_r2;
        if (in_r3 != 0) {
            iVar5 = 0x80;
            iVar4 = 1;
            piVar6 = *0x54;
            while( true ) {
                puVar1[iVar4] = in_r3;
                piVar2 = piVar6 + 1;
                in_r3 = *piVar6;
                if (in_r3 == 0) break;
                iVar4 = iVar4 + 1;
                piVar6 = piVar2;
                if (iVar5 <= iVar4) {
                    puVar1 = rsym.realloc(puVar1,iVar4 * 4 + 0x200);
                    if (puVar1 == NULL) {
                        uVar3 = 0xffffffff;
                        goto code_r0x00001780;
                    }
                    rsym.memset(puVar1 + iVar4,0,0x200);
                    iVar5 = iVar5 + 0x80;
                }
            }
        }
        uVar3 = rsym.safe_exec(*puVar1,puVar1,arg1,arg2,1);
code_r0x00001780:
        rsym.free(puVar1);
    }
    return uVar3;
}

/* ---- safe_exec: fork() + execvp(argv0, argv); no shell interpreter ---- */

uint32_t sym.safe_exec(int32_t arg1,int32_t arg2,int32_t arg_4h)

{
    int iVar1;
    int *piVar2;
    int in_r2;
    int in_r3;
    int iVar3;
    int in_stack_00000000;
    uint32_t uStack_3f0;
    int32_t var_3ech;
    int32_t var_3e8h;
    uchar auStack_36c [4];
    int32_t var_368h;
    uint uStack_2ec;
    int32_t var_2e8h;
    int32_t var_2e4h;
    uint uStack_268;
    int32_t var_264h;
    uchar auStack_260 [4];
    int32_t var_25ch;
    uint uStack_1d4;
    int32_t var_1d0h;
    int32_t var_1cch;
    int iStack_150;
    int32_t var_14ch;
    uchar auStack_148 [4];
    int32_t var_144h;
    uchar auStack_bc [4];
    int32_t var_b8h;

    iVar3 = *0x1580 + 0x11fc;
    if (arg1 == 0) {
        iVar3 = rsym.access(iVar3 + *0x1584,1);
        return iVar3 == 0;
    }
    uStack_2ec = 1;
    rsym.sigemptyset(&sym.safe_exec::var_2e8h);
    uStack_268 = 0;
    iVar1 = rsym.sigaction(2,&stack0xfffffd14,&stack0xffffff44);
    if (iVar1 != -1) {
        iVar1 = rsym.sigaction(3,&stack0xfffffd14,&stack0xfffffeb8);
        if (iVar1 == -1) {
            rsym.sigaction(2,&stack0xffffff44,0);
            return 0xffffffff;
        }
        if (in_stack_00000000 == 0) {
            uStack_1d4 = 1;
            rsym.sigemptyset(&sym.safe_exec::var_1d0h);
            iStack_150 = in_stack_00000000;
            iVar1 = rsym.sigaction(0x11,&stack0xfffffe2c,&stack0xfffffda0);
            if (iVar1 == -1) {
                rsym.sigaction(2,&stack0xffffff44,in_stack_00000000);
                rsym.sigaction(3,&stack0xfffffeb8,in_stack_00000000);
                return 0xffffffff;
            }
        }
        rsym.sigemptyset(&stack0xfffffc94);
        rsym.sigaddset(&stack0xfffffc94,0x11);
        iVar1 = rsym.sigprocmask(0,&stack0xfffffc94,&sym.safe_exec::var_3ech);
        if (iVar1 != -1) {
            iVar1 = rsym.fork();
            if (iVar1 == -1) {
                rsym.sigaction(2,&stack0xffffff44,0);
                rsym.sigaction(3,&stack0xfffffeb8,0);
                if (in_stack_00000000 == 0) {
                    rsym.sigaction(0x11,&stack0xfffffda0,0);
                }
                rsym.sigprocmask(2,&sym.safe_exec::var_3ech,0);
                return 0xffffffff;
            }
            if (iVar1 == 0) {
                rsym.sigaction(2,&stack0xffffff44,0);
                rsym.sigaction(3,&stack0xfffffeb8,0);
                if (in_stack_00000000 == 0) {
                    rsym.sigaction(0x11,&stack0xfffffda0,0);
                }
                rsym.sigprocmask(2,&sym.safe_exec::var_3ech,0);
                if (in_r2 != 0) {
                    iVar1 = rsym.creat();
                    if (iVar1 == -1) {
                        return 0xffffffff;
                    }
                    rsym.dup2(iVar1,1);
                    rsym.close(iVar1);
                }
                if (in_r3 != 0) {
                    iVar1 = rsym.creat();
                    if (iVar1 == -1) {
                        return 0xffffffff;
                    }
                    rsym.dup2(iVar1,2);
                    rsym.close(iVar1);
                }
                rsym.putenv(iVar3 + *0x1588);
                rsym.execvp(arg1,arg2);
                rsym._exit(0x7f);
            }
            if (in_stack_00000000 == 0) {
                rsym.sigaction(2,&stack0xffffff44,0);
                rsym.sigaction(3,&stack0xfffffeb8,0);
                rsym.sigprocmask(2,&sym.safe_exec::var_3ech,0);
                return 0;
            }
            do {
                iVar3 = rsym.waitpid(iVar1,&stack0xfffffc10,0);
                if (iVar3 != -1) goto code_r0x00001544;
                piVar2 = rsym.__errno_location();
            } while (*piVar2 == 4);
            uStack_3f0 = 0xffffffff;
code_r0x00001544:
            rsym.sigaction(2,&stack0xffffff44,0);
            rsym.sigaction(3,&stack0xfffffeb8,0);
            rsym.sigprocmask(2,&sym.safe_exec::var_3ech,0);
            return uStack_3f0;
        }
        rsym.sigaction(2,&stack0xffffff44,0);
        rsym.sigaction(3,&stack0xfffffeb8,0);
        if (in_stack_00000000 == 0) {
            rsym.sigaction(0x11,&stack0xfffffda0,0);
            return 0xffffffff;
        }
    }
    return 0xffffffff;
}
