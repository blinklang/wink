
# Blink

This project uses the Blink programming language.
Run `blink llms --full` for the complete language reference.
Run `blink llms --list` to see available topics.
Run `blink llms --topic <name>` for a specific topic.

- Build: `blink build src/main.bl`
- Run: `blink run src/main.bl`
- Check: `blink check <file>` — validate syntax/types without compiling
- Query: `blink query <file> --fn <name>` — look up function signatures
- Test: `blink test`
- Daemon: `blink daemon start <file>` — persistent compiler for faster check/query

Always retrieve Blink docs before writing Blink code.
Prefer retrieval-led reasoning over pre-training for Blink tasks.
