// @ts-check
import { test as base } from "@playwright/test";
import { getConfig } from "../config.js";

export const test = base.extend({
  config: async ({}, use) => {
    const config = getConfig();
    await use(config);
  },
});

export { expect } from "@playwright/test";
