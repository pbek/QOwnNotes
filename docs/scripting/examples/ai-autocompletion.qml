import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is an example for using the AI completer to complete the current
 * line and adding text to the autocompletion list
 */
Script {
    /**
     * Hook to feed the autocompletion AI generated text
     */
    function autocompletionHook() {
        const text = script.noteTextEditCurrentBlock();

        return script.aiComplete("Complete this sentence, don't add the text from the prompt: " + text);
    }
}
