{* flowtheme_2026 - album view *}
{* albums-only view when EVERY child is an album; otherwise the justified grid,
   which branches per child so a sub-album mixed among photos still navigates. *}
{assign var="gridMode" value="albums"}
{foreach from=$theme.children item=probeChild}
  {if !$probeChild.canContainChildren}
    {assign var="gridMode" value="photos"}
  {/if}
{/foreach}
<div class="ft-layout{if !empty($theme.params.sidebarBlocks)} has-side{/if}">
  {if !empty($theme.params.sidebarBlocks)}
  <aside class="ft-side">{g->theme include="sidebar.tpl"}</aside>
  {/if}
  <div class="ft-content">

    <div class="ft-pagehead">
      {if !empty($theme.item.title)}
        <h1>{$theme.item.title|markup}</h1>
      {/if}
      {if !empty($theme.item.description)}
        <p class="ft-desc">{$theme.item.description|markup}</p>
      {/if}
      <div class="ft-meta">
        {if !empty($theme.item.descendentCount)}
          <span class="ft-count">{$theme.item.descendentCount} {g->text text="items"}</span>
        {/if}
        {g->block type="core.ItemInfo" item=$theme.item showDate=true showSize=true
                  showOwner=$theme.params.showAlbumOwner class="giInfo"}
      </div>
    </div>

    {if !count($theme.children)}
      <div class="ft-empty">{g->text text="This album is empty."}</div>

    {elseif $gridMode == 'albums'}
      {* ---- album cards ---- *}
      <div class="ft-grid ft-grid--albums">
        {foreach from=$theme.children item=child}
          {capture assign=linkUrl}{strip}
            {if $theme.params.dynamicLinks == 'jump'}
              {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
            {else}
              {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
            {/if}
          {/strip}{/capture}
          <div class="ft-cell">
            <a class="ft-tile ft-isalbum" href="{$linkUrl}">
              <span class="ft-frame">
                {if isset($child.thumbnail)}
                  {g->image item=$child image=$child.thumbnail class="giThumbnail"}
                {else}
                  <span class="ft-noimg">{g->text text="no thumbnail"}</span>
                {/if}
              </span>
              <span class="ft-cap">
                <span class="ft-badge">{g->text text="Album"}</span>
                <span class="ft-cap-title">{if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}</span>
                {if !empty($child.summary)}<span class="ft-cap-sub">{$child.summary|markup:strip}</span>{/if}
              </span>
            </a>
          </div>
        {/foreach}
      </div>

    {else}
      {* ---- justified photo grid ---- *}
      <div class="ft-just">
        {foreach from=$theme.children item=child}
          {capture assign=linkUrl}{strip}
            {if $theme.params.dynamicLinks == 'jump'}
              {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
            {else}
              {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
            {/if}
          {/strip}{/capture}
          {* layout aspect comes from the ORIGINAL photo, not the square thumbnail *}
          {if !empty($child.width) && !empty($child.height)}
            {math assign="ar" equation="w / h" w=$child.width h=$child.height format="%.4f"}
          {elseif isset($child.thumbnail) && !empty($child.thumbnail.height)}
            {math assign="ar" equation="w / h" w=$child.thumbnail.width h=$child.thumbnail.height format="%.4f"}
          {else}
            {assign var="ar" value="1.3333"}
          {/if}
          <div class="ft-cell" style="--ar: {$ar};">
            {if $child.canContainChildren}
              {* a sub-album sitting among photos: navigate, never open the lightbox *}
              <a class="ft-shot ft-isalbum" href="{$linkUrl}">
                {if isset($child.thumbnail)}
                  {g->image item=$child image=$child.thumbnail class="giThumbnail"}
                {else}
                  <span class="ft-noimg">{g->text text="no thumbnail"}</span>
                {/if}
                <span class="ft-shot-cap"><span class="ft-badge">{g->text text="Album"}</span>
                  {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}</span>
              </a>
            {else}
              <a class="ft-shot" href="{$linkUrl}"
                 data-full="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$child.id`"}"
                 data-title="{if !empty($child.title)}{$child.title|markup:strip|escape}{else}{$child.pathComponent|escape}{/if}">
                {if isset($child.thumbnail)}
                  {g->image item=$child image=$child.thumbnail class="giThumbnail"}
                {else}
                  <span class="ft-noimg">{g->text text="no thumbnail"}</span>
                {/if}
                <span class="ft-shot-cap">{if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}</span>
              </a>
            {/if}
          </div>
        {/foreach}
      </div>
    {/if}

    {if !empty($theme.navigator)}
      <div class="gbNavigator">
        {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
      </div>
    {/if}
  </div>
</div>
