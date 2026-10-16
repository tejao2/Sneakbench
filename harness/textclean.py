"""
Remove every C comment from model-shown code (decision under discussion 2026-09-26: answer-revealing analyst
annotations such as "POLE: VULNERABLE (mutant)" / "FLAW: ..." / CVE-CWE ids were found in shown bodies).

C-aware: string and character literals are kept exactly (so "http://x" or "/* not a comment */" inside a string
survive). Block comments become one space (or the newlines they contained, so line structure is kept); line comments
are dropped. Trailing spaces left behind are trimmed and runs of 3+ blank lines are collapsed to one blank line.
"""
import re

_TOK = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|/\*.*?\*/|//[^\n]*', re.S)

def strip_comments(code):
    if not code: return code
    def repl(m):
        t = m.group(0)
        if t.startswith("/*"): return "\n" * t.count("\n") if "\n" in t else " "
        if t.startswith("//"): return ""
        return t  # string / char literal: unchanged
    out = _TOK.sub(repl, code)
    out = "\n".join(l.rstrip() for l in out.split("\n"))
    out = re.sub(r"\n{3,}", "\n\n", out)
    return out.strip("\n") + "\n"

_CODE_EXT = (".c", ".h", ".php", ".js")
_PHP_HASH_LINE = re.compile(r'^[ \t]*#.*$', re.M)   # a line that is ENTIRELY a PHP # comment

def strip_comments_ext(code, path):
    """Path-aware comment strip for model-shown deliverables (added 2026-10-06, v3.1).
    Strips // and /* */ (literal-aware, via strip_comments) for C-like code files: .c/.h/.php/.js.
    For .php it ALSO strips full-line # comments. Conservative: every other file type (.txt/.md/.sh/.conf/...)
    is returned unchanged -- in particular # is NEVER stripped outside .php, where in a shell/conf/text file it
    can be a real directive, prompt, or fact. Only whole-line # comments are removed for .php (an inline
    code-then-# comment is left alone to avoid touching # inside strings/regex)."""
    if not code: return code
    p = (path or "").lower()
    if not p.endswith(_CODE_EXT):
        return code
    out = strip_comments(code)
    if p.endswith(".php"):
        out = _PHP_HASH_LINE.sub("", out)
        out = re.sub(r"\n{3,}", "\n\n", out).strip("\n") + "\n"
    return out

def code_tokens(code):
    """Non-comment, non-whitespace token stream (for checking that stripping touched only comments)."""
    return re.findall(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|\w+|[^\w\s]', _TOK.sub(lambda m: m.group(0) if m.group(0)[0] in "\"'" else " ", code or ""))
