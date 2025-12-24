// @ts-check
import { test, expect } from "./fixtures.js";

test("frontpage has popup with survey question", async ({ page, config }) => {
  await page.goto(config.baseURL);

  // Check for the poll/popup with the survey question
  await expect(
    page.getByText("How did you find out about QOwnNotes?"),
  ).toBeVisible();

  // Check for the textbox
  await expect(
    page.getByRole("textbox", {
      name: "How did you find out about QOwnNotes?",
    }),
  ).toBeVisible();

  // Check for the character counter
  await expect(page.getByText("0 / 200")).toBeVisible();

  // Check for the Submit button
  await expect(page.getByRole("button", { name: "Submit" })).toBeVisible();

  // Check for the Close poll button
  await expect(page.getByRole("button", { name: "Close poll" })).toBeVisible();
});

test("overview page has popup with survey question", async ({
  page,
  config,
}) => {
  await page.goto(config.baseURL + "/getting-started/overview.html");

  // Check the survey popup is also on other pages
  await expect(
    page.getByText("How did you find out about QOwnNotes?"),
  ).toBeVisible();
  await expect(page.getByRole("button", { name: "Submit" })).toBeVisible();
  await expect(page.getByRole("button", { name: "Close poll" })).toBeVisible();
});

test("blog page has popup with survey question", async ({ page, config }) => {
  await page.goto(config.baseURL + "/blog/");

  // Check the survey popup is on blog page
  await expect(
    page.getByText("How did you find out about QOwnNotes?"),
  ).toBeVisible();
  await expect(page.getByRole("button", { name: "Submit" })).toBeVisible();
});
