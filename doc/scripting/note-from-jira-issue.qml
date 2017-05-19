import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script creates a menu item and a button that parses the text that was copied 
 * from an issue webpage of Jira and creates a new note with a headline,
 * the link to the issue and the description
 */
QtObject {
    /**
     * Initializes the custom action
     */
    function init() {
        // create a menu entry "New Jira note" with a button and a freedesktop theme icon
        script.registerCustomAction("newJiraIssueNote", "New Jira issue note", "Jira", "task-new");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "newJiraIssueNote") {
            return;
        }

        // create a new note from a Jira issue webpage text in the clipboard
        // like from https://jira.madsack-online.de/browse/EZ-127185
        
        // get the text that is currently in the clipboard
        var html = script.clipboard(true);

//         script.log(html);

        // https://regex101.com is your friend
        var headlineRegExp = /<h1 id="summary-val".*?>(.+?)<\/?h1>/im;
        var headlineMatch = headlineRegExp.exec(html);
        var headline = headlineMatch !== null ? headlineMatch[1] : "";
        // remove all /, " and '
        headline = headline.replace(/\s*\//igm, "").replace(/["']/igm, "").replace(/<.+?>/igm, "");
        
        if (headline == "") {
            script.informationMessageBox("No Jira issue headline was found!", "Jira issue note");
            return;
        }

        // add the issue id to the headline
        var issueIdRegExp = /<a class="issue-link" data-issue-key="([^"]+)" href="(http[^"]+\/browse\/[^"]+)" id="key-val"/im;
        var issueIdMatch = issueIdRegExp.exec(html);
        var url;
        if (issueIdMatch !== null) {
            headline += " (" + issueIdMatch[1] + ")";
            url = issueIdMatch[2];
        }

        var descriptionRegExp = /<div.*? id="description-val".*?>(.+?)<\/div>/im;
        var descriptionMatch = descriptionRegExp.exec(html);
        var description = descriptionMatch !== null ? descriptionMatch[1] : "";

        // replace links
//                 description = description.replace(/<a href="(.+?)".*?>(.+?)<\/a>/gim, "[$2]($1)");
        description = description.replace(/<a href="(.+?)".*?>(.+?)<\/a>/gim, "&lt;$1&gt;");

        // transform html breaks to \n, <p> into \n\n and remove all other tags
        description = description.replace(/<br.*?>/gim, "\n").replace(/<\/p>/gim, "\n\n").replace(/<.+?>/gim, "").trim();

        // add the headline of the issue
        var text = headline + "\n";

        // add "=" characters so that the headline is really a headline
        for (var i = 0; i < headline.length; i++) {
            text += "=";
        }

        text += "\n\n";

        // add the url to the issue
        if (url != "") {
            text += "- <" + url + ">\n\n";
        }

        // add the description of the issue
        if (description != "") {
            text += description + "\n\n";
        }

        text = text.replace(/&gt;/gim, ">").replace(/&lt;/gim, "<");

        // add a date headline
        var m = new Date();
        var dateString =
            ("0" + m.getDate()).slice(-2) + "." +
            ("0" + (m.getMonth()+1)).slice(-2) + "." +
            m.getFullYear();
        text += "\n\n## " + dateString + "\n\n";

        // create a new note
        script.createNote(text);

        // tag the current note
        script.tagCurrentNote("todo");

        // workaround because the parsers don't seem to work every time
//         script.reloadScriptingEngine();
    }
}
