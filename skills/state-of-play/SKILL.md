---
name: state-of-play
description: Use when the user runs or asks for state-of-play, /state-of-play, or a current branch status compared with a supplied ticket. Summarize the current branch against the ticket, including completion percentage, correctness status, immediate next steps, and remaining TODOs. Inspect the branch, diff, ticket context, tests or checks only as appropriate, and produce a concise at-a-glance status report.
---

# State Of Play

## Purpose

Give the user a concise but evidence-based snapshot of where the current branch stands against the supplied ticket. This is a branch health and completion report, not a full PR review unless the user asks for one.

## Inputs

Before reporting, identify:

- current repo, branch, base branch, and working tree status
- supplied ticket text, ticket link, PR description, local handoff, or user-provided acceptance criteria
- changed files and diff against the relevant base branch
- relevant test/check status if already available or clearly cheap and appropriate to run

If no ticket or acceptance criteria were supplied in the thread or repo context, ask the user for the ticket before giving a completion percentage.

## Workflow

1. Orient on branch state.
   - Run lightweight git inspection: branch name, status, changed files, and diff/stat against the base branch.
   - Prefer existing PR/ticket context if available in the thread, local docs, branch name, or PR description.
   - Do not run heavy test/lint/typecheck commands unless the user asked, the branch is near completion, or correctness cannot be assessed without them.

2. Compare against the ticket.
   - Break the ticket into explicit requirements and acceptance criteria.
   - Map each requirement to current implementation evidence: done, partial, missing, unclear, or blocked.
   - Separate ticket completion from code correctness. A branch can be mostly complete but still correctness-risky.

3. Assess completion percentage.
   - Use a rounded percentage in 5-10% increments.
   - Base it on requirements completed, integration wiring, validation evidence, and remaining risk.
   - Do not claim 100% unless requirements are implemented, wired, and have reasonable validation evidence or the user explicitly accepts no validation.

4. Assess correctness status.
   Use one of:
   - `Looks correct`
   - `Mostly correct, minor gaps`
   - `Partially correct, material gaps`
   - `Correctness unclear`
   - `Likely incorrect`

   Give the status in plain language and name the evidence or uncertainty behind it.

5. Produce the report.
   Keep it skimmable and practical. Lead with the useful answer, then evidence.

## Output Shape

Use this structure unless the user asks for a different format:

```markdown
**State Of Play**

Completion: 75%
Correctness: Mostly correct, minor gaps
Branch: `feature/foo`
Compared Against: `PP-1234` / ticket summary

**At A Glance**
- Done: ...
- Partial: ...
- Missing: ...
- Risk: ...

**Immediate Steps**
1. ...
2. ...
3. ...

**Remaining TODO**
- ...
- ...

**Evidence Checked**
- Diff/status: ...
- Ticket/PR context: ...
- Validation: ...
```

## Guardrails

- Be direct. The user wants the branch state, not a long essay.
- Do not hide uncertainty. If the ticket is missing, stale, or vague, say so.
- Do not inflate completion percentages because the diff is large.
- Do not bury blockers under generic TODOs; call them out.
- Do not stage, commit, push, or modify files during state-of-play unless the user separately asks.
- If you discover an obvious bug while preparing the status, report it as a correctness risk and immediate step; do not fix it unless asked.
