{* timeline_2026 - album view: a date-grouped stream, or a grid of album cards *}

{* albums-only layout when every child is an album; otherwise the photo stream,
   which branches per child so a sub-album mixed among photos still navigates *}
{assign var="gridMode" value="albums"}
{foreach from=$theme.children item=probeChild}
  {if !$probeChild.canContainChildren}
    {assign var="gridMode" value="photos"}
  {/if}
{/foreach}

<div class="tl-head">
  {if !empty($theme.item.title)}
    <h1>{$theme.item.title|markup}</h1>
  {else}
    <h1>{g->text text="Photos"}</h1>
  {/if}
  {if !empty($theme.item.description)}
    <p class="tl-desc">{$theme.item.description|markup}</p>
  {/if}
  <div class="tl-meta">
    {if !empty($theme.item.descendentCount)}
      <span>{$theme.item.descendentCount} {g->text text="items"}</span>
    {/if}
    {g->block type="core.ItemInfo" item=$theme.item showDate=true showSize=true class="giInfo"}
  </div>
</div>

{if !count($theme.children)}
  <div class="tl-empty">
    <p>{g->text text="This album is empty."}</p>
    {if isset($theme.permissions.core_addDataItem)}
      <a class="tl-btn" href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd" arg3="itemId=`$theme.item.id`"}">
        {g->text text="Add items"}</a>
    {/if}
  </div>

{elseif $gridMode == 'albums'}
  {* ---------------- album cards ---------------- *}
  <div class="tl-albums">
    {if isset($theme.permissions.core_addAlbumItem)}
      <a class="tl-card tl-card--new"
         href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAddAlbum" arg3="itemId=`$theme.item.id`"}">
        <span class="tl-card-new-mark" aria-hidden="true">+</span>
        <span class="tl-card-title">{g->text text="Create album"}</span>
      </a>
    {/if}
    {foreach from=$theme.children item=child}
      {capture assign=linkUrl}{strip}
        {if $theme.params.dynamicLinks == 'jump'}
          {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
        {else}
          {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
        {/if}
      {/strip}{/capture}
      <a class="tl-card" href="{$linkUrl}">
        <span class="tl-card-cover">
          {if isset($child.thumbnail)}
            {g->image item=$child image=$child.thumbnail class="giThumbnail"
                       loading="lazy" decoding="async"}
          {else}
            <span class="tl-noimg" aria-hidden="true"></span>
          {/if}
        </span>
        <span class="tl-card-title">
          {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}
        </span>
        <span class="tl-card-sub">
          {if !empty($child.descendentCount)}{$child.descendentCount} {g->text text="items"}{/if}
          {if !empty($child.originationTimestamp)}
            &middot; {g->date timestamp=$child.originationTimestamp format="%Y"}
          {/if}
        </span>
      </a>
    {/foreach}
  </div>

{else}
  {* ---------------- date-grouped photo stream ---------------- *}
  <div class="tl-stream" id="tlStream">
  {assign var="lastGroup" value="__none__"}
  {foreach from=$theme.children item=child name=kids}

    {* labels are precomputed in theme.inc: three strftime calls per item is
       far too expensive across a whole-library stream *}
    {assign var="grpKey" value=$child.groupKey}
    {if $child.groupKey == 'undated'}
      {capture assign="grpLabel"}{g->text text="Undated"}{/capture}
    {else}
      {assign var="grpLabel" value=$child.groupLabel}
    {/if}

    {if $grpKey != $lastGroup}
      {if !$smarty.foreach.kids.first}
        </div></section>
      {/if}
      <section class="tl-group" data-key="{$grpKey}">
        <h2 class="tl-groupdate">{$grpLabel}</h2>
        <div class="tl-just">
      {assign var="lastGroup" value=$grpKey}
    {/if}

    {* row height comes from the ORIGINAL photo's shape, not the square thumb *}
    {if !empty($child.width) && !empty($child.height)}
      {math assign="ar" equation="w / h" w=$child.width h=$child.height format="%.4f"}
    {elseif isset($child.thumbnail) && !empty($child.thumbnail.height)}
      {math assign="ar" equation="w / h" w=$child.thumbnail.width h=$child.thumbnail.height format="%.4f"}
    {else}
      {assign var="ar" value="1.3333"}
    {/if}

    {capture assign=linkUrl}{strip}
      {if $theme.params.dynamicLinks == 'jump'}
        {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
      {else}
        {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
      {/if}
    {/strip}{/capture}

    <div class="tl-cell" style="--ar: {$ar};">
      {if $child.canContainChildren}
        {* a sub-album among photos: navigate, never open the viewer *}
        <a class="tl-tile tl-tile--album" href="{$linkUrl}">
          {if isset($child.thumbnail)}
            {g->image item=$child image=$child.thumbnail class="giThumbnail"
                       loading="lazy" decoding="async"}
          {else}<span class="tl-noimg" aria-hidden="true"></span>{/if}
          <span class="tl-tile-cap">
            <span class="tl-badge">{g->text text="Album"}</span>
            {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}
          </span>
        </a>
      {else}
        <a class="tl-tile" href="{$linkUrl}"
           data-full="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$child.id`"}"
           data-title="{if !empty($child.title)}{$child.title|markup:strip|escape}{else}{$child.pathComponent|escape}{/if}"
           {if !empty($child.dateLabel)}data-date="{$child.dateLabel}"{/if}>
          {if isset($child.thumbnail)}
            {g->image item=$child image=$child.thumbnail class="giThumbnail"
                       loading="lazy" decoding="async"}
          {else}<span class="tl-noimg" aria-hidden="true"></span>{/if}
          <span class="tl-tile-cap">
            {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}
          </span>
        </a>
      {/if}
    </div>
  {/foreach}
  {if count($theme.children)}
        </div>
      </section>
  {/if}
  </div>

  {* the scrubber is rendered complete from the whole item set, so it does not
     grow as further pages load *}
  {if !empty($theme.yearMarks)}
    <nav class="tl-scrub" id="tlScrub" aria-label="{g->text text="Jump to year"}">
      {foreach from=$theme.yearMarks item=mark}
        <a class="tl-scrub-year" data-page="{$mark.page}" data-year="{$mark.year}"
           href="{g->url params=$theme.pageUrl arg1="page=`$mark.page`"}">{$mark.year}</a>
      {/foreach}
    </nav>
  {/if}
{/if}

{if !empty($theme.navigator)}
  <div class="gbNavigator tl-nav-pages">
    {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
  </div>
{/if}
