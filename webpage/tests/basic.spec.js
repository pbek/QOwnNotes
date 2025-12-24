// @ts-check
import { test, expect } from "@playwright/test";
import { getConfig } from "../config.js";

// Test that all activities show
test("basic", async ({ page }) => {
  const config = getConfig();
  await page.goto(config.baseURL);
  await expect(page.locator("title")).toContainText(/QOwnNotes/);
});
