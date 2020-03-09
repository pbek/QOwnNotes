/* Highlight */
(function(hljs) {
    hljs.initHighlightingOnLoad();
})(hljs);

/**
 * Open external links in new browser tab
 */
(function() {
    var links = document.links;

    for (var i = 0, linksLength = links.length; i < linksLength; i++) {
        if (links[i].hostname != window.location.hostname) {
            links[i].target = '_blank';
        }
    }
})();
