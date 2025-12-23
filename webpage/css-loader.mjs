// Node.js ESM loader hook to handle CSS imports during SSR
// This allows Vuetify CSS imports to work with Node.js ESM loader

export async function resolve(specifier, context, defaultResolve) {
  // Intercept CSS file imports
  if (specifier.endsWith('.css')) {
    return {
      url: new URL(specifier, context.parentURL || 'file:///').href,
      format: 'module',
      shortCircuit: true
    };
  }

  return defaultResolve(specifier, context);
}

export async function load(url, context, defaultLoad) {
  // Return empty module for CSS files
  if (url.endsWith('.css')) {
    return {
      format: 'module',
      source: 'export default {}',
      shortCircuit: true
    };
  }

  return defaultLoad(url, context);
}

