// @ts-check
import { test, expect } from "./fixtures.js";

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
