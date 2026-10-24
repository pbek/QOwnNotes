# External Git log command

The external Git log command in the Git settings is an executable followed by arguments.
It is launched directly, without a shell, on every platform.

Examples:

- `gitk --`
- `gitk -- %notePath%`
- `"/path with spaces/git viewer" -- "%notePath%"`
- `"C:\Program Files\Git\cmd\gitk.exe" -- "%notePath%"`

`%notePath%` must be a separate argument, optionally quoted. If it is absent, the note
path is appended as one argument. Filenames are never parsed as command text. A relative
path beginning with `-` is prefixed with `./` so it is not interpreted as an option.

Single or double quotes group text containing spaces. Backslashes are literal, including
in Windows paths. Unmatched quotes and placeholders embedded in other text are rejected.

Shell pipelines, redirections, environment-variable expansion, and inline shell-script
templates are not supported. Existing configurations relying on implicit shell execution
must be changed to an executable and arguments. For more complex behavior, use a wrapper
script that accepts the note path as a positional argument and handles it as data (for
example, `gitk -- "$1"` inside a POSIX shell script). Do not evaluate the path as shell code.
