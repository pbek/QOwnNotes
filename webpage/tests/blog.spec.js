// @ts-check
import { test, expect } from "./fixtures.js";

test("blog overview page", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/");
  await expect(page).toHaveTitle(/Overview.*QOwnNotes/);
  await expect(
    page.getByRole("heading", { level: 1, name: "Blog" }),
  ).toBeVisible();
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
