// @ts-check
import { test, expect } from "./fixtures.js";

test("sitemap", async ({ page, config }) => {
  const response = await page.goto(config.baseURL + "/sitemap.xml");
  expect(response.status()).toBe(200);
  expect(response.headers()["content-type"]).toContain("xml");
  const content = await page.content();
  expect(content).toContain("<urlset");
  expect(content).toContain("<url>");
  expect(content).toContain("<loc>");
  expect(content).toContain("/getting-started/concept.html");
});
