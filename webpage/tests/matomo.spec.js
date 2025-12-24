// @ts-check
import { test, expect } from "./fixtures.js";

test("Matomo tracking script is loaded", async ({ page, config }) => {
  await page.goto(config.baseURL);

  // Check if _paq object exists (Matomo tracking array)
  const hasPaq = await page.evaluate(() => typeof window._paq !== "undefined");
  expect(hasPaq).toBeTruthy();
});

test("Matomo tracking requests are sent", async ({ page, config }) => {
  // Track all network requests
  const matomoRequests = [];

  page.on("request", (request) => {
    const url = request.url();
    // Check for Matomo tracking requests (matomo.php or piwik.php)
    if (url.includes("matomo.php") || url.includes("piwik.php")) {
      matomoRequests.push({
        url: url,
        method: request.method(),
        postData: request.postData(),
      });
    }
  });

  // Navigate to the page
  await page.goto(config.baseURL);

  // Wait a bit for tracking to be sent
  await page.waitForTimeout(2000);

  // Verify that at least one Matomo tracking request was sent
  expect(matomoRequests.length).toBeGreaterThan(0);

  // Verify the tracking request contains expected parameters
  const firstRequest = matomoRequests[0];
  expect(firstRequest.url).toMatch(/matomo\.php|piwik\.php/);
});

test("Matomo tracking includes site ID", async ({ page, config }) => {
  // Track Matomo requests
  let matomoRequestUrl = null;

  page.on("request", (request) => {
    const url = request.url();
    if (url.includes("matomo.php") || url.includes("piwik.php")) {
      matomoRequestUrl = url;
    }
  });

  await page.goto(config.baseURL);
  await page.waitForTimeout(2000);

  expect(matomoRequestUrl).not.toBeNull();

  // Check if the request contains idsite parameter
  expect(matomoRequestUrl).toMatch(/idsite=\d+/);
});

test("Matomo tracking includes page URL", async ({ page, config }) => {
  let matomoRequestUrl = null;

  page.on("request", (request) => {
    const url = request.url();
    if (url.includes("matomo.php") || url.includes("piwik.php")) {
      matomoRequestUrl = url;
    }
  });

  await page.goto(config.baseURL);
  await page.waitForTimeout(2000);

  expect(matomoRequestUrl).not.toBeNull();

  // Check if the request contains url parameter
  expect(matomoRequestUrl).toMatch(/url=/);
});

test("Matomo tracking on blog page", async ({ page, config }) => {
  const matomoRequests = [];

  page.on("request", (request) => {
    const url = request.url();
    if (url.includes("matomo.php") || url.includes("piwik.php")) {
      matomoRequests.push(url);
    }
  });

  // Navigate to a blog page
  await page.goto(
    config.baseURL + "/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html",
  );
  await page.waitForTimeout(2000);

  // Verify tracking request was sent for blog page
  expect(matomoRequests.length).toBeGreaterThan(0);

  // Verify the URL in the tracking request contains the blog page path
  const trackingUrl = matomoRequests[0];
  expect(trackingUrl).toMatch(/AI-support-was-added-to-QOwnNotes/);
});

test("Matomo server responds successfully", async ({ page, config }) => {
  let matomoResponse = null;

  page.on("response", (response) => {
    const url = response.url();
    if (url.includes("matomo.php") || url.includes("piwik.php")) {
      matomoResponse = response;
    }
  });

  await page.goto(config.baseURL);
  await page.waitForTimeout(2000);

  expect(matomoResponse).not.toBeNull();

  // Verify the Matomo server responds with a successful status
  expect(matomoResponse.status()).toBeLessThan(400);
});
