{*
 * $Revision: 17067 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html lang="{g->language}" xmlns="http://www.w3.org/1999/xhtml">
  <head>
    {* Let Gallery print out anything it wants to put into the <head> element *}
    {g->head}

    {* If Gallery doesn't provide a header, we use the album/photo title (or filename) *}
    {if empty($head.title)}
      <title>{$theme.item.title|markup:strip|default:$theme.item.pathComponent}</title>
    {/if}

    {* Include this theme's style sheet *}
    <link rel="stylesheet" type="text/css" href="{g->theme url="theme.css"}" />
    {if $theme.pageType == 'album' or $theme.pageType == 'photo'}
    <script type="text/javascript">
      // <![CDATA[
      var DEBUG_AJAXIAN = false;  /* Show debug output down the side */
      var LOADING_IMAGE = '{g->text text="Loading Image..." forJavascript=true}';
      var PHOTO_DATA = '{g->text text="Photo Data" forJavascript=true}';
      var FILE_SIZE = '{g->text text="Full image size: %s kilobytes."
                arg1="%SIZE%" forJavascript=true}';
      var SUMMARY = '{g->text text="Summary: " forJavascript=true}';
      var DESCRIPTION = '{g->text text="Description: " forJavascript=true}';
      var VIEW_IMAGE = '{g->text text="View fullsize image" forJavascript=true}';
      // ]]>
    </script>
    <script type="text/javascript" src="{g->theme url="javascript/common-functions.js"}"></script>
    <script type="text/javascript" src="{g->theme url="javascript/thumbnail-functions.js"}"></script>
    <script type="text/javascript" src="{g->theme url="javascript/slideshow-functions.js"}"></script>
    {/if}
    {* Remove thumbnail rollover effect if image frames are used *}
    {if isset($theme.params.itemFrame) || isset($theme.params.photoFrame)}
    {literal}
    <style type="text/css">
    #gsThumbMatrix td {
        padding: 0;
    }
    #gsThumbMatrix a:hover .giThumbnail {
        height: 44px;
        width: 44px;
        margin: -1px;
        border: none;
    }
    #gsThumbMatrix a:hover:active .giThumbnail {
        height: 44px;
        width: 44px;
    }
    </style>
    {/literal}
    {/if}
  </head>
  <body class="gallery">
    <div id="msgarea"></div>

    <div {g->mainDivAttributes}>
    <div id="white-rap">
      {*
       * Some module views (eg slideshow) want the full screen.  So for those, we don't draw
       * a header, footer, navbar, etc.  Those views are responsible for drawing everything.
       *}
      {if $theme.useFullScreen}
    {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
      {elseif $theme.pageType == 'progressbar'}
    <div id="header">
      <h1><img src="{g->url href="images/galleryLogo_sm.gif"}" width="107" height="48" alt=""
       id="main-logo" /></h1>
    </div>
    <div id="main"><div id="frame">
      {g->theme include="progressbar.tpl"}
    </div></div>
      {else}
      <div id="header">
    <h1><a href="{g->url}"><img src="{g->url href="images/galleryLogo_sm.gif"}"
      width="107" height="48" alt="Gallery" id="main-logo" /></a></h1>
    {g->block type="search.SearchBlock" showAdvancedLink=false}

    <div class="gbBreadCrumb">
      {g->block type="core.BreadCrumb"}
    </div>
      </div>

      {include file="gallery:modules/core/templates/JavaScriptWarning.tpl" l10Domain="modules_core"}

      <div id="main">
    <div id="frame">
      {* Include the appropriate content type for the page we want to draw. *}
      {if $theme.pageType == 'album'}
        {g->theme include="album.tpl"}
      {elseif $theme.pageType == 'photo'}
        {g->theme include="photo.tpl"}
      {elseif $theme.pageType == 'admin'}
        {g->theme include="admin.tpl"}
      {elseif $theme.pageType == 'module'}
        {g->theme include="module.tpl"}
      {/if}
    </div>
      </div>

      <div id="footer">
    <p>{g->logoButton type="validation"}
    {g->logoButton type="gallery2"}
    {g->logoButton type="gallery2-version"}</p>

    <p id="footerSystemLinks">
      {g->block type="core.SystemLinks"
            order="core.SiteAdmin core.YourAccount core.Login core.Logout"
            othersAt=4}
    </p>
      </div>
      {/if}
    </div>
    </div>

    {*
     * Give Gallery a chance to output any cleanup code, like javascript that needs to be run
     * at the end of the <body> tag.  If you take this out, some code won't work properly.
     *}
    {g->trailer}

    {* Put any debugging output here, if debugging is enabled *}
    {g->debug}
  </body>
</html>
