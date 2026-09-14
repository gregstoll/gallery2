# timeline_2026

A date-grouped photo stream theme, in the shape of a modern photo app: one
continuous stream of thumbnails split into month sections, a year/month
scrubber down the side, an album rail, and a keyboard-navigable lightbox.

    Site Admin -> Themes -> timeline_2026

It renders albums the same way, so it can be used as the default theme for
the whole gallery. The "Photos" entry in the rail points at
`dynamicalbum.UpdatesAlbum`, so the **dynamicalbum** module must be active
for the stream view; everything else works without it.

## Why this theme needs more from the server than the stock ones

The stream is deliberately one long page rather than paginated — that is the
whole point of the layout — so a single request can cover thousands of items.
The theme defaults to `rows = 50, columns = 50`, and Gallery2's page size is
`rows * columns`, so the default page holds 2,500 items. On a gallery of a
few thousand photos the stock configuration will feel slow or appear to hang,
and the fixes are all outside the theme. They are worth knowing before
concluding the theme is broken.

### 1. FastCGI buffering must be on

This is the big one, and it is easy to get wrong because some Gallery2
install guides turn buffering off to make the installer's progress bars
stream. With `fastcgi_buffering off` nginx cannot gzip the response and
cannot cache it, and neither failure announces itself.

On a 2,197-item stream, turning buffering and gzip back on took the response
from **1,140,558 bytes to 54,020** — a factor of 21, on exactly the same
markup.

    fastcgi_buffering on;
    fastcgi_buffers 16 32k;
    gzip on;
    gzip_proxied any;
    gzip_types text/html text/css application/javascript;

If you deliberately need unbuffered output for the installer, scope it to the
installer rather than leaving it on for the whole site.

### 2. Thumbnails must actually exist

The stream is nothing but thumbnails, so a gallery with unbuilt or broken
derivatives looks catastrophically wrong here while looking merely untidy in
a paginated theme. Build them before switching:

    Site Admin -> Maintenance -> Build all thumbnails/resizes

If derivatives were built while an image toolkit was misconfigured they can
be marked broken in the database and will not rebuild on their own. Check
`Site Admin -> Graphics Toolkits` first — in particular, the NetPBM toolkit
needs `jhead` present or it discards derivatives it has successfully built.

### 3. PHP-FPM worker count

One stream request holds a worker for as long as it takes to assemble the
page. If `pm.max_children` is small, a couple of concurrent visitors will
queue behind each other and the site will appear to stall rather than to be
slow. Size it against your memory budget and expected concurrency.

## Optional: micro-caching the stream

The stream view is the most expensive page and the most cacheable, so a short
FastCGI micro-cache helps a lot. **It is also the easiest way to break your
site, so read this before enabling it.**

Gallery2 embeds a per-session `g2_authToken` in page markup. If you cache a
page containing one and serve it to somebody else, they get another user's
token — and a cached login page will hand the same token to everyone. Bypass
the cache for anything session-bearing:

    set $g2_nocache 1;
    if ($arg_g2_view = "dynamicalbum.UpdatesAlbum") { set $g2_nocache 0; }
    if ($http_cookie ~* "GALLERYSID") { set $g2_nocache 1; }
    if ($request_method != GET)       { set $g2_nocache 1; }
    if ($arg_g2_authToken)            { set $g2_nocache 1; }

Cache the stream only, never the whole site. If you also add a redirect
sending `/main.php` to the stream, make it **GET-only** — a redirect that
matches on URI alone will swallow the login POST, discard the body, and log
nobody in, with no error anywhere.

## How the page is built

Handing a 2,500-item album to the browser as markup costs seconds of layout
before the page responds at all, so `theme.inc` splits the work:

- Items are grouped into month sections. A contiguous prefix — the first
  ~260 items, `_buildGroups()`'s `$eagerItems` — is rendered as real markup so
  the page is complete and readable on arrival.
- Every later section ships as compact JSON plus three URL templates, and
  `timeline.js` hydrates each section from an `IntersectionObserver` as it
  approaches the viewport.
- Sections carry `content-visibility: auto` with an estimated intrinsic size,
  so off-screen sections cost no layout.
- The scrubber is built server-side from the full item list, so it shows the
  gallery's true date range immediately rather than growing as you scroll.

The prefix is contiguous on purpose. An earlier version picked whichever
months fit a budget, which scattered the eager set across the timeline and
produced a page that was partly hydrated in the middle and empty at the top.

## Gallery2 gotchas worth knowing

- **Theme settings appear to do nothing until the theme cache is cleared.**
  Changing rows/columns writes to the database but the rendered page is
  served from `g2data/cache/theme`. Clear it (Site Admin -> Maintenance ->
  Delete template cache, or remove `cache/theme`) or you will conclude the
  setting is ignored.
- **Page size is `rows * columns`**, not `rows`.
- Gallery2 answers conditional requests with 304 by design and treats its
  URLs as immutable, so if you change derivative bytes or MIME types by hand,
  bump `g_serialNumber` or browsers will hold the old copy indefinitely.

## Companion patch

`modules/dynamicalbum` sorts its date album by `creationTimestamp`. Galleries
built by import often share one creation timestamp across every item, which
leaves the stream in no meaningful order; `originationTimestamp` — when the
photo was actually taken — is the useful key there. A separate patch makes
that field configurable per album type, defaulting to current behaviour. The
theme works either way.
