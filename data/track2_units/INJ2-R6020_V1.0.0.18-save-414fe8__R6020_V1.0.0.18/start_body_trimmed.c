/* TRIMMED start body of sym.save @ 0x00414fe8
 * netgear/R6020_V1.0.0.18/bin/htpwd_recovery.cgi  (MIPS32, stripped)
 * Decompiled with: r2 -q -c 's 0x414fe8; af; pdg' (radare2 6.1.5).
 *
 * Why this is trimmed: the full decompilation of sym.save is about 127k
 * characters, larger than every model window. This slice keeps only what is
 * needed to settle the CWE-78 verdict, that is the raw system()/COMMAND() sinks
 * and the values that reach them. Long unrelated middle sections (method check,
 * form-field parsing, many nvram_set writes, ipv6/qos/wlan sub-branches, html
 * output) are removed and shown as:
 *     ... [N lines elided: <what>] ...
 * The leading indentation from the decompiler's deep if/else tree is reduced for
 * reading. The tokens on each kept line are verbatim from pdg.
 *
 * What the slice shows:
 *   1. Every argument reaching system() or COMMAND() is a fixed .rodata command
 *      string, or a fixed format string plus a fixed argument. The pcVar19
 *      passed to COMMAND(pcVar19) at 0x00417dc4 and 0x00417f7c is reassigned to
 *      a constant command string on every path just before each call.
 *   2. The nvram_get values this handler reads flow only into inet_network(),
 *      strcmp()/strstr() comparisons, and sym.wan_connected(). None reach a
 *      shell. No cgi or nvram value is spliced into a command line.
 *
 * Constant-address legend (raw pointer seen below -> .rodata string):
 *   0x477470 = "lan_ipaddr"  (nvram key)      0x47d564 = "" (default empty)
 *   0x47bf88 = "BRS"   0x4871cc = "pptp"      (strstr compare targets)
 *   0x47c064 = "/usr/sbin/rc lan restart&"
 *   0x47bddc = "/usr/sbin/rc wan restart&"
 *   0x478fe0 = "/usr/sbin/rc dhcpd restart&"
 *   0x47c5cc = "/usr/sbin/rc ddns restart&"
 *   0x47c5e8 = "/usr/sbin/rc ddns register"                          (= str._usr_sbin_rc_ddns_register)
 *   0x47c7c4 = "/usr/sbin/rc ddns register > /dev/null 2>&1 &"       (= str._usr_sbin_rc_ddns_register____dev_null_2_1_)
 */

uint sym.save(int32_t arg_0h, int32_t arg_8h)
{
    /* ... locals: iVar1..iVar11, pcVar6/15/16, puVar12/17, pcVar18/19 (code*),
     *     auStack_644[512], auStack_834[496], iStack_848, uStack_444.. ... */

    /* ---- prologue: nvram values feed inet_network(), never a shell ---- */
    uStack_3c = sym.find_val(in_a0, str.h_value_changed);
    iStack_848 = 0;
    iVar1 = sym.imp.nvram_get(0x477470);            /* "lan_ipaddr" */
    if (iVar1 == 0) { iVar1 = 0x47d564; }
    uVar2 = sym.imp.inet_network(iVar1);            /* -> numeric, off the shell path */
    iVar1 = sym.imp.nvram_get(str.lan_netmask);
    if (iVar1 == 0) { iVar1 = 0x47d564; }
    uVar3 = sym.imp.inet_network(iVar1);
    iVar1 = sym.imp.nvram_get(str.dhcp_start_ip);
    if (iVar1 == 0) { iVar1 = 0x47d564; }
    iVar1 = sym.imp.inet_network(iVar1);
    iVar4 = sym.imp.nvram_get(str.dhcp_end_ip);
    if (iVar4 == 0) { iVar4 = 0x47d564; }
    iStack_38 = sym.imp.inet_network(iVar4);

    ... [ ~925 lines elided (65-989): request-method check, form-field parsing,
          and many nvram_set / nvram_bcm_set writes. Every nvram value read here
          goes to inet_network() or a strcmp()/strstr() compare, none to a sink.
          COMMAND() calls in this range all take fixed .rodata command strings
          (str._usr_sbin_rc_firewall_start, killall, dnshj_restart, etc.). ] ...

    /* ---- WAN apply: the COMMAND(pcVar19) sinks at 0x00417dc4 / 0x00417f7c ----
     * iVar5 is the .htm page name being matched, not attacker payload. pcVar19
     * is set to a constant command string on every branch before the sink. */
    iVar8 = sym.imp.strstr(iVar5, 0x47bf88);        /* "BRS" */
    if (iVar8 == 0) {
        iVar8 = sym.imp.strcmp(iVar5, str.WAN_wan.htm);
        if ((((iVar8 != 0) || (iVar1 == 0)) || (uStack_40 == 0)) || (iVar7 == 0)) {
            ... [ ~14 lines elided (995-1008): strcmp chain over BAS_*.htm page
                  names, then a puts/printf redirect to BAS_basic.htm ] ...
            pcVar19 = str._usr_sbin_rc_wan_stop__usr_sbin_rc_wan_start;
            goto code_r0x00417f7c;
        }
        iVar1 = (**0x4a56d8)(iVar1, 0x47d2a8);
        if (iVar1 == 0) {
            iVar1 = sym.imp.strcmp(uStack_40, 0x48b4d0);
            if (iVar1 == 0) {
                pcVar19 = str._usr_sbin_rc_igmp_stop__usr_sbin_rc_igmp_start;
                goto code_r0x00417dc4;
            }
        }
        else {
            pcVar19 = str._usr_sbin_rc_wan_stop__usr_sbin_rc_wan_start;
code_r0x00417dc4:
            sym.imp.COMMAND(pcVar19);               /* SINK: pcVar19 is a constant string */
        }
        iVar1 = sym.imp.strcmp(iVar7, 0x48b4d0);
        if (iVar1 == 0) {
            sym.imp.COMMAND(str._usr_sbin_rc_firewall_start___dev_null_2_1);
            pcVar19 = str._usr_sbin_rc_dnrd_restart;
            goto code_r0x00417f7c;
        }
    }
    else {
        iVar1 = sym.imp.strstr(iVar5, str.pppoe);
        if ((iVar1 != 0) || (iVar1 = sym.imp.strstr(iVar5, 0x4871cc), iVar1 != 0)) {   /* "pptp" */
            sym.imp.COMMAND(str._usr_sbin_touch__s, str._tmp_brs_trigger_wan_idle);    /* fixed fmt + fixed arg */
        }
        iVar1 = sym.imp.nvram_get(str.wan_iptype);
        if (iVar1 == 0) { iVar1 = 0x47d564; }
        iVar1 = sym.imp.strcmp(iVar1, str.Fixed);   /* nvram value -> compare only */
        if (iVar1 == 0) {
            iVar1 = sym.imp.nvram_get(0x480198);
            if (iVar1 == 0) { iVar1 = 0x47d564; }
            iVar1 = sym.imp.strcmp(iVar1, 0x476de4);
            if (iVar1 == 0) {
                pcVar19 = 0x47bddc;                  /* "/usr/sbin/rc wan restart&" */
                goto code_r0x00417f7c;
            }
        }
        pcVar19 = str._usr_sbin_rc_wan_start;
code_r0x00417f7c:
        sym.imp.COMMAND(pcVar19);                   /* SINK: pcVar19 is a constant string */
    }
    sym.imp.COMMAND(str._usr_sbin_rc_ipv6_radvd_restart);
    sym.imp.COMMAND(str._usr_sbin_rc_ipv6_6to4_restart);
    sym.imp.COMMAND(str._usr_sbin_rc_ipv6_natpt_restart);
    iVar1 = sym.imp.strcmp(iVar5, str.BAS_pptp.htm);
    if (iVar1 != 0) {
        sym.imp.COMMAND(0x478fe0);                  /* "/usr/sbin/rc dhcpd restart&" */
    }

    ... [ ~256 lines elided (1065-1320): schedule/wlan/wifi-isolation/qos restart
          branches. Each is COMMAND(<fixed .rodata command string>) or
          COMMAND(pcVar19) after pcVar19 = <constant command string>. ] ...

    /* ---- WPS / DDNS page branches: system()/COMMAND() with constant args ---- */
    iVar1 = sym.imp.strcmp(iVar5, str.WPS.htm);
    if (iVar1 == 0) {
        ... [ nvram_set of wps_config / wps_state_change elided ] ...
        sym.imp.system(str._usr_sbin_wps_det_stop_wps___dev_console_2_1);
        pcVar19 = str._usr_sbin_wps_det_do_client_pbc___dev_console_2_1_;
        pcVar18 = *0x4a5914;
    }
    /* (str.WPS_PIN.htm branch: same shape, system(<const>) then pcVar19=<const>) */
    else if (sym.imp.strcmp(iVar5, str.DNS_ddns.htm) == 0) {
        sym.imp.unlink(str._tmp_ddns_update.log);
        sym.imp.COMMAND(0x47c5cc);                  /* "/usr/sbin/rc ddns restart&" */
    }
    else if (sym.imp.strcmp(iVar5, str.Netgear_ddns_registration.htm) == 0) {
        iVar1 = sym.wan_connected();               /* nvram wan-state consumer, off the shell path */
        if (iVar1 == 0) {
            ... [ set NGR_DDNSerrmsg, html_parser redirect, return ] ...
        }
        sym.imp.unlink(str._tmp_ngr_account_exist);
        sym.imp.COMMAND(str._usr_sbin_rc_ddns_register);   /* = 0x47c5e8, constant */
        sym.imp.sleep(10);
        ... [ ~120 lines elided (1391-1501): status-file logging via fopen/fprintf
              and retry handling ] ...
        iVar1 = sym.imp.nvram_get(str.ddns_enable);
        if (iVar1 == 0) { iVar1 = 0x47d564; }
        if (sym.imp.atoi(iVar1) != 0) {            /* nvram value -> atoi test, not a sink arg */
            sym.imp.system(str._usr_sbin_touch__tmp_ngr_account_exist____dev_null_2_1_);
            sym.imp.system(str._usr_sbin_rc_ddns_register____dev_null_2_1_);   /* = 0x47c7c4, constant */
            sym.imp.sleep(10);
            ... [ func_0x00414a9c status parse ] ...
            sym.imp.system(str._usr_sbin_rc_ddns_restart____dev_null_2_1_);    /* constant */
        }
    }

    ... [ ~277 lines elided (1520-1796): further ddns/status handling and html
          output ] ...

    if (iStack_848 != 0) {
        sym.imp.printf(str._script__alert___s______script_, auStack_644);
    }
    return 0;
}
