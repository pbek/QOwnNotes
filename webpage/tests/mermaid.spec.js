// @ts-check
import { test, expect } from "./fixtures.js";

test("concept-mermaid", async ({ page, config }) => {
  await page.goto(config.baseURL + "/getting-started/concept.html");

  // Check that at least one of the mermaid diagram selectors works
  // 1st test: vuepress 2 mermaid diagram
  // 2nd test: vuepress 1 mermaid diagram
  // const selector1 = page.locator("span.nodeLabel p").getByText("Command-line snippet manager");
  const selector1 = page
    .locator("g.node")
    .getByTitle("Command-line snippet manager");
  console.log(selector1);
  const selector2 = page
    .locator(".mermaid-diagram")
    .getByText("Command-line snippet manager");

  // await expect(page.locator("div.mermaid-content")).toBeVisible();

  const results = await Promise.allSettled([
    selector1.isVisible(),
    selector2.isVisible(),
  ]);

  const atLeastOneVisible = results.some(
    (result) => result.status === "fulfilled" && result.value === true,
  );
  await expect(
    atLeastOneVisible,
    'At least one mermaid selector should find "Command-line snippet manager"',
  ).toBeTruthy();
});
