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
    lbImg.addEventListener('load',  function () { lb.classList.remove('is-loading'); });
    lbImg.addEventListener('error', function () { lb.classList.remove('is-loading'); });
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

  build();
  collect();

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
        /* already loaded: scroll instead of reloading the page */
        e.preventDefault();
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
    function markCurrent() {
      var groups = stream.querySelectorAll('.tl-group');
      var top = null;
      for (var i = 0; i < groups.length; i++) {
        var r = groups[i].getBoundingClientRect();
        if (r.top <= 140) { top = groups[i]; } else { break; }
      }
      if (!top) { top = groups[0]; }
      if (!top) { return; }
      var y = (top.getAttribute('data-key') || '').slice(0, 4);
      for (var k in byYear) {
        if (Object.prototype.hasOwnProperty.call(byYear, k)) {
          byYear[k].classList.toggle('is-current', k === y);
        }
      }
    }
    var ticking = false;
    window.addEventListener('scroll', function () {
      if (ticking) { return; }
      ticking = true;
      window.requestAnimationFrame(function () { markCurrent(); ticking = false; });
    }, { passive: true });
    markCurrent();
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
