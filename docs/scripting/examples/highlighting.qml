import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    function init() {
        // Highlight a text line like "BLOCK: some text" as blockquote (state 18)
        script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

        // Mask out (state 24) all characters after 32 characters in a line
        script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
    }
}
