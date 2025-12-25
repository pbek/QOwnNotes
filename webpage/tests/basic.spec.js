// @ts-check
import { test, expect } from "./fixtures.js";

// Test that all activities show
test("frontpage", async ({ page, config }) => {
  await page.goto(config.baseURL);
  await expect(page).toHaveTitle(/QOwnNotes/);
  await expect(
    page.getByText("Free open source plain-text file markdown note-taking"),
  ).toBeVisible();
  await expect(page.getByRole("link", { name: "Quick Start →" })).toBeVisible();
});

test("concept", async ({ page, config }) => {
  await page.goto(config.baseURL + "/getting-started/concept.html");
  await expect(page).toHaveTitle(/Concept/);
  await expect(
    page.getByRole("heading", { level: 1, name: "Concept" }),
  ).toBeVisible();
});
