// @ts-check
import { test, expect } from "./fixtures.js";

test("overview page has Algolia search field", async ({ page, config }) => {
  await page.goto(config.baseURL + "/getting-started/overview.html");

  // Check that search button exists
  const searchButton = page.getByRole("button", { name: "Search" });
  await expect(searchButton).toBeVisible();

  // Click the search button to open the search modal
  await searchButton.click();

  // Verify the search modal appears with searchbox
  const searchBox = page.getByRole("searchbox", { name: "Search" });
  await expect(searchBox).toBeVisible();

  // Wait a moment for the search modal to fully load
  await page.waitForTimeout(500);

  // Verify Algolia branding is present (it's in a link)
  await expect(
    page.getByRole("link", { name: /Powered by Algolia/i }),
  ).toBeVisible();
});

test("frontpage has Algolia search field", async ({ page, config }) => {
  await page.goto(config.baseURL);

  // Check that search button exists on frontpage
  const searchButton = page.getByRole("button", { name: "Search" });
  await expect(searchButton).toBeVisible();

  // Click the search button to open the search modal
  await searchButton.click();

  // Verify the search modal appears with searchbox
  const searchBox = page.getByRole("searchbox", { name: "Search" });
  await expect(searchBox).toBeVisible();

  // Wait a moment for the search modal to fully load
  await page.waitForTimeout(500);

  // Verify Algolia branding is present (it's in a link)
  await expect(
    page.getByRole("link", { name: /Powered by Algolia/i }),
  ).toBeVisible();
});

test("blog page has Algolia search field", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/");

  // Check that search button exists on blog page
  const searchButton = page.getByRole("button", { name: "Search" });
  await expect(searchButton).toBeVisible();

  // Click the search button to open the search modal
  await searchButton.click();

  // Verify the search modal appears with searchbox
  const searchBox = page.getByRole("searchbox", { name: "Search" });
  await expect(searchBox).toBeVisible();
});
