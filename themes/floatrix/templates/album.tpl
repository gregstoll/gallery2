{*
 * $Revision: 17175 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
      <div id="gsContent" class="gcBorder1">
        <div id="gbTitleBar" class="gbBlock gcBackground1">
          <div id="gbSearch">
            {g->block type="search.SearchBlock"}
          </div>

          {if !empty($theme.item.title)}
          <h2> {$theme.item.title|markup} </h2>
          {/if}
          {if !empty($theme.item.description)}
          <p class="giDescription">
            {$theme.item.description|markup}
          </p>
          {/if}

          {g->block type="core.ItemInfo"
                    item=$theme.item
                    showDate=true
                    showSize=true
                    showOwner=$theme.params.showAlbumOwner
                    class="giInfo"}
	  {if !empty($theme.userLinks)}
            {g->block type="core.ItemLinks" useDropdown=false
		      links=$theme.userLinks class="floatrix-userLinks"}
	  {/if}
        </div>

        {if !empty($theme.navigator)}
        <div class="gbBlock gcBackground2 gbNavigator">
          {if !empty($theme.jumpRange)}
        <div class="gbPager">
          {g->block type="core.Pager"}
        </div>
          {/if}
          {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
        </div>
        {/if}

        {if !count($theme.children)}
        <div class="gbBlock giDescription gbEmptyAlbum">
          <h3 class="emptyAlbum">
          {g->text text="This album is empty."}
          {if isset($theme.permissions.core_addDataItem)}
          <br/>
          <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd" arg3="itemId=`$theme.item.id`"}"> {g->text text="Add a photo!"} </a>
          {/if}
          </h3>
        </div>
        {else}
        <div id="gsThumbMatrix">
            {foreach from=$theme.children item=child}
            <div class="{if $child.canContainChildren}giAlbumCell gcBackground1{else}giItemCell{/if}" style="width: {$theme.params.columnWidth}px; height: {$theme.params.rowHeight}px;">

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

            {strip}
            {if isset($theme.params.$frameType) && isset($child.thumbnail)}
                {g->container type="imageframe.ImageFrame" frame=$theme.params.$frameType
                              width=$child.thumbnail.width height=$child.thumbnail.height}
		    <a href="{$linkUrl}">
		      {g->image id="%ID%" item=$child image=$child.thumbnail
				class="%CLASS% giThumbnail"}
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

            {g->block type="core.ItemLinks" item=$child links=$child.itemLinks}

	    {if !empty($child.title)}
	    <p class="giTitle">
		{if $child.canContainChildren && (!isset($theme.params.albumFrame)
		 || $theme.params.albumFrame == $theme.params.itemFrame)}
		  {* Add prefix for albums unless imageframe will differentiate *}
		  {g->text text="Album: %s" arg1=$child.title|markup}
		{else}
		  {$child.title|markup}
		{/if}
	    </p>
	    {/if}

            {if !empty($child.summary)}
            <p class="giDescription">{$child.summary|markup|entitytruncate:256}</p>
            {/if}

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
            </div>
            {/foreach}
        </div>
        {/if}

        {* Show any other album blocks (comments, etc) *}
        <div id="gbAlbumBlocks">
        {foreach from=$theme.params.albumBlocks item=block}
          {g->block type=$block.0 params=$block.1}
        {/foreach}
        </div>

        {if !empty($theme.navigator)}
        <div class="gbBlock gcBackground2 gbNavigator">
          {if !empty($theme.jumpRange)}
        <div class="gbPager">
          {g->block type="core.Pager"}
        </div>
          {/if}
          {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
        </div>
        {/if}

        {g->block type="core.GuestPreview" class="gbBlock"}

	{* Our emergency edit link, if the user removes all blocks containing edit links *}
	{g->block type="core.EmergencyEditItemLink" class="gbBlock" checkBlocks="sidebar,album"}

      </div>
