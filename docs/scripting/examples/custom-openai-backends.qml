import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script provides custom OpenAI backends
 */
Script {
    function openAiBackendsHook() {
        const result = [
            {
                "id": "my-custom-ai",
                "name": "My Custom AI12",
                "baseUrl": "http://localhost:5000",
                "apiKey": "kDFJkjk3asdm",
                "models": ["gpt-3.5-turbo", "gpt-4.0-turbo"],
            },
            {
                "id": "my-custom-ai2",
                "name": "My Custom AI 2",
                "baseUrl": "http://localhost:5001",
                "apiKey": "lOikf7eNdb9",
                "models": ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
            }
        ];

        return result;
    }
}
