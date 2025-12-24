// @ts-check
import { test, expect } from "./fixtures.js";

// Test that all activities show
test("frontpage", async ({ page, config }) => {
  await page.goto(config.baseURL);
  await expect(page.locator("title")).toContainText(/QOwnNotes/);
});
