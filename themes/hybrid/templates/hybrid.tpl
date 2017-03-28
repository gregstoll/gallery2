{*
 * $Revision: 20936 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="popup_details" class="gcBorder2"><object type="text/html"></object></div>

<div id="popup_titlebar" class="gcBackground1 giDescription">
  <div style="float: right; margin-left: 2px">
    <img src="{$theme.themeUrl}/images/down.png" width="18" height="18"
     onclick="popup_vis(0)" alt=""/>
  </div>
  <strong> {g->text text="Item Details"} </strong>
</div>

<div id="imageview" class="gcBackground2">
  <div id="imagearea"><div id="imagediv" onclick="image_vis(0)"></div></div>
  <div id="textdiv" class="gcBackground1 gcBorder2">
    <div id="tools_left">
      <img id="text_on" src="{$theme.themeUrl}/images/up.png"
       width="18" height="18" onclick="text_onoff()" style="display: none"
       alt="{g->text text="Show text when available"}"
       title="{g->text text="Show text when available"}"
     /><img id="text_none" src="{$theme.themeUrl}/images/up-off.png"
       width="18" height="18" onclick="text_onoff()" style="display: none"
       alt="{g->text text="Show text when available"}"
       title="{g->text text="Show text when available"}"
     /><img id="text_off" src="{$theme.themeUrl}/images/down.png"
       width="18" height="18" onclick="text_onoff()"
       alt="{g->text text="Keep text hidden"}" title="{g->text text="Keep text hidden"}"
     /><img src="{$theme.themeUrl}/images/alb.png"
       width="18" height="18" onclick="image_vis(0)"
       alt="{g->text text="Back to Album View"}" title="{g->text text="Back to Album View"}"
     /><img id="slide_poz" src="{$theme.themeUrl}/images/poz.png"
       width="18" height="18" onclick="slide_onoff()" style="display: none"
       alt="{g->text text="Pause Slideshow"}" title="{g->text text="Pause Slideshow"}"
     /><img id="slide_fwd" src="{$theme.themeUrl}/images/fwd.png"
       width="18" height="18" onclick="slide_onoff()"
       alt="{g->text text="Start Slideshow"}" title="{g->text text="Start Slideshow"}"
     /><img id="slide_rev" src="{$theme.themeUrl}/images/rev.png"
       width="18" height="18" onclick="slide_onoff()" style="display: none"
       alt="{g->text text="Start Slideshow"}" title="{g->text text="Start Slideshow"}"
     /><img id="slide_rand" src="{$theme.themeUrl}/images/rand.png"
       width="18" height="18" onclick="slide_onoff()" style="display: none"
       alt="{g->text text="Start Slideshow"}" title="{g->text text="Start Slideshow"}"/>
     <span id="date" class="giInfo"></span>
    </div>
    <div id="tools_right">
      <img id="full_size" src="{$theme.themeUrl}/images/full.png"
       width="18" height="18" onclick="image_zoom(1)" style="display: none"
       alt="{g->text text="Full Size"}" title="{g->text text="Full Size"}"
     /><img id="fit_size" src="{$theme.themeUrl}/images/fit.png"
       width="18" height="18" onclick="image_zoom(0)" style="display: none"
       alt="{g->text text="Fit Size"}" title="{g->text text="Fit Size"}"
     /><img src="{$theme.themeUrl}/images/info.png"
       width="18" height="18" onclick="popup_info(-1)"
       alt="{g->text text="Item Details"}" title="{g->text text="Item Details"}"
     /><img id="prev_off" src="{$theme.themeUrl}/images/prev-off.png"
       width="18" height="18" style="display: none"
       alt="{g->text text="No Previous Image"}" title="{g->text text="No Previous Image"}"
     /><img id="prev_img" src="{$theme.themeUrl}/images/prev.png"
       width="18" height="18" onclick="image_prev()"
       alt="{g->text text="Previous Image"}" title="{g->text text="Previous Image"}"
     /><img id="prev_page" src="{$theme.themeUrl}/images/prevpage.png"
       width="18" height="18" onclick="image_prev()" style="display: none"
       alt="{g->text text="Previous Image (Load new page)"}"
       title="{g->text text="Previous Image (Load new page)"}"
     /><img id="next_off" src="{$theme.themeUrl}/images/next-off.png"
       width="18" height="18" style="display: none"
       alt="{g->text text="No Next Image"}" title="{g->text text="No Next Image"}"
     /><img id="next_img" src="{$theme.themeUrl}/images/next.png"
       width="18" height="18" onclick="image_next()"
       alt="{g->text text="Next Image"}" title="{g->text text="Next Image"}"
     /><img id="next_page" src="{$theme.themeUrl}/images/nextpage.png"
       width="18" height="18" onclick="image_next()" style="display: none"
       alt="{g->text text="Next Image (Load new page)"}"
       title="{g->text text="Next Image (Load new page)"}"/>
    </div>
    <div id="title" class="giTitle"></div>
    <div id="text" class="gcBackground2 gcBorder2"></div>
  </div>
</div>

<table id="hybridMain" border="0" cellspacing="0" cellpadding="0"><tr valign="top">
<td>
  <div id="sidebar" class="gcBackground1 gcBorder2" style="display: none">
    {g->theme include="sidebar.tpl"}
  </div>
</td><td id="gsContent">
  <div id="album_titlebar" class="gcBackground1 gcBorder2">
   <div id="album_tools">
      <img id="sidebar_min" src="{$theme.themeUrl}/images/left.png"
       width="18" height="18" onclick="sidebar_onoff()" style="display: none"
       alt="{g->text text="Hide sidebar"}" title="{g->text text="Hide sidebar"}"
     /><img id="sidebar_max" src="{$theme.themeUrl}/images/right.png"
       width="18" height="18" onclick="sidebar_onoff()"
       alt="{g->text text="Show sidebar"}" title="{g->text text="Show sidebar"}"
     /><img id="slide__fwd" src="{$theme.themeUrl}/images/fwd.png"
       width="18" height="18" onclick="slide_onoff()"
       {if $theme.imageCount==0}style="display: none"{/if}
       alt="{g->text text="Start Slideshow"}" title="{g->text text="Start Slideshow"}"
     /><img id="slide__rev" src="{$theme.themeUrl}/images/rev.png"
       width="18" height="18" onclick="slide_onoff()" style="display: none"
       alt="{g->text text="Start Slideshow"}" title="{g->text text="Start Slideshow"}"
     /><img id="slide__rand" src="{$theme.themeUrl}/images/rand.png"
       width="18" height="18" onclick="slide_onoff()" style="display: none"
       alt="{g->text text="Start Slideshow"}" title="{g->text text="Start Slideshow"}"/>
    </div>

    <table width="90%"><tr><td style="width: 50%">
      <div class="giTitle"> {$theme.item.title|markup} </div>
      <div id="album_desc" class="giDescription"> {$theme.item.description|markup} </div>
      {include file="gallery:modules/core/templates/JavaScriptWarning.tpl" l10Domain="modules_core"}
    </td><td>
      <div id="album_info">
	{g->block type="core.ItemInfo" item=$theme.item
		  showSummaries=false showDate=$theme.params.showAlbumDate
		  showSize=$theme.params.showSize showViewCount=$theme.params.showViewCount
		  showOwner=$theme.params.showAlbumOwner class="giInfo"}
      </div>
    </td></tr></table>
  </div>
  <div id="gsAlbumContent">
    <table class="content">
    {foreach name="hybrid" from=$theme.children key=i item=it}
      {if ($i % $theme.params.columns == 0)}<tr>{/if}
      <td class="i">
	{capture name="thumbTitle"}{strip}
	  {if $theme.params.thumbTitle == 'title'}
	    {$it.title|markup:strip}
	  {elseif $theme.params.thumbTitle == 'date'}
	    {g->date timestamp=$it.originationTimestamp}
	  {elseif $theme.params.thumbTitle == 'datetime'}
	    {g->date timestamp=$it.originationTimestamp style="datetime"}
	  {elseif $theme.params.thumbTitle == 'titledate'}
	    {$it.title|markup:strip} ({g->date timestamp=$it.originationTimestamp})
	  {elseif $theme.params.thumbTitle == 'titledatetime'}
	    {$it.title|markup:strip} ({g->date timestamp=$it.originationTimestamp style="datetime"})
	  {/if}
	{/strip}{/capture}
	{if isset($it.image)}
	  {if isset($it.renderItem)}
	    <a id="img_{$it.imageIndex}" href="{g->url params=$theme.pageUrl
	     arg1="itemId=`$it.id`" arg2="renderId=`$it.image.id`"}"></a>
	  {else}
	    <a id="img_{$it.imageIndex}" href="{g->url arg1="view=core.DownloadItem"
	     arg2="itemId=`$it.image.id`" arg3="serialNumber=`$it.image.serialNumber`"}"></a>
	  {/if}
	  <a id="info_{$it.imageIndex}" href="{g->url params=$theme.pageUrl
	   arg1="itemId=`$it.id`" arg2="detail=1"}"></a>
	  {capture assign=href}
	    <a href="" onclick="image_show({$it.imageIndex});return false">
	  {/capture}
	  {if isset($it.thumbnail) && isset($theme.params.itemFrame)}
	    {g->container type="imageframe.ImageFrame" frame=$theme.params.itemFrame
			  width=$it.thumbnail.width height=$it.thumbnail.height}
	      {$href}
	      {g->image item=$it image=$it.thumbnail id="%ID%" class="%CLASS% giThumbnail"
			title=$smarty.capture.thumbTitle}
	      </a>
	    {/g->container}
	  {else}
	    {$href}
	    {if isset($it.thumbnail)}
	      {g->image item=$it image=$it.thumbnail class="giThumbnail"
			title=$smarty.capture.thumbTitle}
	    {else}
	      {g->text text="no thumbnail"}
	    {/if}
	    </a>
	  {/if}
	{elseif ($it.canContainChildren || $it.entityType == 'GalleryLinkItem')}
	  {capture assign=href}
	    <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$it.id`"}">
	  {/capture}
	  {if isset($it.thumbnail) && isset($theme.params.albumFrame)}
	    {g->container type="imageframe.ImageFrame" frame=$theme.params.albumFrame
			  width=$it.thumbnail.width height=$it.thumbnail.height}
	      {$href}
	      {g->image item=$it image=$it.thumbnail id="%ID%" class="%CLASS% giThumbnail"
			title=$smarty.capture.thumbTitle}
	      </a>
	    {/g->container}
	  {else}
	    {$href}
	    {if isset($it.thumbnail)}
	      {g->image item=$it image=$it.thumbnail class="giThumbnail"
			title=$smarty.capture.thumbTitle}
	    {else}
	      {g->text text="no thumbnail"}
	    {/if}
	    </a>
	  {/if}
	{else}
	  <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$it.id`"}">
	    {if isset($it.thumbnail)}
	      {g->image item=$it image=$it.thumbnail class="giThumbnail"
			title=$smarty.capture.thumbTitle}
	    {else}
	      {g->text text="no thumbnail"}
	    {/if}
	  </a>
	{/if}
      </td>
      <td class="t">
      {if $theme.params.showText}
	<table class="itemtext"><tr><td>
	  <div class="title gcBackground1">
      {/if}
	    {if isset($it.image) || isset($it.itemLinks)}
	      <span{if $theme.params.showText} style="float: right"{/if}><img
	       src="{$theme.themeUrl}/images/menu.png" class="popup_button" width="18" height="18"
	       alt="{g->text text="Item Actions"}" title="{g->text text="Item Actions"}"
	       onclick="popup_menu(event,{$i}
		{if isset($it.image)},{$it.imageIndex})"/></span>
	      <span id="title_{$it.imageIndex}" class="giTitle"
		{else},-1)"/></span>
	      <span class="giTitle"{/if}
	    {else}
	      <span class="giTitle"
	    {/if}
	    {if !$theme.params.showText} style="display: none"{/if}>
	    {$it.title|markup}</span>
      {if $theme.params.showText}
	  </div>
	</td></tr><tr><td class="giDescription">
	  {if isset($it.summary)}{$it.summary|markup}{/if}
	  {if $it.canContainChildren}{assign var="showOwner" value=$theme.params.showAlbumOwner}
	  {else}{assign var="showOwner" value=$theme.params.showImageOwner}{/if}
	  {if $it.canContainChildren}{assign var="showDate" value=$theme.params.showAlbumDate}
	  {else}{assign var="showDate" value=$theme.params.showImageDate}{/if}
	  {g->block type="core.ItemInfo" item=$it class="giInfo"
		    showSummaries=true showDate=$showDate showSize=$theme.params.showSize
		    showViewCount=$theme.params.showViewCount showOwner=$showOwner}
      {/if}
	  {if isset($it.image)}
	    <span id="text_{$it.imageIndex}" style="display: none">{$it.description|markup}</span>
	    <span id="date_{$it.imageIndex}" style="display: none">{if
	     $theme.params.showDateInViewer}{g->date timestamp=$it.originationTimestamp}{/if}</span>
	  {/if}
      {if $theme.params.showText}
	</td></tr></table>
      {/if}
	{if isset($it.itemLinks)}
	  <span id="links_{$i}" style="display: none">
	  {foreach from=$it.itemLinks item=link}
	    {g->itemLink link=$link class=null}<br/>
	  {/foreach}
	  </span>
	{/if}
      </td>
      {if (($i+1) % $theme.params.columns == 0)}</tr>
      {elseif $smarty.foreach.hybrid.last}
	{assign var="i" value=$i%$theme.params.columns}
	{assign var="i" value=$theme.params.columns-$i-1}
	{"<td></td><td></td>"|repeat:$i}</tr>
      {/if}
    {/foreach}
    {if $theme.totalPages > 1}
      <tr><td colspan="{$theme.params.columns*2}">
	{g->block type="core.Pager" class="gbBlock gcBackground1"}
      </td></tr>
    {/if}
    </table>
  </div>
  {* Show any other album blocks *}
  {foreach from=$theme.params.albumBlocks item=block}
    {g->block type=$block.0 params=$block.1 class="gbBlock"}
  {/foreach}
</td>
</tr></table>

<div id="popup_menu" class="gcBackground1 gcBorder2" onmouseover="clearTimeout(popup_timer)"
 onmouseout="popup_timer=setTimeout('ui_vis(\'popup_menu\',0)',1000)">
  <div id="popup_links" onclick="ui_vis('popup_menu',0)"></div>
</div>

<script type="text/javascript">app_init();</script>
