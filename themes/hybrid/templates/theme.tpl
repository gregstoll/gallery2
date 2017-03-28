{*
 * $Revision: 17075 $
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
    <link rel="stylesheet" type="text/css" href="{g->theme url="theme.css"}"/>
  </head>
  <body class="gallery">
    <div {g->mainDivAttributes}>
      {if $theme.pageType == 'album' || $theme.pageType == 'photo'}
	{g->theme include="hybrid.tpl"}
      {elseif $theme.pageType == 'progressbar'}
	<div id="gsHeader">
	  <img src="{g->url href="images/galleryLogo_sm.gif"}" width="107" height="48" alt=""/>
	</div>
	{g->theme include="progressbar.tpl"}
      {elseif $theme.useFullScreen}
	{include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
      {else}
	<div id="gsHeader">
	  <a href="{g->url}"><img src="{g->url href="images/galleryLogo_sm.gif"}"
	   width="107" height="48" alt=""/></a>
	</div>

	<div id="gsNavBar" class="gcBorder1">
	  <div class="gbSystemLinks">
	    {g->block type="core.SystemLinks"
		      order="core.SiteAdmin core.YourAccount core.Login core.Logout"
		      othersAt=4}
	  </div>
	  <div class="gbBreadCrumb">
	    {g->block type="core.BreadCrumb"}
	  </div>
        </div>

	{if $theme.pageType == 'admin'}
	  {include file="gallery:`$theme.adminTemplate`" l10Domain=$theme.adminL10Domain}
	{elseif $theme.pageType == 'module'}
	<table width="100%" cellspacing="0" cellpadding="0">
	  <tr valign="top">
	    <td id="gsSidebarCol">
	      <div id="gsSidebar" class="gcBorder1">
		{* Show the sidebar blocks chosen for this theme *}
		{foreach from=$theme.params.sidebarBlocks item=block}
		  {g->block type=$block.0 params=$block.1 class="gbBlock"}
		{/foreach}
	      </div>
	    </td>
	    <td>
	      {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
	    </td>
	  </tr>
	</table>
	{/if}

	<div id="gsFooter">
	  {g->logoButton type="validation"}
	  {g->logoButton type="gallery2"}
	  {g->logoButton type="gallery2-version"}
          {g->logoButton type="donate"}
	</div>
      {/if}
    </div>

    {g->trailer}
    {g->debug}
  </body>
</html>
