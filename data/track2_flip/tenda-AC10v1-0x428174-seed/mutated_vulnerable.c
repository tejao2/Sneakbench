/* umUserCanAccessURL  @ 0x004280d4   (seed source_va 0x428174)
 * Tenda AC10v1_V15.03.06.46 /bin/httpd  (MIPS32, goahead-based)
 * Source: um.c  ~lines 1314-1409
 *
 * POLE: VULNERABLE (mutant).  CWE-285 (Improper Authorization).
 *
 * FLAW: the terminal group-match decision (um.c:1398) is changed so the
 *       group-MISMATCH branch returns 1 (allow) instead of 0 (deny). The
 *       authorization predicate now grants access to any valid, enabled user
 *       in any enabled, privileged group, regardless of whether that group is
 *       the one the URL's access-limit rule actually requires. A low-privilege
 *       user's group therefore passes a high-privilege URL's access check.
 *
 * Only the single terminal return value is changed; every other line is
 * identical to original_patched.c.
 */

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

    return 1;   /* FLAW: was `return 0` (deny on group mismatch). Now grants
                 * access even when the user's group is NOT the required group. */
}
