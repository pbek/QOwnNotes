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

test("closing the survey hides it for 30 days", async ({ page, config }) => {
  const now = new Date("2026-10-16T12:00:00Z");
  await page.clock.setFixedTime(now);
  await page.goto(config.baseURL + "/getting-started/overview.html");

  const question = page.getByRole("textbox", {
    name: "How did you find out about QOwnNotes?",
  });
  await expect(question).toBeVisible();
  await page.getByRole("button", { name: "Close poll" }).click();
  await expect(question).toBeHidden();

  const expiresAt = now.getTime() + 30 * 24 * 60 * 60 * 1000;
  await expect
    .poll(() =>
      page.evaluate(() =>
        Number(localStorage.getItem("qon-poll-1-dismissed-until")),
      ),
    )
    .toBe(expiresAt);

  await page.reload();
  await expect(question).toBeHidden();

  await page.clock.setFixedTime(expiresAt - 1);
  await page.goto(config.baseURL + "/blog/");
  await expect(question).toBeHidden();

  await page.clock.setFixedTime(expiresAt);
  await page.reload();
  await expect(question).toBeVisible();
});

test("submitted surveys stay hidden after 30 days", async ({
  page,
  config,
}) => {
  const now = new Date("2026-10-16T12:00:00Z");
  await page.clock.setFixedTime(now);
  await page.goto(config.baseURL + "/getting-started/overview.html");

  const question = page.getByRole("textbox", {
    name: "How did you find out about QOwnNotes?",
  });
  await question.fill("A friend");
  await page.getByRole("button", { name: "Submit" }).click();
  await expect(question).toBeHidden();

  await page.clock.setFixedTime(now.getTime() + 31 * 24 * 60 * 60 * 1000);
  await page.reload();
  await expect(question).toBeHidden();
});
