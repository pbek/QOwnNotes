import process from "node:process";
import { test } from "@playwright/test";
import fs from "node:fs";

if (fs.existsSync(".env")) {
  process.loadEnvFile(".env");
}

let defaultConfig = {
  baseURL: "https://www.qownnotes.org",
};

let localOverride = {
  baseURL: "http://localhost:8080/",
};

function deepMerge(target, source) {
  const result = structuredClone(target);

  function merge(obj, src) {
    for (const key in src) {
      if (
        src[key] &&
        typeof src[key] === "object" &&
        !Array.isArray(src[key])
      ) {
        obj[key] = obj[key] || {};
        merge(obj[key], src[key]);
      } else {
        obj[key] = src[key];
      }
    }
  }

  merge(result, source);
  return result;
}

const configs = {
  prod: defaultConfig,
  local: deepMerge(defaultConfig, localOverride),
};

/**
 * @returns {defaultConfig}
 */
export function getConfig() {
  // Only call test.info() to make sure we are running in a test context, in case
  // we want to incorporate test-specific logic in the future.
  test.info();
  const env = process.env.TEST_ENV || "dev";
  const config = configs[env];
  if (!config) {
    throw new Error(
      `Unknown environment: ${env}. Available: ${Object.keys(configs).join(", ")}`,
    );
  }
  return config;
}
