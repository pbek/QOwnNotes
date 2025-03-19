---
title: Legacy update service up again
description: The legacy update service is up again for older versions of QOwnNotes
date: 2020-10-26
order: 20201026000000
---

# Legacy update service up again

<BlogDate v-bind:fm="$frontmatter" />

The legacy update service is up again for older versions of QOwnNotes.

There already was a legacy update service implemented for [api.qownnotes.org](https://api.qownnotes.org/), but the proxying to it didn't work properly.

Now there is a proper proxy in place that tunnels the old requests to the new "legacy api" so old versions of QOwnNotes should get update notifications again.
