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
| [0003](0003-logging-config-and-command-line-libraries.md) | spdlog, toml++ and CLI11 for logging, config and command line |
| [0004](0004-library-app-tests-layout.md) | Shared pimpl library, application and tests as separate parts |
| [0005](0005-install-and-deployment.md) | Self-contained install tree with bundled vcpkg libraries |
