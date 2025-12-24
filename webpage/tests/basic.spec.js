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
  await expect(
    page.locator(".mermaid-diagram").getByText("Command-line snippet manager"),
  ).toBeVisible();
});

test("overview page has image carousel", async ({ page, config }) => {
  await page.goto(config.baseURL + "/getting-started/overview.html");
  await expect(page).toHaveTitle(/Overview.*QOwnNotes/);

  // Check carousel navigation buttons exist
  await expect(
    page.getByRole("button", { name: "Previous visual" }),
  ).toBeVisible();
  await expect(page.getByRole("button", { name: "Next visual" })).toBeVisible();

  // Check carousel slide indicators exist
  await expect(
    page.getByRole("button", { name: /Carousel slide \d+ of 22/ }),
  ).toHaveCount(22);

  // Check the main image is visible
  await expect(
    page.getByRole("img", { name: "QOwnNotes screenshot" }),
  ).toBeVisible();
});
