{*
 * $Revision: 17693 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !empty($theme.imageViews)}
  {assign var="image" value=$theme.imageViews[$theme.imageViewsIndex]}
{/if}
{* Check for exif block *}
{assign var="showExifLink" value=false}
{if $theme.params.photoProperties}
  {foreach from=$theme.params.photoBlocks item=block}
    {if $block.0 == 'exif.ExifInfo'}
      {capture name="exifBlock"}{g->block type=$block.0 params=$block.1}{/capture}
      {if $smarty.capture.exifBlock|trim}
	{assign var="showExifLink" value=true}
	<div id="exif" class="gcPopupBackground"
	 style="position:absolute; left:0px; top:0px; padding:1px; visibility:hidden; z-index:5000">
	  <table cellspacing="0" cellpadding="0">
	    <tr>
	      <td style="padding-left:5px;">
		<h2>{g->text text="Exif"}</h2>
	      </td>
	      <td align="right">
		<div class="buttonClose"><a href="javascript:void(0);"
		 onclick="toggleExif('photo','exif'); return false;"
		 title="{g->text text="Close"}"></a></div>
	      </td>
	    </tr>
	    <tr>
	      <td colspan="2" class="gcBackground2" style="padding-bottom:5px;">
		{$smarty.capture.exifBlock}
	      </td>
	    </tr>
	  </table>
	</div>
      {/if}
    {/if}
  {/foreach}
{/if}

<table class="gcBackground1" width="100%" cellspacing="0" cellpadding="0">
  <tr valign="top">
    <td>
      <div id="gsContent" class="gcBorder1">
	<div class="gbBlockTop">
	  <table>
	    <tr>
	      <td class="gsActionIcon">
		<div class="buttonShowSidebar"><a href="{g->url params=$theme.pageUrl 
		  arg1="jsWarning=true"}"
		 onclick="slideIn('sidebar'); return false;"
		 title="{g->text text="Show Sidebar"}"></a></div>
	      </td>
	      {if (isset($links) || isset($theme.itemLinks))}
		{if !isset($links)}{assign var="links" value=$theme.itemLinks}{/if}

		{foreach from=$links item=itemLink}
		  {if $itemLink.moduleId == "slideshow"}
		  <td class="gsActionIcon">
		    <div class="buttonViewSlideshow">{g->itemLink link=$itemLink
		     title="`$itemLink.text`" text="" class=""}</div>
		  </td>
		  {elseif $itemLink.moduleId == "comment"}
		    {if $itemLink.params.view == "comment.ShowAllComments"}
		    <td class="gsActionIcon">
		      <div class="buttonViewComments">{g->itemLink link=$itemLink
		     title="`$itemLink.text`" text="" class=""}</div>
		    </td>
		    {/if}
		  {/if}
		{/foreach}
	      {/if}
	    </tr>
	  </table>
	</div>

	<div class="gsContentPhoto">
	  <table align="center" cellpadding="0" cellspacing="0">
	    {if $theme.params.navigatorPhotoTop}
	    <tr>
	      <td class="gbNavigatorPhoto">
		<div class="gbNavigator">
		  {g->theme include="navigator.tpl"}
		</div>
	      </td>
	      <td>&nbsp;</td>
	    </tr>
	    {/if}
	    <tr>
	      <td>
		<div id="gsImageView" class="gbBlock">
		  {if !empty($theme.imageViews)}
		    {capture name="fallback"}
		    <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$theme.item.id`"
				     forceFullUrl=true forceSessionId=true}">
		      {g->text text="Download %s" arg1=$theme.sourceImage.itemTypeName.1}
		    </a>
		    {/capture}
		    {if ($image.viewInline)}
		      {if isset($theme.photoFrame)}
			{g->container type="imageframe.ImageFrame" frame=$theme.photoFrame
				      width=$image.width height=$image.height}
			  <div id="photo">
			  {g->image id="%ID%" item=$theme.item image=$image
				    fallback=$smarty.capture.fallback class="%CLASS%"}
			  </div>
			{/g->container}
		      {else}
			<div id="photo">
			{g->image item=$theme.item image=$image fallback=$smarty.capture.fallback}
			</div>
		      {/if}
		    {else}
		      {$smarty.capture.fallback}
		    {/if}
		  {else}
		    {g->text text="There is nothing to view for this item."}
		  {/if}
		</div>
	      </td>
	      <td align="left" width="240" valign="top">
		{if $theme.params.showMicroThumbs}
		<div class="gsContentDetail gcBorder1">
		  <div class="gbNavigatorMicroThums">
		    {g->theme include="navigatorMicroThumbs.tpl"}
		  </div>
		</div>
		{/if}
		<div class="gsContentDetail">
		  <div class="gbBlock">
		    {if !empty($theme.item.title)}
		    <h2> {$theme.item.title|markup} </h2>
		    {/if}
		    {if !empty($theme.item.description)}
		    <p class="giDescription">
		      {$theme.item.description|markup}
		    </p>
		    {/if}
		  </div>
		  <div class="gbBlock">
		    {g->block type="core.ItemInfo"
			      item=$theme.item
			      showDate=true
			      showOwner=$theme.params.showImageOwner
			      class="giInfo"}
		  </div>
		  <div class="gbBlock">
		    {* Show the photo blocks chosen for this theme *}
		    {foreach from=$theme.params.photoUpperBlocks item=block}
		      {g->block type=$block.0 params=$block.1}
		    {/foreach}
		  </div>
		</div>
	      </td>
	    </tr>
	    {if $theme.params.navigatorPhotoBottom}
	    <tr>
	      <td class="gbNavigatorPhoto">
		<div class="gbNavigator">
		  {g->theme include="navigator.tpl"}
		</div>
	      </td>
	      <td>&nbsp;</td>
	    </tr>
	    {/if}
	  </table>
	</div>

	{if $theme.pageUrl.view != 'core.ShowItem' && $theme.params.dynamicLinks == 'jumplink'}
	<div class="gbBlock">
	  <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$theme.item.id`"}">
	    {g->text text="View in original album"}
	  </a>
	</div>
	{/if}

	{* Download link for item in original format *}
	{if !empty($theme.sourceImage) && $theme.sourceImage.mimeType != $theme.item.mimeType}
	<div class="gbBlock">
	  <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$theme.item.id`"}">
	    {g->text text="Download %s in original format" arg1=$theme.sourceImage.itemTypeName.1}
	  </a>
	</div>
	{/if}

	{* Show any other photo blocks (comments, etc) *}
	{foreach from=$theme.params.photoBlocks item=block}
	  {if !$theme.params.photoProperties || $block.0 != 'exif.ExifInfo'}
	    {g->block type=$block.0 params=$block.1}
	  {/if}
	{/foreach}

	{* Our emergency edit link, if the user removes all blocks containing edit links *}
	{g->block type="core.EmergencyEditItemLink" class="gbBlock" 
		  checkBlocks="sidebar,photo,photoUpper"}
      </div>
    </td>
  </tr>
</table>
{if !empty($theme.params.sidebarBlocks)}
  {g->theme include="sidebar.tpl"}
{/if}
