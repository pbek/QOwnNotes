import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script provides custom OpenAI backends
 */
Script {
    function openAiBackendsHook() {
        const result = [
            {
                "id": "my-custom-ai"
            },
            {
                "id": "my-custom-ai2"
            }
        ];

        return result;
    }
}
