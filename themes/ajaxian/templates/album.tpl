{*
 * $Revision: 17955 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<h2 id="item-title">{$theme.item.title|markup|default:" "}</h2>

{if !empty($theme.navigator)}
  <div class="gbBlock gcBackground2 gbNavigator">
    {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
  </div>
{/if}

{if !count($theme.children)}
  <div class="gbBlock giDescription gbEmptyAlbum">
    <p class="emptyAlbum">
    {g->text text="This album is empty."}
      {if isset($theme.permissions.core_addDataItem)}
    <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd"
             arg3="itemId=`$theme.item.id`"}">{g->text text="Add a photo!"}</a>
      {/if}
    </p>
  </div>
{else}
  <div id="main-image-container">
    {if $theme.imageCount > 0}
      <div id="slideshow-controls">
        <ul id="control-buttons">
    <li><button id="controls-left">
      <img src="{g->theme url="images/controls-left.png"}" alt="{g->text text="Left"}" />
      </button></li>
    <li><button id="controls-play">
      <img src="{g->theme url="images/controls-play.png"}" alt="{g->text text="Play/Pause"}" />
      </button></li>
    <li><button id="controls-right">
      <img src="{g->theme url="images/controls-right.png"}" alt="{g->text text="Right"}" />
      </button></li>
        </ul>
      </div>

    <div id="sliding-frame">
      <div id="loading">
    {g->text text="Loading Album..."}
      </div>

      <p><img src="{g->theme url="images/blank.png"}"
      alt="{g->text text="Main image placeholder"}" id="main-image" /></p>
    </div>
    {/if}

    {assign var="childrenInColumnCount" value=0}
    {assign var="subalbumCount" value=0}
    <div id="thumbs-container"{if $theme.imageCount < 1 } style="background:none; margin-top:30px"{/if}>
      <table id="gsThumbMatrix">
    <tr valign="top">
    {foreach from=$theme.children item=child}

      {if !$child.canContainChildren && $child.entityType != 'GalleryLinkItem'}
        {if ($childrenInColumnCount == $theme.params.columns)}
          {* Move to a new row *}
          </tr><tr valign="top">
          {assign var="childrenInColumnCount" value=0}
        {/if}
        {assign var=childrenInColumnCount value="`$childrenInColumnCount+1`"}
	{assign var=childSummary value=$child.summary|markup|escape:html}
	{assign var=childDescription value=$child.description|markup|escape:html}
        <td class="giItemCell">
          {if isset($theme.params.itemFrame) && isset($child.thumbnail)}
        {g->container type="imageframe.ImageFrame" frame=$theme.params.itemFrame}
          <a href="{g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}">{g->image
            id="%ID%" item=$child image=$child.thumbnail
            class="%CLASS% giThumbnail size:=`$child.size`= summary:=`$childSummary`= description:=`$childDescription`="}</a>
        {/g->container}
          {elseif isset($child.thumbnail)}
        <a href="{g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}">{g->image
            item=$child image=$child.thumbnail
            class="r giThumbnail size:=`$child.size`= summary:=`$childSummary`= description:=`$childDescription`="}</a>
          {else}
        <a href="{g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}"
           class="giMissingThumbnail">
          {g->text text="no thumbnail"}
        </a>
          {/if}
        </td>
      {else}
        {assign var=subalbumCount value="`$subalbumCount+1`"}
      {/if}
    {/foreach}

    {* flush the rest of the row with empty cells *}
    {section name="flush" start=$childrenInColumnCount loop=$theme.params.columns}
      <td>&nbsp;</td>
    {/section}
    </tr>
      </table>
    </div>
  </div>

  {* Loop around for the albums this time *}
  {assign var="childrenInColumnCount" value=0}
  {if $subalbumCount > 0}
    <div id="subalbums-container">
    <h3>{g->text text="Subalbums"}</h3>
    <table id="gsSubAlbumMatrix">
      <tr valign="top">
      {foreach from=$theme.children item=child}

    {if $child.canContainChildren || $child.entityType == 'GalleryLinkItem'}
      {if ($childrenInColumnCount == 2)}
        {* Move to a new row *}
        </tr><tr>
        {assign var="childrenInColumnCount" value=0}
      {/if}
      {assign var=childrenInColumnCount value="`$childrenInColumnCount+1`"}
      <td class="giAlbumCell gcBackground1">
        {if isset($child.thumbnail)}
          <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}">
        {g->image item=$child image=$child.thumbnail class="giThumbnail"}
        {$child.title|entitytruncate:25}</a>
        {else}
          <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}"
         class="giMissingThumbnail">
        <img src="{g->theme url="images/missing.png"}" width="40" height="40"
             alt="{g->text text="no thumbnail"}" />
        {$child.title|entitytruncate:25}</a>
        {/if}

        {g->block type="core.ItemInfo" item=$child
          showDate=false
          showOwner=$theme.params.showAlbumOwner
          showSize=true
          showViewCount=true
          showSummaries=true
          class="giInfo"}
      </td>
    {/if}
      {/foreach}

      {* flush the rest of the row with empty cells *}
      {section name="flush" start=$childrenInColumnCount loop=2}
    <td>&nbsp;</td>
      {/section}
      </tr>
    </table>
  </div>
  {/if}
{/if}

{if !empty($theme.navigator)}
  <div class="gbBlock gcBackground2 gbNavigator">
    {g->block type="core.Navigator" navigator=$theme.navigator}
  </div>
{/if}

{* Store these results in a JavaScript-accessible set of arrays so the slideshow can get at them *}
<script type="text/javascript">
  // <![CDATA[
  var slideshowImageWidths = new Array({if $theme.imageCount==1}1);
  slideshowImageWidths[0] = {$theme.imageWidths};
  {else}{$theme.imageWidths});{/if}
  var slideshowImages = new Array();
  {foreach from=$theme.children key=i item=it}
    {if !$it.canContainChildren && $it.entityType != 'GalleryLinkItem'}
    slideshowImages.push('{if isset($it.image)}{g->url arg1="view=core.DownloadItem"
      arg2="itemId=`$it.image.id`" arg3="serialNumber=`$it.image.serialNumber`"
      htmlEntities=false}{else}{g->url params=$theme.pageUrl arg1="itemId=`$it.id`"
      htmlEntities=false}{/if}');
    {/if}
  {/foreach}
  // ]]>
</script>

{* Show any other album blocks (comments, etc) *}
{foreach from=$theme.params.albumBlocks item=block}
  {g->block type=$block.0 params=$block.1}
{/foreach}

{g->block type="core.GuestPreview" class="gbBlock"}

{* Our emergency edit link, if the user removes all blocks containing edit links *}
{g->block type="core.EmergencyEditItemLink" class="gbBlock" checkBlocks="album"}
