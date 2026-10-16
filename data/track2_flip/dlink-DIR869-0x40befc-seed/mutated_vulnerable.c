/* SYNTHETIC SEED - vulnerable pole (injected in-body flaw)
 * source_va: 0x40befc  symbol: sess_ispoweruser  mechanism: role
 * mutation_name: default-true-fallthrough (unauth->poweruser)
 *
 * REAL MACHINE SEMANTICS (verified against the binary, 0x40bf14..0x40bf24):
 *   0x40bf14 bgez v0,0x40bf20   ; if sess_validate() >= 0 skip the deny
 *   0x40bf18 slti v1,v0,0x64    ; delay slot: v1 = (v0 < 100) signed
 *   0x40bf1c move v1,zero       ; runs only when v0 < 0  -> deny
 *   0x40bf24 move v0,v1         ; return v1
 * i.e. the true gate is  sess_validate() >= 0 ? (sess_validate() < 100) : 0.
 * The -1 sentinel (unauth / invalid / expired session) takes the negative
 * branch and returns 0 (deny). The r2ghidra pdg simplified this to the
 * `return uVar1 < 100` form kept in original_patched.c; for every value
 * sess_validate can actually return (-1, or a small non-negative level) the
 * two forms agree, so original_patched.c stays the reference body.
 *
 * FLAW: a single line is injected before the preserved original return.
 *       It forces ONLY the sentinel / negative branch (the unauth, invalid or
 *       expired session, where sess_validate() == -1) to return power-user =
 *       true (grant). Every legitimately authenticated input is untouched:
 *       levels 0..99 still return true (power-user), levels >= 100 still
 *       return false. So exactly one input class flips: the unauthenticated
 *       session is now classified as a power user. Real CWE-284/285 privilege
 *       escalation, and clean: only the unauth path changes, unlike a broad
 *       comparison flip that also mis-handles every legitimate user.
 *
 * Caller polarity (pigwidgeoncgi_main @0x40f24c `beqz v0,0x40f270`):
 *   return 0 -> beqz taken -> strcpy "no power" (deny)
 *   return non-zero -> falls through to the privileged power-user branch (grant)
 * So the injected `return true` on the unauth path = power-user granted.
 *
 * Only the marked line is added; the rest equals original_patched.c.
 */

bool sym.sess_ispoweruser(void)

{
    uint32_t uVar1;

    uVar1 = sym.sess_validate();
    if ((int32_t)uVar1 < 0) return true;   /* FLAW: unauth/invalid session (sess_validate()==-1) granted power-user; original denies (returns 0) on this path */
    return uVar1 < 100;
}
