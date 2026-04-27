---
allowed-tools: Bash(git push:*), Bash(git commit:*), Bash(blink build:*), Bash(blink test:*)
description: Commit and push wink changes to main
---

Create a git commit with the following workflow:

1. Run `blink build src/main.bl && blink test` to make sure all code is ready.
2. Fix any errors before proceeding.
3. IMPORTANT: DO NOT CONTINUE UNLESS THE BUILD AND TESTS PASS (exit code 0)
   - (if tests say "up to date" or "no tests changed" that means green, no need to force a rerun)
4. Stage all changes with `git add .`
5. Review `git status` and `git diff --staged` to understand what's being committed
6. Draft a commit message:
   - First line: Simple, descriptive summary (50-72 chars)
   - Body: More detailed explanation of why things changed, with a balance between useful context and brevity
   - No need to mention added or updated tests, that goes without saying
   - DO NOT mention `br` issues. (br is local only and wouldn't make sense)
7. Show me the commit message and wait for my approval
8. After approval, create the commit
9. Do a git pull to make sure current branch is up to date
10. Push directly to main: `git push`
11. Show me the final result
