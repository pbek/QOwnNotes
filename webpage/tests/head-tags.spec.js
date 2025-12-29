// @ts-check
import { test, expect } from "./fixtures.js";

test("blog head tags - meta description", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  const metaDescription = page.locator('meta[name="description"]');
  await expect(metaDescription).toHaveAttribute(
    "content",
    "QOwnNotes now has AI support, currently for Groq and OpenAI!",
  );
});

test("blog head tags - Open Graph meta tags", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  // Check Open Graph title
  const ogTitle = page.locator('meta[property="og:title"]');
  await expect(ogTitle).toHaveAttribute(
    "content",
    "AI support was added to QOwnNotes",
  );

  // Check Open Graph description
  const ogDescription = page.locator('meta[property="og:description"]');
  await expect(ogDescription).toHaveAttribute(
    "content",
    "QOwnNotes now has AI support, currently for Groq and OpenAI!",
  );

  // Check Open Graph type
  const ogType = page.locator('meta[property="og:type"]');
  await expect(ogType).toHaveAttribute("content", "article");

  // Check Open Graph URL
  const ogUrl = page.locator('meta[property="og:url"]');
  await expect(ogUrl).toHaveAttribute(
    "content",
    "https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  // Check Open Graph image
  const ogImage = page.locator('meta[property="og:image"]');
  await expect(ogImage).toHaveAttribute(
    "content",
    "https://www.qownnotes.org/assets/img/robot-taking-notes.jpg",
  );
});

test("blog head tags - Twitter Card meta tags", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  // Check Twitter card type
  const twitterCard = page.locator('meta[name="twitter:card"]');
  await expect(twitterCard).toHaveAttribute("content", "summary_large_image");

  // Check Twitter title
  const twitterTitle = page.locator('meta[name="twitter:title"]');
  await expect(twitterTitle).toHaveAttribute(
    "content",
    "AI support was added to QOwnNotes",
  );

  // Check Twitter description
  const twitterDescription = page.locator('meta[name="twitter:description"]');
  await expect(twitterDescription).toHaveAttribute(
    "content",
    "QOwnNotes now has AI support, currently for Groq and OpenAI!",
  );

  // Check Twitter image
  const twitterImage = page.locator('meta[name="twitter:image"]');
  await expect(twitterImage).toHaveAttribute(
    "content",
    "https://www.qownnotes.org/assets/img/robot-taking-notes.jpg",
  );

  // Check Twitter creator
  const twitterCreator = page.locator('meta[name="twitter:creator"]');
  await expect(twitterCreator).toHaveAttribute("content", "@PatrizioBekerle");
});

test("blog head tags - favicon link", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  const favicon = page.locator('link[rel="icon"]');
  await expect(favicon).toHaveAttribute("href", "/favicon.png");
  await expect(favicon).toHaveAttribute("type", "image/png");
});

test("blog head tags - RSS feed links", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  // Check RSS feed link
  const rssFeed = page.locator('link[rel="alternate"][href*="rss.xml"]');
  await expect(rssFeed).toHaveAttribute(
    "href",
    "https://www.qownnotes.org/rss.xml",
  );
  await expect(rssFeed).toHaveAttribute("type", "application/rss+xml");

  // Check Atom feed link
  const atomFeed = page.locator('link[rel="alternate"][href*="feed.atom"]');
  await expect(atomFeed).toHaveAttribute(
    "href",
    "https://www.qownnotes.org/feed.atom",
  );
  await expect(atomFeed).toHaveAttribute("type", "application/atom+xml");

  // Check JSON feed link
  const jsonFeed = page.locator('link[rel="alternate"][href*="feed.json"]');
  await expect(jsonFeed).toHaveAttribute(
    "href",
    "https://www.qownnotes.org/feed.json",
  );
  await expect(jsonFeed).toHaveAttribute("type", "application/json");
});

test("blog head tags - viewport meta tag", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  const viewport = page.locator('meta[name="viewport"]').first();
  const content = await viewport.getAttribute("content");
  // Accept both formats: "width=device-width,initial-scale=1" and "width=device-width, initial-scale=1.0"
  expect(content).toMatch(/width=device-width,?\s?initial-scale=1(\.0)?/);
});

test("blog head tags - theme color", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  const themeColor = page.locator('meta[name="theme-color"]');
  await expect(themeColor).toHaveAttribute("content", "#3eaf7c");
});

test("blog head tags - social profile links", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );

  // Check for rel="me" links (social profile verification)
  const socialLinks = page.locator('link[rel="me"]');
  await expect(socialLinks).toHaveCount(2);

  // Check for Mastodon link
  const mastodonLink = page.locator(
    'link[rel="me"][href*="social.qownnotes.org/@qownnotes"]',
  );
  await expect(mastodonLink).toHaveAttribute(
    "href",
    "https://social.qownnotes.org/@qownnotes",
  );
});
