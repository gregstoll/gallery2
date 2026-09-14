/*
 * flowtheme_2026 - lightbox + infinite scroll
 * Progressive enhancement: with JS off, Gallery2's normal pagination and
 * per-photo pages keep working unchanged.
 */
(function () {
  'use strict';

  var grid = document.querySelector('.ft-just');
  if (!grid) { return; }

  /* ============================ LIGHTBOX ============================ */

  var lb, lbImg, lbCap, lbCount, lbSpin, lbPrev, lbNext;
  var shots = [];      // live list of {href, full, title}
  var index = -1;
  var lastFocus = null;

  function collect() {
    shots = [];
    var nodes = grid.querySelectorAll('.ft-shot:not(.ft-isalbum)');
    for (var i = 0; i < nodes.length; i++) {
      var a = nodes[i];
      a.setAttribute('data-lb', String(i));
      shots.push({
        full:  a.getAttribute('data-full') || a.getAttribute('href'),
        title: a.getAttribute('data-title') || '',
        href:  a.getAttribute('href')
      });
    }
  }

  function build() {
    lb = document.createElement('div');
    lb.className = 'ft-lb';
    lb.setAttribute('role', 'dialog');
    lb.setAttribute('aria-modal', 'true');
    lb.setAttribute('aria-label', 'Image viewer');
    lb.hidden = true;
    lb.innerHTML =
      '<button class="ft-lb-close" type="button" aria-label="Close">×</button>' +
      '<button class="ft-lb-nav ft-lb-prev" type="button" aria-label="Previous">←</button>' +
      '<button class="ft-lb-nav ft-lb-next" type="button" aria-label="Next">→</button>' +
      '<div class="ft-lb-stage"><div class="ft-lb-spin" aria-hidden="true"></div>' +
      '<img class="ft-lb-img" alt=""/></div>' +
      '<div class="ft-lb-bar"><span class="ft-lb-cap"></span>' +
      '<span class="ft-lb-count"></span></div>';
    document.body.appendChild(lb);

    lbImg   = lb.querySelector('.ft-lb-img');
    lbCap   = lb.querySelector('.ft-lb-cap');
    lbCount = lb.querySelector('.ft-lb-count');
    lbSpin  = lb.querySelector('.ft-lb-spin');
    lbPrev  = lb.querySelector('.ft-lb-prev');
    lbNext  = lb.querySelector('.ft-lb-next');

    lb.querySelector('.ft-lb-close').addEventListener('click', close);
    lbPrev.addEventListener('click', function (e) { e.stopPropagation(); step(-1); });
    lbNext.addEventListener('click', function (e) { e.stopPropagation(); step(1); });
    lb.addEventListener('click', function (e) {
      if (e.target === lb || e.target.classList.contains('ft-lb-stage')) { close(); }
    });
  }

  function preload(i) {
    if (i < 0 || i >= shots.length) { return; }
    var im = new Image();
    im.src = shots[i].full;
  }

  function show(i) {
    if (i < 0 || i >= shots.length) { return; }
    index = i;
    var s = shots[i];
    lb.classList.add('is-loading');
    lbImg.src = '';
    lbImg.src = s.full;
    lbCap.textContent = s.title;
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
    document.documentElement.style.overflow = 'hidden';
    show(i);
    lb.querySelector('.ft-lb-close').focus();
  }

  function close() {
    if (!lb) { return; }
    lb.hidden = true;
    lb.classList.remove('is-open');
    lbImg.src = '';
    document.documentElement.style.overflow = '';
    if (lastFocus && lastFocus.focus) { lastFocus.focus(); }
  }

  function step(d) {
    var n = index + d;
    if (n >= 0 && n < shots.length) { show(n); }
  }

  document.addEventListener('keydown', function (e) {
    if (!lb || lb.hidden) { return; }
    if (e.key === 'Escape')     { close(); }
    else if (e.key === 'ArrowLeft')  { step(-1); }
    else if (e.key === 'ArrowRight') { step(1); }
  });

  grid.addEventListener('click', function (e) {
    var a = e.target.closest ? e.target.closest('.ft-shot') : null;
    if (!a || a.classList.contains('ft-isalbum')) { return; }
    if (!a.getAttribute('data-full')) { return; }
    if (e.metaKey || e.ctrlKey || e.shiftKey || e.button !== 0) { return; }
    e.preventDefault();
    collect();
    open(parseInt(a.getAttribute('data-lb'), 10) || 0);
  });

  if (lb === undefined) { build(); }
  lbImg.addEventListener('load',  function () { lb.classList.remove('is-loading'); });
  lbImg.addEventListener('error', function () { lb.classList.remove('is-loading'); });

  /* ========================= INFINITE SCROLL ========================= */

  function nextUrl() {
    var a = document.querySelector('.gbNavigator a.next');
    return a ? a.getAttribute('href') : null;
  }

  if (!('IntersectionObserver' in window) || !window.fetch) { return; }
  if (!nextUrl()) { return; }               /* single page - nothing to do */

  var sentinel = document.createElement('div');
  sentinel.className = 'ft-sentinel';
  sentinel.innerHTML = '<span class="ft-sentinel-dot"></span>';
  grid.parentNode.insertBefore(sentinel, grid.nextSibling);

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
        var incoming = doc.querySelectorAll('.ft-just .ft-cell');
        for (var i = 0; i < incoming.length; i++) {
          grid.appendChild(document.importNode(incoming[i], true));
        }
        /* swap in the new page's navigator so nextUrl() advances */
        var oldNav = document.querySelector('.gbNavigator');
        var newNav = doc.querySelector('.gbNavigator');
        if (oldNav && newNav) { oldNav.parentNode.replaceChild(document.importNode(newNav, true), oldNav); }
        else if (oldNav) { oldNav.remove(); }

        collect();
        loading = false;
        sentinel.classList.remove('is-loading');
        if (!nextUrl()) { done = true; sentinel.remove(); }
      })
      .catch(function () {
        /* leave Gallery2's pagination links in place as the fallback */
        loading = false; done = true;
        sentinel.classList.remove('is-loading');
        sentinel.remove();
      });
  }

  new IntersectionObserver(function (entries) {
    for (var i = 0; i < entries.length; i++) {
      if (entries[i].isIntersecting) { loadMore(); }
    }
  }, { rootMargin: '600px 0px' }).observe(sentinel);
})();
