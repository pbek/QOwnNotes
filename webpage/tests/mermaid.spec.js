// @ts-check
import { test, expect } from "./fixtures.js";

const conceptPath = "/getting-started/concept.html";

test("concept-mermaid", async ({ page, config }) => {
  await page.goto(config.baseURL + conceptPath);

  // The mermaid diagrams are rendered as SVG by Pele, depending on the screen
  // width either the desktop or the mobile diagrams are visible
  await expect(
    page.locator(".pele-diagram svg.pele:visible").first(),
  ).toBeVisible();

  // Node labels may wrap into several lines, so check the node link instead
  // of the full label text
  const snippetManagerNode = page
    .locator(
      '.pele-diagram a[href$="#qownnotes-command-line-snippet-manager"]:visible',
    )
    .first();
  await expect(snippetManagerNode).toBeVisible();
  await expect(snippetManagerNode).toContainText("Command-line");
});

test("concept-mermaid-mobile", async ({ page, config }) => {
  await page.setViewportSize({ width: 390, height: 844 });
  await page.goto(config.baseURL + conceptPath);

  await expect(page.locator(".pele-diagram--desktop")).toBeHidden();
  const diagrams = page.locator(".pele-diagram--mobile");
  await expect(diagrams).toHaveCount(3);

  for (const diagram of await diagrams.all()) {
    await expect(diagram.locator("svg.pele")).toBeVisible();

    // The mobile diagrams have to fit without scrolling sideways
    const overflow = await diagram.evaluate(
      (element) => element.scrollWidth - element.clientWidth,
    );
    expect(overflow).toBeLessThanOrEqual(1);
  }
});

test("concept-mermaid-desktop", async ({ page, config }) => {
  await page.setViewportSize({ width: 1920, height: 1080 });
  await page.goto(config.baseURL + conceptPath);

  await expect(page.locator(".pele-diagram--desktop svg.pele")).toBeVisible();
  await expect(page.locator(".pele-diagram--mobile").first()).toBeHidden();
});
