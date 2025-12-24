// @ts-check
import { test, expect } from "./fixtures.js";

test("overview page has Algolia search field", async ({ page, config }) => {
  await page.goto(config.baseURL + "/getting-started/overview.html");

  // Check Algolia search input exists
  const searchInput = page.locator("#algolia-search-input");
  await expect(searchInput).toBeVisible();

  // Verify it has the correct Algolia DocSearch classes
  await expect(searchInput).toHaveClass(/search-query/);
  await expect(searchInput).toHaveClass(/ds-input/);

  // Verify the search input is a combobox
  await expect(
    page.getByRole("combobox", { name: "search input" }),
  ).toBeVisible();
});

test("frontpage has Algolia search field", async ({ page, config }) => {
  await page.goto(config.baseURL);

  // Check Algolia search input exists on frontpage
  const searchInput = page.locator("#algolia-search-input");
  await expect(searchInput).toBeVisible();

  // Verify it's a combobox
  await expect(
    page.getByRole("combobox", { name: "search input" }),
  ).toBeVisible();
});

test("blog page has Algolia search field", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/");

  // Check Algolia search input exists on blog page
  const searchInput = page.locator("#algolia-search-input");
  await expect(searchInput).toBeVisible();
});
