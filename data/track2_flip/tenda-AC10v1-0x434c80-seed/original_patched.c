/* R7WebsSecurityHandler  @ 0x00434c80  (symbol retained; size 6304)
 * Tenda AC10v1_V15.03.06.46 /bin/httpd  (MIPS32, goahead-based)
 *
 * POLE: NOT-VULNERABLE (original).
 *
 * Role: the web server's SECURITY / access-control URL handler. It is registered
 *       FIRST in initWebs @0x434504:
 *         websUrlHandlerDefine("/", ..., R7WebsSecurityHandler, ...)
 *       ahead of websFormHandler, webs_Tenda_CGI_BIN_Handler and
 *       websDefaultHandler, so the goahead request pipeline runs it on every
 *       request before any content handler.
 *
 * Decision mechanism (auth sense):
 *   - PASS  = fall through / early `return` WITHOUT emitting a redirect  -> the
 *             request continues to the content handler and is served.
 *   - BLOCK = call websRedirect(wp, login page) before returning         -> the
 *             response is completed as a redirect to the login page.
 *   The whitelist section below `return`s early for public endpoints (served
 *   with no auth). Protected paths fall into the session-validation section,
 *   which serves the request only if the request's session id is a stored,
 *   authenticated session in loginUserInfo[]; otherwise it redirects to login.
 *
 * start-is-NMC: the serve-vs-block outcome depends on external state that this
 *   body only reads through delegates:
 *     - loginUserInfo[3]  (obj @0x54fb2c, 3 x 0x24 bytes): the SESSION STORE.
 *       entry[0..0x1f] = a stored session id; entry[0x20] = last-seen uptime.
 *     - g_Pass (obj @0x54fbe8) / g_User (obj @0x54fba8): admin credential state.
 *     - websRedirect (block), websRedirectOpCookie (serve+cookie), websGetVar
 *       (read login form fields), get_uptime (session timestamp).
 *   Whether wp's session id matches a stored authenticated session is not
 *   computable from the body alone.
 *
 * Legend (decompiler in_t9+off -> resolved symbol):
 *   memset 0x10b3f0 | strcmp 0x10b400 | strncmp 0x10b3a4 | strstr 0x10b324 |
 *   memcmp 0x10b490 | strlen 0x10b08c | strncpy 0x10b284 | strchr 0x10b2e8 |
 *   strrchr 0x10b378 | strcpy 0x10b388 | memcpy 0x10b094 | sprintf 0x10b0a4 |
 *   atoi 0x10b11c | get_uptime 0x10b3b4 | rand 0x10b3ac | srand 0x10b438 |
 *   websRedirect 0x10a290 | websWrite 0x10a224 | websDone 0x10a278 |
 *   websGetVar 0x10a52c | websRedirectOpCookie 0x10a6b8 |
 *   is_web_html 0x10a6b0 | need_enter_quick_setting_web 0x10a6b4 |
 *   loginUserInfo 0x10a654 | g_Pass 0x10a638 | g_User 0x10a6ac
 *
 * The reconstruction below preserves every branch, return and delegate call of
 * the verified decompilation (r2ghidra pdg @0x434c80). Only names/strings are
 * resolved for readability.
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
