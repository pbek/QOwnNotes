// @ts-check
import { test, expect } from "./fixtures.js";

test("German translation - homepage exists", async ({ page, config }) => {
  const response = await page.goto(config.baseURL + "/de/");
  expect(response.status()).toBe(200);
  await expect(page).toHaveTitle("QOwnNotes");
});

test("German translation - content is in German", async ({ page, config }) => {
  await page.goto(config.baseURL + "/de/");

  // Check German navigation links
  await expect(page.getByRole("link", { name: "Loslegen" })).toBeVisible();
  await expect(page.getByRole("link", { name: "Installation" })).toBeVisible();
  await expect(page.getByRole("link", { name: "Spenden" })).toBeVisible();

  // Check German heading and description
  await expect(
    page.getByRole("heading", { level: 1, name: "QOwnNotes" }),
  ).toBeVisible();
  await expect(
    page.getByText("Kostenlose Open-Source-Markdown-Notizen"),
  ).toBeVisible();
  await expect(page.getByRole("link", { name: "Quick Start →" })).toBeVisible();
});

test("German translation - feature sections", async ({ page, config }) => {
  await page.goto(config.baseURL + "/de/");

  // Check German feature headings
  await expect(
    page.getByRole("heading", { level: 2, name: "Besitze deine Notizen" }),
  ).toBeVisible();
  await expect(
    page.getByRole("heading", { level: 2, name: "Kleiner Fußabdruck" }),
  ).toBeVisible();
  await expect(
    page.getByRole("heading", { level: 2, name: "Anpassung" }),
  ).toBeVisible();
});

test("German translation - RSS links", async ({ page, config }) => {
  await page.goto(config.baseURL + "/de/");

  await expect(page.getByRole("link", { name: "Blog RSS" })).toBeVisible();
  await expect(
    page.getByRole("link", { name: "Veröffentlichungen RSS" }),
  ).toBeVisible();
});

test("German translation - footer", async ({ page, config }) => {
  await page.goto(config.baseURL + "/de/");

  await expect(
    page.getByText("Erstellt von Patrizio Bekerle mit ❤️"),
  ).toBeVisible();
});

test("Persian (Farsi) translation - homepage exists", async ({
  page,
  config,
}) => {
  const response = await page.goto(config.baseURL + "/fa/");
  expect(response.status()).toBe(200);
  await expect(page).toHaveTitle("QOwnNotes");
});

test("Persian (Farsi) translation - content is in Persian", async ({
  page,
  config,
}) => {
  await page.goto(config.baseURL + "/fa/");

  // Check Persian navigation links with exact match to avoid strict mode violation
  await expect(
    page.getByRole("link", { name: "شروع", exact: true }),
  ).toBeVisible();
  await expect(page.getByRole("link", { name: "نصب" })).toBeVisible();
  await expect(page.getByRole("link", { name: "حمایت مالی" })).toBeVisible();

  // Check Persian heading and description
  await expect(
    page.getByRole("heading", { level: 1, name: "QOwnNotes" }),
  ).toBeVisible();
  await expect(
    page.getByText("یادداشت برداری مارک داون پرونده با متن ساده"),
  ).toBeVisible();
  await expect(page.getByRole("link", { name: "Quick Start →" })).toBeVisible();
});

test("Persian (Farsi) translation - feature sections", async ({
  page,
  config,
}) => {
  await page.goto(config.baseURL + "/fa/");

  // Check Persian feature headings
  await expect(
    page.getByRole("heading", {
      level: 2,
      name: "یادداشت های خود را داشته باشید",
    }),
  ).toBeVisible();
  await expect(
    page.getByRole("heading", { level: 2, name: "ردپای کوچک" }),
  ).toBeVisible();
  await expect(
    page.getByRole("heading", { level: 2, name: "سفارشی سازی" }),
  ).toBeVisible();
});

test("Persian (Farsi) translation - RSS links", async ({ page, config }) => {
  await page.goto(config.baseURL + "/fa/");

  await expect(page.getByRole("link", { name: "وبلاگ RSS" })).toBeVisible();
  await expect(page.getByRole("link", { name: "RSS انتشارها" })).toBeVisible();
});

test("Persian (Farsi) translation - footer", async ({ page, config }) => {
  await page.goto(config.baseURL + "/fa/");

  await expect(
    page.getByText("ارائه شده توسط پاتریزیو بکرله همراه با ❤️"),
  ).toBeVisible();
});

test("German translation - getting started page", async ({ page, config }) => {
  await page.goto(config.baseURL + "/de/getting-started/overview.html");
  await expect(page).toHaveTitle(/bersicht.*QOwnNotes/);
});

test("Persian translation - getting started page", async ({ page, config }) => {
  await page.goto(config.baseURL + "/fa/getting-started/overview.html");
  await expect(page).toHaveTitle(/مرور کلی.*QOwnNotes/);
});

test("Language selector button exists", async ({ page, config }) => {
  await page.goto(config.baseURL + "/");

  // Check for language selector button with globe icon
  const languageButton = page.getByRole("button", { name: "Select language" });
  await expect(languageButton).toBeVisible();
});

test("Translation URLs follow correct pattern", async ({ page, config }) => {
  // German URL pattern
  await page.goto(config.baseURL + "/de/");
  expect(page.url()).toContain("/de/");

  // Persian URL pattern
  await page.goto(config.baseURL + "/fa/");
  expect(page.url()).toContain("/fa/");
});

test("German and Persian pages have correct lang attribute", async ({
  page,
  config,
}) => {
  // Check German page
  await page.goto(config.baseURL + "/de/");
  const deLangAttr = await page.evaluate(() => document.documentElement.lang);
  expect(deLangAttr).toBe("de-DE");

  // Check Persian page
  await page.goto(config.baseURL + "/fa/");
  const faLangAttr = await page.evaluate(() => document.documentElement.lang);
  expect(faLangAttr).toBe("fa-IR");
});
