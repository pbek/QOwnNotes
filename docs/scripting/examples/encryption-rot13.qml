import QtQml 2.0

/**
 * This script will "encrypt" and "decrypt" notes with ROT13
 */
QtObject {
    /**
     * This is called when the script is loaded by QOwnNotes
     */
    function init() {
        // disable the password dialog
        script.encryptionDisablePassword();
    }

    /**
     * This function is called when text has to be encrypted or decrypted
     * 
     * @param text string the text to encrypt or decrypt
     * @param password string the password
     * @param decrypt bool if false encryption is demanded, if true decryption is demanded
     * @return the encrypted decrypted text
     */
    function encryptionHook(text, password, decrypt) {
        // do the rot13 transformation
        text = text.replace(/[a-zA-Z]/g, function(c){
            return String.fromCharCode((c <= "Z" ? 90 : 122) >= (c = c.charCodeAt(0) + 13) ? c : c - 26);
        });
        
        return text;
    }
}
