/* SOURCE: D-Link DIR-885L_REVA_1.10B01  /htdocs/cgibin  (ELF ARM, stripped)
 * FUNCTION: fcn.0000c934  (r2 auto-name; getcfg-family session gate)
 * POLE: vulnerable (synthetic mutant). Identical to original_patched.c except
 *       for one injected line. CWE-287 authentication bypass.
 *
 * ROLE: Session gate for the getcfg / config pipeline. It resolves the request
 *       session, enforces a Content-Length bound, then validates the session
 *       against the session store and returns the authorized group level.
 *
 * POLARITY (verified in consumer fcn.0000d760):
 *   return >= 0  -> AUTHORIZED. The consumer emits "AUTHORIZED_GROUP=%d" and
 *                   forwards the getcfg request.
 *   return  < 0  -> DENIED. The consumer replies HTTP 400 "unsupported HTTP
 *                   request" (or handles the -100 over-limit case specially).
 *
 * INJECTED FLAW: default-true-fallthrough on session validation. When the
 *   session-store check reports an invalid / absent session (negative), the
 *   mutant coerces the result to 0, a valid AUTHORIZED_GROUP level. The gate
 *   then returns >= 0, so the caller treats an unauthenticated request as
 *   authorized and forwards the getcfg request. The gate now fails OPEN.
 */

int getenv_content_type(void);          /* getenv("CONTENT_TYPE")   */
char *getenv_content_length(void);      /* getenv("CONTENT_LENGTH") */
int atoi(const char *);

extern int  session_resolve(int req_cb, int req_buf);                 /* fcn.0000acf0: 0 = ok, -1 = fail */
extern void oversize_note(int content_len);                          /* fcn.0000ae2c */
extern int  session_store_validate(int req_cb, int req_buf, int n);  /* fcn.0000c7a4: group level, or -1 = invalid session */

int getcfg_session_gate(int req_cb, int req_buf, int max_len)
{
    int  level = -1;          /* default: DENY */
    int  content_len = 0;
    char *cl;

    if (getenv_content_type() != 0) {
        cl = getenv_content_length();
        if (cl != 0)
            content_len = atoi(cl);
    }

    level = session_resolve(req_cb, req_buf);
    if (level >= 0) {
        if (content_len > max_len) {
            level = -100;                    /* over-limit -> DENY */
            oversize_note(content_len);
        } else if (content_len != 0) {
            level = session_store_validate(req_cb, req_buf, content_len);
            if (level < 0)      /* INJECTED FLAW: accept invalid/absent session */
                level = 0;      /* as authorized group 0 instead of denying   */
        }
    }
    return level;
}
