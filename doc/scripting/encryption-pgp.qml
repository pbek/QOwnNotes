import QtQml 2.0

/**
 * This script will encrypt and decrypt notes with PGP
 * 
 * You have to use your own public key instead of "F5161BD3"
 * Decryption only will work if you don't have to enter a password
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
        // encrypt the text for public key "F5161BD3" or decrypt with gpg
        // decryption only will work if you don't have to enter a password
        var param = decrypt ? ["--decrypt"] : ["--encrypt", "--armor", "-r", "F5161BD3"];
        var result = script.startSynchronousProcess("/usr/bin/gpg", param, text);
        return result;
    }
}
