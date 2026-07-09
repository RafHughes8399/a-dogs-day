---
name: map
description: >-
  Map all relevant codebase context before design or implementation. Use when
  the user says /map, asks to map a ticket, supplies a ticket description or
  requirements for upcoming work, or begins a feature/bug design process where
  Codex should first trace related flows, code paths, type definitions, API
  endpoints, request and response objects, database calls, schemas, state,
  integrations, tests, and owning repos before discussing possible designs or
  implementations. Produce a sourced map with file and line citations, relevant
  snippets, and clear boundaries. Do not propose or implement designs during
  the map unless the user separately asks to move past mapping.
---

# Map

## Purpose

Build the factual terrain before design starts. Given a ticket description,
requirements, notes, screenshots, errors, or acceptance criteria, trace the
related code and data flows thoroughly enough that later design discussion is
grounded in evidence rather than guesses.

This skill is a pre-design step. It should happen before `design-check`,
`fill-in-the-gaps`, pseudocode, implementation planning, or coding when the
task is non-trivial.

## Core Rule

Map first. Design later.

During `/map`, do not propose a new implementation, choose an architecture,
write code, edit files, or ask the user to approve a design. The output should
answer:

- what exists
- where it lives
- how data and control move
- which types, contracts, endpoints, schemas, and tests are involved
- which boundaries or gaps are still unknown
- what evidence supports each claim

If the user asks for mapping and design in the same message, complete the map
first, then ask whether to proceed into design discussion.

## Workflow

1. Parse the ticket and notes.
   - Extract business nouns, user actions, feature flags, error strings,
     endpoint fragments, UI labels, table names, event names, provider names,
     object fields, permissions, roles, and acceptance criteria.
   - Translate those into likely code search terms and synonyms.
   - Keep the original requirements visible so the map stays relevant.

2. Identify ownership boundaries.
   - Determine which repo, package, service, frontend surface, backend service,
     proxy, worker, shared library, or database likely owns each part.
   - Search sibling repos when the behavior plausibly crosses boundaries.
   - Explicitly mark "checked" versus "not checked" repos or layers.

3. Trace user-facing and control flows.
   - Follow entry points from UI routes/components/actions through hooks,
     state/selectors, clients, API calls, backend routes/controllers/services,
     workers/jobs, provider calls, database access, and responses back to the
     caller.
   - Include failure, loading, permission, feature-flag, validation, retry,
     fallback, cache, or async behavior when it affects the ticket.

4. Map data contracts.
   - Locate request and response shapes, TypeScript types/interfaces, runtime
     validators, OpenAPI/GraphQL schemas, DTOs, models, migrations, database
     queries, fixtures, and tests.
   - Note required versus optional fields, nullability, enum values, derived
     values, transformations, and compatibility constraints.

5. Collect evidence.
   - Cite files with exact line references.
   - Quote or paste compact snippets when a function, type, endpoint, query,
     or branch is central to understanding the flow.
   - Prefer short snippets over long dumps; include only enough code to prove
     the point.
   - If quoting a function is useful, show the relevant function body or the
     decisive branch in the chat.

6. Separate facts from inferences.
   - Label inferred ownership or flow when the code evidence is indirect.
   - Call out stale-looking code, generated code, test-only code, dead paths,
     feature flags, environment-dependent behavior, and missing local repos.
   - Do not overstate confidence when only one side of a request/response pair
     was found.

7. Stop at the map.
   - End with open questions and recommended next inspection only if evidence
     is missing.
   - If the map is complete enough for design, say that the next step is a
     design discussion or `design-check`, but do not run it unless requested.

## Search Guidance

Search behavior, not just names. Start with targeted `rg` queries and broaden
carefully:

- exact ticket terms, UI labels, errors, field names, endpoint fragments
- synonyms and historical names for the same business concept
- route definitions, API clients, controllers, services, jobs, and tests
- type names, interfaces, schemas, validators, fixtures, and generated clients
- database table/model/query/migration names
- feature flags, permissions, roles, config/env vars, metrics, logs, and events
- sibling repos under likely workspace roots when ownership is cross-service

Use current repo evidence first, then widen. When remote source search would be
useful but was not run, name that limitation.

## Output Shape

Use this shape by default:

````markdown
**Map**

Ticket / requirement readback:
- ...

Search scope:
- Current repo: ...
- Wider repos/layers: ...
- Not checked: ...

Flow map:
- Entry point: ... ([file](path:line))
- UI/state/client path: ...
- API/backend path: ...
- Data/provider/database path: ...
- Response/error path: ...

Key code and contracts:
- Endpoint/request/response: ...
- Types/interfaces/schemas: ...
- Database/models/queries: ...
- Feature flags/permissions/config: ...
- Tests/fixtures/docs: ...

Relevant snippets:
```ts
// compact snippet here
```

Ownership and boundaries:
- ...

Unknowns / follow-up inspection:
- ...

Ready for design discussion:
- Yes/No, with reason.
````

Omit categories that truly do not apply, but keep `Search scope`, `Flow map`,
`Key code and contracts`, and `Unknowns / follow-up inspection` for any
non-trivial ticket.

## Guardrails

- Do not implement, scaffold, refactor, or edit files during `/map`.
- Do not design the solution during `/map`; keep recommendations to inspection
  next steps unless the user separately asks for design.
- Do not ask the user questions before searching when the repo can answer them.
- Do not claim "nothing exists" after searching only one file, one exact term,
  or one repo when ownership might be elsewhere.
- Do not hide uncertainty. Say what was checked, what was not, and what is
  inferred.
- Do not cite files without reading them.
- Do not dump entire large files. Quote only the important snippets and cite the
  rest.
