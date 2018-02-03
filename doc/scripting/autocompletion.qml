import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is an example for adding text to the autocompletion list
 */
Script {
    /**
     * Hook to feed the autocompletion with tags if the current word starts with an "@"
     */
    function autocompletionHook() {
        // get the current word plus non-word-characters before the word to also get the "@"-character
        var word = script.noteTextEditCurrentWord(true);

        if (!word.startsWith("@")) {
            return [];
        }

        // cut the "@" off of the string and do a substring search for tags
        var tags = script.searchTagsByName(word.substr(1));
        return tags;
    }
}
