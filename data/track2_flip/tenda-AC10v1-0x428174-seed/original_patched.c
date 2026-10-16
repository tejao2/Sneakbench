/* umUserCanAccessURL  @ 0x004280d4  (function start; seed source_va 0x428174 is
 *                                     0xa0 into this body, at the umGetGroupPrivilege call)
 * Tenda AC10v1_V15.03.06.46 /bin/httpd  (MIPS32, goahead-based; dynsym retained)
 * Source: um.c  ~lines 1314-1409
 *
 * POLE: NOT-VULNERABLE (original).
 *
 * Role: capability / RBAC authorization predicate. Given (user, url) it decides
 *       whether the user's group is allowed to access the URL.
 *
 * Return polarity (confirmed against the binary and the function name/structure):
 *       1 = access allowed,  0 = access denied.
 *       Every failed check returns 0; 1 is returned only when the user's group
 *       matches (or the URL sets no required group).
 *
 * start-is-NMC: the verdict is NOT computable from this body alone. It delegates
 *       to the user-management DB and the per-URL access-limit config through:
 *         umUserExists, umGetUserEnabled, umGetUserGroup,
 *         umGetGroupPrivilege, umGetGroupEnabled, umGetGroupAccessMethod,
 *         umGetAccessLimit, umGetAccessLimitMethod, umGetAccessLimitGroup.
 *       (umGetGroupPrivilege @0x42700c was confirmed to read a global DB handle
 *        *(...+0x3578) with group-table column offsets; see ctx/.)
 */

int umUserCanAccessURL(char *user, char *url)
{
    void *acl;            /* access-limit rule object for this URL   */
    char *acl_group;      /* group name the URL's rule requires      */
    char *user_group;     /* group the requesting user belongs to    */
    int   priv;           /* the user's group privilege level        */
    int   acl_method;     /* the URL rule's access method            */

    /* --- resolve and validate the requesting user (um.c:1325-1333) --- */
    if (umUserExists(user) == 0)            /* user not in the DB        */
        return 0;                           /* deny */
    if (umGetUserEnabled(user) == 0)        /* user account disabled     */
        return 0;                           /* deny */

    user_group = umGetUserGroup(user);      /* um.c:1339 */

    /* --- validate the user's group (um.c:1340-1356) --- */
    priv = umGetGroupPrivilege(user_group); /* int16 privilege level     */
    if (priv == 0)                          /* group has no privilege    */
        return 0;                           /* deny */
    if (umGetGroupEnabled(user_group) == 0)         /* group disabled    */
        return 0;                           /* deny */
    if (umGetGroupAccessMethod(user_group) == 0)    /* no access method  */
        return 0;                           /* deny */

    /* --- look up the access-limit rule for this URL (um.c:1362) --- */
    acl = umGetAccessLimit(url);
    if (acl == 0)                           /* URL is not restricted     */
        return 1;                           /* allow (public URL) */

    acl_method = umGetAccessLimitMethod(acl);   /* um.c:1364 */
    acl_group  = umGetAccessLimitGroup(acl);    /* um.c:1365 */
    bfree(acl);                                 /* um.c:1366 */

    if (acl_method == 0)                    /* rule carries no method     */
        return 0;                           /* deny */

    /* --- terminal authorization decision (um.c:1386-1407) ---
     * Allow when the rule sets no required group, or when the user's group
     * equals the group the rule requires. Otherwise deny. */
    if (acl_group == NULL || *acl_group == '\0' ||
        user_group == NULL ||
        strcmp(acl_group, user_group) == 0)     /* groups match -> allow  */
        return 1;                           /* allow */

    return 0;                               /* deny: user's group is not the required group (um.c:1398) */
}
