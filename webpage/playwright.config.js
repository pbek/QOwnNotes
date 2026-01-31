// @ts-check
import process from "node:process";
import { devices } from "@playwright/test";
import {
  installBrowsersForNpmInstall,
  registry,
} from "playwright-core/lib/server";

async function setup() {
  const browsersToInstall = [];
  if (!process.env.FIREFOX_BIN) {
    browsersToInstall.push("firefox");
    browsersToInstall.push("ffmpeg");
  }
  if (!process.env.CHROMIUM_BIN) {
    browsersToInstall.push("chromium");
    browsersToInstall.push("ffmpeg");
  }
  if (browsersToInstall.length > 0) {
    await installBrowsersForNpmInstall(browsersToInstall);
  }
  if (!process.env.FIREFOX_BIN) {
    process.env.FIREFOX_BIN = registry
      .findExecutable("firefox")
      .executablePath();
  }
  if (!process.env.CHROMIUM_BIN) {
    process.env.CHROMIUM_BIN = registry
      .findExecutable("chromium")
      .executablePath();
  }
}

// In CI things can take longer in case there is too much load on the system.
let timeoutMultiplier = process.env.CI ? 3 : 1;

await setup();

/**
 * @see https://playwright.dev/docs/test-configuration
 * @type {import('@playwright/test').PlaywrightTestConfig}
 */
const config = {
  testDir: "./",
  /* Maximum time one test can run for. */
  timeout: 30 * 1000 * timeoutMultiplier,
  expect: {
    /**
     * Maximum time expect() should wait for the condition to be met.
     * For example in `await expect(locator).toHaveText();`
     */
    timeout: 5 * 1000 * timeoutMultiplier,
  },
  /* Run tests in files in parallel */
  fullyParallel: false,
  /* Fail the build on CI if you accidentally left test.only in the source code. */
  forbidOnly: !!process.env.CI,
  /* Retry on CI only */
  retries: process.env.CI ? 2 : 0,
  workers: 1,
  /* Reporter to use. See https://playwright.dev/docs/test-reporters */
  reporter: [["html", { open: "never" }]],
  /* Shared settings for all the projects below. See https://playwright.dev/docs/api/class-testoptions. */
  use: {
    /* Maximum time each action such as `click()` can take. Defaults to 0 (no limit). */
    actionTimeout: 0,
    /* Base URL to use in actions like `await page.goto('/')`. */
    // baseURL: 'http://localhost:3000',
    // ignoreHTTPSErrors: true,

    /* Collect trace when retrying the failed test. See https://playwright.dev/docs/trace-viewer */
    trace: "on-first-retry",
    video: "on-first-retry",
  },

  /* Configure projects for major browsers */
  projects: [
    {
      name: "chromium",
      use: {
        ...devices["Desktop Chrome"],
        launchOptions: {
          executablePath: process.env.CHROMIUM_BIN,
          args: [
            "--ignore-certificate-errors",
            "--allow-file-access-from-files",
            "--use-fake-ui-for-media-stream",
            "--use-fake-device-for-media-stream",
            "--force-color-profile=srgb",
          ],
        },
      },
    },
    {
      name: "firefox",
      use: {
        ...devices["Desktop Firefox"],
        launchOptions: {
          executablePath: process.env.FIREFOX_BIN,
        },
      },
    },
  ],

  /* Folder for test artifacts such as screenshots, videos, traces, etc. */
  outputDir: "test-results/",
};

export default config;
