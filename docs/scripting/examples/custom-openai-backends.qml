import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script provides custom OpenAI backends
 */
Script {
    /**
     * This function is called when the OpenAI service config is reloaded
     * It returns a list of objects with config parameters for new OpenAI backends
     */
    function openAiBackendsHook() {
        return [
            {
                "id": "my-custom-ai",
                "name": "My Custom AI",
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
            },
            {
                "id": "custom-groq",
                "name": "Custom Groq",
                "baseUrl": "https://api.groq.com/openai/v1/chat/completions",
                "apiKey": "gsk_Kfj477MosEC5LwKN2nrSyw4Yj4Zt44KsLn7AM5M4KQGUu87xqgX",
                "models": ["llama3-70b-8192"],
            }
        ];
    }
}
