// @ts-check
import { test, expect } from "./fixtures.js";

test("sitemap", async ({ request, config }) => {
  // Use request API with XML Accept header to get raw XML instead of HTML
  const response = await request.get(config.baseURL + "/sitemap.xml", {
    headers: {
      Accept: "application/xml, text/xml",
    },
  });
  expect(response.status()).toBe(200);
  expect(response.headers()["content-type"]).toContain("xml");
  const content = await response.text();
  expect(content).toContain("<urlset");
  expect(content).toContain("<url>");
  expect(content).toContain("<loc>");
  expect(content).toContain("/getting-started/concept.html");
});
