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
            }
        ];
        script.log('openAiBackends: ' + result);

        return result;
    }
}
