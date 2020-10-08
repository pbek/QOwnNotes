import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Create a new note with page url, screenshot and page text if a page is
     * sent to QOwnNotes from the browser extension
     */
    function websocketRawDataHook(requestType, pageUrl, pageTitle, rawData,
                                  screenshotDataUrl) {
        // we only want to handle page requests, selection requests are ignored
        if (requestType != "page") {
            return false;
        }

        let underline = "";

        // add underline characters
        for (var i = 0; i < (pageTitle.length - 1); i++) {
            underline += "=";
        }

        // create a new note with page title as headline
        const text = pageTitle + "\n" + underline + "\ntest\n";
        script.createNote(text);
        console.log(rawData);

        // add html for page url and the screenshot to the note as markdown
        const html = "<a href=\"" + pageUrl + "\">" + pageTitle + "</a><br />" +
            "<br /><img src=\"" + screenshotDataUrl + "\" /><br /><br />";
        mainWindow.insertHtmlAsMarkdownIntoCurrentNote(html);

        // add html to the note as markdown
        // we do that separately so it the raw html of the page can't disturbe the rest of our note
        mainWindow.insertHtmlAsMarkdownIntoCurrentNote(rawData);

        return true;
    }
}
