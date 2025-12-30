// @ts-check
import { test, expect } from "./fixtures.js";

test("concept-mermaid", async ({ page, config }) => {
  await page.goto(config.baseURL + "/getting-started/concept.html");

  // The mermaid diagram should contain "Command-line snippet manager"
  // In the new VuePress, it's rendered as a span within the SVG
  const mermaidText = page
    .locator("text, span")
    .filter({ hasText: "Command-line snippet manager" });

  // Wait for at least one element with this text to be visible
  await expect(mermaidText.first()).toBeVisible();

  // Verify the text is actually present in the page
  await expect(page.locator("body")).toContainText(
    "Command-line snippet manager",
  );
});
