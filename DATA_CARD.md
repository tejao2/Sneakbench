# Data card

## Task 1: `data/track1_instances.jsonl`
One JSON object per line, 175 instances. Each is a single decompiled function.

| field | meaning |
|---|---|
| `id` | unique instance id (`<category>/<unit>/<address>`) |
| `body` | the decompiled function shown to the model (comments stripped at load time) |
| `category` | `cve`, `synthetic-original`, `synthetic-mutant`, `hardneg`, `injection-supp`, `qnap-inj` |
| `gt_track1` | ground-truth label: `vulnerable`, `not-vulnerable`, or `need-more-context` |
| `gt_terminal` | the Task 2 terminal verdict for the parent unit (`vulnerable` / `not-vulnerable`) |
| `cwe` | CWE of the weakness, or a mutation name for synthetic items |
| `source_rel` | path of the firmware component the function comes from |
| `unit` | the parent unit this slice belongs to |
| `va` | virtual address of the function |

**Label meaning.** `vulnerable` = the flaw is complete and provable in this body.
`need-more-context` = the function is on the security path but the body alone cannot decide
(it delegates, forwards, or depends on a caller/callee/stored value). `not-vulnerable` =
plumbing, or a correct self-contained gate.

**Categories.** *CVE* slices come from confirmed CVEs. *Synthetic* pairs start from a
correct check in real firmware with one injected bug (original = not-vulnerable, variant =
vulnerable). *Hard negatives* look like the bug class but are safe once context is read.
*Command-injection* (`injection-supp`, `qnap-inj`) are command-execution sinks that count
as vulnerable only when an unauthenticated request can reach the sink.

## Task 2: `data/track2_units/<unit>/`
- `track2_block.json` — the unit definition:
  - `start_slice` / `start_body` — the function the model starts from.
  - `terminal` — ground-truth verdict (`vulnerable` / `not-vulnerable`).
  - `min_rounds` — on-target items needed beyond the start body to decide (never 0).
  - `context_menu[]` — menu items. `kind`: `on-target` (belongs to the unit, is delivered),
    `off-target` (a bait; picking it ends the run), `reshow` (shows the start body again).
    `deliver` points to the slice shown when the item is picked.
  - `reconcile_note` / `provenance` — human rationale for the label (kept for transparency;
    **not** shown to the model).
- `ctx/*.c|*.php|*.asm|*.txt` — the delivered context slices.

`data/track2_index.json` lists the `core` units, the `flip` pairs, and totals.
`data/excluded_from_scoring.txt` lists the 4 units held out of the main run (3 big-context
units too large for the model window, plus one duplicate pole). The main run scores
**41 of the 44** units; the flip set adds 45 instances.

## `data/cve_list.csv`
`cve, vendor_device, cwe, track1_slices, tasks`. `track1_slices` is the number of Task 1
instances from that CVE (0 for the four Task-2-only CVEs). `tasks` says whether the CVE
appears in `both` tracks or Task 2 only.
