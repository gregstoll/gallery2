{* flowtheme_2026 - page wrapper *}
<!DOCTYPE html>
<html lang="{g->language}">
  <head>
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <meta name="color-scheme" content="light dark"/>
    {g->head}
    {if empty($head.title)}
      <title>{$theme.item.title|markup:strip|default:$theme.item.pathComponent}</title>
    {/if}
    <link rel="stylesheet" type="text/css" href="{g->theme url="theme.css"}?v=1"/>
    {literal}<script type="text/javascript">
      /* Apply the stored preference before first paint to avoid a flash. */
      (function () {
        try {
          var t = window.localStorage.getItem('ft-theme');
          if (t === 'dark' || t === 'light') {
            document.documentElement.setAttribute('data-theme', t);
          }
        } catch (e) {}
        document.documentElement.className += ' ft-has-infinite';
      })();
    </script>{/literal}
  </head>
  <body class="gallery">
    <div {g->mainDivAttributes}>
    {if $theme.useFullScreen}
      {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
    {else}
      <header class="ft-header">
        <div class="ft-header-inner">
          <a class="ft-brand" href="{g->url}">{g->text text="Gallery"}</a>
          <div class="ft-crumb">{g->block type="core.BreadCrumb"}</div>
          <button type="button" class="ft-toggle" id="ftThemeToggle"
                  title="{g->text text="Switch between system, light and dark appearance"}">
            <span id="ftThemeLabel">Auto</span>
          </button>
          <div class="ft-syslinks">
            {g->block type="core.SystemLinks"
                      order="core.SiteAdmin core.YourAccount core.Login core.Logout"
                      othersAt=4}
          </div>
        </div>
      </header>
      <main class="ft-main">
        {if $theme.pageType == 'progressbar'}
          {g->theme include="progressbar.tpl"}
        {elseif $theme.pageType == 'album'}
          {g->theme include="album.tpl"}
        {elseif $theme.pageType == 'photo'}
          {g->theme include="photo.tpl"}
        {elseif $theme.pageType == 'admin'}
          {g->theme include="admin.tpl"}
        {elseif $theme.pageType == 'module'}
          {g->theme include="module.tpl"}
        {/if}
        <footer class="ft-footer">
          <span>{g->text text="Gallery"}</span>
          {g->logoButton type="gallery2-version"}
        </footer>
      </main>
    {/if}
    </div>
    {literal}<script type="text/javascript">
      (function () {
        var btn = document.getElementById('ftThemeToggle');
        var lbl = document.getElementById('ftThemeLabel');
        if (!btn || !lbl) { return; }
        var order = ['auto', 'light', 'dark'];
        var mq = window.matchMedia
               ? window.matchMedia('(prefers-color-scheme: dark)') : null;
        function current() {
          try {
            var t = window.localStorage.getItem('ft-theme');
            return (t === 'light' || t === 'dark') ? t : 'auto';
          } catch (e) { return 'auto'; }
        }
        /* Say what the setting actually resolved to, so "Auto" is never
           ambiguous about which appearance the system asked for. */
        function label() {
          var m = current();
          if (m === 'auto') { return 'Auto \u00b7 ' + (mq && mq.matches ? 'Dark' : 'Light'); }
          return m === 'dark' ? 'Dark' : 'Light';
        }
        function apply(mode) {
          if (mode === 'auto') {
            document.documentElement.removeAttribute('data-theme');
            try { window.localStorage.removeItem('ft-theme'); } catch (e) {}
          } else {
            document.documentElement.setAttribute('data-theme', mode);
            try { window.localStorage.setItem('ft-theme', mode); } catch (e) {}
          }
          lbl.textContent = label();
        }
        lbl.textContent = label();
        btn.addEventListener('click', function () {
          apply(order[(order.indexOf(current()) + 1) % order.length]);
        });
        if (mq) {
          var onChange = function () { lbl.textContent = label(); };
          /* addEventListener on MediaQueryList is recent; iOS Safari < 14 needs addListener */
          if (mq.addEventListener) { mq.addEventListener('change', onChange); }
          else if (mq.addListener) { mq.addListener(onChange); }
        }
      })();
    </script>{/literal}
    <script type="text/javascript" src="{g->theme url="flow.js"}?v=1"></script>
    {g->trailer}
  </body>
</html>
