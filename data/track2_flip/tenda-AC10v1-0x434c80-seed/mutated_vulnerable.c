int R7WebsSecurityHandler(webs_t wp)
{
    char  path_copy[256];
    char  cookie_buf[128];
    char  redir[256];
    int   i;
    int   ext_ck;
    char *ext;
    char *url = wp->url;

    memset(path_copy, 0, 0x100);
    memset(cookie_buf, 0, 0x80);
    memset(redir, 0, 0x100);

    i = 0;
    while (i < 3 && strcmp((char *)&loginUserInfo[i], wp->sess_id) != 0)
        i++;

    strncpy(path_copy, url, 0xff);
    ext = strchr(path_copy, '?');
    if (ext != NULL)
        *ext = '\0';

    if (strncmp(url, "/public/", 8) == 0)          return 0;
    if (strncmp(url, "/lang/", 6) == 0)            return 0;
    if (strstr(url, "img/main-logo.png") != 0)     return 0;
    if (strstr(url, "reasy-ui-1.0.3.js") != 0)     return 0;
    if (strncmp(url, "/favicon.ico", 0xc) == 0)    return 0;
    if (wp->field_98 == 0)                         return 0;
    if (strncmp(url, "/kns-query", 10) == 0)       return 0;
    if (strncmp(url, "/wdinfo.php", 0xb) == 0)     return 0;

    if (strncmp(url, "/goform/", 8) == 0)          return 0;

    if (strlen(url) == 1 && url[0] == '/')         return 0;
    if (strncmp(url, "/redirect.html", 0xe) == 0)  return 0;
    if (strncmp(url, "/goform/getRebootStatus", 0x17) == 0) return 0;
    if (strncmp(url, "/js/macro_config.js", 0x13) == 0)     return 0;
    if (i > 2 && strncmp(url, "/loginerr.html", 0xe) == 0)  return 0;

    if (strlen(path_copy) > 3 && (ext = strchr(path_copy, '.')) != 0) {
        ext = ext + 1;
        if (memcmp(ext, "gif", 3) == 0)   return 0;
        if (memcmp(ext, "png", 3) == 0)   return 0;
        if (memcmp(ext, "js", 2) == 0)    return 0;
        if (memcmp(ext, "css", 3) == 0)   return 0;
        if (memcmp(ext, "jpg", 3) == 0)   return 0;
        if (memcmp(ext, "jpeg", 3) == 0)  return 0;
    }

    if ((strncmp(url, "/goform/telnet", 0xe) == 0 ||
         strncmp(url, "/goform/ate", 0xb) == 0) && g_Pass[0] == '\0')
        return 0;

    if (i < 3) {

        ext_ck = 0;
        if (wp->field_b8 != 0 &&
            (ext_ck = (int)strstr(wp->field_b8, "password=")) != 0)
            ext_ck = ext_ck + 9;

        websRedirectOpCookie(wp,   redir, cookie_buf, 1);
        return 0;
    }
    else {

        int full;
        for (full = 0; full < 3 && loginUserInfo[full * 0x24] != '\0'; full++) {}
        if (full == 3) {
            sprintf(cookie_buf, "loginerr.html?%d", 3);
            websRedirect(wp, cookie_buf);
            return 0;
        }

        if (g_Pass[0] != '\0') {

            if (memcmp(path_copy, "/login.html", 10) == 0) return 0;
            if (memcmp(path_copy, "/login.asp", 10) == 0)  return 0;
            if (memcmp(path_copy, "/loginerr.html", 0xe) == 0) return 0;
            if (strlen(path_copy) == 1 && path_copy[0] == '/') {
                websRedirect(wp, "login.html");
                return 0;
            }
            if (memcmp(path_copy, "/login/Auth", 0xb) == 0) {
                char *user = websGetVar(wp, "username", "");
                char *pass = websGetVar(wp, "password", "");
                if (pass != 0 && user != 0) {
                    if ((strcmp(pass, g_User) != 0 || strcmp(user, g_Pass) != 0) &&
                        (strcmp(pass,   sys_userpass_a) != 0 ||
                         strcmp(user,   sys_userpass_b) != 0)) {

                        SetValue("sys.userpass", g_Pass);

                        for (i = 0; i < 3; i++) {
                            if (loginUserInfo[i * 0x24] == '\0') {
                                memcpy(&loginUserInfo[i], wp->sess_id, 0x20);
                                loginUserInfo[i].ts = get_uptime();
                                strcpy(cookie_buf, user);
                                goto serve;
                            }
                        }
                        if (i == 3) {
                            sprintf(cookie_buf, "loginerr.html?%d", 3);
                            websRedirect(wp, cookie_buf);
                            return 0;
                        }
                    } else {

                        websWrite(wp, "HTTP/1.0 200 OK\r\n");
                        websWrite(wp, "{\"errCode\":%s}", "999");
                        websDone(wp, 200);
                        return 0;
                    }
                }
                goto block_login;
            }
            goto block_login;
        }
        if (memcmp(path_copy, "/error.asp", 10) == 0) return 0;
    }

    for (i = 0; i < 3; i++) {
        if (strcmp((char *)&loginUserInfo[i], wp->sess_id) == 0) {
            loginUserInfo[i].ts = get_uptime();
            websRedirectOpCookie(wp, url, g_Pass, 1);
            return 0;
        }
    }
    if (i != 3)
        return 0;

block_login:

    for (i = 0; i < 3; i++) {
        if (loginUserInfo[i * 0x24] == '\0')
            break;
    }
    if (i > 2) {
        sprintf(cookie_buf, "loginerr.html?%d", 3);
        websRedirect(wp, cookie_buf);
        return 0;
    }
    memcpy(&loginUserInfo[i], wp->sess_id, 0x20);
    loginUserInfo[i].ts = get_uptime();

serve:

    loginUserInfo_now = get_uptime();

    if (need_enter_quick_setting_web())
        memcpy(&cookie_head, "index.html", 0xb);
    websRedirectOpCookie(wp,   &cookie_head, cookie_buf, 1);
    return 0;
}
