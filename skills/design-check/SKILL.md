---
name: design-check
description: >-
  Run an umbrella pre-implementation design audit and conceptual design review
  before coding or reviewing a proposed change. Use when the user says
  design-check, /design-check, preflight, pressure-test this design,
  sanity-check this plan, review this design, or asks whether a feature,
  implementation approach, UI/state/API shape, helper boundary, data contract,
  or repo ownership decision is ready to build. Combine the behaviors of map,
  programming-tasks, fill-in-the-gaps, do-we-have-this-already,
  keep-it-simple-stupid, and planning: map relevant flows, code, contracts,
  endpoints, data access, and ownership first; search for existing reusable
  patterns; identify missing decisions; challenge risky assumptions; scrutinise
  the design like a strict senior developer reviewing a graduate engineer;
  grade the concept on a scale out of 100; offer alternatives, extensions, and
  improvements; check whether the same outcome can be achieved with less; and
  return a concise design check report. Do not implement during this check
  unless the user separately asks to proceed after the decisions are settled.
---

# Design Check

## Purpose

Run a deliberate pre-code audit for implementation ideas. This is an umbrella
check over the user's collaboration skills: map the relevant code and data
flows, inspect the codebase, find reuse, map gaps, pressure-test the design,
review the conceptual design, and ask for the user's decisions before edits
begin.

Use this skill to stop the common failure mode where Codex recognizes a relevant
planning skill but still drifts into designing or coding unapproved details.

## Core Rule

Design-check is a gate, not an implementation pass.

Unless the user explicitly asks to continue into code after the check, stop at:

- evidence gathered
- existing patterns or reusable code found
- missing decisions
- risks and tradeoffs
- recommended next questions or implementation slices

Do not edit files, scaffold code, change tests, or make commits as part of the
check.

## Workflow

0. Map the existing terrain.
   - Given the ticket, notes, or proposed change, first trace related flows,
     code paths, type definitions, API endpoints, request and response objects,
     database calls, schemas, state, integrations, tests, and owning repos.
   - Cite files and lines for key evidence, and include compact snippets when a
     function, type, endpoint, query, or branch is central to the design.
   - Keep facts separate from inferences, and mark any repos/layers that were
     not checked.
   - Do not start design discussion until this map exists.

1. Classify the design surface.
   - Identify whether the task touches architecture, repo ownership, APIs,
     data contracts, helper boundaries, TypeScript types, React components,
     hooks/state/effects, UI behavior, validation, tests, rollout, or failure
     handling.
   - If it is a tiny fully specified mechanical edit, say design-check is not
     needed and answer directly.

2. Inspect enough local context.
   - Read nearby code, tests, routes, schemas, components, hooks, selectors,
     service clients, and docs before asking questions when the repo can answer.
   - Trace the current behavior through the owning layer rather than assuming
     the file in front of you is the source of truth.
   - For branch-level or ticket-level work, inspect git status/diff and the
     supplied ticket or acceptance criteria if available.

3. Run reuse discovery.
   - Search for the capability, not just the exact name.
   - Check current repo first, then likely sibling repos when ownership could
     plausibly sit elsewhere.
   - Look for helpers, endpoints, provider integrations, type shapes, UI/state
     patterns, validation logic, feature flags, and tests that can be reused or
     adapted.
   - If remote search would be useful but was not checked, say so.

4. Fill in the gaps.
   - Name every unresolved decision that would change the code.
   - Include small implementation details when they matter: function params,
     return shape, error behavior, optionality, nullability, hook dependencies,
     memoization, derived vs stored state, prop ownership, cache keys, fallback
     behavior, and validation level.
   - Do not accept vague approval as a substitute for these choices.

5. Pressure-test the plan.
   - Challenge assumptions with repo evidence.
   - Separate "this exists and should be reused" from "this is a pattern only".
   - Identify wrong-layer risks, coupling, backwards compatibility issues,
     async/failure edge cases, stale data, hidden consumers, and test gaps.
   - Recommend a conservative implementation slice only after the unknowns are
     visible.

6. Run the design review pass.
   - Switch into a strict senior-reviewer posture. Treat the user as a graduate
     engineer whose design should be improved, not merely approved.
   - Evaluate the concept, not the code. Judge architecture, ownership,
     contracts, data flow, state model, dependency direction, rollout path,
     observability, failure behavior, testability, maintainability, and whether
     the design fits the existing system.
   - Grade the design with a score out of 100 plus a short verdict, such as
     "82/100, mostly sound but underspecified", "64/100, overbuilt",
     "48/100, wrong layer", "55/100, risky", or "not ready".
   - Justify the score with concrete evidence. Penalise missing ownership,
     unproven assumptions, unclear contracts, avoidable complexity, weak
     failure handling, and missing validation. Do not give high scores for
     designs that cannot yet be checked.
   - Be strict and specific. Call out weak assumptions, missing proof, hidden
     coupling, ambiguous ownership, speculative scope, and places where the
     design is likely to age badly.
   - Offer alternatives when the proposed design is not the best option. For
     each serious alternative, state the tradeoff and when it would be better.
   - Suggest extensions and improvements only when they protect correctness,
     reduce risk, improve maintainability, or prepare for a known near-term
     requirement. Label nice-to-have ideas as optional.
   - Separate blocking design flaws from advisory feedback and nits.

7. Run the simplicity check.
   - Ask whether the same required outcome can be achieved with fewer files,
     fewer repos, fewer database changes, fewer new or edited types, fewer API
     contract changes, fewer helpers, or less state.
   - Name which moving parts are necessary, which are optional, and which look
     speculative.
   - Prefer the smallest viable design only when it preserves correctness,
     ownership, failure behavior, and future maintainability.

8. Ask for direction.
   - Ask concrete questions that let the user conduct the design.
   - Offer options when helpful, but make clear the user chooses.
   - If enough is already settled, summarize the agreed implementation shape and
     ask whether to proceed into coding.

## Output Shape

Use this shape by default:

```markdown
**Design Check**

Scope:
- ...

Evidence checked:
- Map: ...
- Current repo: ...
- Wider search: ...
- Ticket/branch context: ...

Existing pieces / reuse:
- Direct reuse: ...
- Pattern to adapt: ...
- Nothing found: ...

Gaps that still change the code:
- Architecture/ownership: ...
- Data/contracts: ...
- Functions/helpers: ...
- Types/interfaces: ...
- UI/components/hooks/state: ...
- Control flow/failure cases: ...
- Validation/tests: ...

Risks / pushback:
- ...

Design review:
- Grade: .../100, ...
- Blocking issues: ...
- Advisory feedback: ...
- Alternatives: ...
- Extensions / improvements: ...

Can we do this with less?
- ...

Recommended next slice:
- ...

Questions:
1. ...
2. ...
3. ...
```

Omit empty sections, but do not omit "Evidence checked", "Existing pieces /
reuse", or "Gaps that still change the code" when the task is non-trivial.

## Trigger Companion Skills

When this skill triggers, also apply the relevant behavior from these local
skills without needing the user to name them separately:

- `map`: first map relevant flows, code paths, contracts, endpoints, data
  access, tests, and ownership before design discussion
- `programming-tasks`: collaborative implementation shaping before edits
- `fill-in-the-gaps`: strict interrogation of missing design details
- `do-we-have-this-already`: reuse and ownership discovery
- `keep-it-simple-stupid`: simpler-design pressure test across files, repos,
  database changes, types, contracts, helpers, and state
- `senior-grad-code-review`: senior-review posture, adapted from code review to
  conceptual design review rather than diff review
- `state-of-play`: branch/ticket completion snapshot, only when the user asks
  whether a current branch is ready or where it stands

Do not merely mention those skills. Perform their checks and report the result.

## Guardrails

- Do not implement during design-check unless the user separately confirms the
  implementation shape and asks for edits.
- Do not ask questions that a quick local inspection can answer.
- Do not claim something does not exist after searching only one file or one
  exact name.
- Do not invent ownership, API contracts, state shape, hook behavior, helper
  boundaries, or validation strategy silently.
- Do not produce a long generic checklist. Anchor the report in the actual
  code, ticket, branch, or design under discussion.
- If the check is blocked by missing ticket text, unavailable repos, or tool
  access, say exactly what could not be checked and continue with the evidence
  available.
