// @ts-check
import { test, expect } from "./fixtures.js";

test("frontpage has popup with survey question", async ({ page, config }) => {
  await page.goto(config.baseURL);

  // Check for the textbox (use .first() since there are multiple polls on frontpage)
  await expect(
    page
      .getByRole("textbox", {
        name: "How did you find out about QOwnNotes?",
      })
      .first(),
  ).toBeVisible();

  // Check for the Submit button
  await expect(
    page.getByRole("button", { name: "Submit" }).first(),
  ).toBeVisible();

  // Check for the Close poll button
  await expect(
    page.getByRole("button", { name: "Close poll" }).first(),
  ).toBeVisible();
});

test("overview page has popup with survey question", async ({
  page,
  config,
}) => {
  await page.goto(config.baseURL + "/getting-started/overview.html");

  // Check the survey popup is also on other pages
  await expect(
    page.getByRole("textbox", {
      name: "How did you find out about QOwnNotes?",
    }),
  ).toBeVisible();
  await expect(page.getByRole("button", { name: "Submit" })).toBeVisible();
  await expect(page.getByRole("button", { name: "Close poll" })).toBeVisible();
});

test("blog page has popup with survey question", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/");

  // Check the survey popup is on blog page
  await expect(
    page.getByRole("textbox", {
      name: "How did you find out about QOwnNotes?",
    }),
  ).toBeVisible();
  await expect(page.getByRole("button", { name: "Submit" })).toBeVisible();
});
