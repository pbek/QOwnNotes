import QtQml 2.0

/**
 * This script will encrypt and decrypt notes with PGP
 */
QtObject {
    /**
     * This function is called when text has to be encrypted or decrypted
     * 
     * @param text string the text to encrypt or descrypt
     * @param password string the password
     * @param decrypt bool if false encryption is demanded, if true decryption is demanded
     * @return the exncrypted or decrypted text
     */
    function encryptionHook(text, password, decrypt) {
        script.log("password");
        script.log(password);
        script.log("before");
        script.log(text);
        
        // do the rot13 transformation
        text = text.replace(/[a-zA-Z]/g, function(c){
            return String.fromCharCode((c <= "Z" ? 90 : 122) >= (c = c.charCodeAt(0) + 13) ? c : c - 26);
        });
        
        script.log("after");
        script.log(text);

        return text;
    }
}
