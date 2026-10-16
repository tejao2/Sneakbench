# Exact prompts (generated from the code: run_track1.py, run_track2.py)

## Track 1 (one new conversation per function)

**System:**
> You are auditing one decompiled firmware function for an authentication or authorization logic weakness. Decide from THIS function body alone.

**User:** `<decompiled function body>` followed by:
> Answer with exactly one label: vulnerable, need-more-context, or not-vulnerable. The FIRST line must be exactly 'LABEL: <label>'. Then give one or two lines of reasoning. vulnerable = the flaw is complete and provable inside this body. need-more-context = the function is on the security path but the body alone cannot decide (it delegates, forwards, or depends on a caller/callee/external state). not-vulnerable = plumbing, or a correct self-contained gate.

**Follow-up, only if the answer is cut off or has no label (row marked forced):**
> Stop analysing. Reply with only one line: 'LABEL: <vulnerable|need-more-context|not-vulnerable>'.

Label rule: the LAST `LABEL:` line in the final answer (text after any reasoning) is the prediction.

## Track 2 (one new conversation per unit)

**System:**
> You are auditing one decompiled firmware function for an authentication or authorization logic weakness: a privileged operation that a wrong or unauthenticated party can reach. This includes pre-authentication command injection (attacker input reaching a shell command with no authentication on the path). Command injection that requires a valid login is out of scope. Decide from the evidence you have.

**Turn 0 (user):** `<start function body>` followed by the Track-1 instruction above.

**Menu turn (user):** `Context menu:` + numbered item labels, then:
> You may request ONE item from the menu with 'PICK: <number>'.
> Give 'LABEL: vulnerable' or 'LABEL: not-vulnerable' only when you are ready to decide; that ends the investigation. Write 'UNDECIDABLE' if you believe the case cannot be decided with the evidence available; that also ends it.
> Warning: some menu items are not part of this case; picking one ends the investigation immediately and cannot be undone. There is no backtracking.
> A LABEL here is your final verdict on the whole case, using ALL evidence shown so far (not only the first body): vulnerable = the evidence shows a privileged operation reachable by a wrong or unauthenticated party; not-vulnerable = the evidence shows it is protected, or attacker input does not reach the operation.
> Reply with exactly one line: 'PICK: <number>', 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.

**After a correct pick:** `Item <n> (<label>):` + the delivered content in a code block + the menu again, delivered items marked `(already delivered)`.

**Nudge (first `LABEL: need-more-context` on a menu turn), placed before the menu:**
> need-more-context is not a final answer at this stage. To get more evidence, PICK an item from the menu; otherwise give 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.

**Forced final question, all correct items delivered:**
> You now have all the evidence available for this case. Give your final verdict in exactly one line: 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.

**Forced final question, round limit:**
> The investigation has reached its round limit. Give your final verdict in exactly one line: 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.

**Format retry (reply had no parseable move):**
> Reply with one line only: 'PICK: <number>', 'LABEL: vulnerable', 'LABEL: not-vulnerable', or 'UNDECIDABLE'.

## Best-case follow-up (only difference)

Off-target items removed from every menu, and this sentence appended to the system prompt and to every menu turn:
> The context menu for this case contains everything needed to decide it: the items that belong to the case are sufficient to reach a verdict.

## Move rules
- Last move in a reply wins; a reply with a PICK and only a need-more-context label counts as the PICK (added after the 2026-09-26 audit; affects Foundation-Sec only).
- Round cap = number of correct items + 2.
