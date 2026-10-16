int umUserCanAccessURL(char *user, char *url)
{
    void *acl;
    char *acl_group;
    char *user_group;
    int   priv;
    int   acl_method;

    if (umUserExists(user) == 0)
        return 0;
    if (umGetUserEnabled(user) == 0)
        return 0;

    user_group = umGetUserGroup(user);

    priv = umGetGroupPrivilege(user_group);
    if (priv == 0)
        return 0;
    if (umGetGroupEnabled(user_group) == 0)
        return 0;
    if (umGetGroupAccessMethod(user_group) == 0)
        return 0;

    acl = umGetAccessLimit(url);
    if (acl == 0)
        return 1;

    acl_method = umGetAccessLimitMethod(acl);
    acl_group  = umGetAccessLimitGroup(acl);
    bfree(acl);

    if (acl_method == 0)
        return 0;

    if (acl_group == NULL || *acl_group == '\0' ||
        user_group == NULL ||
        strcmp(acl_group, user_group) == 0)
        return 1;

    return 1;

}
