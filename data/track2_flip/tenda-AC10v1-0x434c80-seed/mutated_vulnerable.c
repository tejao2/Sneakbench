/* R7WebsSecurityHandler  @ 0x00434c80  (symbol retained; size 6304)
 * Tenda AC10v1_V15.03.06.46 /bin/httpd  (MIPS32, goahead-based)
 *
 * POLE: VULNERABLE (mutant).  CWE-306 (Missing Authentication for Critical
 *       Function).
 *
 * FLAW: exactly one whitelist branch is added to the pass-through section:
 *
 *         if (strncmp(url, "/goform/", 8) == 0)  return 0;
 *
 *       This makes the whole "/goform/" admin form tree public. In the original,
 *       the whitelist admits only specific pre-login endpoints (for example
 *       /goform/getRebootStatus, and /goform/telnet or /goform/ate while no
 *       admin password is set). Every other /goform/* action is a config write
 *       (reboot, set password, set wifi, restore) and is protected: it falls
 *       through to the session-validation section below. The added branch makes
 *       all of them hit the pass-through `return 0` before the session check, so
 *       state-changing admin actions are served to an unauthenticated caller.
 *
 * The body below is identical to original_patched.c except for this one added
 * branch. In particular the pre-provision /goform/telnet and /goform/ate block
 * is left intact, so the injected "/goform/" whitelist is the only difference.
 */

int R7WebsSecurityHandler(webs_t wp)
{
    char  path_copy[256];       /* acStack_488: url copy, truncated at '?' */
    char  cookie_buf[128];      /* auStack_388 */
    char  redir[256];           /* auStack_2f0 */
    int   i;                    /* iStack_4a8 */
    int   ext_ck;               /* iStack_4b0 */
    char *ext;
    char *url = wp->url;        /* in_stack_00000010: the request path */

    memset(path_copy, 0, 0x100);
    memset(cookie_buf, 0, 0x80);
    memset(redir, 0, 0x100);

    /* Find whether wp's session id (wp+0x30) is already a stored session.
     * i ends = index of the matching loginUserInfo[] entry, or 3 if none. */
    i = 0;
    while (i < 3 && strcmp((char *)&loginUserInfo[i], wp->sess_id) != 0)
        i++;

    strncpy(path_copy, url, 0xff);
    ext = strchr(path_copy, '?');
    if (ext != NULL)
        *ext = '\0';

    /* ---------------- WHITELIST: public endpoints, served with no auth ------
     * Each early `return 0` here PASSES the request through without the
     * session check below. */
    if (strncmp(url, "/public/", 8) == 0)          return 0;
    if (strncmp(url, "/lang/", 6) == 0)            return 0;
    if (strstr(url, "img/main-logo.png") != 0)     return 0;
    if (strstr(url, "reasy-ui-1.0.3.js") != 0)     return 0;
    if (strncmp(url, "/favicon.ico", 0xc) == 0)    return 0;
    if (wp->field_98 == 0)                         return 0;
    if (strncmp(url, "/kns-query", 10) == 0)       return 0;
    if (strncmp(url, "/wdinfo.php", 0xb) == 0)     return 0;

    /* >>> INJECTED FLAW (synthetic skip-check): whitelist the whole admin form
     *     tree. A protected /goform/* config action now short-circuits to this
     *     pass-through return, BEFORE the login-session check below, so it is
     *     served to an unauthenticated request. This is the only added branch. <<< */
    if (strncmp(url, "/goform/", 8) == 0)          return 0;

    if (strlen(url) == 1 && url[0] == '/')         return 0;   /* root */
    if (strncmp(url, "/redirect.html", 0xe) == 0)  return 0;
    if (strncmp(url, "/goform/getRebootStatus", 0x17) == 0) return 0;
    if (strncmp(url, "/js/macro_config.js", 0x13) == 0)     return 0;
    if (i > 2 && strncmp(url, "/loginerr.html", 0xe) == 0)  return 0;

    /* static-asset extensions: pass through image/js/css */
    if (strlen(path_copy) > 3 && (ext = strchr(path_copy, '.')) != 0) {
        ext = ext + 1;
        if (memcmp(ext, "gif", 3) == 0)   return 0;
        if (memcmp(ext, "png", 3) == 0)   return 0;
        if (memcmp(ext, "js", 2) == 0)    return 0;
        if (memcmp(ext, "css", 3) == 0)   return 0;
        if (memcmp(ext, "jpg", 3) == 0)   return 0;
        if (memcmp(ext, "jpeg", 3) == 0)  return 0;
    }

    /* pre-provision: /goform/telnet (str@0x7e6c) and /goform/ate (str@0x7e7c) are
     * whitelisted only while no admin password is set (g_Pass empty). The real
     * /login/Auth memcmp is elsewhere at 0x7eb4, not here. (fold-fix M2 2026-09-25) */
    if ((strncmp(url, "/goform/telnet", 0xe) == 0 ||
         strncmp(url, "/goform/ate", 0xb) == 0) && g_Pass[0] == '\0')
        return 0;

    /* ---------------- PROTECTED PATHS: require a valid login session -------- */
    if (i < 3) {
        /* request's session id is already in the table (a known session):
         * refresh / serve, or (for /login.* etc.) special-case handling.
         * (Full known-session handling: cookie refresh, is_web_html /
         *  need_enter_quick_setting_web checks, and either serve via
         *  websRedirectOpCookie or emit an error/redirect. All paths here
         *  complete the response for an already-authenticated session.) */
        ext_ck = 0;
        if (wp->field_b8 != 0 &&
            (ext_ck = (int)strstr(wp->field_b8, "password=")) != 0)
            ext_ck = ext_ck + 9;
        /* build the served page / redirect for the known session ... */
        /* (serve path calls websRedirectOpCookie; deny path calls
         *  websRedirect(wp, "login.html"/"loginerr.html?%d") ) */
        /* ... preserved from decompilation ... */
        websRedirectOpCookie(wp, /*session-key*/ redir, cookie_buf, 1);
        return 0;
    }
    else {
        /* unknown session id: this is an UNAUTHENTICATED request. */
        /* if the session table is full -> loginerr */
        int full;
        for (full = 0; full < 3 && loginUserInfo[full * 0x24] != '\0'; full++) {}
        if (full == 3) {
            sprintf(cookie_buf, "loginerr.html?%d", 3);
            websRedirect(wp, cookie_buf);           /* BLOCK -> login error */
            return 0;
        }

        if (g_Pass[0] != '\0') {
            /* a password is configured: try to process a login form POST. */
            if (memcmp(path_copy, "/login.html", 10) == 0) return 0;
            if (memcmp(path_copy, "/login.asp", 10) == 0)  return 0;
            if (memcmp(path_copy, "/loginerr.html", 0xe) == 0) return 0;
            if (strlen(path_copy) == 1 && path_copy[0] == '/') {
                websRedirect(wp, "login.html");     /* BLOCK -> login */
                return 0;
            }
            if (memcmp(path_copy, "/login/Auth", 0xb) == 0) {
                char *user = websGetVar(wp, "username", "");
                char *pass = websGetVar(wp, "password", "");
                if (pass != 0 && user != 0) {
                    if ((strcmp(pass, g_User) != 0 || strcmp(user, g_Pass) != 0) &&
                        (strcmp(pass, /*cfg*/ sys_userpass_a) != 0 ||
                         strcmp(user, /*cfg*/ sys_userpass_b) != 0)) {
                        /* bad credentials */
                        SetValue("sys.userpass", g_Pass);
                        /* install a new session in a free loginUserInfo slot */
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
                            websRedirect(wp, cookie_buf);   /* BLOCK */
                            return 0;
                        }
                    } else {
                        /* good credentials: 200 OK errCode 999 (login ok) */
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

    /* Session-store gate: serve only if wp's session id is a stored session. */
    for (i = 0; i < 3; i++) {
        if (strcmp((char *)&loginUserInfo[i], wp->sess_id) == 0) {
            loginUserInfo[i].ts = get_uptime();      /* refresh */
            websRedirectOpCookie(wp, url, g_Pass, 1);/* SERVE the authenticated request */
            return 0;
        }
    }
    if (i != 3)
        return 0;

block_login:
    /* no stored session matched: find a free slot / redirect to login */
    for (i = 0; i < 3; i++) {
        if (loginUserInfo[i * 0x24] == '\0')
            break;
    }
    if (i > 2) {
        sprintf(cookie_buf, "loginerr.html?%d", 3);
        websRedirect(wp, cookie_buf);                /* BLOCK -> login error */
        return 0;
    }
    memcpy(&loginUserInfo[i], wp->sess_id, 0x20);
    loginUserInfo[i].ts = get_uptime();

serve:
    /* build the session cookie and serve with Set-Cookie */
    loginUserInfo_now = get_uptime();
    /* ... assemble cookie (uStack_158 = expired_cookie_option template),
     *     stat("/webroot/main.html"), random suffix ... preserved ... */
    if (need_enter_quick_setting_web())
        memcpy(&cookie_head, "index.html", 0xb);
    websRedirectOpCookie(wp, /*cookie*/ &cookie_head, cookie_buf, 1); /* SERVE */
    return 0;
}
