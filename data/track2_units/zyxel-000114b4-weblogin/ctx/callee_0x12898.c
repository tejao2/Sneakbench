/*
 * fcn.00012898  @ VA 0x00012898   (weblogin.cgi, ARM 32-bit LSB, EABI5, stripped)
 * Recovered with radare2 + r2ghidra (pdg). Symbol names are decompiler-assigned.
 *
 * This is the callee that main invokes at 0x122e4 on every login request, right after
 * the admin/1234 predicate (fcn.000114b4) at 0x12294. It drives a full PAM transaction
 * against the real credential store and returns 0 on success, -1 on any failure.
 *
 * KEY POINTS FROM THE DISASSEMBLY
 *   1. The two "factory default" flags produced by the admin/1234 match arrive here as the
 *      5th/6th arguments. Their ONLY use is sprintf("zyxel_reauthtime=%s") /
 *      sprintf("zyxel_leasetime=%s") followed by pam_putenv. They set reauth/lease TIMING
 *      in the PAM environment. They do NOT feed the credential check.
 *   2. pam_authenticate(pamh, 0x8000) is called UNCONDITIONALLY once pam_start and the two
 *      pam_set_item calls succeed. It is not gated on the factory-default flags, so a match
 *      on admin/1234 cannot force it to pass or be skipped.
 *   3. Login success requires pam_authenticate == 0, then pam_acct_mgmt, pam_setcred and
 *      pam_open_session to also succeed. The return is 0 only on that full chain, else -1.
 *      main treats return >= 0 as success (0x122f0 bge).
 *
 * Consequence: the real authentication and session establishment happen HERE, against the
 * stored password, not in the admin/1234 predicate. If the operator changed the default
 * password, PAM rejects admin/1234; the factory flags only affect reauth/lease timing.
 *
 * ----- literal evidence (pdg / pdf @ 0x12898) -----
 *   func_0x00011330(user, "[a-zA-Z]...")   -> charset validation of username (0x11330)
 *   func_0x00011330(pass, "...")           -> charset validation of password
 *   pam_start("weblogin", user, &conv, &pamh)          ; service name @ 0x134cc
 *   pam_set_item(pamh, 4, rhost)           -> PAM_RHOST
 *   pam_set_item(pamh, 3, tty)             -> PAM_TTY
 *   sprintf(buf,"zyxel_reauthtime=%s",flagA); pam_putenv(pamh,buf)   ; timing only
 *   sprintf(buf,"zyxel_leasetime=%s", flagB); pam_putenv(pamh,buf)   ; timing only
 *   pam_authenticate(pamh, 0x8000)         -> the actual credential check (UNCONDITIONAL)
 *   pam_acct_mgmt(pamh, 0x8000)            -> 0xc (NEW_AUTHTOK_REQD) triggers pam_chauthtok
 *   pam_setcred(pamh, 0x8002)              -> establish credentials
 *   pam_open_session(pamh, 0x8000)         -> establish session
 *   pam_end(pamh, 0)                       -> teardown; return 0 on success else 0xffffffff
 */

int fcn_00012898(char *user, char *pass, int reauth_arg, int lease_arg,
                 char *factory_flag_A, char *factory_flag_B)
{
    void *pamh = 0;
    int rc;
    char envbuf[64];

    /* input charset validation; reject early on bad characters */
    if (fcn_00011330(user, "[a-zA-Z]...") != 1) return -1;   /* 0x11330 */
    if (fcn_00011330(pass, "...")        != 1) return -1;

    rc = pam_start("weblogin", user, &conv, &pamh);          /* begin transaction */
    if (rc != 0) { /* log to stderr */ return -1; }

    rc = pam_set_item(pamh, 4 /*PAM_RHOST*/, rhost);
    if (rc == 0) {
        rc = pam_set_item(pamh, 3 /*PAM_TTY*/, tty);
        if (rc == 0) {
            /* ---- factory-default flags consumed HERE, timing only ---- */
            sprintf(envbuf, "zyxel_reauthtime=%s", factory_flag_A);
            pam_putenv(pamh, envbuf);                 /* failure only warns to stderr */
            sprintf(envbuf, "zyxel_leasetime=%s", factory_flag_B);
            pam_putenv(pamh, envbuf);                 /* failure only warns to stderr */

            /* ---- the real credential check: runs regardless of the flags ---- */
            rc = pam_authenticate(pamh, 0x8000);
            if (rc == 0) {
                rc = pam_acct_mgmt(pamh, 0x8000);
                if (rc == 0xc) {                      /* PAM_NEW_AUTHTOK_REQD */
                    rc = pam_chauthtok(pamh, 0x20);
                } else if (rc == 0) {
                    rc = pam_setcred(pamh, 0x8002);   /* establish creds */
                    if (rc == 0) {
                        rc = pam_open_session(pamh, 0x8000);  /* establish session */
                    }
                }
            }
        }
        pam_end(pamh, 0);
    }
    return (rc == 0) ? 0 : -1;                        /* success only if PAM chain passed */
}
