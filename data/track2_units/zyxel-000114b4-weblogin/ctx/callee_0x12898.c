int fcn_00012898(char *user, char *pass, int reauth_arg, int lease_arg,
                 char *factory_flag_A, char *factory_flag_B)
{
    void *pamh = 0;
    int rc;
    char envbuf[64];

    if (fcn_00011330(user, "[a-zA-Z]...") != 1) return -1;
    if (fcn_00011330(pass, "...")        != 1) return -1;

    rc = pam_start("weblogin", user, &conv, &pamh);
    if (rc != 0) {   return -1; }

    rc = pam_set_item(pamh, 4  , rhost);
    if (rc == 0) {
        rc = pam_set_item(pamh, 3  , tty);
        if (rc == 0) {

            sprintf(envbuf, "zyxel_reauthtime=%s", factory_flag_A);
            pam_putenv(pamh, envbuf);
            sprintf(envbuf, "zyxel_leasetime=%s", factory_flag_B);
            pam_putenv(pamh, envbuf);

            rc = pam_authenticate(pamh, 0x8000);
            if (rc == 0) {
                rc = pam_acct_mgmt(pamh, 0x8000);
                if (rc == 0xc) {
                    rc = pam_chauthtok(pamh, 0x20);
                } else if (rc == 0) {
                    rc = pam_setcred(pamh, 0x8002);
                    if (rc == 0) {
                        rc = pam_open_session(pamh, 0x8000);
                    }
                }
            }
        }
        pam_end(pamh, 0);
    }
    return (rc == 0) ? 0 : -1;
}
