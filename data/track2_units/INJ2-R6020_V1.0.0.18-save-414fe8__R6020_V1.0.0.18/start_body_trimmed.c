uint sym.save(int32_t arg_0h, int32_t arg_8h)
{

    uStack_3c = sym.find_val(in_a0, str.h_value_changed);
    iStack_848 = 0;
    iVar1 = sym.imp.nvram_get(0x477470);
    if (iVar1 == 0) { iVar1 = 0x47d564; }
    uVar2 = sym.imp.inet_network(iVar1);
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

    iVar8 = sym.imp.strstr(iVar5, 0x47bf88);
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
            sym.imp.COMMAND(pcVar19);
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
        if ((iVar1 != 0) || (iVar1 = sym.imp.strstr(iVar5, 0x4871cc), iVar1 != 0)) {
            sym.imp.COMMAND(str._usr_sbin_touch__s, str._tmp_brs_trigger_wan_idle);
        }
        iVar1 = sym.imp.nvram_get(str.wan_iptype);
        if (iVar1 == 0) { iVar1 = 0x47d564; }
        iVar1 = sym.imp.strcmp(iVar1, str.Fixed);
        if (iVar1 == 0) {
            iVar1 = sym.imp.nvram_get(0x480198);
            if (iVar1 == 0) { iVar1 = 0x47d564; }
            iVar1 = sym.imp.strcmp(iVar1, 0x476de4);
            if (iVar1 == 0) {
                pcVar19 = 0x47bddc;
                goto code_r0x00417f7c;
            }
        }
        pcVar19 = str._usr_sbin_rc_wan_start;
code_r0x00417f7c:
        sym.imp.COMMAND(pcVar19);
    }
    sym.imp.COMMAND(str._usr_sbin_rc_ipv6_radvd_restart);
    sym.imp.COMMAND(str._usr_sbin_rc_ipv6_6to4_restart);
    sym.imp.COMMAND(str._usr_sbin_rc_ipv6_natpt_restart);
    iVar1 = sym.imp.strcmp(iVar5, str.BAS_pptp.htm);
    if (iVar1 != 0) {
        sym.imp.COMMAND(0x478fe0);
    }

    ... [ ~256 lines elided (1065-1320): schedule/wlan/wifi-isolation/qos restart
          branches. Each is COMMAND(<fixed .rodata command string>) or
          COMMAND(pcVar19) after pcVar19 = <constant command string>. ] ...

    iVar1 = sym.imp.strcmp(iVar5, str.WPS.htm);
    if (iVar1 == 0) {
        ... [ nvram_set of wps_config / wps_state_change elided ] ...
        sym.imp.system(str._usr_sbin_wps_det_stop_wps___dev_console_2_1);
        pcVar19 = str._usr_sbin_wps_det_do_client_pbc___dev_console_2_1_;
        pcVar18 = *0x4a5914;
    }

    else if (sym.imp.strcmp(iVar5, str.DNS_ddns.htm) == 0) {
        sym.imp.unlink(str._tmp_ddns_update.log);
        sym.imp.COMMAND(0x47c5cc);
    }
    else if (sym.imp.strcmp(iVar5, str.Netgear_ddns_registration.htm) == 0) {
        iVar1 = sym.wan_connected();
        if (iVar1 == 0) {
            ... [ set NGR_DDNSerrmsg, html_parser redirect, return ] ...
        }
        sym.imp.unlink(str._tmp_ngr_account_exist);
        sym.imp.COMMAND(str._usr_sbin_rc_ddns_register);
        sym.imp.sleep(10);
        ... [ ~120 lines elided (1391-1501): status-file logging via fopen/fprintf
              and retry handling ] ...
        iVar1 = sym.imp.nvram_get(str.ddns_enable);
        if (iVar1 == 0) { iVar1 = 0x47d564; }
        if (sym.imp.atoi(iVar1) != 0) {
            sym.imp.system(str._usr_sbin_touch__tmp_ngr_account_exist____dev_null_2_1_);
            sym.imp.system(str._usr_sbin_rc_ddns_register____dev_null_2_1_);
            sym.imp.sleep(10);
            ... [ func_0x00414a9c status parse ] ...
            sym.imp.system(str._usr_sbin_rc_ddns_restart____dev_null_2_1_);
        }
    }

    ... [ ~277 lines elided (1520-1796): further ddns/status handling and html
          output ] ...

    if (iStack_848 != 0) {
        sym.imp.printf(str._script__alert___s______script_, auStack_644);
    }
    return 0;
}
