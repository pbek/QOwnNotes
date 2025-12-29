// @ts-check
import { test, expect } from "./fixtures.js";

test("blog overview page", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/");
  await expect(page).toHaveTitle(/Overview.*QOwnNotes/);
  await expect(
    page.getByRole("heading", { level: 1, name: "Blog" }),
  ).toBeVisible();

  // Check that blog entries are displayed
  // There should be multiple blog post articles
  const blogPosts = page.locator("article.blog-post");
  await expect(blogPosts).not.toHaveCount(0);

  // Check that the first blog entry has a title link
  const firstPost = blogPosts.first();
  const firstLink = firstPost.locator("a.blog-link");
  await expect(firstLink).toBeVisible();
  await expect(firstLink).toHaveAttribute("href", /\/blog\/.+\.html/);

  // Check that blog entries have dates
  const dateElement = firstPost.locator("time.blog-date");
  await expect(dateElement).toBeVisible();
  // Date should be in YYYY-MM-DD format
  await expect(dateElement).toHaveText(/\d{4}-\d{2}-\d{2}/);

  // Verify we have at least a few blog entries (should have 80 based on generation)
  const postCount = await blogPosts.count();
  expect(postCount).toBeGreaterThan(5);
});

test("blog entry - AI support", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );
  await expect(page).toHaveTitle(/AI support was added to QOwnNotes/);
  await expect(
    page.getByRole("heading", {
      level: 1,
      name: "AI support was added to QOwnNotes",
    }),
  ).toBeVisible();
});

test("blog entry - Command-line snippet manager", async ({ page, config }) => {
  await page.goto(
    config.baseURL +
      "/blog/2021-11-23-QOwnNotes-command-line-snippet-manager.html",
  );
  await expect(page).toHaveTitle(/QOwnNotes command-line snippet manager/);
  await expect(
    page.getByRole("heading", {
      level: 1,
      name: "QOwnNotes command-line snippet manager",
    }),
  ).toBeVisible();
  await expect(
    page.getByText("Use the QOwnNotes command-line snippet manager"),
  ).toBeVisible();
});

test("blog entry - Nextcloud Deck integration", async ({ page, config }) => {
  await page.goto(
    config.baseURL +
      "/blog/2023-06-17-Nextcloud-Deck-integration-in-QOwnNotes.html",
  );
  await expect(page).toHaveTitle(/Nextcloud Deck integration/);
  await expect(
    page.getByRole("heading", {
      level: 1,
      name: "Nextcloud Deck integration in QOwnNotes",
    }),
  ).toBeVisible();
});

test("blog entry - 1000th release", async ({ page, config }) => {
  await page.goto(
    config.baseURL + "/blog/2022-12-06-Happy-1000th-release-of-QOwnNotes.html",
  );
  await expect(page).toHaveTitle(/1000th release/);
  await expect(
    page.getByRole("heading", {
      level: 1,
      name: "Happy 1000th release of QOwnNotes",
    }),
  ).toBeVisible();
});

test("blog entry - Joplin import", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/2021-07-18-Joplin-import.html");
  await expect(page).toHaveTitle(/Joplin import/);
  await expect(
    page.getByRole("heading", { level: 1, name: "Joplin import" }),
  ).toBeVisible();
  await expect(page.getByText("Import your notes from Joplin")).toBeVisible();
});
