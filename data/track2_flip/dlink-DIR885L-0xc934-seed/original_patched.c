/* SOURCE: D-Link DIR-885L_REVA_1.10B01  /htdocs/cgibin  (ELF ARM, stripped)
 * FUNCTION: fcn.0000c934  (r2 auto-name; getcfg-family session gate)
 * POLE: not-vulnerable (original, sound gate). Static reconstruction from
 *       radare2/r2ghidra pdg + pdf. Semantics verified against the binary.
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
 * DELEGATION (start-is-NMC): the grant/deny cannot be decided from this body
 *   alone. session_resolve() (fcn.0000acf0) reads REQUEST_URI and parses the
 *   request-side session id; session_store_validate() (fcn.0000c7a4) dispatches
 *   on CONTENT_TYPE and validates against the external session store, returning
 *   the stored group level or a negative on an invalid session.
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
            /* Validate the request against the session store. A negative
             * result means the session is absent / invalid, and the gate
             * returns it unchanged so the caller denies (fails closed). */
            level = session_store_validate(req_cb, req_buf, content_len);
        }
    }
    return level;
}
