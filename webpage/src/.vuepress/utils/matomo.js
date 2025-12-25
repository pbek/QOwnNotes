/**
 * Matomo Analytics Integration for VuePress 2
 *
 * This replaces the old vuepress-plugin-matomo plugin.
 * Configuration equivalent to:
 * {
 *   siteId: 7,
 *   trackerUrl: "https://p.bekerle.com/",
 *   trackerJsFile: "matomo.js"
 * }
 */

export const initMatomo = (router) => {
  // Only run in browser
  if (typeof window === "undefined") return;

  // Configuration
  const config = {
    siteId: 7,
    trackerUrl: "https://p.bekerle.com/",
    trackerJsFile: "matomo.js",
  };

  // Initialize Matomo
  window._paq = window._paq || [];
  window._paq.push(["trackPageView"]);
  window._paq.push(["enableLinkTracking"]);

  (function () {
    const u = config.trackerUrl;
    window._paq.push(["setTrackerUrl", u + "matomo.php"]);
    window._paq.push(["setSiteId", config.siteId.toString()]);

    const d = document;
    const g = d.createElement("script");
    const s = d.getElementsByTagName("script")[0];
    g.async = true;
    g.src = u + config.trackerJsFile;
    s.parentNode.insertBefore(g, s);
  })();

  // Track page changes on route navigation (SPA support)
  router.afterEach((to) => {
    if (window._paq) {
      window._paq.push(["setCustomUrl", window.location.href]);
      window._paq.push(["setDocumentTitle", document.title]);
      window._paq.push(["trackPageView"]);
    }
  });
};
