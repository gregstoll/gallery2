{*
 * $Revision: 17524 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
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
		  {if $itemLink.moduleId == "cart"}
		  <td class="gsActionIcon">
		    <div class="buttonCart">{g->itemLink link=$itemLink
		     title="`$itemLink.text`" text="" class=""}</div>
		  </td>
		  {elseif $itemLink.moduleId == "comment"}
		    {if $itemLink.params.view == "comment.AddComment" }
		    <td class="gsActionIcon">
		      <div class="buttonAddComment">{g->itemLink link=$itemLink
		     title="`$itemLink.text`" text="" class=""}</div>
		    </td>
		    {elseif $itemLink.params.view == "comment.ShowAllComments"}
		    <td class="gsActionIcon">
		      <div class="buttonViewComments">{g->itemLink link=$itemLink
		     title="`$itemLink.text`" text="" class=""}</div>
		    </td>
		    {/if}
		  {elseif $itemLink.moduleId == "slideshow"}
		  <td class="gsActionIcon">
		    <div class="buttonViewSlideshow">{g->itemLink link=$itemLink
		     title="`$itemLink.text`" text="" class=""}</div>
		  </td>
		  {/if}
		{/foreach}
	      {/if}
	    </tr>
	  </table>
	</div>

	{if !empty($theme.navigator)}
	<div class="gbNavigator">
	  {* {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true} *}
	  {g->theme include="navigator.tpl"}
	</div>
	{/if}

	<table width="100%" cellspacing="0" cellpadding="0">
	  <tr valign="top">
	    <td width="30%">
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
			    showSize=true
			    showOwner=$theme.params.showAlbumOwner
			    class="giInfo"}
		</div>
		<div class="gbBlock">
		  {* Show the album blocks chosen for this theme *}
		  {foreach from=$theme.params.albumUpperBlocks item=block}
		    {g->block type=$block.0 params=$block.1}
		  {/foreach}
		</div>
	      </div>
	    </td>
	    <td>
	      {if !count($theme.children)}
	      <div class="giDescription gbEmptyAlbum">
		<h3 class="emptyAlbum">
		  {g->text text="This album is empty."}
		  {if isset($theme.permissions.core_addDataItem)}
		  <br/>
		  <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd"
				   arg3="itemId=`$theme.item.id`"}">
		    {g->text text="Add a photo!"}
		  </a>
		  {/if}
		</h3>
	      </div>
	      {else}
	      {assign var="childrenInColumnCount" value=0}
	      <div class="gsContentAlbum">
		<table id="gsThumbMatrix" width="100%">
		  <tr valign="top">
		    {foreach from=$theme.children item=child}

		    {* Move to a new row *}
		    {if ($childrenInColumnCount == $theme.params.columns)}
		  </tr>
		  <tr valign="top">
		    {assign var="childrenInColumnCount" value=0}
		    {/if}

		    {assign var=childrenInColumnCount value="`$childrenInColumnCount+1`"}
		    <td class="{if $child.canContainChildren}giAlbumCell{else}giItemCell{/if}"
			style="width: {$theme.columnWidthPct}%">
		      {if ($child.canContainChildren || $child.entityType == 'GalleryLinkItem')}
		        {assign var=frameType value="albumFrame"}
		        {capture assign=linkUrl}{g->url arg1="view=core.ShowItem"
						        arg2="itemId=`$child.id`"}{/capture}
		      {else}
		        {assign var=frameType value="itemFrame"}
			{capture assign=linkUrl}{strip}
			  {if $theme.params.dynamicLinks == 'jump'}
			    {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
			  {else}
			    {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
			  {/if}
			{/strip}{/capture}
		      {/if}
		      <div>
                        {strip}
			{if isset($theme.params.$frameType) && isset($child.thumbnail)}
			{g->container type="imageframe.ImageFrame" frame=$theme.params.$frameType
				      width=$child.thumbnail.width height=$child.thumbnail.height}
			  <a href="{$linkUrl}">
			    {g->image id="%ID%" item=$child image=$child.thumbnail class="%CLASS% giThumbnail"}
			  </a>
			{/g->container}
			{elseif isset($child.thumbnail)}
			  <a href="{$linkUrl}">
			    {g->image item=$child image=$child.thumbnail class="giThumbnail"}
			  </a>
			{else}
			  <a href="{$linkUrl}" class="giMissingThumbnail">
			    {g->text text="no thumbnail"}
			  </a>
			{/if}
                        {/strip}
		      </div>

		      {g->block type="core.ItemLinks" item=$child links=$child.itemLinks}

		      {if !empty($child.title)}
			{if $child.canContainChildren}
			<table cellpadding="0" cellspacing="0">
			  <tr>
			    <td class="giTitleIcon">
			      <img src="{g->url href="themes/carbon/images/album.gif"}" alt=""/>
			    </td>
			    <td>
			      <p class="giTitle">{$child.title|markup}</p>
			    </td>
			  </tr>
			</table>
			{else}
			<p class="giTitle">{$child.title|markup}</p>
			{/if}
		      {/if}

		      {if !empty($child.summary)}
		      <p class="giDescription">
			{$child.summary|markup|entitytruncate:256}
		      </p>
		      {/if}

		      {if !$theme.params.itemDetails}
			{g->block type="core.ItemInfo"
				  item=$child
				  showSummaries=true
				  class="giInfo"}
		      {else}
			{if ($child.canContainChildren && $theme.params.showAlbumOwner) ||
			    (!$child.canContainChildren && $theme.params.showImageOwner)}
			{assign var="showOwner" value=true}
			{else}
			{assign var="showOwner" value=false}
			{/if}
			{g->block type="core.ItemInfo"
				  item=$child
				  showDate=true
				  showOwner=$showOwner
				  showSize=true
				  showViewCount=true
				  showSummaries=true
				  class="giInfo"}
		      {/if}
		    </td>
		    {/foreach}

		    {* flush the rest of the row with empty cells *}
		    {section name="flush" start=$childrenInColumnCount loop=$theme.params.columns}
		    <td>&nbsp;</td>
		    {/section}
		  </tr>
		</table>
	      </div>
	      {/if}
	    </td>
	  </tr>
	</table>

	{if !empty($theme.navigator)}
	<div class="gbNavigator">
	  {* {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true} *}
	  {g->theme include="navigator.tpl"}
	</div>
	{/if}

	{* Our emergency edit link, if the user removes all blocks containing edit links *}
	{g->block type="core.EmergencyEditItemLink" class="gbBlock" 
		  checkBlocks="sidebar,album,albumUpper"}

	{* Show any other album blocks (comments, etc) *}
	{foreach from=$theme.params.albumBlocks item=block}
	  {g->block type=$block.0 params=$block.1}
	{/foreach}
      </div>
    </td>
  </tr>
</table>
{if !empty($theme.params.sidebarBlocks)}
  {g->theme include="sidebar.tpl"}
{/if}
