            0x0001f3f0      movs r1, 1
            0x0001f3f2      blx sym.imp.dup2
            0x0001f3f6      movs r0, 2
            0x0001f3f8      blx sym.imp.close                          ; int close(int fildes)
            0x0001f3fc      mov r0, r4
            0x0001f3fe      movs r1, 2
            0x0001f400      blx sym.imp.dup2
            0x0001f404      mov r0, r4
            0x0001f406      blx sym.imp.close                          ; int close(int fildes)
            0x0001f40a      mov r0, sp
            0x0001f40c      movw r1, 0xeb98
            0x0001f410      movt r1, 3
            0x0001f414      mov r2, r5
            0x0001f416      blx sym.imp.sprintf                        ; int sprintf(char *s, const char *format, ...)
            0x0001f41a      mov r0, sp
            0x0001f41c      blx sym.imp.system                         ; int system(const char *string)
            0x0001f420      movs r0, 0
            0x0001f422      blx sym.imp.exit                           ; void exit(int status)
            0x0001f426      movw r0, 0xebc8
            0x0001f42a      movt r0, 3
            0x0001f42e      blx sym.imp.system                         ; int system(const char *string)
        ┌─< 0x0001f432      b 0x1f43e
        │   0x0001f434      mov.w r7, -1
       ┌──< 0x0001f438      b 0x1f43e

string at 0x0003eb98: /etc/init.d/antivirus.sh scan %d &> /dev/null

string at 0x0003ebc8: /etc/init.d/antivirus.sh restart_crond &> /dev/null
