int getenv_content_type(void);
char *getenv_content_length(void);
int atoi(const char *);

extern int  session_resolve(int req_cb, int req_buf);
extern void oversize_note(int content_len);
extern int  session_store_validate(int req_cb, int req_buf, int n);

int getcfg_session_gate(int req_cb, int req_buf, int max_len)
{
    int  level = -1;
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
            level = -100;
            oversize_note(content_len);
        } else if (content_len != 0) {
            level = session_store_validate(req_cb, req_buf, content_len);
            if (level < 0)
                level = 0;
        }
    }
    return level;
}
