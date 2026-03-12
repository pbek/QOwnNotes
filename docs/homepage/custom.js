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
  const BLOCK_QON_TEXT_SEARCH = "__QON_BLOCK_TEXT_SEARCH__";
  const ACTIVE_SCROLL_MARGIN_PX = 4;
  const KEYBOARD_NAV_DIALOG_CLASS = "qon-keyboard-nav";
  const DEBUG = false;

  const log = (...a) => {
    if (DEBUG) console.log("[QON-MERGE]", ...a);
  };

  const origFetch = window.fetch.bind(window);
  const origOpen = window.open.bind(window);
  const origResponseJson = Response.prototype.json;
  const suggestionToUrl = new Map();
  let applyingSuggestionUi = false;
  let keyboardNavActive = false;

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
        // Skip unmapped plain-text suggestions, because they would be searched
        // as text instead of opening a bookmark URL.
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

      if (decoded.startsWith(QON_LABEL)) {
        // Never pass QOwnNotes label text to a search engine.
        return BLOCK_QON_TEXT_SEARCH;
      }

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

    // Extract candidate text strings from a button to match against the map.
    // When HeadlessUI activates an item it may replace the DOM node entirely,
    // so we collect multiple candidate strings and try each one.
    const candidateTexts = (row) => {
      const candidates = new Set();
      candidates.add(norm(row.textContent || ""));
      for (const node of row.childNodes) {
        if (node.nodeType === Node.TEXT_NODE) {
          candidates.add(norm(node.textContent || ""));
        }
      }
      for (const span of row.querySelectorAll("span")) {
        candidates.add(norm(span.textContent || ""));
      }
      const stripped = norm(
        (row.textContent || "")
          .replace(/[^\x20-\x7E]/g, " ")
          .replace(/\s+/g, " "),
      );
      candidates.add(stripped);
      return [...candidates].filter((s) => s.length > 0);
    };

    const urlFromCandidates = (texts) => {
      for (const text of texts) {
        if (!text.includes(norm(QON_LABEL))) continue;
        const direct = suggestionToUrl.get(text);
        if (direct) return direct;
        let bestMatchKey = "";
        let bestMatchUrl = null;
        for (const [key, url] of suggestionToUrl.entries()) {
          if (!key.startsWith(norm(QON_LABEL))) continue;
          if (!text.includes(key)) continue;
          if (key.length > bestMatchKey.length) {
            bestMatchKey = key;
            bestMatchUrl = url;
          }
        }
        if (bestMatchUrl) return bestMatchUrl;
      }
      return null;
    };

    // Attach/update the URL hint on the <li> element, which React does not
    // replace when toggling the active state on the child <button>.
    const setLiUrlHint = (li, url) => {
      if (!url) {
        // Only mutate if something needs changing — prevents observer loop.
        if (
          !li.classList.contains("qon-li-has-url") &&
          !li.querySelector(".qon-url-hint")
        )
          return;
        for (const old of li.querySelectorAll(".qon-url-hint")) old.remove();
        li.classList.remove("qon-li-has-url");
        return;
      }

      // Check if already correct — if so, do nothing (prevents observer loop).
      const existing = li.querySelector(".qon-url-hint");
      if (
        existing &&
        existing.textContent === url &&
        li.classList.contains("qon-li-has-url")
      )
        return;

      // Remove stale hints and set fresh one.
      for (const old of li.querySelectorAll(".qon-url-hint")) old.remove();
      li.classList.add("qon-li-has-url");
      const hint = document.createElement("div");
      hint.className = "qon-url-hint";
      hint.textContent = url;
      hint.setAttribute("aria-hidden", "true");
      li.appendChild(hint);
    };

    for (const row of rows) {
      const li = row.closest("li");
      if (!li) continue;

      const texts = candidateTexts(row);
      const isQon = texts.some((t) => t.includes(norm(QON_LABEL)));

      if (isQon) {
        row.classList.add("qon-suggestion");
        const url = urlFromCandidates(texts);
        // If we resolved a URL, store it on the <li> so it survives button
        // node replacement by React.
        if (url) {
          setLiUrlHint(li, url);
        } else {
          // URL not resolved yet (map not populated). Keep any existing hint.
        }
      } else {
        row.classList.remove("qon-suggestion");
        // Only remove the hint if this <li> never had a QON button.
        if (!li.classList.contains("qon-li-has-url")) {
          setLiUrlHint(li, null);
        }
      }
    }
  }

  function ensureSuggestionListScrollable() {
    const lists = document.querySelectorAll('div[role="dialog"] dialog ul');
    for (const list of lists) {
      list.style.maxHeight = "60vh";
      list.style.overflowY = "auto";
      list.style.overflowX = "hidden";
      list.style.overscrollBehavior = "contain";
      list.style.WebkitOverflowScrolling = "touch";
    }
  }

  function getActiveSuggestionButtons() {
    return document.querySelectorAll(
      [
        'div[role="dialog"] dialog ul li > button.bg-theme-300\\/50',
        'li[aria-selected="true"] > button',
        'button[aria-selected="true"]',
        'button[aria-current="true"]',
        'button[data-headlessui-state~="active"]',
        'li[data-headlessui-state~="active"] > button',
      ].join(", "),
    );
  }

  function keepActiveSuggestionVisible() {
    const activeButtons = getActiveSuggestionButtons();
    for (const active of activeButtons) {
      const list = active.closest("ul");
      if (!list) continue;
      if (list.scrollHeight <= list.clientHeight) continue;

      const activeRect = active.getBoundingClientRect();
      const listRect = list.getBoundingClientRect();
      const topBoundary = listRect.top + ACTIVE_SCROLL_MARGIN_PX;
      const bottomBoundary = listRect.bottom - ACTIVE_SCROLL_MARGIN_PX;

      if (activeRect.top < topBoundary) {
        list.scrollTop -= topBoundary - activeRect.top;
      } else if (activeRect.bottom > bottomBoundary) {
        list.scrollTop += activeRect.bottom - bottomBoundary;
      }
    }
  }

  function setKeyboardNavActive(active) {
    keyboardNavActive = active;
    document.documentElement.classList.toggle(
      KEYBOARD_NAV_DIALOG_CLASS,
      active,
    );
  }

  function isDialogSuggestionButton(el) {
    return !!el?.closest?.('div[role="dialog"] dialog ul li > button');
  }

  window.open = function (url, target, features) {
    const raw = typeof url === "string" ? url : String(url ?? "");
    const direct = decodeIfSearchEngineRedirect(raw);
    if (direct === BLOCK_QON_TEXT_SEARCH) return null;
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

  const observer = new MutationObserver(() => {
    if (applyingSuggestionUi) return;
    applyingSuggestionUi = true;
    try {
      markQonSuggestionRows();
      ensureSuggestionListScrollable();
      keepActiveSuggestionVisible();
    } finally {
      applyingSuggestionUi = false;
    }
  });
  observer.observe(document.documentElement, {
    childList: true,
    subtree: true,
  });
  markQonSuggestionRows();
  ensureSuggestionListScrollable();

  document.addEventListener(
    "keydown",
    (event) => {
      const key = event.key;
      if (
        key === "ArrowDown" ||
        key === "ArrowUp" ||
        key === "PageDown" ||
        key === "PageUp" ||
        key === "Home" ||
        key === "End"
      ) {
        if (
          isDialogSuggestionButton(document.activeElement) ||
          document.querySelector('div[role="dialog"] dialog ul')
        ) {
          setKeyboardNavActive(true);
        }
        requestAnimationFrame(() => {
          keepActiveSuggestionVisible();
        });
      }
    },
    true,
  );

  document.addEventListener(
    "mouseover",
    (event) => {
      if (!keyboardNavActive) return;
      if (!isDialogSuggestionButton(event.target)) return;
      event.stopPropagation();
    },
    true,
  );

  document.addEventListener(
    "pointerdown",
    () => {
      if (keyboardNavActive) setKeyboardNavActive(false);
    },
    true,
  );

  document.addEventListener(
    "pointermove",
    () => {
      if (keyboardNavActive) setKeyboardNavActive(false);
    },
    true,
  );

  keepActiveSuggestionVisible();
})();
