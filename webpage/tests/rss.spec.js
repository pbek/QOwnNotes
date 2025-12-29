// @ts-check
import { test, expect } from "./fixtures.js";

// Helper function to fetch RSS feed with proper XML headers
async function getRssFeed(request, config) {
  return await request.get(config.baseURL + "/rss.xml", {
    headers: {
      Accept: "application/xml, text/xml, application/rss+xml",
    },
  });
}

test("RSS feed is accessible", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  expect(response.status()).toBe(200);
});

test("RSS feed has correct content type", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const contentType = response.headers()["content-type"];
  expect(contentType).toMatch(/(xml|rss)/i);
});

test("RSS feed has valid RSS structure", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const content = await response.text();

  // Check for basic RSS structure
  expect(content).toContain("<rss");
  expect(content).toContain('version="2.0"');
  expect(content).toContain("<channel>");
  expect(content).toContain("</channel>");
  expect(content).toContain("</rss>");
});

test("RSS feed has channel metadata", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const content = await response.text();

  // Check channel metadata
  expect(content).toContain("<title>QOwnNotes Blog</title>");
  // Accept link with or without trailing slash
  expect(content).toMatch(/<link>https:\/\/www\.qownnotes\.org\/?<\/link>/);
  expect(content).toContain("<description>News about QOwnNotes");
  expect(content).toContain("<lastBuildDate>");
  expect(content).toContain("</lastBuildDate>");
});

test("RSS feed contains blog items", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const content = await response.text();

  // Check that there are items
  expect(content).toContain("<item>");
  expect(content).toContain("</item>");
});

test("RSS feed items have required fields", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const content = await response.text();

  // Each item should have title, link, guid, and pubDate
  const itemMatches = content.match(/<item>/g);
  expect(itemMatches).not.toBeNull();
  expect(itemMatches.length).toBeGreaterThan(0);

  // Check for required fields in items
  expect(content).toContain("<title>");
  expect(content).toContain("<link>");
  expect(content).toContain("<guid>");
  expect(content).toContain("<pubDate>");
});

test("RSS feed contains specific blog entries", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const content = await response.text();

  // Check for some known blog entries
  expect(content).toContain("Universal binary for macOS");
  expect(content).toContain("AI support was added to QOwnNotes");
  expect(content).toContain("QOwnNotes command-line snippet manager");
});

test("RSS feed atom link is present", async ({ request, config }) => {
  const response = await getRssFeed(request, config);
  const content = await response.text();

  // Check for atom:link
  expect(content).toContain("<atom:link");
  expect(content).toContain('href="https://www.qownnotes.org/');
  expect(content).toContain('type="application/rss+xml"');
});
