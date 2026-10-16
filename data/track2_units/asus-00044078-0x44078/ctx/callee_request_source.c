// Attacker-input sources in fcn.00044078 (the ASUS JSON "apply" handler).
// arg2 is the parsed request object; auStack_a4 is the current variable NAME
// (built by snprintf from the nvram defaults table), pcVar8 is the VALUE.

// func_0x0001218c(name, arg2) == webs/json get-string
int fcn.0001218c(name, arg2) {
    if (json_object_get_type(arg2)==4 && json_object_object_length(arg2)!=0) {
        if (json_object_object_get_ex(arg2, name, &val, ...)) {
            if (!json_object_is_type(val,4))
                return json_object_get_string(val);   // <-- request value
        }
    }
}

// func_0x00012140(name) == CGI param lookup via hsearch_r (webs_get)
uint fcn.00012140(name) {
    if (**0x12188 != 0) {
        hsearch_r(name, ..., *0x12188);
        return entry->data;                            // <-- request value
    }
}

// WHERE THE REQUEST VALUE (pcVar8) GOES in the body:
//   nvram_set(auStack_a4, pcVar8)           <- config storage, NOT a shell
//   nvram_set_int / nvram_set(fd, pcVar8)   <- config storage
//   cprintf / dbg / strcmp / strstr / atoi  <- compare / log / numeric
//   set_ovpn_key(...) , json_object_*       <- config / json
// The request value never becomes a shell command line. The single command
// sink doSystem(*0x45210) takes a fixed constant, not pcVar8.
