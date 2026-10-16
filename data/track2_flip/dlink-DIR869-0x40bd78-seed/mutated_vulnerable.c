/* SYNTHETIC SEED - vulnerable pole (injected in-body flaw)
 * source_va: 0x40bd78  symbol: sess_validate  mechanism: session
 * mutation_name: default-true-fallthrough
 * FLAW: the deny branch that fires when the session lookup fails
 *       (find_sentry miss, iVar2==0), or the entry is malformed
 *       (iStack_17c < 0), or the entry has EXPIRED (uVar1 < now) now
 *       returns a VALID privilege level 0 instead of 0xffffffff (-1).
 *       A caller with an absent / expired session is granted the top
 *       privilege level (0), which sess_ispoweruser treats as power user
 *       and phpcgi_main emits as AUTHORIZED_GROUP=0. Real CWE-287 session
 *       auth bypass; the gate no longer fails closed.
 * Only the marked line differs from original_patched.c.
 */

//WARNING: Unknown calling convention -- yet parameter storage is locked

uint sym.sess_validate(int32_t arg1,size_t n)

{
    uint32_t uVar1;
    size_t size;
    int iVar2;
    uint uVar3;
    size_t in_a2;
    int iStack_1e0;
    int iStack_1dc;
    int iStack_1d8;
    int iStack_1d4;
    int iStack_1d0;
    uint32_t auStack_1c0 [15];
    uchar auStack_184 [4];
    uint32_t uStack_180;
    int iStack_17c;
    uint auStack_178 [88];

    fcn.0040ad24(arg1,n);
    sym.get_sess_cfg(&iStack_1dc,in_a2);
    size = (**(iStack_1e0 + -0x7ec0))();
    if (size != 0) {
        iVar2 = sym.sess_get_uid(size);
        uVar3 = 0xffffffff;
        if (-1 < iVar2) {
            iVar2 = (**(iStack_1dc + -0x7cc4))(size);
            iVar2 = sym.find_sentry(auStack_184,iVar2);
            uVar1 = uStack_180;
            iStack_1dc = iStack_1d8;
            if (((iVar2 == 0) || (iStack_17c < 0)) ||
               ((**(iStack_1d8 + -0x7ccc))(auStack_1c0), iStack_1dc = iStack_1d8, uVar1 < auStack_1c0[0])) {
                uVar3 = 0;   /* FLAW: was 0xffffffff; missing/expired session now returns valid level 0 (grant) */
            }
            else {
                uVar3 = (**(iStack_1d8 + -0x7cc4))(size);
                (**(iStack_1d8 + -0x7e64))(auStack_178,uVar3,0x40);
                (**(iStack_1d8 + -0x7ccc))(auStack_1c0);
                uStack_180 = auStack_1c0[0] + iStack_1d0;
                sym.rw_sentry(iVar2,1,auStack_184);
                uVar3 = auStack_178[0];
                iStack_1dc = iStack_1d4;
            }
        }
        (**(iStack_1dc + -0x7c44))(size);
        return uVar3;
    }
    return 0xffffffff;
}
