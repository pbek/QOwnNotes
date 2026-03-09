/*
  Homepage custom.js integration for QOwnNotes suggestions.

  What this script does:
  1) Intercepts Homepage search-suggestion API responses.
  2) Fetches QOwnNotes suggestions from QON_URL and merges them.
  3) Prefixes QOwnNotes suggestions with QON_LABEL (for visibility).
  4) Maps displayed suggestion text to bookmark URLs and opens those URLs
     directly instead of sending them to a search engine query.
  5) Prevents Homepage's client-side 4-item trimming for merged suggestions.
  6) Adds a CSS hook class (qon-suggestion) to matching suggestion rows.

  Tune behavior with:
  - QON_URL: QOwnNotes suggestion endpoint
  - QON_LIMIT: number of QOwnNotes suggestions to request
  - QON_LABEL: marker shown in UI for QOwnNotes items
  - DEBUG: set true to print debug logs
*/

(() => {
  "use strict";

  const QON_URL = "http://127.0.0.1:22224/suggest?q=";
  const QON_LIMIT = 20;
  const QON_LABEL = "[QON] ";
  const DEBUG = false;

  const log = (...a) => {
    if (DEBUG) console.log("[QON-MERGE]", ...a);
  };

  const origFetch = window.fetch.bind(window);
  const origOpen = window.open.bind(window);
  const origResponseJson = Response.prototype.json;
  const suggestionToUrl = new Map();

  const norm = (s) => (typeof s === "string" ? s.trim().toLowerCase() : "");

  const itemText = (v) => {
    if (typeof v === "string") return v;
    if (!v || typeof v !== "object") return "";
    return v.label || v.title || v.value || v.text || v.name || "";
  };

  function dedupeItems(arr) {
    const out = [];
    const seen = new Set();
    for (const v of arr) {
      const t = itemText(v);
      if (!t) continue;
      const k = norm(t);
      if (!k || seen.has(k)) continue;
      seen.add(k);
      out.push(v);
    }
    return out;
  }

  function dedupeStrings(arr) {
    return dedupeItems(arr)
      .map((v) => itemText(v))
      .filter((s) => typeof s === "string" && s.length > 0);
  }

  function isHttpUrl(s) {
    return typeof s === "string" && /^https?:\/\//i.test(s.trim());
  }

  function buildQownSuggestionsAndMap(values) {
    const out = [];
    const list = dedupeStrings(values);

    for (let i = 0; i < list.length; i += 1) {
      const current = list[i];
      const next = list[i + 1];

      if (!isHttpUrl(current) && isHttpUrl(next)) {
        const key = norm(current);
        const marked = `${QON_LABEL}${current}`;
        if (key) {
          suggestionToUrl.set(key, next);
          suggestionToUrl.set(norm(marked), next);
          out.push(marked);
        }
        i += 1;
        continue;
      }

      if (!isHttpUrl(current)) {
        out.push(`${QON_LABEL}${current}`);
        continue;
      }

      const marked = `${QON_LABEL}${current}`;
      suggestionToUrl.set(norm(marked), current);
      out.push(marked);
    }

    return dedupeStrings(out);
  }

  async function fetchQown(query) {
    try {
      const r = await origFetch(
        `${QON_URL}${encodeURIComponent(query)}&limit=${QON_LIMIT}`,
      );
      const txt = await r.text();
      const j = JSON.parse(txt);
      const raw =
        Array.isArray(j) && Array.isArray(j[1])
          ? j[1].filter((x) => typeof x === "string")
          : [];
      return buildQownSuggestionsAndMap(raw);
    } catch (e) {
      log("QOwnNotes fetch error:", e);
      return [];
    }
  }

  function normalizeHomepageSuggestions(payload, fallbackQuery) {
    if (
      Array.isArray(payload) &&
      payload.length >= 2 &&
      Array.isArray(payload[1])
    ) {
      return {
        query: typeof payload[0] === "string" ? payload[0] : fallbackQuery,
        suggestions: dedupeStrings(payload[1]),
      };
    }

    if (Array.isArray(payload)) {
      return { query: fallbackQuery, suggestions: dedupeStrings(payload) };
    }

    if (payload && typeof payload === "object") {
      for (const key of ["suggestions", "data", "results", "items"]) {
        if (Array.isArray(payload[key])) {
          return {
            query:
              typeof payload.query === "string" ? payload.query : fallbackQuery,
            suggestions: dedupeStrings(payload[key]),
          };
        }
      }
    }

    return { query: fallbackQuery, suggestions: [] };
  }

  function mergePayload(payload, qownStrings, query) {
    const normalized = normalizeHomepageSuggestions(payload, query);
    return [
      normalized.query,
      dedupeStrings([...qownStrings, ...normalized.suggestions]),
    ];
  }

  function isHomepageSuggestionUrl(rawUrl) {
    try {
      const u = new URL(rawUrl, window.location.origin);
      return /\/api\/search\/searchsuggestion(s)?$/i.test(u.pathname);
    } catch {
      return false;
    }
  }

  function decodeIfSearchEngineRedirect(rawUrl) {
    try {
      const u = new URL(rawUrl, window.location.origin);
      const host = u.hostname.toLowerCase();
      const isSearchHost =
        host.includes("google.") ||
        host.includes("bing.") ||
        host.includes("duckduckgo.") ||
        host.includes("baidu.") ||
        host.includes("brave.") ||
        host.includes("ecosia.");

      if (!isSearchHost) return null;

      const candidate =
        u.searchParams.get("q") ||
        u.searchParams.get("query") ||
        u.searchParams.get("wd") ||
        "";
      if (!candidate) return null;

      const decoded = decodeURIComponent(candidate).trim();
      const mapped = suggestionToUrl.get(norm(decoded));
      if (mapped && /^https?:\/\//i.test(mapped)) return mapped;

      if (!/^https?:\/\//i.test(decoded)) return null;

      const target = new URL(decoded);
      if (!/^https?:$/i.test(target.protocol)) return null;
      return target.toString();
    } catch {
      return null;
    }
  }

  function markQonSuggestionRows() {
    const rows = document.querySelectorAll("li > button");
    for (const row of rows) {
      const text = row.textContent || "";
      if (text.includes(QON_LABEL)) {
        row.classList.add("qon-suggestion");
      }
    }
  }

  window.open = function (url, target, features) {
    const raw = typeof url === "string" ? url : String(url ?? "");
    const direct = decodeIfSearchEngineRedirect(raw);
    if (direct) return origOpen(direct, target, features);
    return origOpen(url, target, features);
  };

  Response.prototype.json = async function (...args) {
    const data = await origResponseJson.apply(this, args);
    try {
      if (
        this.headers?.get("X-QON-MERGED") === "1" &&
        Array.isArray(data) &&
        Array.isArray(data[1])
      ) {
        const fullList = [...data[1]];
        const nativeSplice = Array.prototype.splice;
        data[1].splice = function (start, deleteCount, ...items) {
          if (start === 0 && deleteCount === 4 && items.length === 0) {
            return [...fullList];
          }
          return nativeSplice.call(this, start, deleteCount, ...items);
        };
      }
    } catch (e) {
      log("Response.json patch error:", e);
    }
    return data;
  };

  window.fetch = async function (input, init) {
    const url = typeof input === "string" ? input : input?.url || "";
    if (!isHomepageSuggestionUrl(url)) return origFetch(input, init);

    const res = await origFetch(input, init);

    try {
      const u = new URL(url, window.location.origin);
      const query =
        u.searchParams.get("query") ||
        u.searchParams.get("q") ||
        u.searchParams.get("term") ||
        "";

      const raw = await res.clone().text();
      let payload;
      try {
        payload = JSON.parse(raw);
      } catch {
        return res;
      }

      const qown = await fetchQown(query);
      const merged = mergePayload(payload, qown, query);

      const headers = new Headers(res.headers);
      headers.set("Content-Type", "application/json");
      headers.set("X-QON-MERGED", "1");

      return new Response(JSON.stringify(merged), {
        status: res.status,
        statusText: res.statusText,
        headers,
      });
    } catch (e) {
      log("Merge error:", e);
      return res;
    }
  };

  const observer = new MutationObserver(markQonSuggestionRows);
  observer.observe(document.documentElement, {
    childList: true,
    subtree: true,
  });
  markQonSuggestionRows();
})();
