/*
 * timeline_2026 - appearance, density, date scrubber, viewer, infinite scroll.
 *
 * Every behaviour here is a progressive enhancement: with JavaScript off the
 * grid still renders, per-item pages still work, and Gallery2's own pager
 * remains in place.
 */
(function () {
  'use strict';

  var root = document.documentElement;

  /* ===================== appearance: system / light / dark ================ */
  (function () {
    var btn = document.getElementById('tlThemeToggle');
    var lbl = document.getElementById('tlThemeLabel');
    if (!btn || !lbl) { return; }
    var order = ['auto', 'light', 'dark'];
    var mq = window.matchMedia ? window.matchMedia('(prefers-color-scheme: dark)') : null;

    function current() {
      try {
        var t = window.localStorage.getItem('tl-theme');
        return (t === 'light' || t === 'dark') ? t : 'auto';
      } catch (e) { return 'auto'; }
    }
    /* say what "auto" actually resolved to, so the state is never ambiguous */
    function label() {
      var m = current();
      if (m === 'auto') { return 'Auto · ' + (mq && mq.matches ? 'Dark' : 'Light'); }
      return m === 'dark' ? 'Dark' : 'Light';
    }
    function apply(mode) {
      if (mode === 'auto') {
        root.removeAttribute('data-theme');
        try { window.localStorage.removeItem('tl-theme'); } catch (e) {}
      } else {
        root.setAttribute('data-theme', mode);
        try { window.localStorage.setItem('tl-theme', mode); } catch (e) {}
      }
      lbl.textContent = label();
    }
    lbl.textContent = label();
    btn.addEventListener('click', function () {
      apply(order[(order.indexOf(current()) + 1) % order.length]);
    });
    if (mq) {
      var onChange = function () { lbl.textContent = label(); };
      /* iOS Safari < 14 only has addListener */
      if (mq.addEventListener) { mq.addEventListener('change', onChange); }
      else if (mq.addListener) { mq.addListener(onChange); }
    }
  })();

  /* ============================ tile density ============================= */
  (function () {
    var wrap = document.querySelector('.tl-density');
    if (!wrap) { return; }
    var steps = ['spacious', 'comfortable', 'compact', 'dense'];
    function get() {
      var d = root.getAttribute('data-density');
      return steps.indexOf(d) >= 0 ? d : 'comfortable';
    }
    function set(d) {
      root.setAttribute('data-density', d);
      try { window.localStorage.setItem('tl-density', d); } catch (e) {}
    }
    if (!root.getAttribute('data-density')) { set('comfortable'); }
    wrap.addEventListener('click', function (e) {
      var b = e.target.closest ? e.target.closest('button[data-density]') : null;
      if (!b) { return; }
      var i = steps.indexOf(get());
      /* the two buttons step through the scale rather than jumping to a value */
      i += (b.getAttribute('data-density') === 'compact') ? 1 : -1;
      if (i < 0) { i = 0; }
      if (i >= steps.length) { i = steps.length - 1; }
      set(steps[i]);
    });
  })();

  /* ============================= rail toggle ============================= */
  (function () {
    var btn = document.getElementById('tlRailToggle');
    if (!btn) { return; }
    btn.addEventListener('click', function () {
      root.toggleAttribute
        ? root.toggleAttribute('data-rail-open')
        : (root.hasAttribute('data-rail-open')
            ? root.removeAttribute('data-rail-open')
            : root.setAttribute('data-rail-open', ''));
    });
  })();

  /* mark the rail entry matching this page */
  (function () {
    var here = window.location.search;
    var photos = /dynamicalbum\.UpdatesAlbum/.test(here);
    var item = document.querySelector('.tl-nav-item[data-nav="' + (photos ? 'photos' : 'albums') + '"]');
    if (item) { item.classList.add('is-current'); }
  })();

  var stream = document.getElementById('tlStream');
  if (!stream) { return; }

  /* ============================== viewer ================================= */
  var lb, lbImg, lbCap, lbDate, lbCount, lbPrev, lbNext;
  var shots = [], index = -1, lastFocus = null;

  function collect() {
    shots = [];
    var nodes = stream.querySelectorAll('.tl-tile:not(.tl-tile--album)');
    for (var i = 0; i < nodes.length; i++) {
      nodes[i].setAttribute('data-lb', String(i));
      shots.push({
        full:  nodes[i].getAttribute('data-full'),
        title: nodes[i].getAttribute('data-title') || '',
        date:  nodes[i].getAttribute('data-date') || ''
      });
    }
  }

  function build() {
    lb = document.createElement('div');
    lb.className = 'tl-viewer';
    lb.setAttribute('role', 'dialog');
    lb.setAttribute('aria-modal', 'true');
    lb.hidden = true;
    lb.innerHTML =
      '<button class="tl-v-close" type="button" aria-label="Close">×</button>' +
      '<button class="tl-v-nav tl-v-prev" type="button" aria-label="Previous">←</button>' +
      '<button class="tl-v-nav tl-v-next" type="button" aria-label="Next">→</button>' +
      '<div class="tl-v-stage"><div class="tl-v-spin" aria-hidden="true"></div>' +
      '<img class="tl-v-img" alt=""/></div>' +
      '<div class="tl-v-bar"><span class="tl-v-cap"></span>' +
      '<span class="tl-v-date"></span><span class="tl-v-count"></span></div>';
    document.body.appendChild(lb);
    lbImg   = lb.querySelector('.tl-v-img');
    lbCap   = lb.querySelector('.tl-v-cap');
    lbDate  = lb.querySelector('.tl-v-date');
    lbCount = lb.querySelector('.tl-v-count');
    lbPrev  = lb.querySelector('.tl-v-prev');
    lbNext  = lb.querySelector('.tl-v-next');
    lb.querySelector('.tl-v-close').addEventListener('click', close);
    lbPrev.addEventListener('click', function (e) { e.stopPropagation(); step(-1); });
    lbNext.addEventListener('click', function (e) { e.stopPropagation(); step(1); });
    lb.addEventListener('click', function (e) {
      if (e.target === lb || e.target.classList.contains('tl-v-stage')) { close(); }
    });
    /* Changing src aborts any load already in flight, so a single handler is
       enough; the guard is that the old frame is gone before the new src is set. */
    function settle() { lb.classList.remove('is-loading'); }
    lbImg.addEventListener('load', settle);
    lbImg.addEventListener('error', settle);
  }

  function preload(i) {
    if (i < 0 || i >= shots.length) { return; }
    var im = new Image(); im.src = shots[i].full;
  }
  function show(i) {
    if (i < 0 || i >= shots.length) { return; }
    index = i;
    var s = shots[i];
    lb.classList.add('is-loading');
    /* Drop the old frame outright. Changing src alone keeps the previous
       image painted until the new one decodes, so a smaller image lands on
       top of the larger one still showing underneath. */
    lbImg.removeAttribute('src');
    lbImg.removeAttribute('width');
    lbImg.removeAttribute('height');
    lbImg.src = s.full;
    lbCap.textContent = s.title;
    lbDate.textContent = s.date;
    lbCount.textContent = (i + 1) + ' / ' + shots.length;
    lbPrev.disabled = (i === 0);
    lbNext.disabled = (i === shots.length - 1);
    preload(i + 1); preload(i - 1);
  }
  function open(i) {
    if (!lb) { build(); }
    collect();
    lastFocus = document.activeElement;
    lb.hidden = false;
    lb.classList.add('is-open');
    root.style.overflow = 'hidden';
    show(i);
    lb.querySelector('.tl-v-close').focus();
  }
  function close() {
    if (!lb) { return; }
    lb.hidden = true;
    lb.classList.remove('is-open');
    lbImg.src = '';
    root.style.overflow = '';
    if (lastFocus && lastFocus.focus) { lastFocus.focus(); }
  }
  function step(d) {
    var n = index + d;
    if (n >= 0 && n < shots.length) { show(n); }
  }

  document.addEventListener('keydown', function (e) {
    if (!lb || lb.hidden) { return; }
    if (e.key === 'Escape') { close(); }
    else if (e.key === 'ArrowLeft') { step(-1); }
    else if (e.key === 'ArrowRight') { step(1); }
  });

  stream.addEventListener('click', function (e) {
    var a = e.target.closest ? e.target.closest('.tl-tile') : null;
    if (!a || a.classList.contains('tl-tile--album')) { return; }
    if (!a.getAttribute('data-full')) { return; }
    if (e.metaKey || e.ctrlKey || e.shiftKey || e.button !== 0) { return; }
    e.preventDefault();
    collect();
    open(parseInt(a.getAttribute('data-lb'), 10) || 0);
  });

  /* Build the viewer shell, but do not walk every tile yet: open() collects
     on demand. On a whole-library stream the eager pass wrote an attribute
     onto thousands of anchors before the page could respond. */
  build();

  /* ============================ hydration ================================
     Distant months arrive as data rather than markup. Build their tiles only
     when they come near the viewport, or when the scrubber jumps to them, so
     the page is responsive immediately instead of after laying out thousands
     of tiles. Sections and headers already exist, so scroll position and the
     scrubber work before anything is hydrated. */
  var lazyData = {};
  (function () {
    var el = document.getElementById('tlLazyData');
    if (!el) { return; }
    try { lazyData = JSON.parse(el.textContent || '{}'); } catch (e) { lazyData = {}; }
  })();

  var tplThumb = stream.getAttribute('data-thumb-tpl') || '';
  var tplFull  = stream.getAttribute('data-full-tpl') || '';
  var tplLink  = stream.getAttribute('data-link-tpl') || '';
  var albumWord = stream.getAttribute('data-album-label') || 'Album';

  function buildTile(t) {
    /* t = [thumbId, thumbSerial, itemId, aspect, title, date, isAlbum] */
    var cell = document.createElement('div');
    cell.className = 'tl-cell';
    cell.style.setProperty('--ar', t[3]);

    var a = document.createElement('a');
    a.className = t[6] ? 'tl-tile tl-tile--album' : 'tl-tile';
    a.href = tplLink.replace('__ID__', t[2]);
    if (!t[6]) {
      a.setAttribute('data-full', tplFull.replace('__ID__', t[2]));
      a.setAttribute('data-title', t[4] || '');
      if (t[5]) { a.setAttribute('data-date', t[5]); }
    }

    if (t[0]) {
      var img = document.createElement('img');
      img.className = 'giThumbnail';
      img.loading = 'lazy';
      img.decoding = 'async';
      img.alt = '';
      img.src = tplThumb.replace('__TID__', t[0]).replace('__TSN__', t[1]);
      a.appendChild(img);
    } else {
      var ph = document.createElement('span');
      ph.className = 'tl-noimg';
      ph.setAttribute('aria-hidden', 'true');
      a.appendChild(ph);
    }

    var cap = document.createElement('span');
    cap.className = 'tl-tile-cap';
    if (t[6]) {
      var b = document.createElement('span');
      b.className = 'tl-badge';
      b.textContent = albumWord;
      cap.appendChild(b);
      cap.appendChild(document.createTextNode(' '));
    }
    cap.appendChild(document.createTextNode(t[4] || ''));
    a.appendChild(cap);

    cell.appendChild(a);
    return cell;
  }

  function hydrate(section) {
    if (!section || !section.classList.contains('is-lazy')) { return; }
    var key = section.getAttribute('data-key');
    var rows = lazyData[key];
    section.classList.remove('is-lazy');       /* before building, so a second
                                                  observer entry cannot re-enter */
    if (!rows || !rows.length) { return; }
    var into = section.querySelector('.tl-just');
    if (!into) { return; }
    var frag = document.createDocumentFragment();
    for (var i = 0; i < rows.length; i++) { frag.appendChild(buildTile(rows[i])); }
    into.appendChild(frag);
    into.style.minHeight = '';
    section.style.containIntrinsicSize = '';
    delete lazyData[key];
  }

  if ('IntersectionObserver' in window) {
    var hyd = new IntersectionObserver(function (entries) {
      for (var i = 0; i < entries.length; i++) {
        if (entries[i].isIntersecting) {
          hydrate(entries[i].target);
          hyd.unobserve(entries[i].target);
        }
      }
    }, { rootMargin: '1200px 0px' });
    var lazySections = stream.querySelectorAll('.tl-group.is-lazy');
    for (var i = 0; i < lazySections.length; i++) { hyd.observe(lazySections[i]); }
  } else {
    /* no observer: build everything rather than show empty months */
    var all = stream.querySelectorAll('.tl-group.is-lazy');
    for (var j = 0; j < all.length; j++) { hydrate(all[j]); }
  }

  /* ============================== scrubber =============================== */
  /* The scrubber is rendered server-side from the whole item set, so it is
     already complete on first paint. All that is left is behaviour: jump
     within the page when the year is loaded, otherwise follow the link to
     the page that year starts on, and keep the current year marked. */
  var scrub = document.getElementById('tlScrub');

  function yearSection(year) {
    return stream.querySelector('.tl-group[data-key^="' + year + '-"]');
  }

  if (scrub) {
    scrub.addEventListener('click', function (e) {
      var a = e.target.closest ? e.target.closest('.tl-scrub-year') : null;
      if (!a) { return; }
      if (e.metaKey || e.ctrlKey || e.shiftKey || e.button !== 0) { return; }
      var sec = yearSection(a.getAttribute('data-year'));
      if (sec) {
        e.preventDefault();
        /* build that month, and the two after it, before jumping */
        hydrate(sec);
        var n = sec.nextElementSibling, k = 0;
        while (n && k < 2) { hydrate(n); n = n.nextElementSibling; k++; }
        sec.scrollIntoView({ behavior: 'smooth', block: 'start' });
      }
      /* not loaded yet: the href navigates to the page holding that year */
    });

    /* mark whichever year is under the viewport */
    var marks = scrub.querySelectorAll('.tl-scrub-year');
    var byYear = {};
    for (var i = 0; i < marks.length; i++) {
      byYear[marks[i].getAttribute('data-year')] = marks[i];
    }
    /* Observe the group headers instead of measuring every group on each
       scroll frame: the browser reports only what crosses the line. */
    var current = null;
    function mark(el) {
      if (el === current) { return; }
      current = el;
      var y = (el.getAttribute('data-key') || '').slice(0, 4);
      for (var k in byYear) {
        if (Object.prototype.hasOwnProperty.call(byYear, k)) {
          byYear[k].classList.toggle('is-current', k === y);
        }
      }
    }
    if ('IntersectionObserver' in window) {
      var seen = [];
      var io = new IntersectionObserver(function (entries) {
        for (var i = 0; i < entries.length; i++) {
          var t = entries[i].target;
          if (entries[i].isIntersecting) {
            if (seen.indexOf(t) < 0) { seen.push(t); }
          } else {
            var j = seen.indexOf(t);
            if (j >= 0) { seen.splice(j, 1); }
          }
        }
        if (seen.length) {
          seen.sort(function (a, b) { return a.offsetTop - b.offsetTop; });
          mark(seen[0]);
        }
      }, { rootMargin: '-120px 0px -70% 0px' });
      var groups = stream.querySelectorAll('.tl-group');
      for (var g = 0; g < groups.length; g++) { io.observe(groups[g]); }
    }
  }

  /* =========================== infinite scroll =========================== */
  function nextUrl() {
    var a = document.querySelector('.gbNavigator a.next');
    return a ? a.getAttribute('href') : null;
  }
  if (!('IntersectionObserver' in window) || !window.fetch || !nextUrl()) { return; }

  var sentinel = document.createElement('div');
  sentinel.className = 'tl-sentinel';
  sentinel.innerHTML = '<span class="tl-sentinel-dot"></span>';
  stream.parentNode.insertBefore(sentinel, stream.nextSibling);

  var loading = false, done = false;

  function loadMore() {
    if (loading || done) { return; }
    var url = nextUrl();
    if (!url) { done = true; sentinel.remove(); return; }
    loading = true;
    sentinel.classList.add('is-loading');

    fetch(url, { credentials: 'same-origin' })
      .then(function (r) { return r.ok ? r.text() : Promise.reject(r.status); })
      .then(function (html) {
        var doc = new DOMParser().parseFromString(html, 'text/html');
        var incoming = doc.querySelectorAll('#tlStream .tl-group');
        for (var i = 0; i < incoming.length; i++) {
          var g = incoming[i];
          var last = stream.lastElementChild;
          /* a page boundary can split one month across two pages: merge the
             cells into the existing section rather than repeating its header */
          if (last && last.getAttribute('data-key') === g.getAttribute('data-key')) {
            var into = last.querySelector('.tl-just');
            var cells = g.querySelectorAll('.tl-cell');
            for (var c = 0; c < cells.length; c++) {
              into.appendChild(document.importNode(cells[c], true));
            }
          } else {
            stream.appendChild(document.importNode(g, true));
          }
        }
        var oldNav = document.querySelector('.gbNavigator');
        var newNav = doc.querySelector('.gbNavigator');
        if (oldNav && newNav) {
          oldNav.parentNode.replaceChild(document.importNode(newNav, true), oldNav);
        } else if (oldNav) { oldNav.remove(); }

        collect();
        loading = false;
        sentinel.classList.remove('is-loading');
        if (!nextUrl()) { done = true; sentinel.remove(); }
      })
      .catch(function () {
        /* leave Gallery2's own pager in place as the fallback */
        loading = false; done = true;
        sentinel.classList.remove('is-loading');
        sentinel.remove();
      });
  }

  new IntersectionObserver(function (entries) {
    for (var i = 0; i < entries.length; i++) {
      if (entries[i].isIntersecting) { loadMore(); }
    }
  }, { rootMargin: '800px 0px' }).observe(sentinel);
})();
