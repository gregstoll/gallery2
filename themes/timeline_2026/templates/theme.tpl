{* timeline_2026 - page shell: navigation rail + content column *}
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
    <link rel="stylesheet" type="text/css" href="{g->theme url="theme.css"}?v=5"/>
    {literal}<script type="text/javascript">
      /* Apply the stored appearance before first paint, to avoid a flash. */
      (function () {
        try {
          var t = window.localStorage.getItem('tl-theme');
          if (t === 'dark' || t === 'light') {
            document.documentElement.setAttribute('data-theme', t);
          }
          var d = window.localStorage.getItem('tl-density');
          if (d) { document.documentElement.setAttribute('data-density', d); }
        } catch (e) {}
      })();
    </script>{/literal}
  </head>
  <body class="gallery">
    <div {g->mainDivAttributes}>
    {if $theme.useFullScreen}
      {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
    {else}
      {* Site Admin, Your Account and the item editors bring their own left
         nav and their own panels. Showing the rail beside them gives two
         sidebars and a pane inside a pane, so those run full width. *}
      {assign var="taskPage" value=0}
      {if $theme.pageType == 'admin' || $theme.pageType == 'module'}
        {assign var="taskPage" value=1}
      {/if}
      <div class="tl-shell{if $taskPage} tl-shell--wide{/if}">

        {if !$taskPage}
        <aside class="tl-rail" id="tlRail">
          <a class="tl-brand" href="{g->url}">
            <span class="tl-brand-mark" aria-hidden="true"></span>
            <span class="tl-brand-text">{g->text text="Gallery"}</span>
          </a>

          <nav class="tl-nav" aria-label="{g->text text="Sections"}">
            <a class="tl-nav-item" data-nav="photos"
               href="{g->url arg1="view=dynamicalbum.UpdatesAlbum"}">
              <span class="tl-ico tl-ico-photos" aria-hidden="true"></span>
              {g->text text="Photos"}
            </a>
            <a class="tl-nav-item" data-nav="albums"
               href="{if !empty($theme.rootAlbumId)}{g->url arg1="itemId=`$theme.rootAlbumId`"}{else}{g->url}{/if}">
              <span class="tl-ico tl-ico-albums" aria-hidden="true"></span>
              {g->text text="Albums"}
            </a>
          </nav>

          <div class="tl-rail-blocks">{g->theme include="sidebar.tpl"}</div>
        </aside>
        {/if}

        <div class="tl-column">
          <header class="tl-topbar">
            {if $taskPage}
              <a class="tl-brand tl-brand--bar"
                 href="{if !empty($theme.rootAlbumId)}{g->url arg1="itemId=`$theme.rootAlbumId`"}{else}{g->url}{/if}">
                <span class="tl-brand-mark" aria-hidden="true"></span>
                <span class="tl-brand-text">{g->text text="Gallery"}</span>
              </a>
              <nav class="tl-topnav" aria-label="{g->text text="Sections"}">
                <a href="{g->url arg1="view=dynamicalbum.UpdatesAlbum"}">{g->text text="Photos"}</a>
                <a href="{if !empty($theme.rootAlbumId)}{g->url arg1="itemId=`$theme.rootAlbumId`"}{else}{g->url}{/if}">{g->text text="Albums"}</a>
              </nav>
            {else}
              <button type="button" class="tl-railtoggle" id="tlRailToggle"
                      aria-label="{g->text text="Toggle navigation"}">
                <span aria-hidden="true"></span>
              </button>
            {/if}

            <div class="tl-search">{g->block type="search.SearchBlock" showAdvancedLink=false}</div>

            <div class="tl-tools">
              <div class="tl-density" role="group" aria-label="{g->text text="Tile size"}">
                <button type="button" data-density="comfortable" title="{g->text text="Larger tiles"}">&minus;</button>
                <button type="button" data-density="compact" title="{g->text text="Smaller tiles"}">&plus;</button>
              </div>
              <button type="button" class="tl-appearance" id="tlThemeToggle"
                      title="{g->text text="Switch between system, light and dark appearance"}">
                <span id="tlThemeLabel">Auto</span>
              </button>
              <div class="tl-syslinks">
                {g->block type="core.SystemLinks"
                          order="core.SiteAdmin core.YourAccount core.Login core.Logout"
                          othersAt=4}
              </div>
            </div>
          </header>

          <div class="tl-crumb">{g->block type="core.BreadCrumb"}</div>

          <main class="tl-main">
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

            <footer class="tl-footer">
              <span>{g->text text="Gallery"}</span>
              {g->logoButton type="gallery2-version"}
            </footer>
          </main>
        </div>
      </div>
    {/if}
    </div>
    <script type="text/javascript" defer src="{g->theme url="timeline.js"}?v=5"></script>
    {g->trailer}
  </body>
</html>
