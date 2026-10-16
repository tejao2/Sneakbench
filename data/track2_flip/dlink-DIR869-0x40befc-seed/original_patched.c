/* SYNTHETIC SEED - not-vulnerable pole (correct gate)
 * source: dlink/DIR-869_REVA_1.02B15/htdocs/cgibin  (MIPS, dynsym-named)
 * source_va: 0x40befc  symbol: sess_ispoweruser
 * mechanism: role
 * Clean r2ghidra (pdg) output, verbatim. Power-user test: the session
 * level from sess_validate must be < 100 (low numeric level = high
 * privilege). An invalid/expired session returns 0xffffffff from
 * sess_validate, which is NOT < 100 (unsigned), so it is denied.
 */

bool sym.sess_ispoweruser(void)

{
    uint32_t uVar1;

    uVar1 = sym.sess_validate();
    return uVar1 < 100;
}
