# Decision records

Each file records one design decision: why it was made, what was decided and what follows from it. Records are numbered and never rewritten. If a decision changes, add a new record and set the old one's status to `superseded by NNNN`.

Template:

```markdown
# N. Title

- Status: proposed | accepted | superseded by NNNN
- Date: YYYY-MM-DD

## Context
## Decision
## Consequences
```

| # | Decision |
|---|---|
| [0001](0001-dynamic-linking-via-vcpkg-triplets.md) | Link vcpkg dependencies dynamically |
| [0002](0002-documentation-doxygen-and-markdown.md) | Doxygen for the API, Markdown for design docs |
