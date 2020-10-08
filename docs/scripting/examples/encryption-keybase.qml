import QtQml 2.0

/**
 * This script will encrypt and decrypt notes with https://keybase.io
 * 
 * You have to use your keybase user instead of "pbek"
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
        // encrypt or decrypt text with keybase.io for user pbek
        var param = decrypt ? ["decrypt"] : ["encrypt", "pbek"];
        var result = script.startSynchronousProcess("/usr/bin/keybase", param, text);
        return result;
    }
}
