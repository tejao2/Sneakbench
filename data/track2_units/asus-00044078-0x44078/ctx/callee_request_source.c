int fcn.0001218c(name, arg2) {
    if (json_object_get_type(arg2)==4 && json_object_object_length(arg2)!=0) {
        if (json_object_object_get_ex(arg2, name, &val, ...)) {
            if (!json_object_is_type(val,4))
                return json_object_get_string(val);
        }
    }
}

uint fcn.00012140(name) {
    if (**0x12188 != 0) {
        hsearch_r(name, ..., *0x12188);
        return entry->data;
    }
}
